/// TODO

#pragma once

namespace rc
{
  template <typename T>
  class Weak;

  template <typename T>
  class Strong
  {
    RCObject<T> *rc;

    void clear()
    {
      if (rc)
        rc->release_strong();
      rc = nullptr;
    }

    void acquire()
    {
      if (rc)
        rc->acquire_strong();
    }

  public:
    Strong(RCObject<T> *rc) : rc(rc) {}
    Strong(Strong &&other) : rc(other.rc) { other.rc = nullptr; }
    Strong(const Strong &other) : rc(other.rc) { acquire(); }
    ~Strong()
    {
      clear();
    }

    Strong &operator=(Strong &&other)
    {
      clear();
      rc = other.rc;
      other.rc = nullptr;
      return *this;
    }

    Strong &operator=(const Strong &other)
    {
      clear();
      rc = other.rc;
      acquire();
      return *this;
    }

    operator bool() const { return rc != nullptr; }
    bool operator==(const Strong &other) const { return rc == other.rc; }

    Weak<T> weak() const;
    T *operator->() { return &rc->body; }
    T &operator*() { return rc->body; }
  };
} // namespace rc