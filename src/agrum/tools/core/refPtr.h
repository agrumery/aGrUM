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
 * @brief Class providing aGrUM's "smart" pointers
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef GUM_REFPTR_H
#define GUM_REFPTR_H

#include <new>

#include <agrum/agrum.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template < typename Val >
  class RefPtr;

  template < typename Val >
  class HashFunc;
#endif

  /* DOXYGEN does not like template functions...
   *
   * @brief Swap the contents of two RefPtr.
   * @tparam Val The type referenced by both gum::RefPtr.
   * @param ptr1 The smart pointer the content of which we swap with that of
   * ptr2.
   * @param ptr2 The smart pointer the content of which we replace with that of
   * ptr1.
   */
  /// @brief Swap the contents of two RefPtr.
  /// @ingroup refptr_group
  template < typename Val >
  void swap(RefPtr< Val >& ptr1, RefPtr< Val >& ptr2);

  // ===========================================================================
  // ===                           SMART POINTERS                            ===
  // ===========================================================================
  /**
   * @class RefPtr
   * @headerfile refPtr.h <agrum/tools/core/refPtr.h>
   * @brief Smart pointers
   * @ingroup basicstruct_group
   * @ingroup refptr_group
   *
   * aGrUM's smart pointers keep track of the number of times the value they
   * point to is referenced. When all smart pointers on a given value have been
   * deleted, the value itself is also deleted. Thus, using RefPtr, you do not
   * have to worry anymore about memory leaks. Note however that smart pointers
   * impose some constraints on the way you program. Here are some rules of
   * thumb: when several smart pointers must point to the same value, use only
   * once the constructor taking in argument *val, and use the copy constructor
   * or the assignment operator for the other smart pointers, else all the
   * smart pointers will think they point to different values and thus they
   * will all try to deallocate the dumb pointer they encapsulate, hence
   * resulting in segmentation faults. In fact, the correct way to use the *val
   * constructor is writing things like @code RefPtr ( new myObject ) @endcode
   * In particular, never deallocate yourself a dumb pointer you have
   * encapsulated into a smart pointer.
   *
   * @par Usage example:
   * @code
   * // creation of smart pointer
   * RefPtr<int> ptr1 (new int (4));
   *
   * // copying (and sharing) this pointer into new smart pointers
   * RefPtr<int> ptr2 = ptr1, ptr3;
   * ptr3 = ptr1;
   *
   * // make ptr2 point toward nothing (this does not deallocate int (4) as it
   * // is pointed to by ptr1 and ptr3)
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
   *
   * @tparam Val The type referenced by the gum::RefPtr.
   */
  template < typename Val >
  class RefPtr {
    public:
    /// The swap function must access to gum::RefPtr private parts.
    friend void swap<>(RefPtr< Val >&, RefPtr< Val >&);

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * This constructor creates an object encapsulating the pointer passed in
     * argument. No copy of the value pointed to by the pointer is performed.
     * The RefPtr assumes that the value pointed to has been allocated on the
     * heap using the new operator. If this is not the case, then using RefPtr
     * will result in an undefined behavior when the RefPtr is destroyed (ok,
     * we all know what it means: a segmentation fault). To avoid deleting
     * several times the pointer encapsulated, the safe way to use the RefPtr
     * is certainly through calls like: @code RefPtr( new myObject ) @endcode
     * Passing an already allocated pointer to the constructor is not
     * forbidden.  However, in this case, care should be taken not to allow
     * external functions to delete the value pointed to by val. Moreover, care
     * should be taken not to allow creating multiple RefPtr using this
     * constructor on the same val. This would lead to unexpected results
     * after deletion of the first RefPtr.
     *
     * @param val The dumb pointer encapsulated into the object (make sure it
     * is allocated on the heap)
     * @throws std::bad_alloc Raised if the complete RefPtr structure cannot be
     * set properly.
     */
    explicit RefPtr(Val* val = 0);

    /**
     * @brief Copy constructor.
     * @param from the smart pointer we wish to make a copy.
     */
    RefPtr(const RefPtr< Val >& from);

    /**
     * @brief Copy constructor for downcastable pointers.
     * @param from the smart pointer we wish to make a copy.
     * @tparam DownVal The downcastable type.
     */
    template < typename DownVal >
    RefPtr(const RefPtr< DownVal >& from);

    /**
     * @brief Class destructor.
     *
     * Decrements the ref count and deletes if necessary the dumb pointer.
     */
    ~RefPtr();

    ///@}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Checks whether a RefPtr points toward something.
     *
     * This method enables writing code like @c if (refptr) perform_operation()
     */
    operator bool() const;

    /**
     * @brief Makes the smart pointer point to 0.
     *
     * If necessary, the dumb pointer previously pointed to by the RefPtr is
     * deallocated. In this case, an exception may be thrown by the destructor
     * of the object pointed to. But, even in this case, the RefPtr guarrantees
     * that after the completion of this method, the RefPtr will point toward
     * 0.
     */
    void clear();

    /**
     * @brief Returns the number of smart pointer referencing the contained
     * pointer.
     */
    unsigned int refCount() const;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * The operator= may throw exceptions when the dumb pointer previously
     * pointed to by the RefPtr is deallocated (that is, the destructor of the
     * object pointed to may throw an exception). However, even when this
     * occurs, the RefPtr guarrantees that the copy operation is correctly
     * performed, that is, after the completion of the function, the RefPtr
     * points to the same element as from.
     *
     * @param from The smart pointer we wish to make a copy.
     * @return Returns this gum::RefPtr.
     */
    RefPtr< Val >& operator=(const RefPtr< Val >& from);

    /**
     * @brief Copy operator.
     *
     * The operator= may throw exceptions when the dumb pointer previously
     * pointed to by the RefPtr is deallocated (that is, the destructor of the
     * object pointed to may throw an exception). However, even when this
     * occurs, the RefPtr guarrantees that its state is coherent: either it
     * could succeed to encapsulate the dumb pointer and this one is referenced
     * once, or even encapsulating the new pointer failed and the RefPtr points
     * toward the 0 pointer.
     *
     * @param from the dumb pointer we wish to encapsulate.
     * @return Returns this gum::RefPtr.
     */

    RefPtr< Val >& operator=(Val* from);

    /**
     * @brief Copy operator for downcastable pointers.
     *
     * The operator= may throw exceptions when the dumb pointer previously
     * pointed to by the RefPtr is deallocated (that is, the destructor of the
     * object pointed to may throw an exception). However, even when this
     * occurs, the RefPtr guarrantees that the copy operation is correctly
     * performed, that is, after the completion of the function, the RefPtr
     * points to the same element as from.
     *
     * @tparam DownVal The downcastable type.
     * @param from the smart pointer we wish to make a copy.
     * @return Returns this gum::RefPtr.
     */
    template < typename DownVal >
    RefPtr< Val >& operator=(const RefPtr< DownVal >& from);

    /**
     * @brief Checks whether two RefPtr<Val> are smart pointers for the same
     * element.
     *
     * "Pointing toward the same element" is a little ambiguous: it does not
     * mean that the smart pointers are pointing toward the same Val instance
     * as several RefPtr<Val> created by the constructor with *val may point
     * toward the same val element while being unrelated (they do not share the
     * same reference). Instead, it means that the two smart pointers share the
     * same reference counter, i.e., that at least one of the two smarts
     * pointers has been created using the copy operator. As a consequence both
     * pointers point toward the same Val instance (but the converse is false).
     *
     * @param from The gum::RefPtr to test for equality.
     * @return Returns true if this and from are equal.
     */
    bool operator==(const RefPtr< Val >& from) const;

    /**
     * @brief Checks whether two RefPtr<Val> are smart pointers for different
     * elements.
     *
     * Returns true if either the dumb pointers the smart pointers encapsulate
     * are different or the reference counters are different (i.e., the smart
     * pointers are not related through copy operators).
     *
     * @param from The gum::RefPtr to test for inequality.
     * @return Returns true if this and from differ.
     */
    bool operator!=(const RefPtr< Val >& from) const;

    /**
     * @brief Dereferencing operator.
     *
     * This operator allows developers to write code like @c refptr->member().
     * @return Returns a pointer over the value referenced by this gum::RefPtr.
     * @throws NullElement Raised whenever the smart pointer points toward 0.
     */
    Val* operator->() const;

    /**
     * @brief Dereferencing operator.
     *
     * This operator is provided for convenience but you should prefer using
     * operator -> as this is the syntax you would use with the dumb pointer.
     * Note however that it might be useful for built-in types such as int.
     *
     * @return Returns a reference over the value referenced by this
     * gum::RefPtr.
     * @throw NullElement Raised whenever the RefPtr points to 0.
     */
    Val& operator*();

    /**
     * @brief Const dereferencing operator.
     *
     * This operator is provided for convenience but you should prefer using
     * operator -> as this is the syntax you would use with the dumb pointer.
     * Note however that it might be useful for built-in types such as int.
     *
     * @return Returns a constant reference over the value referenced by this
     * gum::RefPtr.
     * @throw NullElement Raised whenever the RefPtr points to 0.
     */
    const Val& operator*() const;

    /// @}
    // ============================================================================
    /// @name Internals
    // ============================================================================
    private:
    /// A friend to allow downcastings.
    template < typename T >
    friend class RefPtr;

    /// A friend for hashing quickly ref pointers.
    template < typename T >
    friend class HashFunc;

    /// The dumb pointer encapsulated into the "smart" pointer.
    Val* _val_;

    /// A reference counter on *val.
    unsigned int* _refcount_;

    /// A function to remove the content of the smart pointer, if any.
    void _destroy_(unsigned int*, Val*);

    /// A function to return the refcount pointer.
    unsigned int* _refCountPtr_() const;
  };

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::RefPtr< int >;
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::RefPtr< long >;
#    endif
#  endif
#endif

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::RefPtr< double >;
#    endif
#  endif
#endif


// always include the tpl_.h as it contains only templates
#include <agrum/tools/core/refPtr_tpl.h>

#endif /* GUM_REFPTR_H */
