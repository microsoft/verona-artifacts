/// TODO
#pragma once

#include <atomic>
#include <iostream>

namespace rcclassic
{
  template <typename T>
  class RCObjectClassic
  {
    static constexpr size_t SINGLE_RC = 1;

    std::atomic<size_t> strong{SINGLE_RC};
    std::atomic<size_t> weak{1};
    T body;

  public:
    template <typename... Args>
    RCObjectClassic(Args &&...args)
        : body(std::forward<Args>(args)...) {}

    void release_weak()
    {
      if ((weak == 1) || (--weak == 0))
        free(this);
    }

    void release_strong()
    {
      if (strong.fetch_sub(SINGLE_RC) != SINGLE_RC)
        return;

      body.~T();

      release_weak();
    }

    void acquire_strong()
    {
      strong.fetch_add(SINGLE_RC);
    }

    void acquire_weak()
    {
      ++weak;
    }

    bool acquire_strong_from_weak()
    {
      auto curr = strong.load(std::memory_order_relaxed);

      while (curr != 0)
      {
        if (strong.compare_exchange_weak(curr, curr + SINGLE_RC))
          return true;
      }

      return false;
    }
  };

  template <typename T>
  using RCObject = RCObjectClassic<T>;
  static inline std::string Flavour = "Classic";
}