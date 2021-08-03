#ifndef GUM_TEST_COUNTEDALLOC_H
#define GUM_TEST_COUNTEDALLOC_H

#include <vector>

namespace gum_tests {

  struct CountedAlloc {
    static std::size_t allocs;
    static std::size_t deallocs;

    static std::size_t nbAllocations() noexcept { return allocs; }

    static std::size_t nbDeallocations() noexcept { return deallocs; }

    static bool hasMemoryLeak() noexcept {
      return allocs != CountedAlloc::deallocs;
    }
  };

  std::size_t CountedAlloc::allocs{0};
  std::size_t CountedAlloc::deallocs{0};

  template <typename T>
  class DebugCountedAlloc : public CountedAlloc {
    public:
    using value_type = T;

    template <typename _Tp1>
    struct rebind {
      typedef DebugCountedAlloc<_Tp1> other;
    };

    DebugCountedAlloc() noexcept {}

    template <typename U>
    DebugCountedAlloc( const DebugCountedAlloc<U>& ) noexcept {}

    bool operator== ( const DebugCountedAlloc<T>& ) const { return true; }

    bool operator!= ( const DebugCountedAlloc<T>& ) const { return false; }

    T* allocate( std::size_t num ) {
      CountedAlloc::allocs += num;
      return static_cast<T*>(::operator new( num * sizeof( T ) ) );
    }

    void deallocate( T* p, std::size_t num ) {
      CountedAlloc::deallocs += num;
      ::operator delete( p );
    }

    template <typename... Args>
    void construct( T* p, Args&&... args ) {
      ::new ( (void*) p ) T( std::forward<Args>( args )... );
    }

    void destroy( T* p ) { p->~T(); }
  };

} /* namespace gum_tests */

#endif /* GUM_TEST_COUNTEDALLOC_H */
