#include "linear/list.hpp"

#include <string>
#include <iostream>

int main() {
  List<std::string> list;

  auto p = list.insert_after(list.before_first(), "Bar");
  list.insert_after(p, "Baz");
  list.insert_after(list.before_first(), "Foo");

  std::cout << p->value() << "\n";

  std::cout << "content: ";
  for (auto p = list.before_first()->next(); p != nullptr; p = p->next())
    std::cout << p->value() << " ";
  std::cout << '\n';

  for (std::size_t i = 0; i < 100; ++i)
    list.insert_after(list.before_first(), {});
}
