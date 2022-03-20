#include <MQDispatcher.hpp>
#include <iostream>
#include <memory>
#include <string>

template <typename Key, typename Value>
struct ConsumerExample : public IConsumer<Key, Value> {
  void Consume(Key id, const Value &value) override {
    std::cout << id << " " << value << std::endl;
  }
};

int main() {
  auto mqp = create_mqd<int, std::string>();
  auto consumer = std::make_shared<ConsumerExample<int, std::string>>();
  constexpr auto queue_number = 1;
  mqp->Subscribe(queue_number, consumer);
  mqp->Enqueue(queue_number, "first");
  mqp->Enqueue(queue_number, "second");

  std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;
}