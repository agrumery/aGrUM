#ifndef GUM_TEST_MYALLOC_H
#define GUM_TEST_MYALLOC_H

#include <vector>

namespace gum_tests {

  struct MyAllocCount {
    static unsigned int allocs;
    static unsigned int deallocs;

    static unsigned int nbAllocations() noexcept { return allocs; }

    static unsigned int nbDeallocations() noexcept { return deallocs; }

    static bool hasMeroryLeak() noexcept { return allocs != MyAllocCount::deallocs; }
  };

  unsigned int MyAllocCount::allocs{0};
  unsigned int MyAllocCount::deallocs{0};

  template <typename T> class MyAlloc : public MyAllocCount {
    public:
    using value_type = T;

    template <typename _Tp1> struct rebind { typedef MyAlloc<_Tp1> other; };

    MyAlloc() noexcept {}

    template <typename U> MyAlloc(const MyAlloc<U> &) noexcept {}

    T *allocate(std::size_t num) {
      ++MyAllocCount::allocs;
      return static_cast<T *>(::operator new(num * sizeof(T)));
    }

    void deallocate(T *p, std::size_t num) {
      ++MyAllocCount::deallocs;
      ::operator delete(p);
    }

    template <typename... Args> void construct(T *p, Args... args) {
      ::new (p) T(std::forward<Args>(args)...);
    }

    void destroy(T *p) { p->~T(); }
  };

} /* namespace gum_tests */

#endif /* GUM_TEST_MYALLOC_H */
