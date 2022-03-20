#pragma once

#include "interfaces.hpp"

template <typename Value>
struct ExchangeOnOverflowStrategy : public IOnOverflowStrategy<Value> {
  virtual bool Enqueue(Value value, std::shared_ptr<IQueue<Value>> queue,
                       std::size_t max_size) override {
    std::lock_guard<std::mutex> lck(mtx_);
    while (queue->Size() >= max_size) {
      (void)queue->Dequeue();
    }
    return queue->Enqueue(value);
  }
  std::mutex mtx_;
};