/// TODO

#pragma once

namespace rc
{
  template <typename T>
  class Strong;

  template <typename T>
  class Weak
  {
    RCObject<T> *rc;

    void clear()
    {
      if (rc)
        rc->release_weak();
      rc = nullptr;
    }

    void acquire()
    {
      if (rc)
        rc->acquire_weak();
    }

  public:
    Weak(RCObject<T> *rc) : rc(rc) { acquire(); }
    Weak(Weak &&other) : rc(other.rc) { other.rc = nullptr; }
    Weak(const Weak &other) : rc(other.rc) { acquire(); }
    ~Weak()
    {
      clear();
    }

    Weak &operator=(Weak &&other)
    {
      clear();
      rc = other.rc;
      other.rc = nullptr;
      return *this;
    }

    Weak &operator=(const Weak &other)
    {
      clear();
      rc = other.rc;
      acquire();
      return *this;
    }

    Strong<T> upgrade();

    operator bool() const { return rc != nullptr; }
    bool operator==(const Weak &other) const { return rc == other.rc; }
  };
} // namespace rc
