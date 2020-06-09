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
      : mempool__ ( new int[size / sizeof(int) + 1 ] )
      , size__ ( size / sizeof(int) + 1 ) {
      //std::cout << "create a new pool alloc " << mempool__ << std::endl;
    }
    
    MyPoolAlloc ( const MyPoolAlloc& ) = delete;
    MyPoolAlloc ( MyPoolAlloc&& ) = delete;

    ~MyPoolAlloc () {
      delete[] mempool__;
    }

    MyPoolAlloc& operator= ( const MyPoolAlloc& ) = delete;
    MyPoolAlloc& operator= ( MyPoolAlloc&& ) = delete;

    int* allocate ( const std::size_t nb_bytes ) {
      std::size_t nb_int = nb_bytes / sizeof(int);
      if ( nb_bytes % sizeof(int) ) ++nb_int;
      if ( current_index__ + nb_int > size__ )
        throw std::bad_alloc ();
      int* alloc_pointer = mempool__ + current_index__;
      allocations__.push_back ( current_index__ );
      current_index__ += nb_int;

      //std::cout << "LearningAlloc allocated: " << alloc_pointer
      //          << "  of size : " << nb_bytes
      //          << "  mempool = " << mempool__ << std::endl;
      
      return alloc_pointer;
    }

    void deallocate ( int* pointer ) {
      const std::size_t index = pointer - mempool__;
      for ( auto iter = allocations__.rbegin();
            iter != allocations__.rend(); ++iter ) {
        if ( *iter == index ) {
          if ( iter == allocations__.rbegin() )
            current_index__ = index;
          allocations__.erase ( (iter+1).base() );
          return;
        }
      }

      //std::cout << "LearningAlloc cannot find address: " << pointer
      //          << "  in mempool " << mempool__ << std::endl;
      throw std::bad_alloc ();
    }

    std::size_t allocatedSize () {
      return allocations__.size ();
    }
    
    std::string toString () const {
      bool deja = false;
      std::stringstream str;
      str << "allocator " << mempool__ << " = { ";
      for ( auto index : allocations__ ) {
        if ( deja ) str << ", ";
        else deja = true;
        str << ( mempool__ + index );
      }
      str << " }";
      return str.str ();
    }

      
  private:
    // the pool of memory used
    int *mempool__;

    // the size of the pool
    const std::size_t size__;
    
    std::size_t current_index__ { 0 };

    // the set of allocation performed as a set sorted of [begin,end)
    std::vector<std::size_t> allocations__;
    
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
      : mempool__ ( new MyPoolAlloc ( size ) ) {}

    LearningAlloc ( const LearningAlloc<T>& from )
      : mempool__ ( from.mempool__ ) {}
    
    template <typename U>
    LearningAlloc( const LearningAlloc<U>& from )
      : mempool__ ( from.mempool__ ) {}

    LearningAlloc ( LearningAlloc<T>&& from )
      : mempool__ ( from.mempool__ ) {}

    ~LearningAlloc () {}

    LearningAlloc<T>& operator= ( const LearningAlloc<T>& from ) {
      if ( this != &from ) {
        mempool__ = from.mempool__;
      }
      return *this;
    }

    LearningAlloc<T>& operator= ( LearningAlloc<T>&& from ) {
      if ( this != &from ) {
        mempool__ = from.mempool__;
      }
      return *this;
    }

    template <typename U>
    bool operator== ( const LearningAlloc<U>& other ) const {
      return mempool__.get() == other.mempool__.get();
    }

    template <typename U>
    bool operator!= ( const LearningAlloc<U>& other ) const {
      return ! operator== ( other );
    }

    
    T* allocate( std::size_t num ) {
      return reinterpret_cast<T*>( mempool__->allocate ( num * sizeof( T ) ) );
    }

    void deallocate( T* p, std::size_t num ) {
      mempool__->deallocate ( reinterpret_cast<int*> ( p ) );
    }

    template <typename... Args>
    void construct( T* p, Args&&... args ) {
      ::new ( (void*) p ) T( std::forward<Args>( args )... );
    }

    void destroy( T* p ) { p->~T(); }

    std::size_t allocatedSize () {
      return mempool__->allocatedSize ();
    }

    std::string toString () const {
      return mempool__->toString ();
    }

  private:
    std::shared_ptr<MyPoolAlloc> mempool__;

    template <typename U>
    friend class LearningAlloc;
    
  };

} /* namespace gum_tests */

#endif /* GUM_TEST_LEARNING_ALLOC_H */
