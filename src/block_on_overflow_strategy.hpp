#pragma once

#include <mutex>
#include <thread>

#include "interfaces.hpp"

template <typename Value>
struct BlockOnOverflowStrategy : public IOnOverflowStrategy<Value> {
  virtual bool Enqueue(Value value, std::shared_ptr<IQueue<Value>> queue,
                       std::size_t max_size) override {
    std::lock_guard<std::mutex> lck(mtx_);
    while (queue->Size() >= max_size) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    return queue->Enqueue(value);
  }
  std::mutex mtx_;
};