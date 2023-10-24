#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <stdexcept>
#include <utility>

template <std::movable T>
class List {
public:
  class Node {
    friend class List;

  public:
    auto value() noexcept -> T& { return value_; }
    auto next() const noexcept -> Node* { return next_; }

  private:
    Node() noexcept : empty_{} {}
    Node(Node* next, T value) noexcept;
    ~Node() noexcept {}

    static auto make(T, Node*) -> Node*;
    static void erase(Node*) noexcept;

    Node* next_ = nullptr;
    union {
      struct {} empty_;
      T value_;
    };
  };

  ~List() noexcept;

  [[nodiscard]] auto before_first() noexcept -> Node*;
  [[nodiscard]] auto last() noexcept -> Node*;

  auto search(const std::predicate<const T&> auto& f) const
    noexcept(noexcept(f(std::declval<const T&>())))
      -> Node*;

  auto insert_after(Node*, T) -> Node*;
  void remove_after(Node*);

  auto graft_after(Node*, List&) noexcept -> Node*;
  auto extract_between(Node*, Node*) noexcept -> List;

  auto concatenate(List&) noexcept -> Node*;
  auto split_after(Node*) noexcept -> List;

  [[nodiscard]] auto is_empty() const noexcept -> bool;

private:
  Node head_;
  Node* last_ = &head_;
};

template <std::movable T>
List<T>::Node::Node(Node* next, T value) noexcept
  : next_{next}, value_{std::move(value)} {}

template <std::movable T>
auto List<T>::Node::make(T value, Node* next) -> Node* {
  return new Node(next, std::move(value));
}

template <std::movable T>
void List<T>::Node::erase(Node* node) noexcept {
  node->value_.~T();
  delete node;
}

template <std::movable T>
List<T>::~List() noexcept {
  for (auto p = head_.next(); p != nullptr;)
    Node::erase(std::exchange(p, p->next()));
}

template <std::movable T>
auto List<T>::before_first() noexcept -> Node* {
  return &head_;
}

template <std::movable T>
auto List<T>::last() noexcept -> Node* {
  return last_;
}

template <std::movable T>
auto List<T>::search(
  const std::predicate<const T&> auto& f) const
  noexcept(noexcept(f(std::declval<const T&>()))) -> Node* {
  for (auto p = head_.next(); p != nullptr;)
    if (f(p->value()))
      return p;
  return nullptr;
}

template <std::movable T>
auto List<T>::insert_after(Node* prev, T value) -> Node* {
  auto node = prev->next_ =
    Node::make(std::move(value), prev->next_);

  if (node->next_ == nullptr)
    last_ = node;

  return node;
}

template <std::movable T>
void List<T>::remove_after(Node* prev) {
  if (prev->next_ == nullptr)
    throw std::runtime_error{"cannot remove past end"};

  delete std::exchange(prev->next_, prev->next_->next_);

  if (prev->next_ == nullptr)
    last_ = prev;
}

template <std::movable T>
auto List<T>::graft_after(Node* prev, List& list) noexcept
  -> Node* {
  if (list.is_empty())
    return prev;

  auto next = list.last_->next_ = prev->next_;
  prev->next_ = list.head_.next_;

  if (list.last_->next_ == nullptr)
    last_ = list.last_;

  list.head_.next_ = nullptr;
  list.last_ = &list.head_;

  return next ? next : last_;
}

// (previous, last]
template <std::movable T>
auto List<T>::extract_between(Node* prev,
                              Node* last) noexcept -> List {
  if (prev == last or prev->next_ == nullptr)
    return {};

  List list;

  list.head_.next_ = prev->next_;
  list.last_ = last;

  prev->next_ = last->next_;
  last->next_ = nullptr;

  if (prev->next_ == nullptr)
    last_ = prev;

  return list;
}

template <std::movable T>
auto List<T>::concatenate(List& list) noexcept -> Node* {
  if (list.is_empty())
    return last_;

  last_->next_ = list.head_.next_;
  last_ = list.last_;

  list.head_.next_ = nullptr;
  list.last_ = &list.head_;

  return last_;
}

template <std::movable T>
auto List<T>::split_after(Node* prev) noexcept -> List {
  if (prev->next_ == nullptr)
    return {};

  List list;

  list.head_.next_ = prev->next_;
  list.last_ = last_;

  prev->next_ = nullptr;
  last_ = prev;

  return list;
}

template <std::movable T>
auto List<T>::is_empty() const noexcept -> bool {
  return last_ == &head_;
}

#endif // LINKED_LIST_HPP
