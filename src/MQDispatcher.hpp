#pragma once

#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "block_on_overflow_strategy.hpp"
#include "drop_on_overflow_strategy.hpp"
#include "exchange_on_overflow_strategy.hpp"
#include "interfaces.hpp"
#include "list_queue.hpp"
#include "lockfree_queue.hpp"
#include "queue_context.hpp"

template <typename Key, typename Value>
class MultiQueueDispatcher {
 public:
  MultiQueueDispatcher(std::function<std::shared_ptr<IQueue<Value>>()> qmaker)
      : running_{true}, queue_creator_{std::move(qmaker)} {
    auto thread_count = std::thread::hardware_concurrency();
    constexpr auto default_thread_count = 8;
    if (thread_count == 0) thread_count = default_thread_count;
    for (int i = 0; i < thread_count; ++i) {
      std::thread th(std::bind(&MultiQueueDispatcher::Process, this));
      thrds_.push_back(std::move(th));
    }
  }

  ~MultiQueueDispatcher() { StopProcessing(); }

  void StopProcessing() {
    running_ = false;
    for (int i = 0; i < thrds_.size(); ++i) {
      if (thrds_[i].joinable()) thrds_[i].join();
    }
  }

  void Subscribe(Key id, std::shared_ptr<IConsumer<Key, Value>> consumer) {
    std::lock_guard<std::shared_mutex> lock{consumers_mtx_};
    auto iter = consumers_.find(id);
    if (iter == consumers_.end()) {
      consumers_.insert(std::make_pair(id, consumer));
    }
  }

  void Unsubscribe(Key id) {
    std::lock_guard<std::shared_mutex> lock{consumers_mtx_};
    auto iter = consumers_.find(id);
    if (iter != consumers_.end()) consumers_.erase(id);
  }

  bool Enqueue(Key id, Value value) {
    {
      std::shared_lock<std::shared_mutex> lock{queues_mtx_};
      auto iter = queues_.find(id);
      if (iter != queues_.end()) {
        return iter->second->Enqueue(value);
      }
    }
    std::unique_lock<std::shared_mutex> ulck(queues_mtx_);
    auto result = queues_.insert(std::make_pair(id, queue_creator_()));
    if (result.second) return result.first->second->Enqueue(value);
    return false;
  }

  std::optional<Value> Dequeue(Key id) {
    std::shared_lock<std::shared_mutex> lock{queues_mtx_};
    auto iter = queues_.find(id);
    if (iter != queues_.end()) {
      lock.unlock();
      auto val = iter->second->Dequeue();
      return val;
    }
    return std::nullopt;
  }

 protected:
  void Process() {
    while (running_) {
      {
        std::shared_lock<std::shared_mutex> consumers_lock{consumers_mtx_};
        for (auto iter = consumers_.begin(); iter != consumers_.end(); ++iter) {
          auto front = Dequeue(iter->first);
          if (front) iter->second->Consume(iter->first, front.value());
        }
      }
    }
  }

 protected:
  std::shared_mutex consumers_mtx_;
  std::unordered_map<Key, std::shared_ptr<IConsumer<Key, Value>>> consumers_;
  std::shared_mutex queues_mtx_;
  std::unordered_map<Key, std::shared_ptr<IQueue<Value>>> queues_;

  std::atomic<bool> running_;
  std::vector<std::thread> thrds_;
  std::function<std::shared_ptr<IQueue<Value>>()> queue_creator_;
};

template <typename Key, typename Value, typename QueueType = ListQueue<Value>,
          typename OnOverflowStrategy = DropOnOverflowStrategy<Value>>
std::shared_ptr<MultiQueueDispatcher<Key, Value>> create_mqd(
    std::size_t max_size = 0) {
  if (max_size == 0) {
    return std::make_shared<MultiQueueDispatcher<Key, Value>>(
        [] { return std::make_shared<QueueType>(); });
  } else {
    return std::make_shared<MultiQueueDispatcher<Key, Value>>([max_size] {
      auto queue = std::make_shared<QueueType>();
      auto strategy = std::make_shared<OnOverflowStrategy>();
      return std::make_shared<QueueContext<Value>>(max_size, strategy, queue);
    });
  }
}