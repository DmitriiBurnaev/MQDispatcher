#pragma once

#include <list>
#include <mutex>

template <typename Value>
struct ListQueue final : public IQueue<Value> {
  bool Enqueue(Value value) override {
    std::lock_guard<std::mutex> lck(mtx_);
    list_.push_back(value);
    return true;
  }
  std::optional<Value> Dequeue() override {
    std::lock_guard<std::mutex> lck(mtx_);
    if (list_.size() > 0) {
      auto front = list_.front();
      list_.pop_front();
      return front;
    }
    return std::nullopt;
  }
  std::size_t Size() override {
    std::lock_guard<std::mutex> lck(mtx_);
    return list_.size();
  }

 private:
  std::mutex mtx_;
  std::list<Value> list_;
};