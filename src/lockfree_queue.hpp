#pragma once

#include <atomic>
#include <boost/lockfree/queue.hpp>

template<typename Value>
struct LockFreeQueue : public IQueue<Value> {
    bool Enqueue(Value value) override {
        auto result = queue_.push(value); 
        if(result) ++size;
        return result;
        }
    Value Dequeue() override {
            Value val {};
            auto result = queue_.pop(val);
			if(result) --size;
            return val;
    }
	std::size_t Size() override { return size;}
    boost::lockfree::queue<Value> queue_{0};
    std::atomic<std::size_t> size{0};
};