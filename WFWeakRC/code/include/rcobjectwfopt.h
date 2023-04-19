/// TODO
#pragma once

#include <atomic>
#include <iostream>

namespace rcwfopt
{
  template <typename T>
  class RCWaitFreeOpt
  {
    static constexpr size_t SINGLE_RC = 4;
    static constexpr size_t WEAK = 2;
    static constexpr size_t CLOSED = 1;
    std::atomic<size_t> strong{SINGLE_RC};
    std::atomic<size_t> weak{0};
    T body;

  public:
    template <typename... Args>
    RCWaitFreeOpt(Args &&...args)
        : body(std::forward<Args>(args)...) {}

    void release_weak()
    {
      if ((weak == 1) || (--weak == 0))
        free(this);
    }

    void release_strong() {
      auto old = strong.fetch_sub(SINGLE_RC);
      if (old > (SINGLE_RC + WEAK))
        return;

      if ((old & WEAK) == 0) {
        body.~T();
        free(this);
        return;
      }

      old = WEAK;
      if (strong.compare_exchange_strong(old, CLOSED))
        body.~T();
      release_weak();
    }

    void acquire_weak() {
      size_t old = weak;
      if (old == 0) {
        if (weak.compare_exchange_strong(old, 2)) {
          strong += WEAK;
          return;
        }
      }
      weak++;
    }

    void acquire_strong()
    {
      strong.fetch_add(SINGLE_RC);
    }

    bool acquire_strong_from_weak()
    {
      auto old = strong.fetch_add(SINGLE_RC);

      if ((old & CLOSED) != 0)
        return false;

      if (old < SINGLE_RC)
        acquire_weak();

      return true;
    }
  };

  template <typename T>
  using RCObject = RCWaitFreeOpt<T>;
  static inline std::string Flavour = "Wait-free (Opt)";
} // namespace rcwf