#include <MQDispatcher.hpp>
#include <memory>

template <typename Key, typename Value>
struct TestMQD : public IConsumer<Key, Value> {
  TestMQD(std::size_t queues_number_, std::size_t num_of_threads_)
      : mqd{create_mqd<Key, Value, ListQueue<Value>>()},
        queues_number{queues_number_},
        num_of_threads{num_of_threads_},
        arr(queues_number, num_of_threads) {}

  ~TestMQD() {
    for (int i = 0; i < thrds.size(); ++i) {
      if (thrds[i].joinable()) thrds[i].join();
    }
  }

  void Subscribe(std::shared_ptr<IConsumer<Key, Value>> consumer) {
    for (int i = 0; i < queues_number; ++i) {
      mqd->Subscribe(i, consumer);
    }
  }

  void Consume() {
    for (int i = 0; i < num_of_threads; ++i) {
      std::thread th(std::bind(&TestMQD::Fill, this));
      thrds.push_back(std::move(th));
    }

    for (int i = 0; i < thrds.size(); ++i) {
      if (thrds[i].joinable()) thrds[i].join();
    }
  }
  void Consume(Key id, const Value &value) override {
    std::lock_guard<std::mutex> lck(arr_mtx);
    arr[id] -= value;
  }

  void Verify() {
    for (int i = 0; i < queues_number; ++i) {
      assert(arr[i] == 0);
    }
  }

 private:
  void Fill() {
    constexpr auto value = 1;
    for (int i = 0; i < queues_number; ++i) {
      if (!mqd->Enqueue(i, value)) {
        std::lock_guard<std::mutex> lck(arr_mtx);
        --arr[i];
      }
    }
  }

  std::shared_ptr<MultiQueueDispatcher<Key, Value>> mqd;
  const std::size_t queues_number;
  const std::size_t num_of_threads;
  std::mutex arr_mtx;
  std::vector<int> arr;
  std::vector<std::thread> thrds;
};

int main() {
  constexpr auto queues_number = 10000;
  constexpr auto num_of_threads = 8;
  auto tmqd =
      std::make_shared<TestMQD<int, int>>(queues_number, num_of_threads);
  tmqd->Subscribe(std::dynamic_pointer_cast<IConsumer<int, int>>(tmqd));
  tmqd->Consume();
  std::this_thread::sleep_for(std::chrono::seconds(15));
  tmqd->Verify();
  return 0;
}