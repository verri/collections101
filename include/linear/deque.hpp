#ifndef DEQUE_DOUBLE_LINKED_CIRCULAR_HPP
#define DEQUE_DOUBLE_LINKED_CIRCULAR_HPP

#include <memory>
#include <utility>

template <std::movable T>
class Deque {
public:
  ~Deque() noexcept;

  void insert_front(T);
  void insert_rear(T);

  void remove_front();
  void remove_rear();

  auto front() const -> const T&;
  auto rear() const -> const T&;

  auto is_empty() const noexcept -> bool;

private:
  struct Node {
    ~Node() noexcept {}

    Node* previous;
    Node* next;
    union {
      struct {} empty;
      T value;
    };

    static auto make(T, Node*, Node*) -> Node*;
    static void erase(Node*) noexcept;
  };

  Node head_ = { .previous = &head_, .next = &head_, .empty = {}};
};

template <std::movable T>
Deque<T>::~Deque() noexcept {
  for (auto p = head_.next; p != &head_;)
    Node::erase(std::exchange(p, p->next));
}

template <std::movable T>
void Deque<T>::insert_front(T value) {
  head_.next =
    Node::make(std::move(value), &head_, head_.next);
  head_.next->next->previous = head_.next;
}

template <std::movable T>
void Deque<T>::insert_rear(T value) {
  head_.previous =
    Node::make(std::move(value), head_.previous, &head_);
  head_.previous->previous->next = head_.previous;
}

template <std::movable T>
void Deque<T>::remove_front() {
  if (is_empty())
    throw std::runtime_error{"empty deque has no front"};
  head_.next->next->previous = &head_;
  Node::erase(std::exchange(head_.next, head_.next->next));
}

template <std::movable T>
void Deque<T>::remove_rear() {
  if (is_empty())
    throw std::runtime_error{"empty deque has no rear"};
  head_.previous->previous->next = &head_;
  Node::erase(std::exchange(head_.previous,
                            head_.previous->previous));
}

template <std::movable T>
auto Deque<T>::front() const -> const T& {
  if (is_empty())
    throw std::runtime_error{"empty deque has no front"};
  return head_.next->value;
}

template <std::movable T>
auto Deque<T>::rear() const -> const T& {
  if (is_empty())
    throw std::runtime_error{"empty deque has no rear"};
  return head_.previous->value;
}

template <std::movable T>
auto Deque<T>::is_empty() const noexcept -> bool {
  return head_.next == &head_;
}

template <std::movable T>
auto Deque<T>::Node::make(T value, Node* prev, Node* next)
  -> Node* {
  return new Node{
    .previous = prev,
    .next = next,
    .value = std::move(value)};
}

template <std::movable T>
void Deque<T>::Node::erase(Node* node) noexcept {
  node->value.~T();
  delete node;
}

#endif // DEQUE_DOUBLE_LINKED_CIRCULAR_HPP
