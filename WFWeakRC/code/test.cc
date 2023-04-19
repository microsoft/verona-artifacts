#include <rc.h>

#include <cassert>
#include <cstddef>
#include <chrono>
#include <vector>
#include <thread>
#include <snmalloc/pal/pal.h>

#ifdef REPEATS
static constexpr size_t Repeats = REPEATS;
#else
static constexpr size_t Repeats = 2;
#endif

#ifdef CORES
static constexpr size_t Cores = CORES;
#else
static constexpr size_t Cores = 4;
#endif

namespace rc
{

  class Benchmark
  {
    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::high_resolution_clock::time_point t1;
    std::string name;
    size_t iteration;

  public:
    Benchmark(std::string name, size_t iteration) : name(name), iteration(iteration)
    {
      t0 = std::chrono::high_resolution_clock::now();
    }

    ~Benchmark()
    {
      t1 = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
      std::cout << Flavour << "," << name << "," << 1 << "," << duration << "," << iteration << std::endl;
    }
  };

  class ParallelBenchmark
  {
    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::high_resolution_clock::time_point t1;
    std::string name;
    std::string flavour;
    size_t iteration;

    std::atomic<size_t> ready_countdown;
    std::atomic<size_t> done_countdown;

  public:
    ParallelBenchmark(std::string name, size_t count, std::string flavour, size_t iteration)
        : name(name), flavour(flavour), iteration(iteration), ready_countdown(count), done_countdown(count)
    {
    }

    template <typename F>
    void run(F f)
    {
      std::vector<std::thread> threads;
      size_t count = ready_countdown;
      for (size_t i = 0; i < count; ++i)
      {
        threads.push_back(
            std::thread([this, f, i]() mutable
                        {
                          // Ensure an allocator is initialised before we hit this path.
                          void* p = malloc(1024);
                          if (--ready_countdown == 0)
                            t0 = std::chrono::high_resolution_clock::now();
                          while (ready_countdown != 0)
                          {
                          }
                          f(i);
                          if (--done_countdown == 0)
                            t1 = std::chrono::high_resolution_clock::now();
                          free(p); }));
      }

      for (auto &t : threads)
        t.join();

      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
      std::cout << flavour << "," << name << "," << count << "," << duration << "," << iteration << std::endl;
    }
  };

  void correctness()
  {
    auto s = make_rc<int>(42);
    auto w = s.weak();
    {
      auto s2 = s;
      auto w2 = w;
      auto s3 = w.upgrade();
      assert(s2 == s3);
    }
    s = nullptr;
    auto s4 = w.upgrade();
    assert(!s4);
  }

  // Check for double destructor calls
  struct A
  {
    static inline size_t count = 0;
    ~A()
    {
      count++;
    }
  };

  void correctness2()
  {
    {
      auto a = make_rc<A>();
      if (A::count != 0)
      {
        std::cout << "A::count = " << A::count << std::endl;
        abort();
      }
    }
    if (A::count != 1)
    {
      std::cout << "A::count = " << A::count << std::endl;
      abort();
    }
  }

  void perf_strong()
  {
    for (size_t n = 1; n <= Cores; n++)
    {
      for (size_t i = 0; i < Repeats; ++i)
      {
        ParallelBenchmark b("strong", n, rc::Flavour, i);

        b.run([](size_t) mutable
              {
                for (size_t i = 0; i < 1000000; ++i)
                {
                    make_rc<int>(42);
                } });
      }
    }
  }

  void perf_opt_bad()
  {
    for (size_t i = 0; i < Repeats; ++i)
    {
      Benchmark b("opt_bad", i);

      for (size_t i = 0; i < 1000000; ++i)
      {
        auto s = make_rc<int>(42);
        auto w = s.weak();
      }
    }
  }

  void perf_upgrade()
  {
    auto s = make_rc<int>(42);
    auto w = s.weak();

    for (size_t n = 1; n <= Cores; n++)
    {
      for (size_t i = 0; i < Repeats; ++i)
      {
        ParallelBenchmark b("upgrade", n, rc::Flavour, i);

        b.run([w, n](size_t) mutable
              {
                auto count = 1000000 / n;
                for (size_t i = 0; i < count; ++i)
                {
                    w.upgrade();
                } });
      }
    }
  }

  void perf_one()
  {
    auto s = make_rc<int>(42);
    auto w = s.weak();
    for (size_t n = 1; n*n <= Cores; n++)
    {
      for (size_t j = 0; j < Repeats * 10; ++j)
      {
        ParallelBenchmark b("upgrade_one", n*n, rc::Flavour, j);
        std::atomic<bool> stop{false};
        b.run([w, j, n, &stop](size_t tid) mutable
              {
                auto count = 1000;
              std::vector <size_t> v(count);
              size_t i = 0;
              while (!stop)
              {
                  auto start = snmalloc::Aal::benchmark_time_start();          
                  {
                      w.upgrade();
                  }
                  if (tid == 0)
                  {
                    if (i == count)
                    {
                      stop = true;
                    }
                    else
                    {
                      v[i] = snmalloc::Aal::benchmark_time_end() - start;
                    }
                    i++;
                  }
              } 
              if (tid == 0)
                for (size_t k = 0; k < count; ++k)
                {
                  printf("%s,one,%zu,%zu,%zu\n", rc::Flavour.c_str(), n*n, v[k], j);
                } });
      }
    }
  }

  __attribute__((noinline)) void *malloc_noinline(size_t size)
  {
    return malloc(size);
  }

  void perf_malloc()
  {
    for (size_t n = 1; n <= Cores; n++)
    {
      for (size_t i = 0; i < Repeats; ++i)
      {
        ParallelBenchmark b("strong", n, "Allocation-only", i);

        b.run([](size_t) mutable
              {
                for (size_t i = 0; i < 1000000; ++i)
                {
                    free(malloc_noinline(32));
                } });
      }
    }
  }

  int test()
  {
    correctness();
    correctness2();
    perf_opt_bad();
    perf_strong();
    perf_upgrade();
    perf_one();
    if (rc::Flavour == "Classic")
      perf_malloc();
    return 0;
  }
} // namespace rc