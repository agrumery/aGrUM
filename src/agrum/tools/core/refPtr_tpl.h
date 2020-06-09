
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/tools/core/refPtr.h>

namespace gum {

  // default constructor

  template < typename Val >
  INLINE RefPtr< Val >::RefPtr(Val* v) :
      val__(v), refcount__(v ? new unsigned int(1U) : 0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(RefPtr);
  }

  // copy constructor

  template < typename Val >
  INLINE RefPtr< Val >::RefPtr(const RefPtr< Val >& from) :
      val__(from.val__), refcount__(from.refcount__) {
    // for debugging purposes
    GUM_CONS_CPY(RefPtr);

    if (refcount__) ++*refcount__;
  }

  // copy constructor for castable pointers

  template < typename Val >
  template < typename DownVal >
  INLINE RefPtr< Val >::RefPtr(const RefPtr< DownVal >& from) :
      val__(from.val__), refcount__(from.refcount__) {
    // for debugging purposes
    GUM_CONS_CPY(RefPtr);

    if (refcount__) ++*refcount__;
  }

  // removes the current content of the smart pointer

  template < typename Val >
  INLINE void RefPtr< Val >::destroy__(unsigned int* count, Val* v) {
    if (count) {
      if (*count == 1U) {
        // do not change the order of the deletes (this prevents memory leaks
        // when
        // the delete of v fails (note that this should probably never happen))
        delete count;
        delete v;
      } else
        --*count;
    }
  }

  // copy operator

  template < typename Val >
  INLINE RefPtr< Val >& RefPtr< Val >::operator=(const RefPtr< Val >& from) {
    // avoid self assignment
    if (val__ != from.val__) {
      // for debugging purposes
      GUM_OP_CPY(RefPtr);

      // keep track of the current refcount and dumb pointer
      unsigned int* old_refcount = refcount__;
      Val*          old_val = val__;

      // perform the copy
      refcount__ = from.refcount__;
      val__ = from.val__;

      if (refcount__) ++*refcount__;

      // now try to dereference the old dumb pointer
      destroy__(old_refcount, old_val);
    }

    return *this;
  }

  // copy operator

  template < typename Val >
  INLINE RefPtr< Val >& RefPtr< Val >::operator=(Val* from) {
    // avoid self assignment
    if (val__ != from) {
      // for debugging purposes
      GUM_OP_CPY(RefPtr);

      // keep track of the current refcount and dumb pointer
      unsigned int* old_refcount = refcount__;
      Val*          old_val = val__;

      // perform the copy
      try {
        if (from)
          refcount__ = new unsigned int(1U);
        else
          refcount__ = 0;

        val__ = from;
      } catch (std::bad_alloc&) {
        if (*old_refcount == 1) {
          val__ = from;
          delete old_val;
          return *this;
        }

        refcount__ = 0;
        val__ = 0;
        throw;
      }

      // now try to dereference the old dumb pointer
      destroy__(old_refcount, old_val);
    }

    return *this;
  }

  // copy operator for downcastable pointers

  template < typename Val >
  template < typename DownVal >
  INLINE RefPtr< Val >& RefPtr< Val >::operator=(const RefPtr< DownVal >& from) {
    // for debugging purposes
    GUM_OP_CPY(RefPtr);
    // keep track of the current refcount and dumb pointer
    unsigned int* old_refcount = refcount__;
    Val*          old_val = val__;

    // perform the copy
    refcount__ = from.refcount__;
    val__ = from.val__;

    if (refcount__) ++*refcount__;

    // now try to dereference the old dumb pointer
    destroy__(old_refcount, old_val);

    return *this;
  }

  // destructor: it decrements the Val's reference count

  template < typename Val >
  INLINE RefPtr< Val >::~RefPtr() {
    // for debugging purposes
    GUM_DESTRUCTOR(RefPtr);
    destroy__(refcount__, val__);
  }

  // checks whether two RefPtr<Val> are smart pointers for the same element

  template < typename Val >
  INLINE bool RefPtr< Val >::operator==(const RefPtr< Val >& from) const {
    return from.refcount__ == refcount__;
  }

  // checks whether two RefPtr<Val> are smart pointers for differen elements

  template < typename Val >
  INLINE bool RefPtr< Val >::operator!=(const RefPtr< Val >& from) const {
    return from.refcount__ != refcount__;
  }

  // dereferencing operator

  template < typename Val >
  INLINE Val& RefPtr< Val >::operator*() {
    if (!val__) { GUM_ERROR(NullElement, "dereferencing a nullptr pointer"); }

    return *val__;
  }

  // dereferencing operator

  template < typename Val >
  INLINE const Val& RefPtr< Val >::operator*() const {
    if (!val__) { GUM_ERROR(NullElement, "dereferencing a nullptr pointer"); }

    return *val__;
  }

  // dereferencing operator

  template < typename Val >
  INLINE Val* RefPtr< Val >::operator->() const {
    if (!val__) { GUM_ERROR(NullElement, "dereferencing a nullptr pointer"); }

    return val__;
  }

  // checks whether a RefPtr points toward something

  template < typename Val >
  INLINE RefPtr< Val >::operator bool() const {
    return (val__ != 0);
  }

  // dereference what was referenced by the smart pointer

  template < typename Val >
  INLINE void RefPtr< Val >::clear() {
    // keep track of the old pointer and reference count
    unsigned int* old_refcount = refcount__;
    Val*          old_val = val__;
    // set properly the dumb pointer and its refcount
    val__ = 0;
    refcount__ = 0;
    // now try to dereference the old dumb pointer
    destroy__(old_refcount, old_val);
  }

  // returns the number of references on the contained pointer

  template < typename Val >
  INLINE unsigned int RefPtr< Val >::refCount() const {
    if (refcount__ == 0) return 0;

    return *refcount__;
  }

  // returns the refcount pointer

  template < typename Val >
  INLINE unsigned int* RefPtr< Val >::refCountPtr__() const {
    return refcount__;
  }

  // replace the contents of two RefPtr

  template < typename Val >
  void swap(RefPtr< Val >& ptr1, RefPtr< Val >& ptr2) {
    // save from's content
    Val*          tmp_val = ptr2.val__;
    unsigned int* tmp_refcount = ptr2.refcount__;
    // modify from's content
    ptr2.refcount__ = ptr1.refcount__;
    ptr2.val__ = ptr1.val__;
    // modify this's content
    ptr1.val__ = tmp_val;
    ptr1.refcount__ = tmp_refcount;
  }

} /* namespace gum */
