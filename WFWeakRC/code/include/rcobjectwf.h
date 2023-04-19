/// TODO
#pragma once

#include <atomic>
#include <iostream>

namespace rcwf
{
  template <typename T>
  class RCWaitFree
  {
    static constexpr size_t SINGLE_RC = 2;
    static constexpr size_t CLOSED = 1;
    std::atomic<size_t> strong{SINGLE_RC};
    std::atomic<size_t> weak{1};
    T body;

  public:
    template <typename... Args>
    RCWaitFree(Args &&...args)
        : body(std::forward<Args>(args)...) {}

    void release_weak() {
      if ((weak == 1) || (--weak == 0)) /*Latex:\label{line:releaseweak:check}*/
        free(this);
    }

    void release_strong() {
      if (strong.fetch_sub(SINGLE_RC) != SINGLE_RC)
        return;
      // Implicitly holds a weak self-reference here. /*Latex:\label{line:have_weak}*/
      size_t old = 0;
      if (strong.compare_exchange_strong(old, CLOSED)) /*Latex:\label{line:release_strong:cas}*/
        body.~T();

      release_weak();
    }

    void acquire_weak() {
      ++weak;
    }

    void acquire_strong() {
      strong.fetch_add(SINGLE_RC);
    }

    bool acquire_strong_from_weak() {
      auto old = strong.fetch_add(SINGLE_RC);
      if ((old & CLOSED) != 0) /*Latex:\label{line:check:closed1}*/
        return false;          /*Latex:\label{line:check:closed2}*/

      if (old == 0) /*Latex:\label{line:upgrade1}*/
        // Blocked closing; restore implict weak
        // self-reference
        acquire_weak(); /*Latex:\label{line:upgrade2}*/

      return true;
    }
  };

  template <typename T>
  using RCObject = RCWaitFree<T>;
  static inline std::string Flavour = "Wait-free";
} // namespace rcwf