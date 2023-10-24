#include "linear/stack_linked.hpp"

#include <iostream>
#include <string>

int main() {
  Stack<std::string> stack;

  for (auto name : {"foo", "bar", "baz"})
    stack.push(name);

  for (; not stack.is_empty(); stack.pop())
    std::cout << stack.top() << "\n";

  for (std::size_t i = 0; i < 100; ++i)
    stack.push({});
}
