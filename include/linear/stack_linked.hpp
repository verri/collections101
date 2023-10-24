#ifndef STACK_LINKED_HPP
#define STACK_LINKED_HPP

#include <stdexcept>
#include <utility>

template <std::movable T>
class Stack {
public:
  ~Stack() noexcept;

  void push(T);
  void pop();
  [[nodiscard]] auto top() const -> const T&;
  [[nodiscard]] auto is_empty() const noexcept -> bool;

private:
  struct Node {
    T value;
    Node* next;
  };

  Node* top_ = nullptr;
};

template <std::movable T>
Stack<T>::~Stack() noexcept {
  while (top_)
    delete std::exchange(top_, top_->next);
}

template <std::movable T>
void Stack<T>::push(T value) {
  top_ = new Node{std::move(value), top_};
}

template <std::movable T>
void Stack<T>::pop() {
  if (is_empty())
    throw std::runtime_error{"no element to pop"};
  delete std::exchange(top_, top_->next);
}

template <std::movable T>
auto Stack<T>::top() const -> const T& {
  if (is_empty())
    throw std::runtime_error{"empty stack has no top"};
  return top_->value;
}

template <std::movable T>
auto Stack<T>::is_empty() const noexcept -> bool {
  return top_ == nullptr;
}

#endif // STACK_LINKED_HPP
