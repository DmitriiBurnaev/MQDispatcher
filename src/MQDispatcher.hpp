#pragma once

#include "interfaces.hpp"
#include "list_queue.hpp"
#include "drop_on_overflow_strategy.hpp"
#include "exchange_on_overflow_strategy.hpp"
#include "block_on_overflow_strategy.hpp"
#include "queue_context.hpp"
#include "lockfree_queue.hpp"

#include <map>
#include <list>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <vector>

#include <iostream>

template<typename Key, typename Value>
class MultiQueueProcessor
{
public:
	MultiQueueProcessor(std::function<std::shared_ptr<IQueue<Value>>()> qmaker) :
		running{ true },
        queue_creator{std::move(qmaker)} {
			auto thread_count = std::thread::hardware_concurrency();
			if(thread_count == 0) thread_count = 8;
			for(int i = 0; i < thread_count; ++i)
			{
				std::thread th(std::bind(&MultiQueueProcessor::Process, this));
				thrds.push_back(std::move(th));
			}
		}

	~MultiQueueProcessor()
	{
		StopProcessing();
	}

	void StopProcessing()
	{
		running = false;
		for (int i=0; i<thrds.size(); ++i)
		{	
    		if (thrds[i].joinable())
        		thrds[i].join();
		}
	}

	void Subscribe(Key id, std::shared_ptr<IConsumer<Key, Value>> consumer)
	{
		std::lock_guard<std::shared_mutex> lock{ consumers_mtx };
		auto iter = consumers.find(id);
		if (iter == consumers.end())
		{
			consumers.insert(std::make_pair(id, consumer));
		}
	}

	void Unsubscribe(Key id)
	{
		std::lock_guard<std::shared_mutex> lock{ consumers_mtx };
		auto iter = consumers.find(id);
		if (iter != consumers.end())
			consumers.erase(id);
	}

	void Enqueue(Key id, Value value)
	{
		{ 
			std::shared_lock<std::shared_mutex> lock{ queues_mtx };
			auto iter = queues.find(id);
			if (iter != queues.end())
			{
				iter->second->Enqueue(value);
				return;
			}
		}
			std::unique_lock<std::shared_mutex> ulck(queues_mtx);
			queues.insert(std::make_pair(id, queue_creator()));
			auto iter = queues.find(id);
			if (iter != queues.end())
			{
				iter->second->Enqueue(value);
			}
	}

	Value Dequeue(Key id)
	{
		std::shared_lock<std::shared_mutex> lock{ queues_mtx };
		auto iter = queues.find(id);
		if (iter != queues.end())
		{
            lock.unlock(); 
			auto val = iter->second->Dequeue();
			return val;
		}
		return Value{};
	}

protected:
	void Process()
	{
		while (running)
		{
			{
			std::shared_lock<std::shared_mutex> consumers_lock{consumers_mtx};
			for(auto iter = consumers.begin(); iter != consumers.end(); ++iter)
			{
				Value front = Dequeue(iter->first);
					if (front != Value{})
						iter->second->Consume(iter->first, front);
			}
			}
		}
	}

protected:
	std::shared_mutex consumers_mtx;
	std::unordered_map<Key, std::shared_ptr<IConsumer<Key, Value>>> consumers;
	std::shared_mutex queues_mtx;
	std::unordered_map<Key, std::shared_ptr<IQueue<Value>>> queues;

	std::atomic<bool> running;
	std::recursive_mutex mtx;
	std::vector<std::thread> thrds;
    std::function<std::shared_ptr<IQueue<Value>>()> queue_creator;
};

template<typename Key, typename Value>
MultiQueueProcessor<Key,Value> create_mq() {
    return MultiQueueProcessor<Key,Value>([](){
		//auto queue = std::make_shared<ListQueue<Value>>();
		auto queue = std::make_shared<LockFreeQueue<Value>>();
		auto max_size = 5;
		//auto strategy = std::make_shared<DropOnOverflowStrategy<Value>>();
		//auto strategy = std::make_shared<ExchangeOnOverflowStrategy<Value>>();
		auto strategy = std::make_shared<BlockOnOverflowStrategy<Value>>();
		return std::make_shared<QueueContext<Value>>(max_size,strategy,queue);});
}