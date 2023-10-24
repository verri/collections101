#include "linear/queue_circular.hpp"

#include <iostream>
#include <string>

int main() {
  Queue<std::string> queue;

  for (auto name : {"foo", "bar", "baz"})
    queue.enqueue(name);

  for (; not queue.is_empty(); queue.dequeue())
    std::cout << queue.front() << "\n";

  for (auto name : {"foo", "bar", "baz"})
    queue.enqueue(name);

  for (; not queue.is_empty(); queue.dequeue())
    std::cout << queue.front() << "\n";

  for (std::size_t i = 0; i < 100; ++i)
    queue.enqueue("very big string, probably will allocate memory...");
}
