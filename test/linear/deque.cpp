#include "linear/deque.hpp"

#include <string>
#include <iostream>

int main() {
  Deque<std::string> deque;

  deque.insert_rear("Bar");
  deque.insert_front("Foo");
  deque.insert_rear("Baz");

  std::cout << deque.front() << "\n";
  deque.remove_front();

  std::cout << deque.rear() << "\n";
  deque.remove_rear();

  for (std::size_t i = 0; i < 100; ++i)
    deque.insert_rear({});
}
