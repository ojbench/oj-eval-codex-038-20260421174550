// Problem 2609: vector++
// Provide sjtu::vector implementation only; OJ supplies its own main and tests.

#include <cstddef>
#include <new>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <cstring>

namespace sjtu {

template <class T>
class vector {
  T* data_ = nullptr;
  std::size_t size_ = 0;
  std::size_t cap_ = 0;

  std::size_t next_cap(std::size_t need) const {
    if (need <= 1) return 1;
    std::size_t n = cap_ ? cap_ : 1;
    while (n < need) n <<= 1; // 2x growth
    return n;
  }

  void reallocate(std::size_t new_cap) {
    T* nd = static_cast<T*>(::operator new(sizeof(T) * new_cap));
    if constexpr (std::is_trivially_copyable<T>::value) {
      if (size_) std::memcpy(nd, data_, sizeof(T) * size_);
      // trivially destructible: no-op
      ::operator delete(static_cast<void*>(data_));
    } else {
      for (std::size_t i = 0; i < size_; ++i) {
        new (nd + i) T(std::move(data_[i]));
        data_[i].~T();
      }
      ::operator delete(static_cast<void*>(data_));
    }
    data_ = nd;
    cap_ = new_cap;
  }

public:
  // ctors / dtor
  vector() = default;
  explicit vector(std::size_t n) : size_(n), cap_(n) {
    if (cap_) {
      data_ = static_cast<T*>(::operator new(sizeof(T) * cap_));
      for (std::size_t i = 0; i < size_; ++i) new (data_ + i) T();
    }
  }
  vector(std::initializer_list<T> il) : size_(il.size()), cap_(il.size()) {
    if (cap_) {
      data_ = static_cast<T*>(::operator new(sizeof(T) * cap_));
      std::size_t i = 0; for (const auto &x : il) new (data_ + i++) T(x);
    }
  }
  vector(const vector& o) : size_(o.size_), cap_(o.size_) {
    if (cap_) {
      data_ = static_cast<T*>(::operator new(sizeof(T) * cap_));
      if constexpr (std::is_trivially_copyable<T>::value) {
        if (size_) std::memcpy(data_, o.data_, sizeof(T) * size_);
      } else {
        for (std::size_t i = 0; i < size_; ++i) new (data_ + i) T(o.data_[i]);
      }
    }
  }
  vector(vector&& o) noexcept : data_(o.data_), size_(o.size_), cap_(o.cap_) {
    o.data_ = nullptr; o.size_ = 0; o.cap_ = 0;
  }
  ~vector() {
    clear();
    ::operator delete(static_cast<void*>(data_));
  }

  // assignment
  vector& operator=(const vector& o) {
    if (this == &o) return *this;
    // allocate new buffer
    T* nd = nullptr;
    if (o.size_) nd = static_cast<T*>(::operator new(sizeof(T) * o.size_));
    if constexpr (std::is_trivially_copyable<T>::value) {
      if (o.size_) std::memcpy(nd, o.data_, sizeof(T) * o.size_);
    } else {
      for (std::size_t i = 0; i < o.size_; ++i) new (nd + i) T(o.data_[i]);
    }
    // destroy old
    clear();
    ::operator delete(static_cast<void*>(data_));
    // take new
    data_ = nd; size_ = o.size_; cap_ = o.size_;
    return *this;
  }
  vector& operator=(vector&& o) noexcept {
    if (this == &o) return *this;
    // destroy old
    clear();
    ::operator delete(static_cast<void*>(data_));
    // take ownership
    data_ = o.data_; size_ = o.size_; cap_ = o.cap_;
    o.data_ = nullptr; o.size_ = 0; o.cap_ = 0;
    return *this;
  }
  void swap(vector& o) noexcept {
    using std::swap;
    swap(data_, o.data_);
    swap(size_, o.size_);
    swap(cap_, o.cap_);
  }

  // capacity
  std::size_t size() const noexcept { return size_; }
  std::size_t capacity() const noexcept { return cap_; }
  bool empty() const noexcept { return size_ == 0; }
  void reserve(std::size_t n) {
    if (n > cap_) reallocate(n);
  }
  void resize(std::size_t n) {
    if (n > cap_) reallocate(next_cap(n));
    if (n > size_) {
      for (std::size_t i = size_; i < n; ++i) new (data_ + i) T();
    } else {
      if constexpr (!std::is_trivially_destructible<T>::value) {
        for (std::size_t i = n; i < size_; ++i) data_[i].~T();
      }
    }
    size_ = n;
  }
  void clear() noexcept {
    if constexpr (!std::is_trivially_destructible<T>::value) {
      for (std::size_t i = 0; i < size_; ++i) data_[i].~T();
    }
    size_ = 0;
  }

  // element access
  T& operator[](std::size_t i) noexcept { return data_[i]; }
  const T& operator[](std::size_t i) const noexcept { return data_[i]; }
  T* data() noexcept { return data_; }
  const T* data() const noexcept { return data_; }

  // modifiers
  void push_back(const T& v) {
    if (size_ == cap_) reallocate(next_cap(size_ + 1));
    new (data_ + size_) T(v);
    ++size_;
  }
  void push_back(T&& v) {
    if (size_ == cap_) reallocate(next_cap(size_ + 1));
    new (data_ + size_) T(std::move(v));
    ++size_;
  }
  template <class... Args>
  T& emplace_back(Args&&... args) {
    if (size_ == cap_) reallocate(next_cap(size_ + 1));
    new (data_ + size_) T(std::forward<Args>(args)...);
    return data_[size_++];
  }
  void pop_back() {
    if constexpr (!std::is_trivially_destructible<T>::value) {
      data_[size_ - 1].~T();
    }
    --size_;
  }
};

} // namespace sjtu
