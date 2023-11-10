#ifndef COLONY_HPP
#define COLONY_HPP

#include <memory>
#include <utility>

template <std::movable T>
class Colony {
public:
  Colony(std::size_t capacity = 4);
  ~Colony() noexcept;

  auto insert(T) -> T*;
  void remove(T*) noexcept;
  auto search(const std::predicate<const T&> auto&)
    const noexcept -> T*;
  [[nodiscard]] auto is_empty() const noexcept -> bool;

private:
  struct Node {
    Node(Node*, Node*, Node*) noexcept;
    Node(T, Node*, Node*);
    ~Node() noexcept {}

    union {
      T value;
      Node* last_removed;
    };

    Node* previous;
    Node* next;
  };

  class Bucket : private std::allocator<Node> {
  public:
    explicit Bucket(std::size_t capacity);
    explicit Bucket(Bucket* previous);
    ~Bucket() noexcept;

    auto full() const noexcept -> bool;
    auto insert(T, Node*, Node*) -> Node*;

  private:
    std::size_t capacity_;
    Bucket* previous = nullptr;
    std::size_t size_ = 0;
    Node* nodes_ = this->allocate(capacity_);
  };

  auto insert_at_end(T) -> T*;
  auto insert_at_last_removed(T) -> T*;

  Bucket* last_bucket_;
  Node head_ = {nullptr, &head_, &head_};
};

template <std::movable T>
Colony<T>::Colony(std::size_t capacity)
  : last_bucket_{new Bucket(capacity)} {}

template <std::movable T>
Colony<T>::Node::Node(Node* last_removed, Node* previous,
                      Node* next) noexcept
  : last_removed{last_removed},
    next{next},
    previous{previous} {}

template <std::movable T>
Colony<T>::Node::Node(T value, Node* previous,
                      Node* next)
  : value{std::move(value)},
    next{next},
    previous{previous} {}

template <std::movable T>
Colony<T>::Bucket::Bucket(std::size_t capacity)
  : capacity_{capacity < 4 ? 4 : capacity} {}

template <std::movable T>
Colony<T>::Bucket::Bucket(Bucket* previous)
  : capacity_{2 * previous->capacity_},
    previous{std::move(previous)} {}

template <std::movable T>
Colony<T>::Bucket::~Bucket() noexcept {
  delete previous;
  this->deallocate(nodes_, capacity_);
}

template <std::movable T>
auto Colony<T>::Bucket::full() const noexcept -> bool {
  return size_ == capacity_;
}

template <std::movable T>
auto Colony<T>::Bucket::insert(T value, Node* prev,
                               Node* next) -> Node* {
  return new (nodes_ + size_++)
    Node(std::move(value), prev, next);
}

template <std::movable T>
Colony<T>::~Colony() noexcept {
  for (auto node = head_.next; node != &head_;)
    std::destroy_at(
      &std::exchange(node, node->next)->value);
  delete last_bucket_;
}

template <std::movable T>
auto Colony<T>::insert(T value) -> T* {
  if (head_.last_removed)
    return insert_at_last_removed(std::move(value));
  return insert_at_end(std::move(value));
}

template <std::movable T>
void Colony<T>::remove(T* ptr) noexcept {
  Node* to_be_removed = reinterpret_cast<Node*>(ptr);
  auto previous = to_be_removed->previous;

  std::destroy_at(&to_be_removed->value);
  to_be_removed->last_removed = head_.last_removed;

  previous->next = to_be_removed->next;
  previous->next->previous = previous;
  head_.last_removed = to_be_removed;
}

template <std::movable T>
auto Colony<T>::search(
  const std::predicate<const T&> auto& f) const noexcept
  -> T* {
  for (auto node = head_.next; node != &head_;
       node = node->next)
    if (f(static_cast<const T&>(node->value)))
      return &node->value;
  return nullptr;
}

template <std::movable T>
auto Colony<T>::is_empty() const noexcept -> bool {
  return head_.next == &head_;
}

template <std::movable T>
auto Colony<T>::insert_at_end(T value) -> T* {
  if (last_bucket_->full())
    last_bucket_ = new Bucket(std::move(last_bucket_));

  auto* node = last_bucket_->insert(std::move(value),
                                    head_.previous, &head_);

  head_.previous->next = node;
  head_.previous = node;

  return &node->value;
}

template <std::movable T>
auto Colony<T>::insert_at_last_removed(T value)
  -> T* {
  auto node = head_.last_removed;
  auto next_free = node->last_removed;

  new (node) Node(std::move(value), node->previous,
                  node->previous->next);

  node->previous->next = node;
  node->next->previous = node;

  head_.last_removed = next_free;

  return &node->value;
}

#endif // COLONY_HPP
