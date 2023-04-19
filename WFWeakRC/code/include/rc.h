////

#pragma once
// Include the RCObject class.
#include "rcobjectclassic.h"
#include "rcobjectwfopt.h"
#include "rcobjectwf.h"
// Hack namespace to include the correct RCObject class.
#ifdef CLASSIC
#  define rc rcclassic
#elif defined(OPTIMISED)
#  define rc rcwfopt
#else
#  define rc rcwf
#endif
// Include smart pointers.
#include "weak.h"
#include "strong.h"

#include <new>

namespace rc
{
  template <typename T>
  Weak<T> Strong<T>::weak() const
  {
    return Weak<T>(rc);
  }

  template <typename T>
  Strong<T> Weak<T>::upgrade()
  {
    if (rc->acquire_strong_from_weak())
      return Strong<T>(rc);
    return Strong<T>(nullptr);
  }

  template <typename T, typename... Args>
  Strong<T> make_rc(Args &&...args)
  {
    // Call malloc as uses free internally.
    auto p = malloc(sizeof(RCObject<T>));
    return Strong<T>(new (p) RCObject<T>(std::forward<Args>(args)...));
  }
} // namespace rc