// Problem 2609: vector++
// Implements a lightweight dynamic array (vector-like) and a CLI
// that supports typical operations to match performance testing.

#include <bits/stdc++.h>
using namespace std;

template <class T>
class fast_vector {
  T* data_ = nullptr;
  size_t size_ = 0;
  size_t cap_ = 0;

  size_t next_cap(size_t need) const {
    // Growth strategy: 1.5x to balance realloc cost and memory
    if (need <= 1) return 1;
    size_t n = cap_ ? cap_ : 1;
    while (n < need) n = n + (n >> 1); // 1.5x
    return n;
  }

  void reallocate(size_t new_cap) {
    T* nd = (T*)::operator new(sizeof(T) * new_cap);
    // Move-construct where possible
    for (size_t i = 0; i < size_; ++i) {
      new (nd + i) T(std::move(data_[i]));
      data_[i].~T();
    }
    ::operator delete((void*)data_);
    data_ = nd;
    cap_ = new_cap;
  }

public:
  fast_vector() = default;
  ~fast_vector() {
    clear();
    ::operator delete((void*)data_);
  }

  fast_vector(const fast_vector& o) : size_(o.size_), cap_(o.size_) {
    if (cap_) {
      data_ = (T*)::operator new(sizeof(T) * cap_);
      for (size_t i = 0; i < size_; ++i) new (data_ + i) T(o.data_[i]);
    }
  }

  fast_vector(fast_vector&& o) noexcept : data_(o.data_), size_(o.size_), cap_(o.cap_) {
    o.data_ = nullptr; o.size_ = 0; o.cap_ = 0;
  }

  fast_vector& operator=(fast_vector o) {
    swap(data_, o.data_);
    swap(size_, o.size_);
    swap(cap_, o.cap_);
    return *this;
  }

  void reserve(size_t n) {
    if (n > cap_) reallocate(n);
  }
  void resize(size_t n) {
    if (n > cap_) reallocate(next_cap(n));
    if (n > size_) {
      for (size_t i = size_; i < n; ++i) new (data_ + i) T();
    } else {
      for (size_t i = n; i < size_; ++i) data_[i].~T();
    }
    size_ = n;
  }
  void clear() {
    for (size_t i = 0; i < size_; ++i) data_[i].~T();
    size_ = 0;
  }
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
  void pop_back() {
    data_[size_ - 1].~T();
    --size_;
  }
  size_t size() const { return size_; }
  size_t capacity() const { return cap_; }
  T& operator[](size_t i) { return data_[i]; }
  const T& operator[](size_t i) const { return data_[i]; }
};

// Simple command-driven harness to be compatible with generic testing.
// Format:
// n
// commands...
// Commands: push x, pop, set i x, get i, size, reserve n, resize n
// Outputs are printed for get/size only.

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  fast_vector<long long> v;
  int q;
  if (!(cin >> q)) return 0;
  string cmd;
  for (int i = 0; i < q; ++i) {
    cin >> cmd;
    if (cmd == "push") {
      long long x; cin >> x; v.push_back(x);
      cout.flush();
    } else if (cmd == "pop") {
      if (v.size()) v.pop_back();
      cout.flush();
    } else if (cmd == "set") {
      size_t idx; long long x; cin >> idx >> x; if (idx < v.size()) v[idx] = x;
      cout.flush();
    } else if (cmd == "get") {
      size_t idx; cin >> idx; if (idx < v.size()) cout << v[idx] << '\n'; else cout << "-1\n"; cout.flush();
    } else if (cmd == "size") {
      cout << v.size() << '\n';
      cout.flush();
    } else if (cmd == "reserve") {
      size_t n; cin >> n; v.reserve(n);
      cout.flush();
    } else if (cmd == "resize") {
      size_t n; cin >> n; v.resize(n);
      cout.flush();
    } else if (cmd == "sum") {
      long long s = 0; for (size_t j = 0; j < v.size(); ++j) s += v[j]; cout << s << '\n'; cout.flush();
    }
  }
  return 0;
}
