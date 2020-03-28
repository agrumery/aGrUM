#ifndef GUM_TEST_LEARNING_ALLOC_H
#define GUM_TEST_LEARNING_ALLOC_H

#include <vector>
#include <utility>
#include <new>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>

namespace gum_tests {

  class MyPoolAlloc {
  public:
    MyPoolAlloc ( const std::size_t size )
      : __mempool ( new int[size / sizeof(int) + 1 ] )
      , __size ( size / sizeof(int) + 1 ) {
      //std::cout << "create a new pool alloc " << __mempool << std::endl;
    }
    
    MyPoolAlloc ( const MyPoolAlloc& ) = delete;
    MyPoolAlloc ( MyPoolAlloc&& ) = delete;

    ~MyPoolAlloc () {
      delete[] __mempool;
    }

    MyPoolAlloc& operator= ( const MyPoolAlloc& ) = delete;
    MyPoolAlloc& operator= ( MyPoolAlloc&& ) = delete;

    int* allocate ( const std::size_t nb_bytes ) {
      std::size_t nb_int = nb_bytes / sizeof(int);
      if ( nb_bytes % sizeof(int) ) ++nb_int;
      if ( __current_index + nb_int > __size )
        throw std::bad_alloc ();
      int* alloc_pointer = __mempool + __current_index;
      __allocations.push_back ( __current_index );
      __current_index += nb_int;

      //std::cout << "LearningAlloc allocated: " << alloc_pointer
      //          << "  of size : " << nb_bytes
      //          << "  mempool = " << __mempool << std::endl;
      
      return alloc_pointer;
    }

    void deallocate ( int* pointer ) {
      const std::size_t index = pointer - __mempool;
      for ( auto iter = __allocations.rbegin();
            iter != __allocations.rend(); ++iter ) {
        if ( *iter == index ) {
          if ( iter == __allocations.rbegin() )
            __current_index = index;
          __allocations.erase ( (iter+1).base() );
          return;
        }
      }

      //std::cout << "LearningAlloc cannot find address: " << pointer
      //          << "  in mempool " << __mempool << std::endl;
      throw std::bad_alloc ();
    }

    std::size_t allocatedSize () {
      return __allocations.size ();
    }
    
    std::string toString () const {
      bool deja = false;
      std::stringstream str;
      str << "allocator " << __mempool << " = { ";
      for ( auto index : __allocations ) {
        if ( deja ) str << ", ";
        else deja = true;
        str << ( __mempool + index );
      }
      str << " }";
      return str.str ();
    }

      
  private:
    // the pool of memory used
    int *__mempool;

    // the size of the pool
    const std::size_t __size;
    
    std::size_t __current_index { 0 };

    // the set of allocation performed as a set sorted of [begin,end)
    std::vector<std::size_t> __allocations;
    
  };


  template <typename T>
  class LearningAlloc {
  public:
    using value_type = T;

    template <typename _Tp1>
    struct rebind {
      typedef LearningAlloc<_Tp1> other;
    };

    LearningAlloc ( const std::size_t size = 1000 )
      : __mempool ( new MyPoolAlloc ( size ) ) {}

    LearningAlloc ( const LearningAlloc<T>& from )
      : __mempool ( from.__mempool ) {}
    
    template <typename U>
    LearningAlloc( const LearningAlloc<U>& from )
      : __mempool ( from.__mempool ) {}

    LearningAlloc ( LearningAlloc<T>&& from )
      : __mempool ( from.__mempool ) {}

    ~LearningAlloc () {}

    LearningAlloc<T>& operator= ( const LearningAlloc<T>& from ) {
      if ( this != &from ) {
        __mempool = from.__mempool;
      }
      return *this;
    }

    LearningAlloc<T>& operator= ( LearningAlloc<T>&& from ) {
      if ( this != &from ) {
        __mempool = from.__mempool;
      }
      return *this;
    }

    template <typename U>
    bool operator== ( const LearningAlloc<U>& other ) const {
      return __mempool.get() == other.__mempool.get();
    }

    template <typename U>
    bool operator!= ( const LearningAlloc<U>& other ) const {
      return ! operator== ( other );
    }

    
    T* allocate( std::size_t num ) {
      return reinterpret_cast<T*>( __mempool->allocate ( num * sizeof( T ) ) );
    }

    void deallocate( T* p, std::size_t num ) {
      __mempool->deallocate ( reinterpret_cast<int*> ( p ) );
    }

    template <typename... Args>
    void construct( T* p, Args&&... args ) {
      ::new ( (void*) p ) T( std::forward<Args>( args )... );
    }

    void destroy( T* p ) { p->~T(); }

    std::size_t allocatedSize () {
      return __mempool->allocatedSize ();
    }

    std::string toString () const {
      return __mempool->toString ();
    }

  private:
    std::shared_ptr<MyPoolAlloc> __mempool;

    template <typename U>
    friend class LearningAlloc;
    
  };

} /* namespace gum_tests */

#endif /* GUM_TEST_LEARNING_ALLOC_H */
