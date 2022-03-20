#define BOOST_TEST_MAIN

#include <MQDispatcher.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(mqdispatcher_testsuite)

BOOST_AUTO_TEST_CASE(check_list_queue) {
  auto mqd = create_mqd<int, std::string>();
  constexpr auto queue_number = 1;
  mqd->Enqueue(queue_number, "First");
  mqd->Enqueue(queue_number, "Second");
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), "First");
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), "Second");
}

BOOST_AUTO_TEST_CASE(check_lockfree_queue) {
  auto mqd = create_mqd<int, int, LockFreeQueue<int>>();
  constexpr auto queue_number = 1;
  mqd->Enqueue(queue_number, 5);
  mqd->Enqueue(queue_number, 55);
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), 5);
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), 55);
}

BOOST_AUTO_TEST_CASE(check_drop_strategy) {
  constexpr auto queue_max_size = 2;
  auto mqd = create_mqd<int, std::string, ListQueue<std::string>,
                        DropOnOverflowStrategy<std::string>>(queue_max_size);
  constexpr auto queue_number = 1;
  mqd->Enqueue(queue_number, "First");
  mqd->Enqueue(queue_number, "Second");
  mqd->Enqueue(queue_number, "Third");
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), "First");
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), "Second");
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).has_value(), false);
}

BOOST_AUTO_TEST_CASE(check_exchange_strategy) {
  constexpr auto queue_max_size = 2;
  auto mqd =
      create_mqd<int, std::string, ListQueue<std::string>,
                 ExchangeOnOverflowStrategy<std::string>>(queue_max_size);
  constexpr auto queue_number = 1;
  mqd->Enqueue(queue_number, "First");
  mqd->Enqueue(queue_number, "Second");
  mqd->Enqueue(queue_number, "Third");
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), "Second");
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), "Third");
}

BOOST_AUTO_TEST_CASE(check_block_strategy) {
  constexpr auto queue_max_size = 2;
  auto mqd =
      create_mqd<int, int, LockFreeQueue<int>, BlockOnOverflowStrategy<int>>(
          queue_max_size);
  constexpr auto queue_number = 1;
  mqd->Enqueue(queue_number, 5);
  mqd->Enqueue(queue_number, 55);
  std::thread th([mqd, queue_number] {
    mqd->Enqueue(queue_number, 77);
    BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), 55);
  });
  BOOST_REQUIRE_EQUAL(mqd->Dequeue(queue_number).value(), 5);
  th.join();
}

BOOST_AUTO_TEST_SUITE_END()
