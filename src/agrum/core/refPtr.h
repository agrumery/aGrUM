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
 * @brief Class providing aGrUM's "smart" pointers
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * RefPtr are a replacement for the usual pointers: they keep track of the
 * number of "smart" pointers pointing to a given element. When an element is
 * no more referenced by any smart pointer, it is deleted. Hence using RefPtr,
 * developers do not have to worry anymore about memory leaks. The correct way to
 * use RefPtr is by creating unnamed temporaries like:
 * @code RefPtr ( new myObject ) @endcode
 * In any case, if you decide to pass a named pointer as argument to a RefPtr,
 * make sure you will do it only once, that it is allocated on the heap and
 * that you never try to deallocate it yourself, else your program will crash.
 * @par Usage example:
 * @code
 * // creation of smart pointer
 * RefPtr<int> ptr1 (new int (4));
 *
 * // copying (and sharing) this pointer into new smart pointers
 * RefPtr<int> ptr2 = ptr1, ptr3;
 * ptr3 = ptr1;
 *
 * // make ptr2 point toward nothing (this does not deallocate int (4) as it is
 * // pointed to by ptr1 and ptr3)
 * ptr2.clear ();
 *
 * // modifying the value pointed to by the dumb pointer contained in ptr1
 * *ptr1 = 5;
 *
 * // print the content of ptr3
 * cerr << *ptr3 << " = 5" << endl;
 *
 * // check whether ptr1 and ptr3 reference the same dumb pointer
 * if (ptr1 == ptr2) cerr << "reference the same dumb pointer" << endl;
 *
 * // check whether ptr1 and ptr2 contain a dumb pointer
 * if (ptr1 && !ptr2) cerr << "check containers" << endl;
 * @endcode
 */

#ifndef GUM_REFPTR_H
#define GUM_REFPTR_H

#include <new>

#include <agrum/config.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template <typename Val> class RefPtr;
  template <typename Val> void swap(RefPtr<Val> &, RefPtr<Val> &);

  template <typename Val> class HashFunc;
#endif

  /* =========================================================================== */
  /* ===                           SMART POINTERS                            === */
  /* =========================================================================== */
  /** @class RefPtr
   * @brief Smart pointers
   * @ingroup basicstruct_group
   *
   * aGrUM's smart pointers keep track of the number of times the value they point
   * to is referenced. When all smart pointers on a given value have been deleted,
   * the value itself is also deleted. Thus, using RefPtr, you do not have to
   * worry anymore about memory leaks. Note however that smart pointers impose some
   * constraints on the way you program. Here are some rules of thumb:
   * when several smart pointers must point to the same value, use only once
   * the constructor taking in argument *val, and use the copy constructor
   * or the assignment operator for the other smart pointers, else all the smart
   * pointers will think they point to different values and thus they will all try
   * to deallocate the dumb pointer they encapsulate, hence resulting in
   * segmentation faults. In fact, the correct way to use the *val constructor is
   * writing things like @code RefPtr ( new myObject ) @endcode
   * In particular, never deallocate yourself a dumb pointer you have encapsulated
   * into a smart pointer.
   * @par Usage example:
   * @code
   * // creation of smart pointer
   * RefPtr<int> ptr1 (new int (4));
   *
   * // copying (and sharing) this pointer into new smart pointers
   * RefPtr<int> ptr2 = ptr1, ptr3;
   * ptr3 = ptr1;
   *
   * // make ptr2 point toward nothing (this does not deallocate int (4) as it is
   * // pointed to by ptr1 and ptr3)
   * ptr2.clear ();
   *
   * // modifying the value pointed to by the dumb pointer contained in ptr1
   * *ptr1 = 5;
   *
   * // print the content of ptr3
   * cerr << *ptr3 << " = 5" << endl;
   *
   * // check whether ptr1 and ptr3 reference the same dumb pointer
   * if (ptr1 == ptr2) cerr << "reference the same dumb pointer" << endl;
   *
   * // check whether ptr1 and ptr2 contain a dumb pointer
   * if (ptr1 && !ptr2) cerr << "check containers" << endl;
   * @endcode
   */
  /* =========================================================================== */

  template <typename Val> class RefPtr {
    public:
    /// swap the contents of two RefPtr
    friend void swap<>(RefPtr<Val> &, RefPtr<Val> &);

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** This constructor creates an object encapsulating the pointer passed in
     * argument. No copy of the value pointed to by the pointer is performed. The
     * RefPtr assumes that the value pointed to has been allocated on the heap
     * using the new operator. If this is not the case, then using RefPtr will
     * result in an undefined behavior when the RefPtr is destroyed (ok, we all
     * know what it means: a segmentation fault). To avoid deleting several times
     * the pointer encapsulated, the safe way to use the RefPtr is certainly
     * through calls like: @code RefPtr( new myObject ) @endcode
     * Passing an already allocated pointer to the constructor is not forbidden.
     * However, in this case, care should be taken not to allow external functions
     * to delete the value pointed to by val. Moreover, care should be taken not to
     * allow creating multiple RefPtr using this constructor on the same val. This
     * would lead to unexpected results after deletion of the first RefPtr.
     * @param val the dumb pointer encapsulated into the object (make sure it is
     * allocated on the heap)
     * @throws std::bad_alloc exception is thrown if the complete RefPtr structure
     * cannot be set properly. */

    explicit RefPtr(Val *val = 0);

    /// copy constructor
    /** @param from the smart pointer we wish to make a copy */

    RefPtr(const RefPtr<Val> &from);

    /// copy constructor for downcastable pointers
    /** @param from the smart pointer we wish to make a copy */

    template <typename DownVal> RefPtr(const RefPtr<DownVal> &from);

    /** @brief destructor: decrements the ref count and deletes if necessary
     * the dumb pointer */

    ~RefPtr();

    ///@}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// checks whether a RefPtr points toward something
    /** This method enables writing code like @c if (refptr) perform_operation() */

    operator bool() const;

    /// makes the smart pointer point to 0.
    /** If necessary, the dumb pointer previously pointed to by the RefPtr is
     * deallocated. In this case, an exception may be thrown by the destructor of
     * the object pointed to. But, even in this case, the RefPtr guarrantees that
     * after the completion of this method, the RefPtr will point toward 0. */

    void clear();

    /// returns the number of smart pointer referencing the contained pointer

    unsigned int refCount() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** operator= may throw exceptions when the dumb pointer previously pointed to
     * by the RefPtr is deallocated (that is, the destructor of the object pointed
     * to may throw an exception). However, even when this occurs, the RefPtr
     * guarrantees that the copy operation is correctly performed, that is, after
     * the completion of the function, the RefPtr points to the same element
     * as from.
     * @param from the smart pointer we wish to make a copy */

    RefPtr<Val> &operator=(const RefPtr<Val> &from);

    /// copy operator
    /** operator= may throw exceptions when the dumb pointer previously pointed to
     * by the RefPtr is deallocated (that is, the destructor of the object pointed
     * to may throw an exception). However, even when this occurs, the RefPtr
     * guarrantees that its state is coherent: either it could succeed to
     * encapsulate the dumb pointer and this one is referenced once, or even
     * encapsulating the new pointer failed and the RefPtr points toward
     * the 0 pointer.
     * @param from the dumb pointer we wish to encapsulate */

    RefPtr<Val> &operator=(Val *from);

    /// copy operator for downcastable pointers
    /** operator= may throw exceptions when the dumb pointer previously pointed to
     * by the RefPtr is deallocated (that is, the destructor of the object pointed
     * to may throw an exception). However, even when this occurs, the RefPtr
     * guarrantees that the copy operation is correctly performed, that is, after
     * the completion of the function, the RefPtr points to the same element as
     * from.
     * @param from the smart pointer we wish to make a copy */

    template <typename DownVal> RefPtr<Val> &operator=(const RefPtr<DownVal> &from);

    /// checks whether two RefPtr<Val> are smart pointers for the same element.
    /** "Pointing toward the same element" is a little ambiguous: it does not mean
     * that the smart pointers are pointing toward the same Val instance as
     * several RefPtr<Val> created by the constructor with *val may point toward
     * the same val element while being unrelated (they do not share the same
     * reference). Instead, it means that the two smart pointers share the same
     * reference counter, i.e., that at least one of the two smarts pointers has
     * been created using the copy operator. As a consequence both pointers point
     * toward the same Val instance (but the converse is false). */

    bool operator==(const RefPtr<Val> &from) const;

    /// checks whether two RefPtr<Val> are smart pointers for different elements
    /** returns true if either the dumb pointers the smart pointers encapsulate are
     * different or the reference counters are different (i.e., the smart pointers
     * are not related through copy operators) */

    bool operator!=(const RefPtr<Val> &from) const;

    /// dereferencing operator
    /** This operator allows developers to write code like
     * @c refptr->member()
     * @throws NullElement exception is thrown whenever the smart pointer points
     * toward 0 */

    Val *operator->() const;

    /// dereferencing operator
    /** This operator is provided for convenience but you should prefer using
     * operator -> as this is the syntax you would use with the dumb pointer. Note
     * however that it might be useful for built-in types such as int.
     * @throw NullElement exception is thrown whenever the RefPtr points to 0 */

    Val &operator*();

    /// const dereferencing operator
    /** This operator is provided for convenience but you should prefer using
     * operator -> as this is the syntax you would use with the dumb pointer. Note
     * however that it might be useful for built-in types such as int.
     * @throw NullElement exception is thrown whenever the RefPtr points to 0 */
    const Val &operator*() const;

    /// @}

    // ############################################################################
    // internals
    // ############################################################################
    private:
    /// a friend to allow downcastings

    template <typename T> friend class RefPtr;

    /// a friend for hashing quickly ref pointers

    template <typename T> friend class HashFunc;

    /// the dumb pointer encapsulated into the "smart" pointer
    Val *__val;

    /// a reference counter on *val
    unsigned int *__refcount;

    /// a function to remove the content of the smart pointer, if any
    void __destroy(unsigned int *, Val *);

    /// a function to return the refcount pointer
    unsigned int *__refCountPtr() const;
  };

} /* namespace gum */

/* ============================================================================= */
/* ============================================================================= */
/* ===                     SMART POINTERS' IMPLEMENTATION                    === */
/* ============================================================================= */
/* ============================================================================= */
// always include the .tcc as it contains only templates
#include <agrum/core/refPtr.tcc>

#endif /* GUM_REFPTR_H */
