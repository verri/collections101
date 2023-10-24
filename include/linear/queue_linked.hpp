#ifndef QUEUE_LINKED_HPP
#define QUEUE_LINKED_HPP

#include <stdexcept>
#include <utility>

template <std::movable T>
class Queue {
public:
  ~Queue() noexcept;

  void enqueue(T);
  void dequeue();
  [[nodiscard]] auto front() const -> const T&;
  [[nodiscard]] auto is_empty() const noexcept -> bool;

private:
  struct Node {
    T value;
    Node* next;
  };

  Node* front_ = nullptr;
  Node* rear_ = nullptr;
};

template <std::movable T>
Queue<T>::~Queue() noexcept {
  while (front_)
    delete std::exchange(front_, front_->next);
}

template <std::movable T>
void Queue<T>::enqueue(T value) {
  auto node = new Node{std::move(value), nullptr};
  if (is_empty())
    front_ = rear_ = node;
  else
    rear_ = rear_->next = node;
}

template <std::movable T>
void Queue<T>::dequeue() {
  if (is_empty())
    throw std::runtime_error{"no element to dequeue"};
  delete std::exchange(front_, front_->next);
}

template <std::movable T>
auto Queue<T>::front() const -> const T& {
  if (is_empty())
    throw std::runtime_error{"empty queue has no front"};
  return front_->value;
}

template <std::movable T>
auto Queue<T>::is_empty() const noexcept -> bool {
  return front_ == nullptr;
}

#endif // QUEUE_LINKED_HPP
