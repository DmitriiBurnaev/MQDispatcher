#pragma once

#include <mutex>

#include "interfaces.hpp"

template <typename Value>
struct DropOnOverflowStrategy : public IOnOverflowStrategy<Value> {
  virtual bool Enqueue(Value value, std::shared_ptr<IQueue<Value>> queue,
                       std::size_t max_size) override {
    std::lock_guard<std::mutex> lck(mtx_);
    if (queue->Size() >= max_size) return false;
    return queue->Enqueue(value);
  }

 private:
  std::mutex mtx_;
};