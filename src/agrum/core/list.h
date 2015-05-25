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
 * @brief Generic class for manipulating lists.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides class List for manipulating generic lists as well as
 * List<>::iterator, List<>::const_iterator, List<>::iterator_safe and
 * List<>::const_iterator_safe for parsing lists. The List and their (safe)
 * iterators provided here differ from those of the C++ standard library in that
 * they are "safe", i.e., deleting elements which are pointed to by iterators does
 * never produce any segmentation fault nor unexpected results when the iterators
 * are incremented or decremented. Tests performed on a Fedora Core 3 with
 * programs compiled with g++ 3.4 show that List and their iterators are as fast
 * as their counterparts in the standard library. If computation times are an
 * issue, the "_unsafe" iterators provide fast access (but at the expense of
 * safety: dereferencing an unsafe iterator pointing to an erased element will most
 * certainly induce a segfault (like the STL).
 *
 * @warning Developers should keep in mind that whenever a value is inserted
 * into a List, unless it is an R-value, it is actually a copy of this value that
 * is inserted into the List (much like what happens in C++ standard library).
 * However, when inserting rvalues, move insertions are provided.
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
 * list.emplaceBack (22);
 * list.emplaceFront (10);
 *
 * // getting the second element of the list
 * cerr << "10 = " << list1[2] << endl;
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
 * // parse all the elements of a list with unsafe iterators
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
 * // parse all the elements of a list with safe iterators
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
 * List<int>::iterator_safe iter = list2.beginSafe ();
 * List2.erase ( iter );
 * List<int>::iterator iter2 = list2.begin() + 4; // 5th element of the list
 * iter2 = iter + 4;
 *
 * // map a list into another list (assuming function f is defined as
 * // float f (int x) { return (2.5 * x); } )
 * List<float> flist = list2.map (f);
 * @endcode
 */
#ifndef GUM_LIST_H
#define GUM_LIST_H

#include <iostream>
#include <cstddef>
#include <vector>
#include <initializer_list>

#include <agrum/config.h>

#include <agrum/core/refPtr.h>

#define GUM_DEFAULT_ITERATOR_NUMBER 4

namespace gum {

  // ==============================================================================
  // templates provided by this header
  // ==============================================================================

  template <typename Val> class ListBucket;
  template <typename Val> class ListIterator;
  template <typename Val> class ListConstIterator;
  template <typename Val> class ListIteratorSafe;
  template <typename Val> class ListConstIteratorSafe;
  template <typename Val, typename Alloc> class List;

#ifndef SWIG // SWIG cannot read these lines
  /// an << operator for List
  template <typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &stream, const List<Val, Alloc> &list);
#endif // SWIG

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // __list_end is a 'pseudo static' iterator that represents both end and rend
  // iterators for all Lists (whatever their type). This global variable
  // avoids creating the same iterators whithin every List instance (this would
  // be quite inefficient as end and rend are precisely identical for all lists).
  // The type of __list_end is a pointer to void because C++ allows pointers to
  // void to be cast into pointers to other types (and conversely). This avoids
  // the weird strict-aliasing rule warning
  extern const void *const __list_end_safe;
  extern const void *const __list_end;

  /* =========================================================================== */
  /* ===             BUCKETS: SINGLE ELEMENTS OF A CHAINED LIST              === */
  /* =========================================================================== */
  /** @class ListBucket
   * @brief Box for a chained list
   *
   * In aGrUM, each box of a chained list is called a bucket. Lists are doubly
   * linked bucket lists so as to enable efficient rbegin/rend iterators.
   * @warning Values stored in buckets are ALWAYS COPIES. */
  /* =========================================================================== */
  template <typename Val> class ListBucket {
    private:
    /// a dummy type for the emplace constructor
    /** this type is used to prevent the list emplace (int) to compile */
    enum class Emplace { EMPLACE };

    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    ///@{

    /// remove empty constructor
    ListBucket() = delete;

    /// default constructor
    explicit ListBucket(const Val &v);

    /// constructor for Val rvalues
    explicit ListBucket(Val &&v) noexcept;

    /// emplace (universal) constructor
    template <typename... Args> explicit ListBucket(Emplace, Args &&... args);

    /// copy constructor
    ListBucket(const ListBucket<Val> &src);

    /// move constructor should be useless
    ListBucket(ListBucket<Val> &&src) = delete;

    /// destructor
    /** @warning during its deletion, the bucket takes care of properly
     * rechaining the chained list. However, it has no knowledge about the
     * variables that keep track of the beginning/end of the chained list, hence
     * it cannot update them properly. This should be done by the List itself */
    ~ListBucket();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    ListBucket<Val> &operator=(const ListBucket<Val> &src);

    /// move operator should be useless
    ListBucket<Val> &operator=(ListBucket<Val> &&src) = delete;

    /// equality check
    bool operator==(const ListBucket<Val> &src) const;

    /// inequality check
    bool operator!=(const ListBucket<Val> &src) const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// dereferencing operator
    Val &operator*() noexcept;

    /// dereferencing operator
    const Val &operator*() const noexcept;

    /// returns the bucket toward the next element
    const ListBucket<Val> *next() const noexcept;

    /// returns the bucket toward the preceding element
    const ListBucket<Val> *previous() const noexcept;

    /// @}

    private:
    // all the list containers and iterators should be able to access the buckets
    template <typename T, typename A> friend class List;
    friend class ListIterator<Val>;
    friend class ListConstIterator<Val>;
    friend class ListIteratorSafe<Val>;
    friend class ListConstIteratorSafe<Val>;

    /// @{
    /// chaining toward the adjacent elements
    ListBucket<Val> *__prev{nullptr};
    ListBucket<Val> *__next{nullptr};
    /// @}

    /// val is the value contained in the box.
    Val __val;
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /* =========================================================================== */
  /* ===                     GENERIC DOUBLY CHAINED LISTS                    === */
  /* =========================================================================== */
  /** @class List
   * @ingroup basicstruct_group
   * @brief Generic doubly linked lists
   *
   * List enables fast and safe manipulation of chained lists.
   * Unless the elements are rvalues, the insertions of new elements into the
   * lists are @b ALWAYS performed by copy, i.e., each time we add a new element
   * X to the List, a copy of X is actually created and this very copy is stored
   * into the list. For rvalues, move operations are performed.
   *
   * The List iterators are implemented so as to avoid segmentation faults
   * when elements of the list are deleted while some safe iterators are pointing
   * on them. Moreover they ensure that, when elements are removed from a List,
   * iterators on that list will never access these elements (which is not the
   * case for the iterators in the C++ standard library). Note that this guarantee
   * is ensured at low cost as experimental results show that List and
   * ListIterator are as efficient as their STL counterparts. However, this
   * guarantee can hold only if List is aware of all of the iterators pointing
   * to it: thus, when List erases one element, it can parse the list of its
   * iterators and update those that point toward the now deleted element. When
   * parsing elements without removing any element, you can use unsafe iterators
   * instead of safe ones because they are slightly faster. But those will most
   * certainly segfault if they perform some operations on deleted elements.
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
   */
  template <typename Val, typename Alloc = std::allocator<Val>> class List {
    public:
    /// types for STL compliance
    /// @{
    using value_type = Val;
    using reference = Val &;
    using const_reference = const Val &;
    using pointer = Val *;
    using const_pointer = const Val *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = ListIterator<Val>;
    using const_iterator = ListConstIterator<Val>;
    using iterator_safe = ListIteratorSafe<Val>;
    using const_iterator_safe = ListConstIteratorSafe<Val>;
    /// @}

    /// type of the allocator for ListBuckets
    /// @{
    using BucketAllocator = typename Alloc::template rebind<ListBucket<Val>>::other;
    /// @}

    /// locations around iterators where insertions of new elements can take place
    enum class location { BEFORE, AFTER };

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// A basic constructor that creates an empty list
    List();

    /// Copy constructor
    /** The new list and that which is copied do not share their elements: the new
     * list contains new instances of the values stored in the list to be copied.
     * Of course if these values are pointers, the new values point toward the same
     * elements. This constructor runs in linear time.
     * @param src the list the contents of which is copied into the current one.
     */
    List(const List<Val, Alloc> &src);

    /// generalized copy constructor
    template <typename OtherAlloc> List(const List<Val, OtherAlloc> &src);

    /// move constructor
    List(List<Val, Alloc> &&src);

    /// initializer_list constructor
    List(std::initializer_list<Val> list);

    /// Destructor
    ~List();

    /// @}

    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// returns a safe const iterator pointing to the end of the List
    /** Safe const iterators are const iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    const const_iterator_safe &cendSafe() const noexcept;

    /// returns a safe iterator pointing to the end of the List
    /** Safe iterators are iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    const iterator_safe &endSafe() noexcept;

    /// return a safe const iterator pointing just before the beginning of the List
    /** Safe const iterators are const iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    const const_iterator_safe &crendSafe() const noexcept;

    /// returns a safe iterator pointing just before the beginning of the List
    /** Safe iterators are iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    const iterator_safe &rendSafe() noexcept;

    /// returns a safe const iterator pointing to the beginning of the List
    /** Safe const iterators are const iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    const_iterator_safe cbeginSafe() const;

    /// returns a safe iterator pointing to the beginning of the List
    /** Safe iterators are iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    iterator_safe beginSafe();

    /// returns a safe const iterator pointing to the last element of the List
    /** Safe const iterators are const iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    const_iterator_safe crbeginSafe() const;

    /// returns a safe iterator pointing to the last element of the List
    /** Safe iterators are iterators whose state is updated by the list
     * when the element they point to is erased. As such, in this case, they
     * can throw an exception when we try to derefence them and they are
     * able to perform a valid ++ or -- step */
    iterator_safe rbeginSafe();

    /// returns an unsafe const iterator pointing to the end of the List
    /** Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const const_iterator &cend() const noexcept;

    /// returns an unsafe iterator pointing to the end of the List
    /** Unsafe iterators are a little bit faster than safe iterators and
     * they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const iterator &end() noexcept;

    /// returns an unsafe const iterator pointing to the end of the List
    /** Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const const_iterator &end() const noexcept;

    /** @brief returns an unsafe const iterator pointing just before the
     * beginning of the List
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const const_iterator &crend() const noexcept;

    /// returns an unsafe iterator pointing just before the beginning of the List
    /** Unsafe iterators are a little bit faster than safe iterators and
     * they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const iterator &rend() noexcept;

    /** @brief returns an unsafe const iterator pointing just before the
     * beginning of the List
     *
     * Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const const_iterator &rend() const noexcept;

    /// returns an unsafe const iterator pointing to the beginning of the List
    /** Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const_iterator cbegin() const;

    /// returns an unsafe iterator pointing to the beginning of the List
    /** Unsafe iterators are a little bit faster than safe iterators and
     * they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    iterator begin();

    /// returns an unsafe const iterator pointing to the beginning of the List
    /** Unsafe const iterators are a little bit faster than safe const iterators
     * and they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const_iterator begin() const;

    /// returns an unsafe const iterator pointing to the last element of the List
    /** Unsafe iterators are a little bit faster than safe iterators and
     * they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const_iterator crbegin() const;

    /// returns an unsafe iterator pointing to the last element of the List
    /** Unsafe iterators are a little bit faster than safe iterators and
     * they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    iterator rbegin();

    /// returns an unsafe const iterator pointing to the last element of the List
    /** Unsafe iterators are a little bit faster than safe iterators and
     * they consume less memory. However, if the element they point to is
     * erased, their dereference or their increment/decrement will produce a mess,
     * probably a segfault. You should use them only when performance is an
     * issue and if you are sure that they will never point to an
     * element erased. */
    const_iterator rbegin() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// inserts a new element (a copy) at the beginning of the chained list.
    /** The value passed in argument is not actually inserted into the list: this
     * is a copy of this value that is inserted. The method runs in constant time.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    Val &pushFront(const Val &val);

    /// pushFront for rvalues
    Val &pushFront(Val &&val);

    /// an alias for pushFront used for STL compliance
    /** defining push_front allows using, for instance, FrontInserters */
    template <typename... Args> Val &push_front(Args &&... args);

    /// emplace elements at the beginning of the chained list
    /** emplace is a method that allows to construct directly an element of
     * type Val by passing to its constructor all the arguments it needs
     * @param args the arguments passed to the constructor
     * @return a reference on the copy inserted into the list */
    template <typename... Args> Val &emplaceFront(Args &&... args);

    /// inserts a new element (a copy) at the end of the chained list.
    /** The value passed in argument is not actually inserted into the list: this
     * is a copy of this value that is inserted. The method runs in constant time.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    Val &pushBack(const Val &val);

    /// pushBack for rvalues
    Val &pushBack(Val &&val);

    /// an alias for pushBack used for STL compliance
    /** defining push_back allows using, for instance, BackInserters */
    template <typename... Args> Val &push_back(Args &&... args);

    /// emplace elements at the end of the chained list
    /** emplace is a method that allows to construct directly an element of
     * type Val by passing to its constructor all the arguments it needs
     * @param args the arguments passed to the constructor
     * @return a reference on the copy inserted into the list */
    template <typename... Args> Val &emplaceBack(Args &&... args);

    /// inserts a new element at the end of the chained list (alias of pushBack)
    /** @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    Val &insert(const Val &val);

    /// insert for rvalues
    Val &insert(Val &&val);

    /// inserts a new element at the ith pos of the chained list
    /** The first element of the list is at pos 0. After the insert, the element
     * is placed precisely at pos if pos is less than the size of the list
     * before insertion, else it is inserted at the end of the list.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather,
     * it is a copy of val that is inserted. */
    Val &insert(unsigned int pos, const Val &val);

    /// insert an rvalue at the ith pos of the chained list
    /** The first element of the list is at pos 0. After the insert, the element
     * is placed precisely at pos if pos is less than the size of the list
     * before insertion, else it is inserted at the end of the list.
     * @return a reference on the copy inserted into the list. */
    Val &insert(unsigned int pos, Val &&val);

    /// inserts a new element before or after a given iterator
    /** @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather,
     * it is a copy of val that is inserted. */
    Val &insert(const const_iterator_safe &iter, const Val &val,
                location place = location::BEFORE);

    /// inserts an rvalue before or after a given iterator
    /** @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather,
     * it is a copy of val that is inserted. */
    Val &insert(const const_iterator_safe &iter, Val &&val,
                location place = location::BEFORE);

    /// inserts a new element before or after a given iterator
    /** @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather,
     * it is a copy of val that is inserted. */
    Val &insert(const const_iterator &iter, const Val &val,
                location place = location::BEFORE);

    /// inserts an rvalue before or after a given iterator
    /** @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather,
     * it is a copy of val that is inserted. */
    Val &insert(const const_iterator &iter, Val &&val,
                location place = location::BEFORE);

    /// emplace a new element before a given iterator
    /** emplace is a method that allows to construct directly an element of
     * type Val by passing to its constructor all the arguments it needs.
     * The first element of the list is at pos 0. After the insert, the element
     * is placed precisely at pos if pos is less than the size of the list
     * before insertion, else it is inserted at the end of the list.
     * @param iter the position in the list
     * @param args the arguments passed to the constructor
     * @return a reference on the copy inserted into the list */
    template <typename... Args>
    Val &emplace(const const_iterator &iter, Args &&... args);

    /// emplace a new element before a given safe iterator
    /** emplace is a method that allows to construct directly an element of
     * type Val by passing to its constructor all the arguments it needs.
     * The first element of the list is at pos 0. After the insert, the element
     * is placed precisely at pos if pos is less than the size of the list
     * before insertion, else it is inserted at the end of the list.
     * @param iter the position in the list
     * @param args the arguments passed to the constructor
     * @return a reference on the copy inserted into the list */
    template <typename... Args>
    Val &emplace(const const_iterator_safe &iter, Args &&... args);

    /// returns a reference to first element of a list, if any
    /** @throw NotFound exception is thrown if the list is empty */
    Val &front() const;

    /// returns a reference to last element of a list, if any
    /** @throw NotFound exception is thrown if the list is empty */
    Val &back() const;

    /// returns the number of elements in the list.
    /** This method runs in constant time. */
    Size size() const noexcept;

    /// checks whether there exists a given element in the list.
    /** This method runs in linear time.
     * @param val the value of the element we wish to check the existence of.
     * Comparisons between Val instances are performed through == operators. */
    bool exists(const Val &val) const;

    /// erases the ith element of the List (the first one is in position 0)
    /** If the element cannot be found, the function returns without throwing any
     * exception. It runs in linear time in the size of the list.
     * @param i the position in the list of the element we wish to remove. */
    void erase(unsigned int i);

    /// erases the element of the List pointed to by the safe iterator
    /** If the element cannot be found, i.e., it has already been erased or the
     * iterator points to end/rend, the function returns without throwing any
     * exception. It runs in linear time in the size of the list. */
    void erase(const iterator_safe &iter);

    /// erases the element of the List pointed to by the safe const iterator
    /** If the element cannot be found, i.e., it has already been erased or the
     * iterator points to end/rend, the function returns without throwing any
     * exception. It runs in linear time in the size of the list. */
    void erase(const const_iterator_safe &iter);

    /// erases the first element encountered with a given value
    /** If no element equal to \e val can be found, the function
     * returns without throwing any exception. It runs in linear time both in
     * the size of the list and in the number of iterators referenced in the list.
     * Comparisons between Val instances are performed through == operators.
     * @param val the value of the element we wish to remove. */
    void eraseByVal(const Val &val);

    /// erases all the elements encountered with a given value
    /** If no element equal to \e val can be found, the function
     * returns without throwing any exception.
     * @param val the value of the element we wish to remove.
     * Comparisons between Val instances are performed through == operators. */
    void eraseAllVal(const Val &val);

    /// removes the last element of a List, if any
    /** When the list is empty, it does not do anything. */
    void popBack();

    /// removes the first element of a List, if any
    /** When the list is empty, it does not do anything. */
    void popFront();

    /// deletes all the elements of a chained list.
    /** All the iterators of the list will be resetted to rend. The method runs in
     * linear time of both the size of the list and the number of iterators
     * attached to the List. */
    void clear();

    /// returns a boolean indicating whether the chained list is empty
    bool empty() const noexcept;

    /// swap the current list with another one
    void swap(List &other_list);

    /// converts a list into a string
    std::string toString() const;

    /// creates a list of mountains from a list of val
    /** @param f a function that maps any Val element into a Mount */
    template <typename Mount, typename OtherAlloc = std::allocator<Mount>>
    List<Mount, OtherAlloc> map(Mount (*f)(Val)) const;

    /// creates a list of mountains from a list of val
    /** @param f a function that maps any Val element into a Mount */
    template <typename Mount, typename OtherAlloc = std::allocator<Mount>>
    List<Mount, OtherAlloc> map(Mount (*f)(Val &)) const;

    /// creates a list of mountains from a list of val
    /** @param f a function that maps any Val element into a Mount */
    template <typename Mount, typename OtherAlloc = std::allocator<Mount>>
    List<Mount, OtherAlloc> map(Mount (*f)(const Val &)) const;

    /// creates a list of mountains with a given value from a list of val
    /** @param mount the value taken by all the elements of the resulting list  */
    template <typename Mount, typename OtherAlloc = std::allocator<Mount>>
    List<Mount, OtherAlloc> map(const Mount &mount) const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator.
    /** The new list and that which is copied do not share the
     * elements: the new list contains new instances of the values stored in the
     * list to be copied. Of course if these values are pointers, the new values
     * point toward the same elements. The List on which the operator is applied
     * keeps its iterator's list. Of course, if it previously contained some
     * elements, those are removed prior to the copy. This operator runs in
     * linear time.
     * @warning If the current List previously contained iterators, those will
     * be resetted to end()/rend().
     * @param src the list the content of which will be copied into the
     * current List */
    List<Val, Alloc> &operator=(const List<Val, Alloc> &src);

    /// Generalized copy operator.
    /** The new list and that which is copied do not share the
     * elements: the new list contains new instances of the values stored in the
     * list to be copied. Of course if these values are pointers, the new values
     * point toward the same elements. The List on which the operator is applied
     * keeps its iterator's list. Of course, if it previously contained some
     * elements, those are removed prior to the copy. This operator runs in
     * linear time.
     * @warning If the current List previously contained iterators, those will
     * be resetted to end()/rend().
     * @param src the list the content of which will be copied into the
     * current List */
    template <typename OtherAlloc>
    List<Val, Alloc> &operator=(const List<Val, OtherAlloc> &src);

    /// move operator
    List<Val, Alloc> &operator=(List<Val, Alloc> &&src);

    /// inserts a new element at the end of the list (alias of pushBack).
    /** This enables writing code like \c list \c += \c xxx; to add element \c xxx
     * to the list.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    Val &operator+=(const Val &val);

    /// operator += with rvalues
    Val &operator+=(Val &&val);

    /// checks whether two lists are identical (same elements in the same order)
    /** this method runs in time linear in the number of elements of the list */
    template <typename OtherAlloc>
    bool operator==(const List<Val, OtherAlloc> &src) const;

    /// checks whether two lists are different (different elements or orders)
    /** this method runs in time linear in the number of elements of the list */
    template <typename OtherAlloc>
    bool operator!=(const List<Val, OtherAlloc> &src) const;

    /// returns the ith element in the current chained list.
    /** The first of the list element has index 0.
     * This method runs in linear time.
     * @param i the position of the element in the list (0 = first element)
     * @throw NotFound exception is thrown if the element to be retrieved
     * does not exist
     * @return a reference on the element stored at the ith position in the list */
    Val &operator[](const unsigned int i);

    /// returns the const ith element in the current chained list.
    /** The first of the list element has index 0.
     * This method runs in linear time.
     * @param i the position of the element in the list (0 = first element)
     * @throw NotFound exception is thrown if the element to be retrieved
     * does not exist
     * @return a reference on the element stored at the ith position in the list */
    const Val &operator[](const unsigned int i) const;

    /// @}

    private:
    /// a pointer on the first element of the chained list
    ListBucket<Val> *__deb_list{nullptr};

    /// a pointer on the last element of the chained list
    ListBucket<Val> *__end_list{nullptr};

    /// the number of elements in the list
    unsigned int __nb_elements{0};

    /// the list of "safe" iterators attached to the list
    mutable std::vector<const_iterator_safe *> __safe_iterators;

    /// the allocator for the buckets
    mutable BucketAllocator __alloc_bucket;

    /// a function used to perform copies of elements of Lists
    /** before performing the copy, we assume in this function that the current
     * list (this) is empty (else there would be memory leak). */
    template <typename OtherAlloc>
    void __copy_elements(const List<Val, OtherAlloc> &src);

    /// returns the bucket corresponding to the ith position in the list
    /** This method assumes that the list contains at least i+1 elements. The
     * index of the first element of the list is 0. */
    ListBucket<Val> *__getIthBucket(unsigned int i) const noexcept;

    /// returns the bucket corresponding to a given value
    /** Actually, this is the first bucket of value val encountered in the list, if
     * any, that is returned. If the element cannot be found, 0 is returned. This
     * method enables fast removals of buckets. It runs in linear time.
     * @param val the value of the element the bucket of which we wish to return.
     * Comparisons between Val instances are performed through == operators. */
    ListBucket<Val> *__getBucket(const Val &val) const noexcept;

    /// removes an element from a chained list
    /** If parameter \e bucket is equal to 0, then the method does not perform
     * anything, else the bucket is deleted. In the latter case, no test is ever
     * performed to check that the bucket does actually belong to the List. The
     * method runs in constant time.
     * @param bucket a pointer on the bucket in the chained list
     * we wish to remove. */
    void __erase(ListBucket<Val> *bucket);

    /// create a new bucket with a given value
    ListBucket<Val> *__createBucket(const Val &val) const;

    /// create a new bucket with a given value
    ListBucket<Val> *__createBucket(Val &&val) const;

    /// create an emplace bucket
    template <typename... Args>
    ListBucket<Val> *__createEmplaceBucket(Args &&... args) const;

    /// insert a bucket at the front of the list
    Val &__pushFront(ListBucket<Val> *new_elt);

    /// insert a bucket at the end of the list
    Val &__pushBack(ListBucket<Val> *new_elt);

    /// insert a new bucket before another one
    Val &__insertBefore(ListBucket<Val> *new_elt, ListBucket<Val> *current_elt);

    /// insert a new bucket after another one
    Val &__insertAfter(ListBucket<Val> *new_elt, ListBucket<Val> *current_elt);

    /// inserts a new bucket before or after the location pointed to by an iterator
    Val &__insert(const const_iterator_safe &iter, ListBucket<Val> *new_elt,
                  location place);

    /// inserts a new bucket before or after the location pointed to by an iterator
    Val &__insert(const const_iterator &iter, ListBucket<Val> *new_elt,
                  location place);

    /// ListIterator should be a friend to optimize access to elements
    friend class ListIterator<Val>;
    friend class ListConstIterator<Val>;
    friend class ListIteratorSafe<Val>;
    friend class ListConstIteratorSafe<Val>;
  };

  /* =========================================================================== */
  /* ===                      UNSAFE LIST CONST ITERATORS                    === */
  /* =========================================================================== */
  /** @class ListConstIterator
   * @ingroup basicstruct_group
   * @brief unsafe but fast const iterators for Lists.
   *
   * Class ListConstIterator implements unsafe iterators for List. However,
   * developers may consider using List<x>::const_iterator instead of
   * ListConstIterator<x>.
   *
   * These iterators are fast but they are unaware of changes within the List.
   * Therefore, if they point to an element that is being deleted from memory by
   * the list, their accessing this element will most probably produce a
   * segmentation fault. Similarly, incrementing or decrementing such an
   * iterator pointing to a deleted element will most certainly produce a mess.
   * So, ListConstIterator should be used only if you are sure that they
   * will never point to an element that has been removed from the list (a
   * typical use is to iterate over a const List). Whenever you are not sure that
   * this property holds, use ListConstIteratorSafe<x> or
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
   */
  template <typename Val> class ListConstIterator {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Val;
    using reference = Val &;
    using const_reference = const Val &;
    using pointer = Val *;
    using const_pointer = const Val *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor. returns an iterator pointing toward nothing
    ListConstIterator() noexcept;

    /// constructor for a begin
    template <typename Alloc>
    ListConstIterator(const List<Val, Alloc> &theList) noexcept;

    /// copy constructor
    ListConstIterator(const ListConstIterator<Val> &src) noexcept;

    /// move constructor
    ListConstIterator(ListConstIterator<Val> &&src) noexcept;

    /// Constructor for an iterator pointing to the \e ind_eltth element of a List
    /** @throw UndefinedIteratorValue if the element does not exist in the list */
    ListConstIterator(const List<Val> &theList, unsigned int ind_elt);

    /// Destructor
    ~ListConstIterator() noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Makes the iterator point toward nothing
    /** A method for detaching the iterator from the List it is attached to. After
     * being detached, the iterator does not point to any element, i.e., trying to
     * access its content will raise an exception. */
    void clear() noexcept;

    /// positions the iterator to the end of the list
    void setToEnd() noexcept;

    /** @brief returns a bool indicating whether the iterator points to the end
     * of the list */
    bool isEnd() const noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    /** The current iterator now points to the same element as iterator \e from. */
    ListConstIterator<Val> &operator=(const ListConstIterator<Val> &src) noexcept;

    /// move operator
    ListConstIterator<Val> &operator=(ListConstIterator<Val> &&src) noexcept;

    /// makes the iterator point to the next element in the List
    /** for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to parse
     * the whole List as long as no element is added to or deleted from the List
     * while being in the loop. Runs in constant time. */
    ListConstIterator<Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the List
    ListConstIterator<Val> &operator+=(difference_type) noexcept;

    /// makes the iterator point to the preceding element in the List
    /** for (iter=rbegin(); iter!=rend(); --iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from
     * the List while being in the loop. Runs in constant time. */
    ListConstIterator<Val> &operator--() noexcept;

    /// makes the iterator point to i elements befor in the List
    ListConstIterator<Val> &operator-=(difference_type) noexcept;

    /// returns a new iterator
    ListConstIterator<Val> operator+(difference_type) noexcept;

    /// returns a new iterator
    ListConstIterator<Val> operator-(difference_type) noexcept;

    /// checks whether two iterators point toward different elements
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    bool operator!=(const ListConstIterator<Val> &src) const noexcept;

    /// checks whether two iterators point toward the same elements.
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    bool operator==(const ListConstIterator<Val> &src) const noexcept;

    /// gives access to the content of the iterator
    /** @throw UndefinedIteratorValue if the iterator points to nothing */
    const Val &operator*() const;

    /// dereferences the value pointed to by the iterator
    /** @throw UndefinedIteratorValue if the iterator points to nothing */
    const Val *operator->() const;

    /// @}

    private:
    /** class List must be a friend because it uses the getBucket method
     * to speed up some processes. */
    template <typename T, typename A> friend class List;

    /// the bucket in the chained list pointed to by the iterator
    ListBucket<Val> *__bucket{nullptr};

    /// returns the bucket the iterator is pointing to.
    ListBucket<Val> *__getBucket() const noexcept;
  };

  /// for STL compliance, a distance operator
  template <typename Val>
  typename ListConstIterator<Val>::difference_type
  operator-(const ListConstIterator<Val> &iter1,
            const ListConstIterator<Val> &iter2);

  /* =========================================================================== */
  /* ===                         UNSAFE LIST ITERATORS                       === */
  /* =========================================================================== */
  /** @class ListIterator
   * @ingroup basicstruct_group
   * @brief unsafe but fast iterators for Lists.
   *
   * Class ListIterator implements iterators for List. However,
   * developers may consider using List<x>::iterator instead of ListIterator<x>.
   *
   * These iterators are fast but they are unaware of changes within the List.
   * Therefore, if they point to an element that is being deleted from memory by
   * the list, their accessing this element will most probably produce a
   * segmentation fault. Similarly, incrementing or decrementing such an
   * iterator pointing to a deleted element will most certainly produce a mess.
   * So, ListIterator should be used only if you are sure that they
   * will never point to an element that has been removed from the list (a
   * typical use is to iterate over a const List). Whenever you are not sure that
   * this property holds, use ListIteratorSafe<x> or List<x>::iterator_safe.
   * Those iterators are a little bit slower but guarantee that no segmentation
   * fault will ever occur.
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
   */
  template <typename Val> class ListIterator : public ListConstIterator<Val> {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Val;
    using reference = Val &;
    using const_reference = const Val &;
    using pointer = Val *;
    using const_pointer = const Val *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor. returns an iterator pointing toward nothing
    ListIterator() noexcept;

    /// constructor for a begin
    template <typename Alloc> ListIterator(const List<Val, Alloc> &theList) noexcept;

    /// copy constructor
    ListIterator(const ListIterator<Val> &src) noexcept;

    /// move constructor
    ListIterator(ListIterator<Val> &&src) noexcept;

    /// Constructor for an iterator pointing to the \e ind_eltth element of a List
    /** @throw UndefinedIteratorValue if the element does not exist in the list */
    ListIterator(const List<Val> &theList, unsigned int ind_elt);

    /// Destructor
    ~ListIterator() noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    using ListConstIterator<Val>::clear;
    using ListConstIterator<Val>::setToEnd;
    using ListConstIterator<Val>::isEnd;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    /** The current iterator now points to the same element as iterator \e from. */
    ListIterator<Val> &operator=(const ListIterator<Val> &src) noexcept;

    /// move operator
    ListIterator<Val> &operator=(ListIterator<Val> &&src) noexcept;

    /// makes the iterator point to the next element in the List
    /** for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to parse
     * the whole List as long as no element is added to or deleted from the List
     * while being in the loop. Deleting elements during the loop is guaranteed
     * to never produce a segmentation fault. Runs in constant time. */
    ListIterator<Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the List
    ListIterator<Val> &operator+=(difference_type) noexcept;

    /// makes the iterator point to the preceding element in the List
    /** for (iter=rbegin(); iter!=rend(); --iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from
     * the List while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. Runs in constant time. */
    ListIterator<Val> &operator--() noexcept;

    /// makes the iterator point to i elements befor in the List
    ListIterator<Val> &operator-=(difference_type) noexcept;

    /// returns a new iterator
    ListIterator<Val> operator+(difference_type) noexcept;

    /// returns a new iterator
    ListIterator<Val> operator-(difference_type) noexcept;

    using ListConstIterator<Val>::operator==;
    using ListConstIterator<Val>::operator!=;
    using ListConstIterator<Val>::operator*;
    using ListConstIterator<Val>::operator->;

    /// gives access to the content of the iterator
    /** @throw UndefinedIteratorValue */
    Val &operator*();

    /// dereferences the value pointed to by the iterator
    /** @throw UndefinedIteratorValue */
    Val *operator->();

    /// @}
  };

  /* =========================================================================== */
  /* ===                          LIST CONST ITERATORS                       === */
  /* =========================================================================== */
  /** @class ListConstIteratorSafe
   * @ingroup basicstruct_group
   * @brief Safe const iterators for Lists.
   *
   * Class ListConstIteratorSafe implements safe const iterators for List. However,
   * developers may consider using List<x>::const_iterator_safe instead of
   * ListConstIteratorSafe<x>.
   *
   * These const iterators ensure that whenever they point to an element that is
   * being deleted from memory, their accessing this element will never produce a
   * segmentation fault but rather throw an exception. Similarly, incrementing or
   * decrementing an iterator pointing to a deleted element is guaranteed to make
   * the iterator point on the next (or preceding) element that has not been
   * deleted.
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
   */
  template <typename Val> class ListConstIteratorSafe {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Val;
    using reference = Val &;
    using const_reference = const Val &;
    using pointer = Val *;
    using const_pointer = const Val *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor. returns an iterator pointing toward nothing
    ListConstIteratorSafe() noexcept;

    /// constructor for a begin
    template <typename Alloc> ListConstIteratorSafe(const List<Val, Alloc> &theList);

    /// copy constructor
    ListConstIteratorSafe(const ListConstIteratorSafe<Val> &src);

    /// Constructor for an iterator pointing to the \e ind_eltth element of a List
    template <typename Alloc>
    ListConstIteratorSafe(const List<Val, Alloc> &theList, unsigned int ind_elt);

    /// move constructor
    ListConstIteratorSafe(ListConstIteratorSafe<Val> &&src);

    /// Destructor
    ~ListConstIteratorSafe();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Makes the iterator point toward nothing
    /** A method for detaching the iterator from the List it is attached to.
     * It is mainly used by the List when the latter is deleted while the
     * iterator is still alive. After being detached, the iterator does not point
     * to any element, i.e., trying to access its content will raise an exception.
     */
    void clear();

    /// positions the iterator to the end of the list
    void setToEnd();

    /** @brief returns a bool indicating whether the iterator points to the end
     * of the list */
    bool isEnd() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    /** The current iterator now points to the same element as iterator \e from. */
    ListConstIteratorSafe<Val> &operator=(const ListConstIteratorSafe<Val> &src);

    /// move operator
    ListConstIteratorSafe<Val> &operator=(ListConstIteratorSafe<Val> &&src);

    /// makes the iterator point to the next element in the List
    /** for (iter=beginSafe(); iter!=endSafe(); ++iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from the
     * List while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. Runs in constant time. */
    ListConstIteratorSafe<Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the List
    ListConstIteratorSafe<Val> &operator+=(difference_type) noexcept;

    /// makes the iterator point to the preceding element in the List
    /** for (iter=rbeginSafe(); iter!=rendSafe(); --iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from
     * the List while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. Runs in constant time. */
    ListConstIteratorSafe<Val> &operator--() noexcept;

    /// makes the iterator point to i elements befor in the List
    ListConstIteratorSafe<Val> &operator-=(difference_type) noexcept;

    /// returns a new iterator
    ListConstIteratorSafe<Val> operator+(difference_type) noexcept;

    /// returns a new iterator
    ListConstIteratorSafe<Val> operator-(difference_type) noexcept;

    /// checks whether two iterators point toward different elements
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    bool operator!=(const ListConstIteratorSafe<Val> &src) const;

    /// checks whether two iterators point toward the same elements.
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    bool operator==(const ListConstIteratorSafe<Val> &src) const;

    /// gives access to the content of the iterator
    /** @throw UndefinedIteratorValue */
    const Val &operator*() const;

    /// dereferences the value pointed to by the iterator
    /** @throw UndefinedIteratorValue */
    const Val *operator->() const;

    /// @}

    private:
    /** class List must be a friend because it uses the getBucket method
     * to speed up some processes. */
    template <typename T, typename A> friend class List;
    friend class ListConstIterator<Val>;

    /// the list the iterator is pointing to
    const List<Val, std::allocator<Val>> *__list{nullptr};

    /// the bucket in the chained list pointed to by the iterator
    ListBucket<Val> *__bucket{nullptr};

    /** \brief the bucket we should start from when we are pointing on a deleted
     * bucket and we decide to do a ++. */
    ListBucket<Val> *__next_current_bucket{nullptr};

    /** \brief the bucket we should start from when we are pointing on a deleted
     * bucket and we decide to do a --. */
    ListBucket<Val> *__prev_current_bucket{nullptr};

    /// indicates whether the bucket the iterator points to has been deleted
    bool __null_pointing{false};

    /// returns the bucket the iterator is pointing to.
    ListBucket<Val> *__getBucket() const noexcept;

    /// remove the iterator for its list' safe iterators list
    void __removeFromSafeList() const;

    /// makes the iterator point to the next element in the List
    ListConstIteratorSafe<Val> &__opPlus(unsigned int i) noexcept;

    /// makes the iterator point to i elements before in the List
    ListConstIteratorSafe<Val> &__opMinus(unsigned int i) noexcept;
  };

  /// for STL compliance, a distance operator
  template <typename Val>
  typename ListConstIteratorSafe<Val>::difference_type
  operator-(const ListConstIteratorSafe<Val> &iter1,
            const ListConstIteratorSafe<Val> &iter2);

  /* =========================================================================== */
  /* ===                             LIST ITERATORS                          === */
  /* =========================================================================== */
  /** @class ListIteratorSafe
   * @ingroup basicstruct_group
   * @brief Safe iterators for Lists.
   *
   * Class ListIteratorSafe implements iterators for List. However, developers
   * may consider using List<x>::iterator_safe instead of ListIteratorSafe<x>.
   *
   * These iterators ensure that whenever they point to an element that is being
   * deleted from memory, their accessing this element will never produce a
   * segmentation fault but rather throw an exception. Similarly, incrementing or
   * decrementing an iterator pointing to a deleted element is guaranteed to make
   * the iterator point on the next (or preceding) element that has not been
   * deleted. This enables safely writing code like:
   * \code
   * for ( iter = mylist.beginSafe (); iter != mylist.endSafe (); ++iter )
   *   list.erase ( iter );
   * \endcode
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
   */
  template <typename Val>
  class ListIteratorSafe : public ListConstIteratorSafe<Val> {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Val;
    using reference = Val &;
    using const_reference = const Val &;
    using pointer = Val *;
    using const_pointer = const Val *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor. returns an iterator pointing toward nothing
    ListIteratorSafe() noexcept;

    /// constructor for a begin
    template <typename Alloc> ListIteratorSafe(const List<Val, Alloc> &theList);

    /// copy constructor
    ListIteratorSafe(const ListIteratorSafe<Val> &src);

    /// Constructor for an iterator pointing to the \e ind_eltth element of a List
    template <typename Alloc>
    ListIteratorSafe(const List<Val, Alloc> &theList, unsigned int ind_elt);

    /// move constructor
    ListIteratorSafe(ListIteratorSafe<Val> &&src);

    /// Destructor
    ~ListIteratorSafe();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    using ListConstIteratorSafe<Val>::clear;
    using ListConstIteratorSafe<Val>::setToEnd;
    using ListConstIteratorSafe<Val>::isEnd;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    /** The current iterator now points to the same element as iterator \e from. */
    ListIteratorSafe<Val> &operator=(const ListIteratorSafe<Val> &src);

    /// move operator
    ListIteratorSafe<Val> &operator=(ListIteratorSafe<Val> &&src);

    /// makes the iterator point to the next element in the List
    /** for (iter = beginSafe(); iter != endSafe(); ++iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from the
     * List while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. Runs in constant time. */
    ListIteratorSafe<Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the List
    ListIteratorSafe<Val> &operator+=(difference_type) noexcept;

    /// makes the iterator point to the preceding element in the List
    /** for (iter=rbeginSafe(); iter!=rendSafe(); --iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from
     * the List while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. Runs in constant time. */
    ListIteratorSafe<Val> &operator--() noexcept;

    /// makes the iterator point to i elements befor in the List
    ListIteratorSafe<Val> &operator-=(difference_type) noexcept;

    /// returns a new iterator
    ListIteratorSafe<Val> operator+(difference_type) noexcept;

    /// returns a new iterator
    ListIteratorSafe<Val> operator-(difference_type) noexcept;

    using ListConstIteratorSafe<Val>::operator!=;
    using ListConstIteratorSafe<Val>::operator==;
    using ListConstIteratorSafe<Val>::operator*;
    using ListConstIteratorSafe<Val>::operator->;

    /// gives access to the content of the iterator
    /** @throw UndefinedIteratorValue */
    Val &operator*();

    /// dereferences the value pointed to by the iterator
    /** @throw UndefinedIteratorValue */
    Val *operator->();

    /// @}
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // constructor and destructor for the iterator that represents end and rend
  template <> ListConstIteratorSafe<Debug>::ListConstIteratorSafe() noexcept;
  template <> ListConstIteratorSafe<Debug>::~ListConstIteratorSafe();
  template <> ListConstIterator<Debug>::ListConstIterator() noexcept;
  template <> ListConstIterator<Debug>::~ListConstIterator() noexcept;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} /* namespace gum */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// always include the implementation of the templates
#include <agrum/core/list.tcc>

// to optimize compile-link time, provide the usual lists
namespace gum {
  extern template class List<bool>;
  extern template class List<int>;
  extern template class List<unsigned int>;
} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif /* GUM_LIST_H */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
