#ifndef GUM_ACTIVE_TEST_LEARNING_ALLOC_H
#  define GUM_ACTIVE_TEST_LEARNING_ALLOC_H

#  include <iostream>
#  include <memory>
#  include <new>
#  include <sstream>
#  include <string>
#  include <utility>
#  include <vector>

namespace gum_tests {

  class DebugPoolAlloc {
    public:
    DebugPoolAlloc(const std::size_t size) :
        _mempool_(new int[size / sizeof(int) + 1]), _size_(size / sizeof(int) + 1) {
      // std::cout << "create a new pool alloc " <<  _mempool_ << std::endl;
    }

    DebugPoolAlloc(const DebugPoolAlloc&) = delete;
    DebugPoolAlloc(DebugPoolAlloc&&)      = delete;

    ~DebugPoolAlloc() { delete[] _mempool_; }

    DebugPoolAlloc& operator=(const DebugPoolAlloc&) = delete;
    DebugPoolAlloc& operator=(DebugPoolAlloc&&)      = delete;

    int* allocate(const std::size_t nb_bytes) {
      std::size_t nb_int = nb_bytes / sizeof(int);
      if (nb_bytes % sizeof(int)) ++nb_int;
      if (_current_index_ + nb_int > _size_) throw std::bad_alloc();
      int* alloc_pointer = _mempool_ + _current_index_;
      _allocations_.push_back(_current_index_);
      _current_index_ += nb_int;

      // std::cout << "LearningAlloc allocated: " << alloc_pointer
      //           << "  of size : " << nb_bytes
      //           << "  mempool = " <<  _mempool_ << std::endl;

      return alloc_pointer;
    }

    void deallocate(int* pointer) {
      const std::size_t index = pointer - _mempool_;
      for (auto iter = _allocations_.rbegin(); iter != _allocations_.rend(); ++iter) {
        if (*iter == index) {
          if (iter == _allocations_.rbegin()) _current_index_ = index;
          _allocations_.erase((iter + 1).base());
          return;
        }
      }

      // std::cout << "LearningAlloc cannot find address: " << pointer
      //           << "  in mempool " <<  _mempool_ << std::endl;
      throw std::bad_alloc();
    }

    std::size_t allocatedSize() { return _allocations_.size(); }

    std::string toString() const {
      bool              deja = false;
      std::stringstream str;
      str << "allocator " << _mempool_ << " = { ";
      for (auto index: _allocations_) {
        if (deja) str << ", ";
        else deja = true;
        str << (_mempool_ + index);
      }
      str << " }";
      return str.str();
    }


    private:
    // the pool of memory used
    int* _mempool_;

    // the size of the pool
    const std::size_t _size_;

    std::size_t _current_index_{0};

    // the set of allocation performed as a set sorted of [begin,end)
    std::vector< std::size_t > _allocations_;
  };

  template < typename T >
  class LearningAlloc {
    public:
    using value_type = T;

    template < typename _Tp1 >
    struct rebind {
      using other = LearningAlloc< _Tp1 >;
    };

    LearningAlloc(const std::size_t size = 1000) : _mempool_(new DebugPoolAlloc(size)) {}

    LearningAlloc(const LearningAlloc< T >& from) : _mempool_(from._mempool_) {}

    template < typename U >
    LearningAlloc(const LearningAlloc< U >& from) : _mempool_(from._mempool_) {}

    LearningAlloc(LearningAlloc< T >&& from) : _mempool_(from._mempool_) {}

    ~LearningAlloc() {}

    LearningAlloc< T >& operator=(const LearningAlloc< T >& from) {
      if (this != &from) { _mempool_ = from._mempool_; }
      return *this;
    }

    LearningAlloc< T >& operator=(LearningAlloc< T >&& from) {
      if (this != &from) { _mempool_ = from._mempool_; }
      return *this;
    }

    template < typename U >
    bool operator==(const LearningAlloc< U >& other) const {
      return _mempool_.get() == other._mempool_.get();
    }

    template < typename U >
    bool operator!=(const LearningAlloc< U >& other) const {
      return !operator==(other);
    }

    T* allocate(std::size_t num) {
      return reinterpret_cast< T* >(_mempool_->allocate(num * sizeof(T)));
    }

    void deallocate(T* p, std::size_t num) { _mempool_->deallocate(reinterpret_cast< int* >(p)); }

    template < typename... Args >
    void construct(T* p, Args&&... args) {
      ::new ((void*)p) T(std::forward< Args >(args)...);
    }

    void destroy(T* p) { p->~T(); }

    std::size_t allocatedSize() { return _mempool_->allocatedSize(); }

    std::string toString() const { return _mempool_->toString(); }

    private:
    std::shared_ptr< DebugPoolAlloc > _mempool_;

    template < typename U >
    friend class LearningAlloc;
  };

} /* namespace gum_tests */

#endif /* GUM_ACTIVE_TEST_LEARNING_ALLOC_H */
/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
