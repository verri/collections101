#include "linear/colony.hpp"

#include <string>
#include <iostream>
#include <memory>

int main() {
  Colony<std::string> colony(4);

  if (not colony.is_empty())
    std::cout << "Colony is empty\n";

  for (auto name : {"foo", "bar", "baz", "hello", "world"})
    colony.insert(name);

  if (not colony.is_empty())
    std::cout << "Colony is not empty anymore...\n";

  auto p = colony.search([](const std::string& name) { return name == "baz"; });
  std::cout << "Found " << *p << "!\n";

  std::cout << "Removing " << *p << "\n";
  colony.remove(p);

  std::cout << "Colony contains values:";
  colony.search([](const auto& name) { std::cout << " " << name; return false; });
  std::cout << "\n";

  colony.insert("zab");

  std::cout << "Colony now contains values:";
  colony.search([](const auto& name) { std::cout << " " << name; return false; });
  std::cout << "\n";

  auto foo = colony.search([](const auto& name) { return name == "foo"; });
  auto bar = colony.search([](const auto& name) { return name == "bar"; });

  colony.remove(foo);
  colony.remove(bar);

  std::cout << "Colony now contains values:";
  colony.search([](const auto& name) { std::cout << " " << name; return false; });
  std::cout << "\n";

  // works very well with smart pointers
  // useful in game engines, for instance.
  {
    Colony<int> c;
    std::shared_ptr<int> pkeep;
    {
      auto p = std::shared_ptr<int>(c.insert(1), [&](auto* p) { c.remove(p); });
      pkeep = std::shared_ptr<int>(c.insert(2), [&](auto* p) { c.remove(p); });
    }
    std::cout << "Colony now contains values:";
    c.search([](int i) { std::cout << " " << i; return false; });
    std::cout << "\n";
  }

  {
    static Colony<int> c;
    const auto deleter = [](auto* p) { c.remove(p); };

    std::unique_ptr<int, decltype(deleter)> pkeep;
    {
      auto p = std::unique_ptr<int, decltype(deleter)>(c.insert(1));
      pkeep.reset(c.insert(2));
    }
    std::cout << "Colony now contains values:";
    c.search([](int i) { std::cout << " " << i; return false; });
    std::cout << "\n";
  }
}
