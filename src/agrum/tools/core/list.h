/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Generic class for manipulating lists.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#ifndef GUM_LIST_H
#define GUM_LIST_H

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/refPtr.h>
#include <agrum/tools/core/staticInitializer.h>

#define GUM_DEFAULT_ITERATOR_NUMBER 4

namespace gum {

  // ==============================================================================
  // templates provided by this header
  // ==============================================================================

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template < typename Val >
  class ListBucket;
  template < typename Val >
  class ListIterator;
  template < typename Val >
  class ListConstIterator;
  template < typename Val >
  class ListIteratorSafe;
  template < typename Val >
  class ListConstIteratorSafe;
  template < typename Val >
  class List;

#endif   // DOXYGEN_SHOULD_SKIP_THIS

#ifndef SWIG   // SWIG cannot read these lines
  /// an << operator for List
  template < typename Val >
  std::ostream& operator<<(std::ostream& stream, const List< Val >& list);
#endif   // SWIG


  // ===========================================================================
  // ===             BUCKETS: SINGLE ELEMENTS OF A CHAINED LIST              ===
  // ===========================================================================

  /**
   * @class ListBucket
   * @headerfile list.h <agrum/tools/core/list.h>
   * @brief Bucket for a chained list.
   * @ingroup list_group
   *
   * In aGrUM, each box of a chained list is called a bucket. Lists are doubly
   * linked bucket lists so as to enable efficient rbegin/rend iterators.
   *
   * @warning Values stored in buckets are ALWAYS COPIES.
   *
   * @tparam Val The values type stored in the gum::ListBucket.
   */
  template < typename Val >
  class ListBucket {
    private:
    /**
     * @brief C dummy type for the emplace constructor.
     *
     * This type is used to prevent the list emplace (int) to compile.
     */
    enum class Emplace {
      EMPLACE
    };

    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    ///@{

    /// Removes empty constructor.
    ListBucket() = delete;

    /**
     * @brief Default constructor.
     * @param v The value stored in the gum::ListBucket.
     */
    explicit ListBucket(const Val& v);

    /**
     * @brief Constructor for Val rvalues.
     * @param v The value stored in the gum::ListBucket.
     */
    explicit ListBucket(Val&& v) noexcept;

    /**
     * @brief Emplace (universal) constructor.
     * @tparam Args The emplace values type.
     * @param args The emplace values.
     */
    template < typename... Args >
    explicit ListBucket(Emplace, Args&&... args);

    /**
     * @brief Copy constructor.
     * @param src The gum::ListBucket to copy.
     */
    ListBucket(const ListBucket< Val >& src);

    /**
     * @brief Move constructor should be useless.
     * @param src The gum::ListBucket to move.
     */
    ListBucket(ListBucket< Val >&& src) = delete;

    /**
     * @brief Class destructor.
     *
     * @warning during its deletion, the bucket takes care of properly
     * rechaining the chained list. However, it has no knowledge about the
     * variables that keep track of the beginning/end of the chained list,
     * hence it cannot update them properly. This should be done by the List
     * itself.
     */
    ~ListBucket();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param src The gum::ListBucket to copy.
     * @return This gum::ListBucket.
     */
    ListBucket< Val >& operator=(const ListBucket< Val >& src);

    /**
     * @brief Move operator.
     * @param src The gum::ListBucket to move.
     * @return This gum::ListBucket.
     */
    ListBucket< Val >& operator=(ListBucket< Val >&& src) = delete;

    /**
     * @brief Equality check.
     * @param src The gum::ListBucket to test for equality.
     * @return Returns true if src and this gum::ListBucket are equal.
     */
    bool operator==(const ListBucket< Val >& src) const;

    /**
     * @brief Inequality check.
     * @param src The gum::ListBucket to test for inequality.
     * @return Returns true if src and this gum::ListBucket are not equal.
     */
    bool operator!=(const ListBucket< Val >& src) const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Dereferencing operator.
     * @return The value stored in this gum::ListBucket.
     */
    Val& operator*() noexcept;

    /**
     * @brief Dereferencing operator.
     * @return The value stored in this gum::ListBucket.
     */
    const Val& operator*() const noexcept;

    /**
     * @brief Returns the bucket toward the next element.
     * @return Returns the bucket toward the next element.
     */
    const ListBucket< Val >* next() const noexcept;

    /**
     * @brief Returns the bucket toward the preceding element.
     * @return Returns the bucket toward the preceding element.
     */
    const ListBucket< Val >* previous() const noexcept;

    /// @}

    private:
    /// All the list containers and iterators should be able to access the
    /// buckets.
    friend class List< Val >;
    friend class ListIterator< Val >;
    friend class ListConstIterator< Val >;
    friend class ListIteratorSafe< Val >;
    friend class ListConstIteratorSafe< Val >;

    /// @{
    /// Chaining toward the adjacent elements.
    ListBucket< Val >* _prev_{nullptr};
    ListBucket< Val >* _next_{nullptr};
    /// @}

    /// Val is the value contained in the box.
    Val _val_;
  };

  // ===========================================================================
  // ===                     GENERIC DOUBLY CHAINED LISTS                    ===
  // ===========================================================================

  /**
   * @class List
   * @headerfile list.h <agrum/tools/core/list.h>
   * @ingroup basicstruct_group
   * @ingroup list_group
   *
   * @brief Generic doubly linked lists.
   *
   * List enables fast and safe manipulation of chained lists.  Unless the
   * elements are rvalues, the insertions of new elements into the lists are @b
   * ALWAYS performed by copy, i.e., each time we add a new element X to the
   * List, a copy of X is actually created and this very copy is stored into
   * the list. For rvalues, move operations are performed.
   *
   * The List iterators are implemented so as to avoid segmentation faults when
   * elements of the list are deleted while some safe iterators are pointing on
   * them. Moreover they ensure that, when elements are removed from a List,
   * iterators on that list will never access these elements (which is not the
   * case for the iterators in the C++ standard library). Note that this
   * guarantee is ensured at low cost as experimental results show that List
   * and ListIterator are as efficient as their STL counterparts. However, this
   * guarantee can hold only if List is aware of all of the iterators pointing
   * to it: thus, when List erases one element, it can parse the list of its
   * iterators and update those that point toward the now deleted element. When
   * parsing elements without removing any element, you can use unsafe
   * iterators instead of safe ones because they are slightly faster. But those
   * will most certainly segfault if they perform some operations on deleted
   * elements.
   *
   * @par Usage example:
   * @code
   * // creation of an empty list
   * List<int> list1;
   * List<int> list2 { 3, 4, 5 }; // initializer list
   *
   * // adding elements to the list
   * list1.pushFront (23);
   * list1.pushBack (10);
   * list1 += 25;
   * list1.insert (12);
   *
   * // getting the second element of the list
   * cerr << "10 = " << list1[1] << endl;
   *
   * // getting the first and last elements
   * cerr << "first = " << list1.front() << " last = " << list1.back() << endl;
   *
   * // get the number of elements in the list
   * cerr << "number of elements = " << list1.size () << endl;
   *
   * // display the content of the list
   * cerr << list1 << endl;
   *
   * // copy the list
   * List<int> list2 = list1, list3;
   * list3 = list1;
   *
   * // delete the second element from the list
   * list1.erase (1);
   *
   * // delete the first and last elements
   * list1.popFront ();
   * list1.popBack ();
   *
   * // delete element whose value is 25
   * list1.eraseByVal (25);
   *
   * // check whether the list is empty
   * if (list1.empty()) cerr << "empty list" << endl;
   *
   * // remove all elements from the list
   * list1.clear ();
   *
   * // parse all the elements of a list using unsafe iterators
   * for (List<int>::iterator iter = list2.begin();
   *      iter != list2.end(); ++iter)
   *   cerr << *iter << endl;
   * for (List<int>::iterator iter = list2.rbegin();
   *      iter != list2.rend(); --iter)
   *   cerr << *iter << endl;
   * for (List<int>::const_iterator iter = list2.cbegin();
   *      iter != list2.cend(); ++iter)
   *   cerr << *iter << endl;
   * for (List<int>::const_iterator iter = list2.crbegin();
   *      iter != list2.crend(); --iter)
   *   cerr << *iter << endl;
   *
   * // parse all the elements of a list using safe iterators
   * for (List<int>::iterator_safe iter = list2.beginSafe();
   *      iter != list2.endSafe(); ++iter)
   *   cerr << *iter << endl;
   * for (List<int>::iterator_safe iter = list2.rbeginSafe();
   *      iter != list2.rendSafe(); --iter)
   *   cerr << *iter << endl;
   * for (List<int>::const_iterator_safe iter = list2.cbeginSafe();
   *      iter != list2.cendSafe(); ++iter)
   *   cerr << *iter << endl;
   * for (List<int>::const_iterator_safe iter = list2.crbeginSafe();
   *      iter != list2.crendSafe(); --iter)
   *   cerr << *iter << endl;
   *
   * // use an iterator to point the element we wish to erase
   * List<int>::iterator_safe iter = list2.beginSafe();
   * List2.erase ( iter );
   * List<int>::iterator iter2 = list2.begin() + 4; // 5th element of the list
   * iter2 = iter + 4;
   *
   * // map a list into another list (assuming function f is defined as
   * // float f (int x) { return (2.5 * x); } )
   * List<float> flist = list2.map (f);
   * @endcode
   *
   * @tparam Val The values type stored in the gum::List.
   */
  template < typename Val >
  class List {
    public:
    /// Types for STL compliance.
    /// @{
    using value_type          = Val;
    using reference           = Val&;
    using const_reference     = const Val&;
    using pointer             = Val*;
    using const_pointer       = const Val*;
    using size_type           = Size;
    using difference_type     = std::ptrdiff_t;
    using iterator            = ListIterator< Val >;
    using const_iterator      = ListConstIterator< Val >;
    using iterator_safe       = ListIteratorSafe< Val >;
    using const_iterator_safe = ListConstIteratorSafe< Val >;
    /// @}

    /// Locations around iterators where insertions of new elements can take /
    /// place.
    enum class location {
      BEFORE,
      AFTER
    };

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief A basic constructor that creates an empty list.
     */
    explicit List();

    /**
     * @brief Copy constructor.
     *
     * The new list and that which is copied do not share their elements: the
     * new list contains new instances of the values stored in the list to be
     * copied.  Of course if these values are pointers, the new values point
     * toward the same elements. This constructor runs in linear time.
     *
     * @param src the list the contents of which is copied into the current one.
     */
    List(const List< Val >& src);

    /**
     * @brief Move constructor.
     * @param src The gum::List to move.
     */
    List(List< Val >&& src) noexcept;

    /**
     * @brief Initializer_list constructor.
     * @param list The initializer list.
     */
    List(std::initializer_list< Val > list);

    /**
     * @brief Class destructor.
     */
    ~List();

    /// @}
    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /**
     * @brief Returns a safe const iterator pointing to the end of the List.
     *
     * Safe const iterators are const iterators whose state is updated by the
     * list when the element they point to is erased. As such, in this case,
     * they can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step
     *
     * @brief Returns a safe const iterator pointing to the end of the List.
     */
    const const_iterator_safe& cendSafe() const noexcept;

    /**
     * @brief Returns a safe iterator pointing to the end of the List.
     *
     * Safe iterators are iterators whose state is updated by the list when the
     * element they point to is erased. As such, in this case, they can throw
     * an exception when we try to derefence them and they are able to perform
     * a valid ++ or -- step.
     *
     * @brief Ceturns a safe iterator pointing to the end of the List.
     */
    const iterator_safe& endSafe() noexcept;

    /**
     * @brief Return a safe const iterator pointing just before the beginning
     * of the List.
     *
     * Safe const iterators are const iterators whose state is updated by the
     * list when the element they point to is erased. As such, in this case,
     * they can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step.
     *
     * @brief Return a safe const iterator pointing just before the beginning
     * of the List.
     */
    const const_iterator_safe& crendSafe() const noexcept;

    /**
     * @brief Returns a safe iterator pointing just before the beginning of the
     * List.
     *
     * Safe iterators are iterators whose state is updated by the list when the
     * element they point to is erased. As such, in this case, they can throw
     * an exception when we try to derefence them and they are able to perform
     * a valid ++ or -- step.
     *
     * @return Returns a safe iterator pointing just before the beginning of
     * the List.
     */
    const iterator_safe& rendSafe() noexcept;

    /**
     * @brief Returns a safe const iterator pointing to the beginning of the
     * List.
     *
     * Safe const iterators are const iterators whose state is updated by the
     * list when the element they point to is erased. As such, in this case,
     * they can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step.
     *
     * @return Returns a safe const iterator pointing to the beginning of the
     * List.
     */
    const_iterator_safe cbeginSafe() const;

    /**
     * @brief Returns a safe iterator pointing to the beginning of the List.
     *
     * Safe iterators are iterators whose state is updated by the list when the
     * element they point to is erased. As such, in this case, they can throw
     * an exception when we try to derefence them and they are able to perform
     * a valid ++ or -- step.
     *
     * @return Returns a safe iterator pointing to the beginning of the List.
     */
    iterator_safe beginSafe();

    /**
     * @brief Returns a safe const iterator pointing to the last element of the
     * List.
     *
     * Safe const iterators are const iterators whose state is updated by the
     * list when the element they point to is erased. As such, in this case,
     * they can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step.
     *
     * @return Returns a safe const iterator pointing to the last element of
     * the List.
     */
    const_iterator_safe crbeginSafe() const;

    /**
     * @brief Returns a safe iterator pointing to the last element of the List.
     *
     * Safe iterators are iterators whose state is updated by the list when the
     * element they point to is erased. As such, in this case, they can throw
     * an exception when we try to derefence them and they are able to perform
     * a valid ++ or -- step.
     *
     * @return Returns a safe iterator pointing to the last element of the
     * List.
     */
    iterator_safe rbeginSafe();

    /**
     * @brief Returns an unsafe const iterator pointing to the end of the List.
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a
     * mess, probably a segfault. You should use them only when performance is
     * an issue and if you are sure that they will never point to an element
     * erased.
     *
     * @brief Returns an unsafe const iterator pointing to the end of the List.
     */
    const const_iterator& cend() const noexcept;

    /**
     * @brief Returns an unsafe iterator pointing to the end of the List.
     *
     * Unsafe iterators are a little bit faster than safe iterators and they
     * consume less memory. However, if the element they point to is erased,
     * their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe iterator pointing to the end of the List.
     */
    const iterator& end() noexcept;

    /**
     * @brief Returns an unsafe const iterator pointing to the end of the List.
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a
     * mess, probably a segfault. You should use them only when performance is
     * an issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe const iterator pointing to the end of the
     * List.
     */
    const const_iterator& end() const noexcept;

    /**
     * @brief Returns an unsafe const iterator pointing just before the
     * beginning of the List
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a
     * mess, probably a segfault. You should use them only when performance is
     * an issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe const iterator pointing just before the
     * beginning of the List
     */
    const const_iterator& crend() const noexcept;

    /**
     * @brief Returns an unsafe iterator pointing just before the beginning of
     * the List.
     *
     * Unsafe iterators are a little bit faster than safe iterators and they
     * consume less memory. However, if the element they point to is erased,
     * their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an element
     * erased.
     *
     * @brief Returns an unsafe iterator pointing just before the beginning of
     * the List.
     */
    const iterator& rend() noexcept;

    /**
     * @brief Returns an unsafe const iterator pointing just before the
     * beginning of the List.
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a
     * mess, probably a segfault. You should use them only when performance is
     * an issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe const iterator pointing just before the
     * beginning of the List.
     */
    const const_iterator& rend() const noexcept;

    /**
     * @brief Returns an unsafe const iterator pointing to the beginning of the
     * List.
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a
     * mess, probably a segfault. You should use them only when performance is
     * an issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe const iterator pointing to the beginning of
     * the List.
     */
    const_iterator cbegin() const;

    /**
     * @brief Returns an unsafe iterator pointing to the beginning of the List.
     *
     * Unsafe iterators are a little bit faster than safe iterators and they
     * consume less memory. However, if the element they point to is erased,
     * their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe iterator pointing to the beginning of the
     * List.
     */
    iterator begin();

    /**
     * @brief Returns an unsafe const iterator pointing to the beginning of the
     * List.
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a
     * mess, probably a segfault. You should use them only when performance is
     * an issue and if you are sure that they will never point to an element
     * erased.
     *
     * @brief Returns an unsafe const iterator pointing to the beginning of the
     * List.
     */
    const_iterator begin() const;

    /**
     * @brief Returns an unsafe const iterator pointing to the last element of
     * the List.
     *
     * Unsafe iterators are a little bit faster than safe iterators and they
     * consume less memory. However, if the element they point to is erased,
     * their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe const iterator pointing to the last element of
     * the List.
     */
    const_iterator crbegin() const;

    /**
     * @brief Returns an unsafe iterator pointing to the last element of the
     * List.
     *
     * Unsafe iterators are a little bit faster than safe iterators and they
     * consume less memory. However, if the element they point to is erased,
     * their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe iterator pointing to the last element of the
     * List.
     */
    iterator rbegin();

    /**
     * @brief Returns an unsafe const iterator pointing to the last element of
     * the List.
     *
     * Unsafe iterators are a little bit faster than safe iterators and they
     * consume less memory. However, if the element they point to is erased,
     * their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an element
     * erased.
     *
     * @return Returns an unsafe const iterator pointing to the last element of
     * the List.
     */
    const_iterator rbegin() const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Inserts a new element (a copy) at the beginning of the chained
     * list.
     *
     * The value passed in argument is not actually inserted into the list:
     * this is a copy of this value that is inserted. The method runs in
     * constant time.
     *
     * @param val The valus pushed at the front.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& pushFront(const Val& val);

    /**
     * @brief Inserts a new element (a move) at the beginning of the chained
     * list.
     *
     * @param val The valus pushed at the front.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& pushFront(Val&& val);

    /**
     * @brief An alias for pushFront used for STL compliance.
     *
     * Defining push_front allows using, for instance, FrontInserters.
     *
     * @tparam Args The emplace values type.
     * @param args The emplace values.
     * @return Returns a reference on the copy inserted into the list.
     */
    template < typename... Args >
    Val& push_front(Args&&... args);

    /**
     * @brief Emplace elements at the beginning of the chained list.
     *
     * Emplace is a method that allows to construct directly an element of type
     * Val by passing to its constructor all the arguments it needs
     *
     * @param args The arguments passed to the constructor.
     * @return Returns a reference on the copy inserted into the list.
     */
    template < typename... Args >
    Val& emplaceFront(Args&&... args);

    /**
     * @brief Inserts a new element (a copy) at the end of the chained list.
     *
     * The value passed in argument is not actually inserted into the list:
     * this is a copy of this value that is inserted. The method runs in
     * constant time.
     *
     * @param val The value pushed back.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& pushBack(const Val& val);

    /**
     * @brief Inserts a new element (a move) at the end of the chained list.
     *
     * The value passed in argument is not actually inserted into the list:
     * this is a copy of this value that is inserted. The method runs in
     * constant time.
     *
     * @param val The value pushed back.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& pushBack(Val&& val);

    /**
     * @brief An alias for pushBack used for STL compliance.
     *
     * Defining push_back allows using, for instance, BackInserters.
     * @tparam Args The emplace arguments type.
     * @param args The emplace arguments values.
     * @return Returns a reference on the copy inserted into the list.
     */
    template < typename... Args >
    Val& push_back(Args&&... args);

    /**
     * @brief Emplace elements at the end of the chained list.
     *
     * Emplace is a method that allows to construct directly an element of type
     * Val by passing to its constructor all the arguments it needs
     *
     * @tparam Args The emplaced arguments types.
     * @param args The arguments passed to the constructor
     * @return A reference on the copy inserted into the list.
     */
    template < typename... Args >
    Val& emplaceBack(Args&&... args);

    /**
     * @brief Inserts a new element at the end of the chained list (alias of
     * pushBack).
     *
     * @param val The value inserted.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& insert(const Val& val);

    /**
     * @brief Inserts a new element at the end of the chained list (alias of
     * pushBack).
     *
     * @param val The value inserted.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& insert(Val&& val);

    /**
     * @brief Inserts a new element at the ith pos of the chained list.
     *
     * The first element of the list is at pos 0. After the insert, the element
     * is placed precisely at pos if pos is less than the size of the list
     * before insertion, else it is inserted at the end of the list.
     *
     * @param pos The position where to inser the new element.
     * @param val The value to insert.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& insert(Size pos, const Val& val);

    /**
     * @brief Insert an rvalue at the ith pos of the chained list.
     *
     * The first element of the list is at pos 0. After the insert, the element
     * is placed precisely at pos if pos is less than the size of the list
     * before insertion, else it is inserted at the end of the list.
     *
     * @param pos The position where to inser the new element.
     * @param val The value to insert.
     * @return Returns a reference on the copy inserted into the list.
     */
    Val& insert(Size pos, Val&& val);

    /**
     * @brief Inserts a new element before or after a given iterator.
     *
     * @param iter The iterator pointing where to inser the new element.
     * @param val The value to insert.
     * @param place Defines where to insert the new element relatively to iter.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& insert(const const_iterator_safe& iter, const Val& val, location place = location::BEFORE);

    /**
     * @brief Inserts an rvalue before or after a given iterator.
     *
     * @param iter The iterator pointing where to inser the new element.
     * @param val The value to insert.
     * @param place Defines where to insert the new element relatively to iter.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather,
     * it is a copy of val that is inserted.
     */
    Val& insert(const const_iterator_safe& iter, Val&& val, location place = location::BEFORE);

    /**
     * @brief Inserts a new element before or after a given iterator.
     *
     * @param iter The iterator pointing where to inser the new element.
     * @param val The value to insert.
     * @param place Defines where to insert the new element relatively to iter.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     */
    Val& insert(const const_iterator& iter, const Val& val, location place = location::BEFORE);

    /**
     * @brief Inserts an rvalue before or after a given iterator.
     *
     * @param iter The iterator pointing where to inser the new element.
     * @param val The value to insert.
     * @param place Defines where to insert the new element relatively to iter.
     * @return Returns a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather,
     * it is a copy of val that is inserted.
     */
    Val& insert(const const_iterator& iter, Val&& val, location place = location::BEFORE);

    /**
     * @brief Emplace a new element before a given iterator.
     *
     * Emplace is a method that allows to construct directly an element of type
     * Val by passing to its constructor all the arguments it needs.  The first
     * element of the list is at pos 0. After the insert, the element is placed
     * precisely at pos if pos is less than the size of the list before
     * insertion, else it is inserted at the end of the list.
     *
     * @param iter The position in the list
     * @param args The arguments passed to the constructor
     * @return Returns a reference on the copy inserted into the list.
     */
    template < typename... Args >
    Val& emplace(const const_iterator& iter, Args&&... args);

    /**
     * @brief Emplace a new element before a given safe iterator.
     *
     * Emplace is a method that allows to construct directly an element of type
     * Val by passing to its constructor all the arguments it needs.  The first
     * element of the list is at pos 0. After the insert, the element is placed
     * precisely at pos if pos is less than the size of the list before
     * insertion, else it is inserted at the end of the list.
     *
     * @param iter The position in the list.
     * @param args the arguments passed to the constructor.
     * @return Returns a reference on the copy inserted into the list.
     */
    template < typename... Args >
    Val& emplace(const const_iterator_safe& iter, Args&&... args);

    /**
     * @brief Returns a reference to first element of a list, if any.
     * @throw NotFound exception is thrown if the list is empty.
     */
    Val& front() const;

    /**
     * @brief Returns a reference to last element of a list, if any.
     * @throw NotFound exception is thrown if the list is empty.
     */
    Val& back() const;

    /**
     * @brief Returns the number of elements in the list.
     * This method runs in constant time.
     */
    Size size() const noexcept;

    /**
     * @brief Checks whether there exists a given element in the list.
     *
     * This method runs in linear time.
     *
     * Comparisons between Val instances are performed through == operators.
     *
     * @param val the value of the element we wish to check the existence of.
     * @return Returns true if val is in the gum::List.
     */
    bool exists(const Val& val) const;

    /**
     * @brief Erases the ith element of the List (the first one is in position
     * 0).
     *
     * If the element cannot be found, the function returns without throwing
     * any exception. It runs in linear time in the size of the list.
     *
     * @param i The position in the list of the element we wish to remove.
     */
    void erase(Size i);

    /**
     * @brief Erases the element of the List pointed to by the safe iterator.
     *
     * If the element cannot be found, i.e., it has already been erased or the
     * iterator points to end/rend, the function returns without throwing any
     * exception. It runs in linear time in the size of the list.
     *
     * @param iter An iterator pointing to the element to remove.
     */
    void erase(const iterator_safe& iter);

    /**
     * @brief Erases the element of the List pointed to by the safe const
     * iterator.
     *
     * If the element cannot be found, i.e., it has already been erased or the
     * iterator points to end/rend, the function returns without throwing any
     * exception. It runs in linear time in the size of the list.
     *
     * @param iter An iterator pointing to the element to remove.
     */
    void erase(const const_iterator_safe& iter);

    /**
     * @brief erases the first element encountered with a given value.
     *
     * If no element equal to \e val can be found, the function returns without
     * throwing any exception. It runs in linear time both in the size of the
     * list and in the number of iterators referenced in the list.  Comparisons
     * between Val instances are performed through == operators.
     *
     * @param val The value of the element we wish to remove.
     */
    void eraseByVal(const Val& val);

    /**
     * @brief erases all the elements encountered with a given value
     *
     * If no element equal to \e val can be found, the function returns without
     * throwing any exception.
     *
     * Comparisons between Val instances are performed through == operators.
     *
     * @param val the value of the element we wish to remove.
     */
    void eraseAllVal(const Val& val);

    /**
     * @brief Removes the last element of a List, if any.
     *
     * When the list is empty, it does not do anything.
     */
    void popBack();

    /**
     * @brief Removes the first element of a List, if any.
     *
     * When the list is empty, it does not do anything.
     */
    void popFront();

    /**
     * @brief Deletes all the elements of a chained list.
     *
     * All the iterators of the list will be resetted to rend. The method runs
     * in linear time of both the size of the list and the number of iterators
     * attached to the List.
     */
    void clear();

    /**
     * @brief Returns a boolean indicating whether the chained list is empty.
     * @return Returns a boolean indicating whether the chained list is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Swap the current list with another one.
     * @param other_list The list to swap elements with.
     */
    void swap(List& other_list);

    /**
     * @brief Converts a list into a string.
     * @return Returns a std::string representation of the gum::List.
     */
    std::string toString() const;

    /**
     * @brief Creates a list of mountains from a list of val.
     * @param f A function that maps any Val element into a Mount
     * @return Returns a lsit of mountains.
     * @tparam Mount The type of mountains.
     */
    template < typename Mount >
    List< Mount > map(Mount (*f)(Val)) const;

    /**
     * @brief Creates a list of mountains from a list of val.
     * @param f A function that maps any Val element into a Mount
     * @return Returns a lsit of mountains.
     * @tparam Mount The type of mountains.
     */
    template < typename Mount >
    List< Mount > map(Mount (*f)(Val&)) const;

    /**
     * @brief Creates a list of mountains from a list of val.
     * @param f A function that maps any Val element into a Mount
     * @return Returns a lsit of mountains.
     * @tparam Mount The type of mountains.
     */
    template < typename Mount >
    List< Mount > map(Mount (*f)(const Val&)) const;

    /**
     * @brief Creates a list of mountains with a given value from a list of
     * val.
     * @param mount the value taken by all the elements of the resulting list
     * @return Returns a lsit of mountains.
     * @tparam Mount The type of mountains.
     */
    template < typename Mount >
    List< Mount > map(const Mount& mount) const;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * The new list and that which is copied do not share the elements: the new
     * list contains new instances of the values stored in the list to be
     * copied. Of course if these values are pointers, the new values point
     * toward the same elements. The List on which the operator is applied
     * keeps its iterator's list. Of course, if it previously contained some
     * elements, those are removed prior to the copy. This operator runs in
     * linear time.
     *
     * @warning If the current List previously contained iterators, those will
     * be resetted to end()/rend().
     *
     * @param src the list the content of which will be copied into the current
     * List.
     * @return Returns this gum::List.
     */
    List< Val >& operator=(const List< Val >& src);

    /**
     * @brief Move operator.
     *
     * @param src The gum::List to move.
     * @return Returns this gum::List.
     */
    List< Val >& operator=(List< Val >&& src);

    /**
     * @brief Inserts a new element at the end of the list (alias of pushBack).
     *
     * This enables writing code like \c list \c += \c xxx; to add element \c
     * xxx to the list.
     *
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     *
     * @param val Tha value inserted int the list.
     * @return Returns a reference on the copy inserted into the list.
     */
    Val& operator+=(const Val& val);

    /**
     * @brief Inserts a new element at the end of the list (alias of pushBack).
     *
     * This enables writing code like \c list \c += \c xxx; to add element \c
     * xxx to the list.
     *
     * @warning Note that \e val is not actually inserted into the list.
     * Rather, it is a copy of val that is inserted.
     *
     * @param val Tha value inserted int the list.
     * @return Returns a reference on the copy inserted into the list.
     */
    Val& operator+=(Val&& val);

    /**
     * @brief Checks whether two lists are identical (same elements in the same
     * order).
     *
     * This method runs in time linear in the number of elements of the list.
     *
     * @return Returns true if src and this gum::List are identical.
     */
    bool operator==(const List< Val >& src) const;

    /**
     * @brief Checks whether two lists are different (different elements or
     * orders).
     *
     * This method runs in time linear in the number of elements of the list.
     *
     * @return Returns true if src and this gum::List are identical.
     */
    bool operator!=(const List< Val >& src) const;

    /**
     * @brief Returns the ith element in the current chained list.
     *
     * The first of the list element has index 0.
     *
     * This method runs in linear time.
     *
     * @param i The position of the element in the list (0 = first element).
     * @throw NotFound Raised if the element to be retrieved does not exist.
     * @return Returns a reference on the element stored at the ith position in
     * the list.
     */
    Val& operator[](const Size i);

    /**
     * @brief Returns the const ith element in the current chained list.
     *
     * The first of the list element has index 0.
     *
     * This method runs in linear time.
     *
     * @param i the position of the element in the list (0 = first element).
     * @throw NotFound Raised if the element to be retrieved does not exist.
     * @return Returns a reference on the element stored at the ith position in
     * the list.
     */
    const Val& operator[](const Size i) const;

    /// @}

    private:
    /// A pointer on the first element of the chained list.
    ListBucket< Val >* _deb_list_{nullptr};

    /// A pointer on the last element of the chained list.
    ListBucket< Val >* _end_list_{nullptr};

    /// The number of elements in the list.
    Size _nb_elements_{Size(0)};

    /// The list of "safe" iterators attached to the list.
    mutable std::vector< const_iterator_safe* > _safe_iterators_;

    /**
     * @brief A function used to perform copies of elements of Lists.
     *
     * Before performing the copy, we assume in this function that the current
     * list (this) is empty (else there would be memory leak).
     *
     * @param src The gum::List to copy.
     */
    void _copy_elements_(const List< Val >& src);

    /**
     * @brief Returns the bucket corresponding to the ith position in the list.
     *
     * This method assumes that the list contains at least i+1 elements. The
     * index of the first element of the list is 0.
     *
     * @param i The position of the returned element.
     * @return Returns the gum::ListBucket of the ith element.
     */
    ListBucket< Val >* _getIthBucket_(Size i) const noexcept;

    /**
     * @brief Returns the bucket corresponding to a given value.
     *
     * Actually, this is the first bucket of value val encountered in the list,
     * if any, that is returned. If the element cannot be found, 0 is returned.
     * This method enables fast removals of buckets. It runs in linear time.
     *
     * Comparisons between Val instances are performed through == operators.
     *
     * @param val The value of the element the bucket of which we wish to
     * return.
     * @return Returns the bucket corresponding to a given value.
     */
    ListBucket< Val >* _getBucket_(const Val& val) const noexcept;

    /**
     * @brief Removes an element from a chained list.
     *
     * If parameter \e bucket is equal to 0, then the method does not perform
     * anything, else the bucket is deleted. In the latter case, no test is
     * ever performed to check that the bucket does actually belong to the
     * List. The method runs in constant time.
     *
     * @param bucket A pointer on the bucket in the chained list
     * we wish to remove.
     */
    void _erase_(ListBucket< Val >* bucket);

    /**
     * @brief Create a new bucket with a given value.
     * @param val The value of the new bucket.
     * @return Returns the bucket holding val.
     */
    ListBucket< Val >* _createBucket_(const Val& val) const;

    /**
     * @brief Create a new bucket with a given value.
     * @param val The value of the new bucket.
     * @return Returns the bucket holding val.
     */
    ListBucket< Val >* _createBucket_(Val&& val) const;

    /**
     * Create an emplace bucket.
     * @tparam Args The emplace arguments types.
     * @param args The emplace arguments.
     * @return Returns the bucket holding the new value.
     */
    template < typename... Args >
    ListBucket< Val >* _createEmplaceBucket_(Args&&... args) const;

    /**
     * @brief Insert a bucket at the front of the list.
     * @param new_elt The new element pushed at the front of the gum::List.
     * @return Returns a refefence over the value stored in the gum::List.
     */
    Val& _pushFront_(ListBucket< Val >* new_elt);

    /**
     * @brief Insert a bucket at the end of the list.
     * @param new_elt The new element pushed at the end of the gum::List.
     * @return Returns a refefence over the value stored in the gum::List.
     */
    Val& _pushBack_(ListBucket< Val >* new_elt);

    /**
     * @brief Insert a new bucket before another one.
     * @param new_elt The new element to insert in the gum::List.
     * @param current_elt The element before which new_elt will be inserted.
     * @return Returns a reference over the value stored in the gum::List.
     */
    Val& _insertBefore_(ListBucket< Val >* new_elt, ListBucket< Val >* current_elt);

    /**
     * @brief Insert a new bucket after another one.
     * @param new_elt The new element to insert in the gum::List.
     * @param current_elt The element before which new_elt will be inserted.
     * @return Returns a reference over the value stored in the gum::List.
     */
    Val& _insertAfter_(ListBucket< Val >* new_elt, ListBucket< Val >* current_elt);

    /**
     * @brief Inserts a new bucket before or after the location pointed to by
     * an iterator.
     * @param iter An iterator pointing where to insert a new element.
     * @param new_elt The new element ot insert in the gum::List.
     * @param place Where to insert the new element relatively to the iterator.
     * @return Returns a reference over the value stored in the gum::List.
     */
    Val& _insert_(const const_iterator_safe& iter, ListBucket< Val >* new_elt, location place);

    /**
     * @brief Inserts a new bucket before or after the location pointed to by
     * an iterator.
     * @param iter An iterator pointing where to insert a new element.
     * @param new_elt The new element ot insert in the gum::List.
     * @param place Where to insert the new element relatively to the iterator.
     * @return Returns a reference over the value stored in the gum::List.
     */
    Val& _insert_(const const_iterator& iter, ListBucket< Val >* new_elt, location place);

    /// ListIterator should be a friend to optimize access to elements
    /// @{
    friend class ListIterator< Val >;
    friend class ListConstIterator< Val >;
    friend class ListIteratorSafe< Val >;
    friend class ListConstIteratorSafe< Val >;
    /// @}
  };

  // ===========================================================================
  // ===                      UNSAFE LIST CONST ITERATORS                    ===
  // ===========================================================================
  /**
   * @class ListConstIterator
   * @headerfile list.h <agrum/tools/core/list.h>
   * @ingroup list_group
   * @brief Unsafe but fast const iterators for Lists.
   *
   * Class ListConstIterator implements unsafe iterators for List. However,
   * developers may consider using List<x>::const_iterator instead of
   * ListConstIterator<x>.
   *
   * These iterators are fast but they are unaware of changes within the List.
   * Therefore, if they point to an element that is being deleted from memory
   * by the list, their accessing this element will most probably produce a
   * segmentation fault. Similarly, incrementing or decrementing such an
   * iterator pointing to a deleted element will most certainly produce a mess.
   * So, ListConstIterator should be used only if you are sure that they will
   * never point to an element that has been removed from the list (a typical
   * use is to iterate over a const List). Whenever you are not sure that this
   * property holds, use ListConstIteratorSafe<x> or
   * List<x>::const_iterator_safe. Those iterators are a little bit slower but
   * guarantee that no segmentation fault will ever occur.
   *
   * @par Usage example:
   * @code
   * // create a list of strings
   * List<string> list;
   * list.pushBack ("toto"); list.pushBack ("titi");
   *
   * // parse all the elements of a list
   * for ( List<string>::const_iterator iter = list.cbegin ();
   *       iter != list.cend (); ++iter )
   *   cerr << *iter << endl;
   * for ( List<string>::const_iterator iter = list.cbegin ();
   *       iter != list.cend (); iter += 2 ) // step = 2
   *   cerr << *iter << endl;
   * for ( List<string>::const_iterator iter = list.cbegin ();
   *       iter != list.cend (); iter = iter + 2 ) // step = 2
   *   cerr << *iter << endl;
   * for ( List<string>::const_iterator iter = list.crbegin ();
   *       iter != list.crend (); --iter )
   *   cerr << *iter << endl;
   *
   * // use member size() of the strings
   * for ( List<string>::const_iterator iter = list.cbegin ();
   *         iter != list.cend (); ++iter)
   *   cerr << iter->size() << endl;
   * @endcode
   *
   * @tparam Val The gum::List values type.
   */
  template < typename Val >
  class ListConstIterator {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = Val&;
    using const_reference   = const Val&;
    using pointer           = Val*;
    using const_pointer     = const Val*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Returns an iterator pointing toward nothing.
     */
    explicit ListConstIterator() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static cend/crend iterator
    // only list.cpp should use this constructor
    explicit consteval ListConstIterator(StaticInitializer init) noexcept {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for a begin.
     */
    ListConstIterator(const List< Val >& theList) noexcept;

    /**
     * @brief Copy constructor.
     * @param src The gum::ListConstIterator to copy.
     */
    ListConstIterator(const ListConstIterator< Val >& src) noexcept;

    /**
     * @brief Move constructor.
     * @param src The gum::ListConstIterator to move.
     */
    ListConstIterator(ListConstIterator< Val >&& src) noexcept;

    /**
     * @brief Constructor for an iterator pointing to the \e ind_eltth element
     * of a List.
     * @param theList The list to iterate over.
     * @param ind_elt The iterator starting position.
     * @throw UndefinedIteratorValue Raised if the element does not exist in
     * the list.
     */
    ListConstIterator(const List< Val >& theList, Size ind_elt);

    /**
     * @brief Class Desctructor.
     */
    ~ListConstIterator() noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Makes the iterator point toward nothing.
     *
     * A method for detaching the iterator from the List it is attached to.
     * After being detached, the iterator does not point to any element, i.e.,
     * trying to access its content will raise an exception.
     */
    void clear() noexcept;

    /**
     * @brief Positions the iterator to the end of the list.
     */
    void setToEnd() noexcept;

    /**
     * @brief Returns a bool indicating whether the iterator points to the end
     * of the list.
     * @return Returns a bool indicating whether the iterator points to the end
     * of the list.
     */
    bool isEnd() const noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * The current iterator now points to the same element as iterator \e from.
     *
     * @param src The gum::ListConstIterator to copy.
     * @return Returns this gum::ListConstIterator.
     */
    ListConstIterator< Val >& operator=(const ListConstIterator< Val >& src) noexcept;

    /**
     * @brief Move operator.
     *
     * @param src The gum::ListConstIterator to move.
     * @return Returns this gum::ListConstIterator.
     */
    ListConstIterator< Val >& operator=(ListConstIterator< Val >&& src) noexcept;

    /**
     * @brief Makes the iterator point to the next element in the List.
     *
     * @code
     * for (iter = list.begin(); iter != list.end(); ++iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Runs in constant time.
     *
     * @return Returns this gum::ListConstIterator.
     */
    ListConstIterator< Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the List.
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListConstIterator.
     */
    ListConstIterator< Val >& operator+=(difference_type i) noexcept;

    /**
     * @brief Makes the iterator point to the preceding element in the List.
     *
     * @code
     * for (iter = list.rbegin(); iter != list.rend(); --iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Runs in constant time.
     *
     * @return Returns this gum::ListConstIterator.
     */
    ListConstIterator< Val >& operator--() noexcept;

    /**
     * @brief Makes the iterator point to i elements befor in the List.
     *
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListConstIterator.
     */
    ListConstIterator< Val >& operator-=(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i further elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListConstIterator.
     */
    ListConstIterator< Val > operator+(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i preceding elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListConstIterator.
     */
    ListConstIterator< Val > operator-(difference_type i) noexcept;

    /**
     * @brief Checks whether two iterators point toward different elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListConstIterator to test for inequality.
     * @return Returns true if src and this are equal.
     */
    bool operator!=(const ListConstIterator< Val >& src) const noexcept;

    /**
     * @brief Checks whether two iterators point toward the same elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListConstIterator to test for equality.
     * @return Returns true if src and this are equal.
     */
    bool operator==(const ListConstIterator< Val >& src) const noexcept;

    /**
     * @brief Gives access to the content of the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the content of the iterator.
     */
    const Val& operator*() const;

    /**
     * @brief Dereferences the value pointed to by the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the value pointed to by the iterator.
     */
    const Val* operator->() const;

    /// @}

    private:
    /**
     * @brief Class List must be a friend because it uses the getBucket method
     * to speed up some processes.
     */
    friend class List< Val >;

    /// The bucket in the chained list pointed to by the iterator.
    ListBucket< Val >* _bucket_{nullptr};

    /// Returns the bucket the iterator is pointing to.
    ListBucket< Val >* _getBucket_() const noexcept;
  };

  /// For STL compliance, a distance operator.
  template < typename Val >
  typename ListConstIterator< Val >::difference_type
     operator-(const ListConstIterator< Val >& iter1, const ListConstIterator< Val >& iter2);

  // ===========================================================================
  // ===                         UNSAFE LIST ITERATORS                       ===
  // ===========================================================================

  /**
   * @class ListIterator
   * @headerfile list.h <agrum/tools/core/list.h>
   * @ingroup list_group
   * @brief Unsafe but fast iterators for Lists.
   *
   * Class ListIterator implements iterators for List. However, developers may
   * consider using List<x>::iterator instead of ListIterator<x>.
   *
   * These iterators are fast but they are unaware of changes within the List.
   * Therefore, if they point to an element that is being deleted from memory
   * by the list, their accessing this element will most probably produce a
   * segmentation fault. Similarly, incrementing or decrementing such an
   * iterator pointing to a deleted element will most certainly produce a mess.
   * So, ListIterator should be used only if you are sure that they will never
   * point to an element that has been removed from the list (a typical use is
   * to iterate over a const List). Whenever you are not sure that this
   * property holds, use ListIteratorSafe<x> or List<x>::iterator_safe.  Those
   * iterators are a little bit slower but guarantee that no segmentation fault
   * will ever occur.
   *
   * @par Usage example:
   * @code
   * // create a list of strings
   * List<string> list;
   * list.pushBack ("toto"); list.pushBack ("titi");
   *
   * // parse all the elements of a list
   * for ( List<string>::iterator iter = list.begin ();
   *       iter != list.end (); ++iter )
   *   cerr << *iter << endl;
   * for ( List<string>::iterator iter = list.begin ();
   *       iter != list.end (); iter += 2 ) // step = 2
   *   cerr << *iter << endl;
   * for ( List<string>::iterator iter = list.begin ();
   *       iter != list.end (); iter = iter + 2 ) // step = 2
   *   cerr << *iter << endl;
   * for ( List<string>::iterator iter = list.rbegin ();
   *       iter != list.rend (); --iter )
   *   cerr << *iter << endl;
   *
   * // use member size() of the strings
   * for ( List<string>::iterator iter = list.begin ();
   *       iter != list.end (); ++iter)
   *   cerr << iter->size() << endl;
   * @endcode
   *
   * @tparam Val The gum::List values type.
   */
  template < typename Val >
  class ListIterator: public ListConstIterator< Val > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = Val&;
    using const_reference   = const Val&;
    using pointer           = Val*;
    using const_pointer     = const Val*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Returns an iterator pointing toward nothing.
     */
    explicit ListIterator() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static end/rend iterator
    // only list.cpp should use this constructor
    explicit consteval ListIterator(StaticInitializer init) noexcept :
        ListConstIterator< Val >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for a begin.
     * @param theList The list to iterate over.
     */
    ListIterator(const List< Val >& theList) noexcept;

    /**
     * @brief Copy constructor.
     * @param src The gum::ListIterator to copy.
     */
    ListIterator(const ListIterator< Val >& src) noexcept;

    /**
     * @brief Move constructor.
     * @param src The gum::ListIterator to move.
     */
    ListIterator(ListIterator< Val >&& src) noexcept;

    /**
     * @brief Constructor for an iterator pointing to the \e ind_eltth element
     * of a List.
     * @param theList The gum::List to iterate over.
     * @param ind_elt The position to point to.
     * @throw UndefinedIteratorValue Raised if the element does not exist in
     * the list.
     */
    ListIterator(const List< Val >& theList, Size ind_elt);

    /**
     * @brief Class destructor.
     */
    ~ListIterator() noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    using ListConstIterator< Val >::clear;
    using ListConstIterator< Val >::setToEnd;
    using ListConstIterator< Val >::isEnd;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * The current iterator now points to the same element as iterator \e src.
     *
     * @param src The gum::ListIterator to copy.
     * @return Returns this gum::ListIterator.
     */
    ListIterator< Val >& operator=(const ListIterator< Val >& src) noexcept;

    /// move operator
    /**
     * @brief Move operator.
     *
     * The current iterator now points to the same element as iterator \e src.
     *
     * @param src The gum::ListIterator to move.
     * @return Returns this gum::ListIterator.
     */
    ListIterator< Val >& operator=(ListIterator< Val >&& src) noexcept;

    /**
     * @brief Makes the iterator point to the next element in the List.
     *
     * @code
     * for (iter = list.begin(); iter != list.end(); ++iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Deleting elements during the loop is guaranteed to never produce a
     * segmentation fault. Runs in constant time.
     *
     * @return Returns this gum::ListIterator.
     */
    ListIterator< Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the List.
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListIterator.
     */
    ListIterator< Val >& operator+=(difference_type i) noexcept;

    /**
     * @brief Makes the iterator point to the preceding element in the List.
     *
     * @code
     * for (iter = list.rbegin(); iter != list.rend(); --iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Runs in constant time.
     *
     * @return Returns this gum::ListIterator.
     */
    ListIterator< Val >& operator--() noexcept;

    /**
     * @brief Makes the iterator point to i elements befor in the List.
     *
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListIterator.
     */
    ListIterator< Val >& operator-=(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i further elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListIterator.
     */
    ListIterator< Val > operator+(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i preceding elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListIterator.
     */
    ListIterator< Val > operator-(difference_type i) noexcept;

    /**
     * @brief Checks whether two iterators point toward the same elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListIterator to test for equality.
     * @return Returns true if src and this are equal.
     */
    bool operator==(const ListIterator< Val >& src) const noexcept;

    /**
     * @brief Checks whether two iterators point toward different elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListIterator to test for inequality.
     * @return Returns true if src and this are equal.
     */
    bool operator!=(const ListIterator< Val >& src) const noexcept;

    /**
     * @brief Gives access to the content of the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the content of the iterator.
     */
    const Val& operator*() const;

    /**
     * @brief Gives access to the iterator's content.
     * @return Returns the iterator content.
     * @throw UndefinedIteratorValue Raised if the iterator is pointing toward
     * nothing.
     */
    Val& operator*();

    /**
     * @brief Dereferences the value pointed to by the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the value pointed to by the iterator.
     */
    const Val* operator->() const;

    /**
     * @brief Dereferences the value pointed to by the iterator.
     * @return Returns the iterator content.
     * @throw UndefinedIteratorValue Raised if the iterator is pointing toward
     * nothing.
     */
    Val* operator->();

    /// @}
  };

  // ===========================================================================
  // ===                          LIST CONST ITERATORS                       ===
  // ===========================================================================
  /**
   * @class ListConstIteratorSafe
   * @headerfile list.h <agrum/tools/core/list.h>
   * @ingroup list_group
   * @brief Safe const iterators for Lists.
   *
   * Class ListConstIteratorSafe implements safe const iterators for List.
   * However, developers may consider using List<x>::const_iterator_safe
   * instead of ListConstIteratorSafe<x>.
   *
   * These const iterators ensure that whenever they point to an element that
   * is being deleted from memory, their accessing this element will never
   * produce a segmentation fault but rather throw an exception. Similarly,
   * incrementing or decrementing an iterator pointing to a deleted element is
   * guaranteed to make the iterator point on the next (or preceding) element
   * that has not been deleted.
   *
   * @par Usage example:
   * @code
   * // create a list of strings
   * List<string> list;
   * list.pushBack ("toto"); list.pushBack ("titi");
   *
   * // parse all the elements of a list
   * for ( List<string>::const_iterator_safe iter = list.cbeginSafe ();
   *       iter != list.cendSafe (); ++iter )
   *   cerr << *iter << endl;
   * for ( List<string>::const_iterator_safe iter = list.cbeginSafe ();
   *       iter != list.cendSafe (); iter += 2 ) // step = 2
   *   cerr << *iter << endl;
   * for ( List<string>::const_iterator_safe iter = list.cbeginSafe ();
   *       iter != list.cendSafe (); iter = iter + 2 ) // step = 2
   *   cerr << *iter << endl;
   * for ( List<string>::const_iterator_safe iter = list.crbeginSafe ();
   *       iter != list.crendSafe (); --iter )
   *   cerr << *iter << endl;
   *
   * // use member size() of the strings
   * for ( List<string>::const_iterator_safe iter = list.cbeginSafe ();
   *       iter != list.cendSafe (); ++iter )
   *   cerr << iter->size() << endl;
   * @endcode
   *
   * @tparam Val The gum::List values type.
   */
  template < typename Val >
  class ListConstIteratorSafe {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = Val&;
    using const_reference   = const Val&;
    using pointer           = Val*;
    using const_pointer     = const Val*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Returns an iterator pointing toward nothing.
     */
    explicit ListConstIteratorSafe() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static cendSafe/crendSafe iterator
    // only list.cpp should use this constructor
    explicit consteval ListConstIteratorSafe(StaticInitializer init) noexcept {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for a begin.
     */
    ListConstIteratorSafe(const List< Val >& theList);

    /**
     * @brief Copy constructor.
     * @param src The gum::ListConstIteratorSafe to copy.
     */
    ListConstIteratorSafe(const ListConstIteratorSafe< Val >& src);

    /**
     * @brief Constructor for an iterator pointing to the \e ind_eltth element
     * of a List.
     * @param theList The list to iterate over.
     * @param ind_elt The iterator starting position.
     * @throw UndefinedIteratorValue Raised if the element does not exist in
     * the list.
     */
    ListConstIteratorSafe(const List< Val >& theList, Size ind_elt);

    /**
     * @brief Move constructor.
     * @param src The gum::ListConstIterator to move.
     */
    ListConstIteratorSafe(ListConstIteratorSafe< Val >&& src);

    /**
     * @brief Class Desctructor.
     */
    ~ListConstIteratorSafe();

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Makes the iterator point toward nothing.
     *
     * A method for detaching the iterator from the List it is attached to.  It
     * is mainly used by the List when the latter is deleted while the iterator
     * is still alive. After being detached, the iterator does not point to any
     * element, i.e., trying to access its content will raise an exception.
     */
    void clear();

    /**
     * @brief Positions the iterator to the end of the list.
     */
    void setToEnd();

    /**
     * @brief Returns a bool indicating whether the iterator points to the end
     * of the list.
     * @return Returns a bool indicating whether the iterator points to the end
     * of the list.
     */
    bool isEnd() const;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * The current iterator now points to the same element as iterator \e from.
     *
     * @param src The gum::ListConstIteratorSafe to copy.
     * @return Returns this gum::ListConstIteratorSafe.
     */
    ListConstIteratorSafe< Val >& operator=(const ListConstIteratorSafe< Val >& src);

    /**
     * @brief Move operator.
     *
     * @param src The gum::ListConstIteratorSafe to move.
     * @return Returns this gum::ListConstIteratorSafe.
     */
    ListConstIteratorSafe< Val >& operator=(ListConstIteratorSafe< Val >&& src);

    /**
     * @brief Makes the iterator point to the next element in the List.
     *
     * @code
     * for (iter = list.begin(); iter != list.end(); ++iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Runs in constant time.
     *
     * @return Returns this gum::ListConstIteratorSafe.
     */
    ListConstIteratorSafe< Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the List.
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListConstIterator.
     */
    ListConstIteratorSafe< Val >& operator+=(difference_type i) noexcept;

    /**
     * @brief Makes the iterator point to the preceding element in the List.
     *
     * @code
     * for (iter = list.rbegin(); iter != list.rend(); --iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Runs in constant time.
     *
     * @return Returns this gum::ListConstIteratorSafe.
     */
    ListConstIteratorSafe< Val >& operator--() noexcept;

    /**
     * @brief Makes the iterator point to i elements befor in the List.
     *
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListConstIteratorSafe.
     */
    ListConstIteratorSafe< Val >& operator-=(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i further elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListConstIteratoSafe.
     */
    ListConstIteratorSafe< Val > operator+(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i preceding elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListConstIteratorSafe.
     */
    ListConstIteratorSafe< Val > operator-(difference_type i) noexcept;

    /**
     * @brief Checks whether two iterators point toward different elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListConstIteratorSafe to test for inequality.
     * @return Returns true if src and this are equal.
     */
    bool operator!=(const ListConstIteratorSafe< Val >& src) const;

    /**
     * @brief Checks whether two iterators point toward the same elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListConstIteratorSafe to test for equality.
     * @return Returns true if src and this are equal.
     */
    bool operator==(const ListConstIteratorSafe< Val >& src) const;

    /**
     * @brief Gives access to the content of the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the content of the iterator.
     */
    const Val& operator*() const;

    /**
     * @brief Dereferences the value pointed to by the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the value pointed to by the iterator.
     */
    const Val* operator->() const;

    /// @}

    private:
    /// class List must be a friend because it uses the getBucket method to
    /// speed up some processes.
    /// @{
    friend class List< Val >;
    friend class ListConstIterator< Val >;
    /// @}

    /// The list the iterator is pointing to.
    const List< Val >* _list_{nullptr};

    /// The bucket in the chained list pointed to by the iterator.
    ListBucket< Val >* _bucket_{nullptr};

    /// The bucket we should start from when we are pointing on a deleted
    /// bucket and we decide to do a ++.
    ListBucket< Val >* _next_current_bucket_{nullptr};

    /// The bucket we should start from when we are pointing on a deleted
    /// bucket and we decide to do a --.
    ListBucket< Val >* _prev_current_bucket_{nullptr};

    /// Indicates whether the bucket the iterator points to has been deleted.
    bool _null_pointing_{false};

    /// Returns the bucket the iterator is pointing to.
    ListBucket< Val >* _getBucket_() const noexcept;

    /// Remove the iterator for its list' safe iterators list.
    void _removeFromSafeList_() const;

    /// Makes the iterator point to the next element in the List.
    ListConstIteratorSafe< Val >& _opPlus_(Size i) noexcept;

    /// Makes the iterator point to i elements before in the List.
    ListConstIteratorSafe< Val >& _opMinus_(Size i) noexcept;
  };

  /// For STL compliance, a distance operator.
  template < typename Val >
  typename ListConstIteratorSafe< Val >::difference_type
     operator-(const ListConstIteratorSafe< Val >& iter1,
               const ListConstIteratorSafe< Val >& iter2);

  // ===========================================================================
  // ===                             LIST ITERATORS                          ===
  // ===========================================================================

  /**
   * @class ListIteratorSafe
   * @headerfile list.h <agrum/tools/core/list.h>
   * @ingroup basicstruct_group
   * @brief Safe iterators for Lists.
   *
   * Class ListIteratorSafe implements iterators for List. However, developers
   * may consider using List<x>::iterator_safe instead of ListIteratorSafe<x>.
   *
   * These iterators ensure that whenever they point to an element that is
   * being deleted from memory, their accessing this element will never produce
   * a segmentation fault but rather throw an exception. Similarly,
   * incrementing or decrementing an iterator pointing to a deleted element is
   * guaranteed to make the iterator point on the next (or preceding) element
   * that has not been deleted. This enables safely writing code like:
   *
   * @code
   * for ( iter = mylist.beginSafe (); iter != mylist.endSafe (); ++iter )
   *   list.erase ( iter );
   * @endcode
   *
   * @par Usage example:
   * @code
   * // create a list of strings
   * List<string> list;
   * list.pushBack ("toto"); list.pushBack ("titi");
   *
   * // parse all the elements of a list
   * for (List<string>::iterator_safe iter = list.beginSafe();
   *      iter != list.endSafe(); ++iter)
   *   cerr << *iter << endl;
   * for ( List<string>::iterator_safe iter = list.beginSafe ();
   *       iter != list.endSafe (); iter += 2 ) // step = 2
   *   cerr << *iter << endl;
   * for ( List<string>::iterator_safe iter = list.beginSafe ();
   *       iter != list.endSafe (); iter = iter + 2 ) // step = 2
   *   cerr << *iter << endl;
   * for (List<string>::iterator_safe iter = list.rbeginSafe();
   *      iter != list.rendSafe(); --iter)
   *   cerr << *iter << endl;
   *
   * // use member size() of the strings
   * for (List<string>::iterator_safe iter = list.beginSafe();
   *      iter != list.endSafe(); ++iter)
   *   cerr << iter->size() << endl;
   * @endcode
   *
   * @tparam Val The gum::List values type.
   */
  template < typename Val >
  class ListIteratorSafe: public ListConstIteratorSafe< Val > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = Val&;
    using const_reference   = const Val&;
    using pointer           = Val*;
    using const_pointer     = const Val*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Returns an iterator pointing toward nothing.
     */
    explicit ListIteratorSafe() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static endSafe/rendSafe iterator
    // only list.cpp should use this constructor
    explicit consteval ListIteratorSafe(StaticInitializer init) noexcept :
        ListConstIteratorSafe< Val >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for a begin.
     */
    ListIteratorSafe(const List< Val >& theList);

    /**
     * @brief Copy constructor.
     * @param src The gum::ListConstIteratorSafe to copy.
     */
    ListIteratorSafe(const ListIteratorSafe< Val >& src);

    /**
     * @brief Constructor for an iterator pointing to the \e ind_eltth element
     * of a List.
     * @param theList The list to iterate over.
     * @param ind_elt The iterator starting position.
     * @throw UndefinedIteratorValue Raised if the element does not exist in
     * the list.
     */
    ListIteratorSafe(const List< Val >& theList, Size ind_elt);

    /**
     * @brief Move constructor.
     * @param src The gum::ListConstIterator to move.
     */
    ListIteratorSafe(ListIteratorSafe< Val >&& src);

    /**
     * @brief Class Desctructor.
     */
    ~ListIteratorSafe();

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    using ListConstIteratorSafe< Val >::clear;
    using ListConstIteratorSafe< Val >::setToEnd;
    using ListConstIteratorSafe< Val >::isEnd;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * The current iterator now points to the same element as iterator \e from.
     *
     * @param src The gum::ListIteratorSafe to copy.
     * @return Returns this gum::ListIteratorSafe.
     */
    ListIteratorSafe< Val >& operator=(const ListIteratorSafe< Val >& src);

    /**
     * @brief Move operator.
     *
     * @param src The gum::ListIteratorSafe to move.
     * @return Returns this gum::ListIteratorSafe.
     */
    ListIteratorSafe< Val >& operator=(ListIteratorSafe< Val >&& src);

    /**
     * @brief Makes the iterator point to the next element in the List.
     *
     * @code
     * for (iter = list.begin(); iter != list.end(); ++iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Runs in constant time.
     *
     * @return Returns this gum::ListIteratorSafe.
     */
    ListIteratorSafe< Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the List.
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListIterator.
     */
    ListIteratorSafe< Val >& operator+=(difference_type i) noexcept;

    /**
     * @brief Makes the iterator point to the preceding element in the List.
     *
     * @code
     * for (iter = list.rbegin(); iter != list.rend(); --iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole List as long as no
     * element is added to or deleted from the List while being in the loop.
     * Runs in constant time.
     *
     * @return Returns this gum::ListIteratorSafe.
     */
    ListIteratorSafe< Val >& operator--() noexcept;

    /**
     * @brief Makes the iterator point to i elements befor in the List.
     *
     * @param i The number of steps to move the iterator.
     * @return Returns this gum::ListIteratorSafe.
     */
    ListIteratorSafe< Val >& operator-=(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i further elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListConstIteratoSafe.
     */
    ListIteratorSafe< Val > operator+(difference_type i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i preceding elements in the
     * gum::List.
     * @param i The number of steps to move the iterator.
     * @return Returns a new gum::ListIteratorSafe.
     */
    ListIteratorSafe< Val > operator-(difference_type i) noexcept;

    /**
     * @brief Gives access to the content of the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the content of the iterator.
     */
    Val& operator*();

    /**
     * @brief Dereferences the value pointed to by the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the value pointed to by the iterator.
     */
    Val* operator->();

    /**
     * @brief Checks whether two iterators point toward different elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListIteratorSafe to test for inequality.
     * @return Returns true if src and this are equal.
     */
    bool operator!=(const ListIteratorSafe< Val >& src) const;

    /**
     * @brief Checks whether two iterators point toward the same elements.
     *
     * @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend().
     *
     * @param src The gum::ListIteratorSafe to test for equality.
     * @return Returns true if src and this are equal.
     */
    bool operator==(const ListIteratorSafe< Val >& src) const;

    /**
     * @brief Gives access to the content of the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the content of the iterator.
     */
    const Val& operator*() const;

    /**
     * @brief Dereferences the value pointed to by the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
     * @return Returns the value pointed to by the iterator.
     */
    const Val* operator->() const;
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // _static_list_end_ is a 'constant' iterator initialized at compile time
  // that represents the end iterators for all lists (whatever their content).
  // This global variable avoids creating the same iterators within every
  // List instance (this would be quite inefficient as end is precisely
  // identical for all lists). The same holds for reverse and safe end iterators.
  // The type of _list_end_ is a pointer to void because C++ allows
  // pointers to void to be cast into pointers to other types (and conversely).
  // This avoids the painful strict-aliasing rule warning
  extern const ListConstIteratorSafe< Debug > _static_list_end_safe_;
  extern const ListConstIterator< Debug >     _static_list_end_;

  inline constexpr void* const _list_end_safe_ = (void* const)&_static_list_end_safe_;
  inline constexpr void* const _list_end_      = (void* const)&_static_list_end_;
#endif   // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::List< bool >;
extern template class gum::List< int >;
extern template class gum::List< unsigned int >;
#endif


// always include the implementation of the templates
#include <agrum/tools/core/list_tpl.h>

#endif /* GUM_LIST_H */
