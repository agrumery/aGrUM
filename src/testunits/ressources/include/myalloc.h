#ifndef GUM_TEST_MYALLOC_H
#define GUM_TEST_MYALLOC_H

#include <vector>

namespace gum_tests {

  struct MyAllocCount {
    static std::size_t allocs;
    static std::size_t deallocs;

    static std::size_t nbAllocations() noexcept { return allocs; }

    static std::size_t nbDeallocations() noexcept { return deallocs; }

    static bool hasMeroryLeak() noexcept {
      return allocs != MyAllocCount::deallocs;
    }
  };

  std::size_t MyAllocCount::allocs{0};
  std::size_t MyAllocCount::deallocs{0};

  template <typename T>
  class MyAlloc : public MyAllocCount {
    public:
    using value_type = T;

    template <typename _Tp1>
    struct rebind {
      typedef MyAlloc<_Tp1> other;
    };

    MyAlloc() noexcept {}

    template <typename U>
    MyAlloc( const MyAlloc<U>& ) noexcept {}

    bool operator== ( const MyAlloc<T>& ) const { return true; }

    bool operator!= ( const MyAlloc<T>& ) const { return false; }

    T* allocate( std::size_t num ) {
      MyAllocCount::allocs += num;
      return static_cast<T*>(::operator new( num * sizeof( T ) ) );
    }

    void deallocate( T* p, std::size_t num ) {
      MyAllocCount::deallocs += num;
      ::operator delete( p );
    }

    template <typename... Args>
    void construct( T* p, Args&&... args ) {
      ::new ( (void*) p ) T( std::forward<Args>( args )... );
    }

    void destroy( T* p ) { p->~T(); }
  };

} /* namespace gum_tests */

#endif /* GUM_TEST_MYALLOC_H */
