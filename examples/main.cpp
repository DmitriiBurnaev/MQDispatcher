#include <MQDispatcher.hpp>

#include <string>
#include <iostream>
#include <memory>

template<typename Key, typename Value>
struct A : public IConsumer<Key,Value> {
    void Consume(Key id,const Value &value) override
    {
        std::cout << id << " " << value << std::endl;
    }
};

void fill(MultiQueueProcessor<int,std::string>& mqp)
{
    for(int i = 10; i < 5000; ++i)
    {
        mqp.Enqueue(i, "dd");
    }
}

int main(){
    //MultiQueueProcessor<int,std::string> mqp = create_mq<int,std::string>();
    //A<int,std::string>& a = new A<int,std::string>();
    MultiQueueProcessor<int,int> mqp = create_mq<int,int>();
    auto cons =std::make_shared<A<int, int>>();
    auto cons2 =std::make_shared<A<int, std::string>>();
    mqp.Subscribe(1,cons);
    //mqp.Subscribe(2, cons2);
    std::this_thread::sleep_for(std::chrono::seconds(3));
     mqp.Enqueue(1, 3);
      mqp.Enqueue(1, 4);
    /*mqp.Enqueue(1, "ARAR");
    mqp.Enqueue(1, "ALALA");
    mqp.Enqueue(1, "db3");
    mqp.Enqueue(1, "db4");
    mqp.Enqueue(1, "db5");
    mqp.Enqueue(1, "db6");
    mqp.Enqueue(1, "db7");
    mqp.Enqueue(2, "db6");
    mqp.Enqueue(2, "db7");
    for(int i = 0 ; i < 7; ++i)
        std::cout << i << " " << mqp.Dequeue(1) << std::endl;*/
    //std::thread th1{[&mqp]{fill(mqp);}};
    //std::thread th2{[&mqp]{fill(mqp);}};
    //std::thread th3{[&mqp]{fill(mqp);}};
    //fill(mqp);
    //th1.join();
    //th2.join();
    //th3.join();
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}