/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Implementation of aGrUM's "smart" pointers
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/config.h>

#include <agrum/core/refPtr.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  /// default constructor

  template <typename Val>
  INLINE RefPtr<Val>::RefPtr(Val *v)
      : __val(v), __refcount(v ? new unsigned int(1U) : 0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(RefPtr);
  }

  /// copy constructor

  template <typename Val>
  INLINE RefPtr<Val>::RefPtr(const RefPtr<Val> &from)
      : __val(from.__val), __refcount(from.__refcount) {
    // for debugging purposes
    GUM_CONS_CPY(RefPtr);

    if (__refcount)
      ++*__refcount;
  }

  /// copy constructor for castable pointers

  template <typename Val>
  template <typename DownVal>
  INLINE RefPtr<Val>::RefPtr(const RefPtr<DownVal> &from)
      : __val(from.__val), __refcount(from.__refcount) {
    // for debugging purposes
    GUM_CONS_CPY(RefPtr);

    if (__refcount)
      ++*__refcount;
  }

  /// removes the current content of the smart pointer

  template <typename Val>
  INLINE void RefPtr<Val>::__destroy(unsigned int *count, Val *v) {
    if (count) {
      if (*count == 1U) {
        // do not change the order of the deletes (this prevents memory leaks when
        // the delete of v fails (note that this should probably never happen))
        delete count;
        delete v;
      } else
        --*count;
    }
  }

  /// copy operator

  template <typename Val>
  INLINE RefPtr<Val> &RefPtr<Val>::operator=(const RefPtr<Val> &from) {
    // avoid self assignment
    if (__val != from.__val) {
      // for debugging purposes
      GUM_OP_CPY(RefPtr);

      // keep track of the current refcount and dumb pointer
      unsigned int *old_refcount = __refcount;
      Val *old_val = __val;

      // perform the copy
      __refcount = from.__refcount;
      __val = from.__val;

      if (__refcount)
        ++*__refcount;

      // now try to dereference the old dumb pointer
      __destroy(old_refcount, old_val);
    }

    return *this;
  }

  /// copy operator

  template <typename Val> INLINE RefPtr<Val> &RefPtr<Val>::operator=(Val *from) {
    // avoid self assignment
    if (__val != from) {
      // for debugging purposes
      GUM_OP_CPY(RefPtr);

      // keep track of the current refcount and dumb pointer
      unsigned int *old_refcount = __refcount;
      Val *old_val = __val;

      // perform the copy
      try {
        if (from)
          __refcount = new unsigned int(1U);
        else
          __refcount = 0;

        __val = from;
      } catch (std::bad_alloc) {
        if (*old_refcount == 1) {
          __val = from;
          delete old_val;
          return *this;
        }

        __refcount = 0;
        __val = 0;
        throw;
      }

      // now try to dereference the old dumb pointer
      __destroy(old_refcount, old_val);
    }

    return *this;
  }

  /// copy operator for downcastable pointers

  template <typename Val>
  template <typename DownVal>
  INLINE RefPtr<Val> &RefPtr<Val>::operator=(const RefPtr<DownVal> &from) {
    // for debugging purposes
    GUM_OP_CPY(RefPtr);
    // keep track of the current refcount and dumb pointer
    unsigned int *old_refcount = __refcount;
    Val *old_val = __val;

    // perform the copy
    __refcount = from.__refcount;
    __val = from.__val;

    if (__refcount)
      ++*__refcount;

    // now try to dereference the old dumb pointer
    __destroy(old_refcount, old_val);

    return *this;
  }

  /// destructor: it decrements the Val's reference count

  template <typename Val> INLINE RefPtr<Val>::~RefPtr() {
    // for debugging purposes
    GUM_DESTRUCTOR(RefPtr);
    __destroy(__refcount, __val);
  }

  /// checks whether two RefPtr<Val> are smart pointers for the same element

  template <typename Val>
  INLINE bool RefPtr<Val>::operator==(const RefPtr<Val> &from) const {
    return from.__refcount == __refcount;
  }

  /// checks whether two RefPtr<Val> are smart pointers for differen elements

  template <typename Val>
  INLINE bool RefPtr<Val>::operator!=(const RefPtr<Val> &from) const {
    return from.__refcount != __refcount;
  }

  /// dereferencing operator

  template <typename Val> INLINE Val &RefPtr<Val>::operator*() {
    if (!__val) {
      GUM_ERROR(NullElement, "dereferencing a nullptr pointer");
    }

    return *__val;
  }

  /// dereferencing operator

  template <typename Val> INLINE const Val &RefPtr<Val>::operator*() const {
    if (!__val) {
      GUM_ERROR(NullElement, "dereferencing a nullptr pointer");
    }

    return *__val;
  }

  /// dereferencing operator

  template <typename Val> INLINE Val *RefPtr<Val>::operator->() const {
    if (!__val) {
      GUM_ERROR(NullElement, "dereferencing a nullptr pointer");
    }

    return __val;
  }

  /// checks whether a RefPtr points toward something

  template <typename Val> INLINE RefPtr<Val>::operator bool() const {
    return (__val != 0);
  }

  /// dereference what was referenced by the smart pointer

  template <typename Val> INLINE void RefPtr<Val>::clear() {
    // keep track of the old pointer and reference count
    unsigned int *old_refcount = __refcount;
    Val *old_val = __val;
    // set properly the dumb pointer and its refcount
    __val = 0;
    __refcount = 0;
    // now try to dereference the old dumb pointer
    __destroy(old_refcount, old_val);
  }

  /// returns the number of references on the contained pointer

  template <typename Val> INLINE unsigned int RefPtr<Val>::refCount() const {
    if (__refcount == 0)
      return 0;

    return *__refcount;
  }

  /// returns the refcount pointer

  template <typename Val> INLINE unsigned int *RefPtr<Val>::__refCountPtr() const {
    return __refcount;
  }

  /// swap the contents of two RefPtr
  /** @param ptr1 the smart pointer the content of which we swap with that of ptr2
   * @param ptr2 the smart pointer the content of which we swap with that of ptr1
   */

  template <typename Val> void swap(RefPtr<Val> &ptr1, RefPtr<Val> &ptr2) {
    // save from's content
    Val *tmp_val = ptr2.__val;
    unsigned int *tmp_refcount = ptr2.__refcount;
    // modify from's content
    ptr2.__refcount = ptr1.__refcount;
    ptr2.__val = ptr1.__val;
    // modify this's content
    ptr1.__val = tmp_val;
    ptr1.__refcount = tmp_refcount;
  }

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} /* namespace gum */
