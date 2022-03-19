#pragma once

#include "interfaces.hpp"
#include <memory>

template<typename Value>
struct QueueContext : public IQueue<Value>
{
	QueueContext(std::size_t max_size, std::shared_ptr<IOnOverflowStrategy<Value>> strategy,
	             std::shared_ptr<IQueue<Value>> queue) : max_size_{max_size},
				                                         strategy_{strategy},
														 queue_{queue} {}
	bool Enqueue(Value value) override {
		return strategy_->Enqueue(value, queue_, max_size_);
	}
	Value Dequeue() override {
		return queue_->Dequeue();
	}
	std::size_t Size() override {
		return queue_->Size();
	}
private:
	std::size_t max_size_ {0};
	std::shared_ptr<IOnOverflowStrategy<Value>> strategy_;
	std::shared_ptr<IQueue<Value>> queue_;
};