#pragma once

#include <memory>

template <typename Key, typename Value>
struct IConsumer {
  virtual ~IConsumer() = default;
  virtual void Consume(Key id, const Value &value) = 0;
};

template <typename Value>
struct IQueue {
  virtual ~IQueue() = default;
  virtual bool Enqueue(Value value) = 0;
  virtual Value Dequeue() = 0;
  virtual std::size_t Size() = 0;
};

template <typename Value>
struct IOnOverflowStrategy {
  virtual ~IOnOverflowStrategy() = default;
  virtual bool Enqueue(Value value, std::shared_ptr<IQueue<Value>> queue,
                       std::size_t max_size) = 0;
};