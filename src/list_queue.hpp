#pragma once

#include <list>

template<typename Value>
struct ListQueue : public IQueue<Value> {
    bool Enqueue(Value value) override {list_.push_back(value); return true;}
    Value Dequeue() override {
		if(list_.size()>0)
	    {   
			auto front = list_.front();
			list_.pop_front();
        	return front;
		}
		return Value{};
    }
	std::size_t Size() override { return list_.size();}
    std::list<Value> list_;
};