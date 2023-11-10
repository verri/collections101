#ifndef STACK_CONTIGUOUS_HPP
#define STACK_CONTIGUOUS_HPP

#include <memory>
#include <stdexcept>

template <std::movable T>
class Stack : private std::allocator<T> {
public:
  ~Stack() noexcept;

  void push(T);
  void pop();
  [[nodiscard]] auto top() const -> const T&;
  [[nodiscard]] auto is_empty() const noexcept -> bool;

private:
  void double_capacity();

  T* values_ = nullptr;
  std::size_t count_ = 0;
  std::size_t capacity_ = 0;
};

template <std::movable T>
Stack<T>::~Stack() noexcept {
  std::destroy_n(values_, count_);
  this->deallocate(values_, capacity_);
}

template <std::movable T>
void Stack<T>::push(T value) {
  if (count_ == capacity_)
    double_capacity();
  new (values_ + count_) T(std::move(value));
  ++count_;
}

template <std::movable T>
void Stack<T>::pop() {
  if (is_empty())
    throw std::runtime_error{"no element to pop"};
  --count_;
  values_[count_].~T();
}

template <std::movable T>
auto Stack<T>::top() const -> const T& {
  if (is_empty())
    throw std::runtime_error{"empty stack has no top"};
  return values_[count_ - 1];
}

template <std::movable T>
auto Stack<T>::is_empty() const noexcept -> bool {
  return count_ == 0;
}

template <std::movable T>
void Stack<T>::double_capacity() {
  auto new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
  auto buffer = this->allocate(new_capacity);

  if constexpr (std::is_nothrow_move_constructible_v<T>)
    std::uninitialized_move_n(values_, count_, buffer);
  else try {
    std::uninitialized_copy_n(values_, count_, buffer);
  } catch (...) {
    this->deallocate(buffer, new_capacity);
    throw;
  }

  std::destroy_n(values_, count_);
  this->deallocate(values_, capacity_);

  values_ = buffer;
  capacity_ = new_capacity;
}

#endif // STACK_CONTIGUOUS_HPP
