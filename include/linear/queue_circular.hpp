#ifndef QUEUE_CIRCULAR_HPP
#define QUEUE_CIRCULAR_HPP

#include <memory>
#include <stdexcept>

template <std::movable T>
class Queue : private std::allocator<T> {
public:
  ~Queue() noexcept;

  void enqueue(T);
  void dequeue();
  [[nodiscard]] auto front() const -> const T&;
  [[nodiscard]] auto is_empty() const noexcept -> bool;

private:
  void double_capacity();
  auto wrap(std::size_t) const noexcept -> std::size_t;

  std::size_t begin_ = 0;
  std::size_t size_ = 0;
  std::size_t capacity_ = 0;
  T* values_ = nullptr;
};

template <std::movable T>
Queue<T>::~Queue() noexcept {
  for (std::size_t i = 0; i < size_; ++i)
    values_[wrap(begin_ + i)].~T();
  this->deallocate(values_, capacity_);
}

template <std::movable T>
void Queue<T>::enqueue(T value) {
  if (size_ == capacity_)
    double_capacity();
  new (values_ + wrap(begin_ + size_)) T(std::move(value));
  ++size_;
}

template <std::movable T>
void Queue<T>::dequeue() {
  if (is_empty())
    throw std::runtime_error{"no element to dequeue"};
  values_[begin_].~T();
  begin_ = wrap(begin_ + 1);
  --size_;
}

template <std::movable T>
auto Queue<T>::front() const -> const T& {
  if (is_empty())
    throw std::runtime_error{"empty queue has no front"};
  return values_[begin_];
}

template <std::movable T>
auto Queue<T>::is_empty() const noexcept -> bool {
  return size_ == 0;
}

template <std::movable T>
void Queue<T>::double_capacity() {
  auto new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
  auto buffer = this->allocate(new_capacity);

  std::size_t i;
  try {
    for (i = 0; i < size_; ++i)
      new (buffer + i)
        T(std::move_if_noexcept(values_[wrap(begin_ + i)]));
  } catch (...) {
    std::destroy_n(buffer, i);
    this->deallocate(buffer, new_capacity);
    throw;
  }

  for (i = 0; i < size_; ++i)
    values_[wrap(begin_ + i)].~T();
  this->deallocate(values_, capacity_);

  begin_ = 0;
  values_ = buffer;
  capacity_ = new_capacity;
}

template <std::movable T>
auto Queue<T>::wrap(std::size_t i) const noexcept
  -> std::size_t {
  return i % capacity_;
}

#endif // QUEUE_CIRCULAR_HPP
