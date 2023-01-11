/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Implementation of aGrUM's "smart" pointers.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/tools/core/refPtr.h>

namespace gum {

  // default constructor

  template < typename Val >
  INLINE RefPtr< Val >::RefPtr(Val* v) : _val_(v), _refcount_(v ? new unsigned int(1U) : 0) {
    GUM_CONSTRUCTOR(RefPtr);
  }

  // copy constructor

  template < typename Val >
  INLINE RefPtr< Val >::RefPtr(const RefPtr< Val >& from) :
      _val_(from._val_), _refcount_(from._refcount_) {
    GUM_CONS_CPY(RefPtr);

    if (_refcount_) ++*_refcount_;
  }

  // copy constructor for castable pointers

  template < typename Val >
  template < typename DownVal >
  INLINE RefPtr< Val >::RefPtr(const RefPtr< DownVal >& from) :
      _val_(from._val_), _refcount_(from._refcount_) {
    GUM_CONS_CPY(RefPtr);

    if (_refcount_) ++*_refcount_;
  }

  // removes the current content of the smart pointer

  template < typename Val >
  INLINE void RefPtr< Val >::_destroy_(unsigned int* count, Val* v) {
    if (count) {
      if (*count == 1U) {
        // do not change the order of the deletes (this prevents memory leaks
        // when
        // the delete of v fails (note that this should probably never happen))
        delete count;
        delete v;
      } else --*count;
    }
  }

  // copy operator

  template < typename Val >
  INLINE RefPtr< Val >& RefPtr< Val >::operator=(const RefPtr< Val >& from) {
    // avoid self assignment
    if (_val_ != from._val_) {
      GUM_OP_CPY(RefPtr)

      // keep track of the current refcount and dumb pointer
      unsigned int* old_refcount = _refcount_;
      Val*          old_val      = _val_;

      // perform the copy
      _refcount_ = from._refcount_;
      _val_      = from._val_;

      if (_refcount_) ++*_refcount_;

      // now try to dereference the old dumb pointer
      _destroy_(old_refcount, old_val);
    }

    return *this;
  }

  // copy operator

  template < typename Val >
  INLINE RefPtr< Val >& RefPtr< Val >::operator=(Val* from) {
    // avoid self assignment
    if (_val_ != from) {
      GUM_OP_CPY(RefPtr);

      // keep track of the current refcount and dumb pointer
      unsigned int* old_refcount = _refcount_;
      Val*          old_val      = _val_;

      // perform the copy
      try {
        if (from) _refcount_ = new unsigned int(1U);
        else _refcount_ = 0;

        _val_ = from;
      } catch (std::bad_alloc&) {
        if (*old_refcount == 1) {
          _val_ = from;
          delete old_val;
          return *this;
        }

        _refcount_ = 0;
        _val_      = 0;
        throw;
      }

      // now try to dereference the old dumb pointer
      _destroy_(old_refcount, old_val);
    }

    return *this;
  }

  // copy operator for downcastable pointers

  template < typename Val >
  template < typename DownVal >
  INLINE RefPtr< Val >& RefPtr< Val >::operator=(const RefPtr< DownVal >& from) {
    GUM_OP_CPY(RefPtr)
    // keep track of the current refcount and dumb pointer
    unsigned int* old_refcount = _refcount_;
    Val*          old_val      = _val_;

    // perform the copy
    _refcount_ = from._refcount_;
    _val_      = from._val_;

    if (_refcount_) ++*_refcount_;

    // now try to dereference the old dumb pointer
    _destroy_(old_refcount, old_val);

    return *this;
  }

  // destructor: it decrements the Val's reference count

  template < typename Val >
  INLINE RefPtr< Val >::~RefPtr() {
    GUM_DESTRUCTOR(RefPtr);
    _destroy_(_refcount_, _val_);
  }

  // checks whether two RefPtr<Val> are smart pointers for the same element

  template < typename Val >
  INLINE bool RefPtr< Val >::operator==(const RefPtr< Val >& from) const {
    return from._refcount_ == _refcount_;
  }

  // checks whether two RefPtr<Val> are smart pointers for differen elements

  template < typename Val >
  INLINE bool RefPtr< Val >::operator!=(const RefPtr< Val >& from) const {
    return from._refcount_ != _refcount_;
  }

  // dereferencing operator

  template < typename Val >
  INLINE Val& RefPtr< Val >::operator*() {
    if (!_val_) { GUM_ERROR(NullElement, "dereferencing a nullptr pointer") }

    return *_val_;
  }

  // dereferencing operator

  template < typename Val >
  INLINE const Val& RefPtr< Val >::operator*() const {
    if (!_val_) { GUM_ERROR(NullElement, "dereferencing a nullptr pointer") }

    return *_val_;
  }

  // dereferencing operator

  template < typename Val >
  INLINE Val* RefPtr< Val >::operator->() const {
    if (!_val_) { GUM_ERROR(NullElement, "dereferencing a nullptr pointer") }

    return _val_;
  }

  // checks whether a RefPtr points toward something

  template < typename Val >
  INLINE RefPtr< Val >::operator bool() const {
    return (_val_ != 0);
  }

  // dereference what was referenced by the smart pointer

  template < typename Val >
  INLINE void RefPtr< Val >::clear() {
    // keep track of the old pointer and reference count
    unsigned int* old_refcount = _refcount_;
    Val*          old_val      = _val_;
    // set properly the dumb pointer and its refcount
    _val_      = 0;
    _refcount_ = 0;
    // now try to dereference the old dumb pointer
    _destroy_(old_refcount, old_val);
  }

  // returns the number of references on the contained pointer

  template < typename Val >
  INLINE unsigned int RefPtr< Val >::refCount() const {
    if (_refcount_ == 0) return 0;

    return *_refcount_;
  }

  // returns the refcount pointer

  template < typename Val >
  INLINE unsigned int* RefPtr< Val >::_refCountPtr_() const {
    return _refcount_;
  }

  // replace the contents of two RefPtr

  template < typename Val >
  void swap(RefPtr< Val >& ptr1, RefPtr< Val >& ptr2) {
    // save from's content
    Val*          tmp_val      = ptr2._val_;
    unsigned int* tmp_refcount = ptr2._refcount_;
    // modify from's content
    ptr2._refcount_ = ptr1._refcount_;
    ptr2._val_      = ptr1._val_;
    // modify this's content
    ptr1._val_      = tmp_val;
    ptr1._refcount_ = tmp_refcount;
  }

} /* namespace gum */
