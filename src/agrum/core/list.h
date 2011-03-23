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
 * List<>::iterator for parsing lists. The List and their iterators provided
 * here  differ from those of the C++ standard library in that they are "safe",
 * i.e., deleting elements which are pointed to by iterators does never produce any
 * segmentation fault nor unexpected results when the iterators are incremented or
 * decremented. Tests performed on a Fedora Core 3 with programs compiled with
 * g++ 3.4 show that List and their iterators are as fast as their counterparts
 * in the standard library.
 *
 * @warning Developers should keep in mind that whenever a value is inserted
 * into a List, it is actually a copy of this value that is inserted into
 * the List (much like what happens in C++ standard library).
 *
 * @par Usage example:
 * @code
 * // creation of an empty list
 * List<int> list1;
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
 * // parse all the elements of a list
 * for (List<int>::iterator iter = list2.begin(); iter != list2.end(); ++iter)
 *   cerr << *iter << endl;
 * for (List<int>::iterator iter = list2.rbegin(); iter != list2.rend(); --iter)
 *   cerr << *iter << endl;
 *
 * // use an iterator to point the element we wish to erase
 * List<int>::iterator iter = list2.begin();
 * List2.erase ( iter );
 *
 * // map a list into another list (assuming function f is defined as
 * // float f (int x) { return (2.5 * x); } )
 * List<float> flist = list2.map (f);
 * @endcode
 */

#ifndef GUM_LIST_H
#define GUM_LIST_H

#include <iostream>

#include <agrum/core/utils.h>
#include <agrum/core/debug.h>
#include <agrum/core/refPtr.h>


namespace gum {


  // ==============================================================================
  // templates provided by this file
  // ==============================================================================

  template <typename Val> class ListBucket;

  template <typename Val> class ListIterator;

  template <typename Val> class ListConstIterator;

  template <typename Val> class ListBase;

  template <typename Val> class List;

#ifndef SWIG  // SWIG cannot read these lines
  // ==============================================================================
  /// a << operator for ListBase
  // ==============================================================================
  template <typename Val> std::ostream&
  operator<< (std::ostream& stream, const ListBase<Val>& list);

  // ==============================================================================
  /// a << operator for List
  // ==============================================================================
  template <typename Val> std::ostream&
  operator<< (std::ostream& stream, const List<Val>& list);
#endif // SWIG



#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // __list_end is a 'pseudo static' iterator that represents both end and rend
  // iterators for all Lists (whatever their type). This global variable
  // avoids creating the same iterators whithin every List instance (this would
  // be quite inefficient as end and rend are precisely identical for all lists)
  extern const ListConstIterator<Debug> __list_end;

	// a function to get rid of a weird strict-aliasing rule warning
	const void* ___get__list_end();

#define GUM_LIST_ITERATOR_BEGIN  1
#define GUM_LIST_ITERATOR_RBEGIN 2
#define GUM_LIST_ITERATOR_END    3
#define GUM_LIST_ITERATOR_REND   4

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
  public:
    // ============================================================================
    /// default constructor
    // ============================================================================
    explicit ListBucket (const Val& v);

    // ============================================================================
    /// copy constructor
    // ============================================================================
    ListBucket (const ListBucket<Val>& src);

    // ============================================================================
    /// copy operator
    // ============================================================================
    ListBucket<Val>& operator= (const ListBucket<Val>& src);

    // ============================================================================
    /// destructor
    /** @warning during its deletion, the bucket takes care of properly
     * rechaining the chained list. However, it has no knowledge about the
     * variables that keep track of the beginning/end of the chained list, hence
     * it cannot update them properly. This should be done by the List itself */
    // ============================================================================
    ~ListBucket();

    // ============================================================================
    /// equality check
    // ============================================================================
    bool operator== (const ListBucket<Val>& src) const;

    // ============================================================================
    /// inequality check
    // ============================================================================
    bool operator!= (const ListBucket<Val>& src) const;

    // ============================================================================
    /// dereferencing operator
    // ============================================================================
    Val& operator*() ;

    // ============================================================================
    /// dereferencing operator
    // ============================================================================
    const Val& operator*() const ;

    // ============================================================================
    /// returns the bucket toward the next element
    // ============================================================================
    const ListBucket<Val>* next () const;

    // ============================================================================
    /// returns the bucket toward the preceding element
    // ============================================================================
    const ListBucket<Val>* previous () const;


  private:

    template <typename T> friend class List;

    template <typename T> friend class ListBase;

    template <typename T> friend class ListIterator;

    template <typename T> friend class ListConstIterator;

    // chaining toward the adjacent elements
    ListBucket<Val> *__prev;
    ListBucket<Val> *__next;

    // val is the value contained in the box.
    Val __val;
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */



  /* =========================================================================== */
  /* ===       GENERIC SIMPLE DOUBLY CHAINED LISTS WITHOUT ITERATORS         === */
  /* =========================================================================== */
  /** @class ListBase
   * @brief Simple doubly linked lists without iterators
   *
   * ListBase enables fast and safe manipulation of chained lists. Unlike List,
   * ListBase does not support iterators. Note that the insertions of new elements
   * into the lists are @b ALWAYS performed by copy, i.e., each time we add a new
   * element X to the ListBase, a copy of X is actually created and this very
   * copy is stored into the list.
   * @par Usage example:
   * @code
   * // creation of an empty list
   * ListBase<int> list1;
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
   * ListBase<int> list2 = list1, list3;
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
   * @endcode
   */
  /* =========================================================================== */

  template <typename Val> class ListBase {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// A basic constructor that creates an empty list.
    // ============================================================================
    ListBase() ;

    // ============================================================================
    /// Copy constructor
    /** The new list and that which is copied do not share their elements: the new
     * list contains new instances of the values stored in the list to be copied.
     * Of course if these values are pointers, the new values point toward the same
     * elements. This constructor runs in linear time w.r.t. the number of elements
     * in list 'from'.
     * @param from the list the contents of which is copied into the current one.
     */
    // ============================================================================
    ListBase (const ListBase<Val> &src);

    // ============================================================================
    /// Destructor
    /** The destructor runs in linear time in the size of the list. */
    // ============================================================================
    ~ListBase();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// inserts a new element (a copy) at the beginning of the chained list
    /** The value passed in argument is not actually inserted into the list: this
     * is a copy of this value that is inserted. The method runs in constant time.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    Val& pushFront (const Val& val);

    // ============================================================================
    /// alias for push_front
    // ============================================================================
    Val& push_front (const Val& val);

    // ============================================================================
    /// inserts a new element (a copy) at the end of the chained list
    /** The value passed in argument is not actually inserted into the list: this
     * is a copy of this value that is inserted. The method runs in constant time.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    Val& pushBack (const Val& val);

    // ============================================================================
    /// alias for push_back
    // ============================================================================
    Val& push_back (const Val& val);

    // ============================================================================
    /// inserts a new element at the end of the chained list (alias of pushBack)
    /** @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    Val& insert (const Val& val);

    // ============================================================================
    /// returns a reference to first element of a list, if any
    /** @throw NotFound exception is thrown if the list is empty */
    // ============================================================================
    Val& front() const ;

    // ============================================================================
    /// returns a reference to last element of a list, if any
    /** @throw NotFound exception is thrown if the list is empty */
    // ============================================================================
    Val& back() const ;

    // ============================================================================
    /// returns the number of elements currently stored in the list
    /** This method runs in constant time. */
    // ============================================================================
    Size size() const ;

    // ============================================================================
    /// checks whether there exists a given element in the list
    /** Comparisons between Val instances are performed through == operators.
     * This method runs in linear time w.r.t. the number of elements in the list.
     * @param val the value of the element we wish to check the existence of. */
    // ============================================================================
    bool exists (const Val& val) const;

    // ============================================================================
    /// erases the ith element of the List (the first one is in position 0)
    /** If the ith element cannot be found, the function returns without throwing
     * any exception. It runs in linear time in the size of the list.
     * @param i the position in the list of the element we wish to remove. */
    // ============================================================================
    void erase (unsigned int i);

    // ============================================================================
    /// erases the bucket from the List
    // ============================================================================
    void erase ( const ListBucket<Val>& bucket );

    // ============================================================================
    /// erases the first element encountered with a given value
    /** If no element equal to \e val can be found, the function
     * returns without throwing any exception. It runs in linear time in
     * the size of the list. Comparisons between Val instances are performed
     * through == operators.
     * @param val the value of the element we wish to remove. */
    // ============================================================================
    void eraseByVal (const Val& val);

    // ============================================================================
    /// erases all the elements encountered with a given value
    /** If no element equal to \e val can be found, the function
     * returns without throwing any exception.
     * @param val the value of the element we wish to remove.
     * Comparisons between Val instances are performed through == operators. */
    // ============================================================================
    void eraseAllVal (const Val& val);

    // ============================================================================
    /// removes the last element of a ListBase, if any
    /** When the list is empty, it does nothing. In particular, no
     * exception is thrown. */
    // ============================================================================
    void popBack();

    // ============================================================================
    /// removes the first element of a ListBase, if any
    /** When the list is empty, it does nothing. In particular, no
     * exception is thrown.*/
    // ============================================================================
    void popFront();

    // ============================================================================
    /// deletes all the elements of a chained list
    /** The method runs in linear time w.r.t. the size of the list. */
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns a boolean indicating whether the chained list is empty
    // ============================================================================
    bool empty() const ;

    // ============================================================================
    /// returns a bucket pointing to the first element of the list
    // ============================================================================
    const ListBucket<Val>* frontBucket () const;

    // ============================================================================
    /// returns a bucket pointing to the last element of the list
    // ============================================================================
    const ListBucket<Val>* backBucket () const;

    // ============================================================================
    /// creates a list of mountains from a list of val
    /** If a problem arises during the mapping, an exception is thrown, but no
     * memory leak occurs.
     * @param f a function that maps any Val element into a Mount */
    // ============================================================================
    template <typename Mount>
    ListBase<Mount> map (Mount (*f) (Val)) const;
    template <typename Mount>
    ListBase<Mount> map (Mount (*f) (Val&)) const;
    template <typename Mount>
    ListBase<Mount> map (Mount (*f) (const Val&)) const;

    // ============================================================================
    /// creates a list of mountains with a given value from a list of val
    /** If a problem arises during the mapping, an exception is thrown, but no
     * memory leak occurs.
     * @param mount the value taken by all the elements of the resulting list  */
    // ============================================================================
    template <typename Mount> ListBase<Mount> map (const Mount& mount) const;

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator
    /** The new list and that which is copied do not share the
     * elements: the new list contains new instances of the values stored in the
     * list to be copied. Of course if these values are pointers, the new values
     * point toward the same elements. This constructor runs in linear time. If an
     * exception is thrown, the ListBase guarrantees that no memory leak occurs.
     * In such a case, the list returned is empty.
     * @param fr the list the content of which will be copied into the current
     * List */
    // ============================================================================
    ListBase<Val>& operator= (const ListBase<Val> &fr);

    // ============================================================================
    /// inserts a new element at the end of the list (alias of pushBack)
    /** This enables writing code like \c list \c += \c xxx; to add element \c xxx
     * to the list.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    Val& operator+= (const Val& val);

    // ============================================================================
    /// checks whether two lists are identical (same elements in the same order)
    /** this method runs in time linear in the number of elements of the list */
    // ============================================================================
    bool operator== (const ListBase<Val>& src) const;

    // ============================================================================
    /// checks whether two lists are different (different elements or orders)
    /** this method runs in time linear in the number of elements of the list */
    // ============================================================================
    bool operator!= (const ListBase<Val>& src) const;

    // ============================================================================
    /// returns the ith element in the current chained list
    /** The first of the list element has index 0.
     * This method runs in linear time w.r.t. the size of the list.
     * @param i the position of the element in the list (0 = first element)
     * @throw NotFound exception is thrown if the element to be retrieved
     * does not exist
     * @return a reference on the element stored at the ith position in the list */
    // ============================================================================
    Val& operator[] (unsigned int i) ;

    /// returns the const ith element in the current chained list
    /** The first of the list element has index 0.
     * This method runs in linear time w.r.t. the size of the list.
     * @param i the position of the element in the list (0 = first element)
     * @throw NotFound exception is thrown if the element to be retrieved
     * does not exist
     * @return a reference on the element stored at the ith position in the list */
    const Val& operator[] (unsigned int i) const ;

    /// @}


    // ============================================================================
    /// displays the content of a chained list
    // ============================================================================
    std::string toString() const;


  private:
    /// true Lists should have access to the members of ListBase

    template <typename T> friend class List;

    template <typename T> friend class ListIterator;

    template <typename T> friend class ListConstIterator;

#ifndef SWIG // SWIG cannot read these lines
    /// for friendly displaying the content of a list
    friend std::ostream& operator<< <> (std::ostream& stream, const ListBase<Val>& list);
#endif

    /// a pointer on the first element of the chained list
    ListBucket<Val> *__deb_list;

    /// a pointer on the last element of the chained list
    ListBucket<Val> *__end_list;

    /// the number of elements in the list
    unsigned int __nb_elements;

    // ============================================================================
    /// a function used to perform copies of elements of ListBases
    /** before performing the copy, we assume in this function that the current
     * list (this) is empty (else there would be memory leak). */
    // ============================================================================
    void __copy_elements (const ListBase<Val> &src);

    // ============================================================================
    /// returns the bucket corresponding to a given value
    /** Actually, this is the first bucket of value val encountered in the list, if
     * any, that is returned. If the element cannot be found, 0 is returned. This
     * method enables fast removals of buckets. It runs in linear time.
     * @param val the value of the element the bucket of which we wish to return.
     * Comparisons between Val instances are performed through == operators. */
    // ============================================================================
    ListBucket<Val>* __getBucket (const Val& val) const;

    // ============================================================================
    /// suppresses an element from a chained list
    /** If parameter \e bucket is equal to 0, then the method does not perform
     * anything, else the bucket is deleted. In the latter case, no test is ever
     * performed to check that the bucket does actually belong to the ListBase. The
     * method runs in constant time.
     * @param bucket a pointer on the bucket in the chained list
     * we wish to remove. */
    // ============================================================================
    void __erase (ListBucket<Val>* bucket);
  };




  /* =========================================================================== */
  /* ===                     GENERIC DOUBLY CHAINED LISTS                    === */
  /* =========================================================================== */
  /** @class List
   * @brief Generic doubly linked lists
   * @ingroup basicstruct_group
   *
   * List enables fast and safe manipulation of chained lists.
   * The insertions of new elements into the lists are @b ALWAYS performed by
   * copy, i.e., each time we add a new element X to the List, a copy of X
   * is actually created and this very copy is stored into the list.
   *
   * The List iterators are implemented so as to avoid segmentation faults
   * when elements of the list are deleted while some iterators are pointing on
   * them. Moreover they ensure that, when elements are removed from a List,
   * iterators on that list will never access these elements (which is not the
   * case for the iterators in the C++ standard library). Note that this guarrantee
   * is ensured at low cost as experimental results show that List and
   * ListIterator are as efficient as their STL counterparts. However, this
   * guarrantee can hold only if List is aware of all of the iterators pointing
   * to it: thus, when List erases one element, it can parse the list of its
   * iterators and update those that point toward the now deleted element. Making
   * each List have its own list of iterators would be very inefficient in some
   * cases because it would consume too much memory. For instance, in ArcList, to
   * each node is associated a list of parents and a list of children. For a 1000
   * nodes graph, this would require 2000 List iterators lists. Now, it is
   * certainly the case that no more than 10 to 20 iterators will be used at the
   * same time on a given ArcList. Hence, most of the List iterators lists would be
   * empty (but even an empty list requires a few bytes). In this case, a more
   * efficient approach would be that several List share the same iterator's list.
   * This is precisely what is done in aGrUM. This explains why some constructors
   * enable the user to specify which iterator's list will be used by the List.
   *
   * @par Usage example:
   * @code
   * // creation of an empty list
   * List<int> list1;
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
   * // parse all the elements of a list
   * for (List<int>::iterator iter = list2.begin(); iter != list2.end(); ++iter)
   *   cerr << *iter << endl;
   * for (List<int>::iterator iter = list2.rbegin(); iter != list2.rend(); --iter)
   *   cerr << *iter << endl;
   *
   * // use an iterator to point the element we wish to erase
   * List<int>::iterator iter = list2.begin();
   * List2.erase ( iter );
   *
   * // map a list into another list (assuming function f is defined as
   * // float f (int x) { return (2.5 * x); } )
   * List<float> flist = list2.map (f);
   * @endcode
   */
  /* =========================================================================== */

  template <typename Val> class List : private ListBase<Val> {
  public:
    /// developers should always use the List<X>::iterator terminology
    typedef ListIterator<Val> iterator;
    typedef ListConstIterator<Val> const_iterator;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /** @brief A basic constructor that creates an empty list as well as its own
     * iterator's list */
    // ============================================================================
    List();

    // ============================================================================
    /** @brief A basic constructor that creates an empty list. The iterator's list
     * may be shared by several Lists when \e iter_list is different from a
     * RefPtr containing 0.
     *
     * An optional external iterator's list may be passed as argument. In such a
     * case, the current List will share its iterator's list with several other
     * Lists.
     * @param iter_list a pointer to an external (shared) list of iterators. When
     * equal to 0, a new iterator's list is created for the current List.
     * This constructor runs in constant time. */
    // ============================================================================
    List (const RefPtr< ListBase<ListConstIterator<Val>*> >& iter_list);

    // ============================================================================
    /// Copy constructor sharing its iterator's list with that of \c from
    /** The new list and that which is copied do not share their elements: the new
     * list contains new instances of the values stored in the list to be copied.
     * Of course if these values are pointers, the new values point toward the same
     * elements. This constructor runs in linear time.
     * @param from the list the contents of which is copied into the current one.
     */
    // ============================================================================
    List (const List<Val> &src);

    // ============================================================================
    /** @brief Copy constructor. The iterator's list is shared with that passed in
     * argument (or creates its own iterator's list if the latter points to 0)
     *
     * The new list and that which is copied do not share their elements: the new
     * list contains new instances of the values stored in the list to be copied.
     * Of course if these values are pointers, the new values point toward the same
     * elements. This constructor runs in linear time.
     * @param from the list the contents of which is copied into the current one
     * @param iter_list a pointer on the list where iterators on the current List
     * will be referenced. When equal to 0, a new iterator's list is created for
     * the current List. */
    // ============================================================================
    List (const List<Val> &src,
          const RefPtr< ListBase<ListConstIterator<Val>*> >& iter_list);

    // ============================================================================
    /// Destructor
    /** Upon deletion, the iterators pointing on elements of the List
     * are detached from it but are not deleted. Trying to access to the element
     * they point to will throw an exception UndefinedIteratorValue. If the List's
     * iterator's list was shared by other Lists, then it is kept in memory, else
     * it is removed. The destructor runs in linear time, both in the size of the
     * list and in the number of iterators on the list. */
    // ============================================================================
    ~List();

    /// @}


    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns an iterator pointing to the end of the List
    // ============================================================================
    const const_iterator& end() const ;
    const iterator& end();

    // ============================================================================
    /// returns an iterator pointing just before the beginning of the List
    // ============================================================================
    const const_iterator& rend() const ;
    const iterator& rend();

    // ============================================================================
    /// returns an iterator pointing to the beginning of the List
    // ============================================================================
    const_iterator begin() const;
    iterator begin();

    // ============================================================================
    /// returns an iterator pointing to the last element of the List
    // ============================================================================
    const_iterator rbegin() const;
    iterator rbegin();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// inserts a new element (a copy) at the beginning of the chained list.
    /** The value passed in argument is not actually inserted into the list: this
     * is a copy of this value that is inserted. The method runs in constant time.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    using ListBase<Val>::pushFront;
    using ListBase<Val>::push_front;

    // ============================================================================
    /// inserts a new element (a copy) at the end of the chained list.
    /** The value passed in argument is not actually inserted into the list: this
     * is a copy of this value that is inserted. The method runs in constant time.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    using ListBase<Val>::pushBack;
    using ListBase<Val>::push_back;

    // ============================================================================
    /// inserts a new element at the end of the chained list (alias of pushBack)
    /** @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    using ListBase<Val>::insert;

    // ============================================================================
    /// returns a reference to first element of a list, if any
    /** @throw NotFound exception is thrown if the list is empty */
    // ============================================================================
    using ListBase<Val>::front;

    // ============================================================================
    /// returns a reference to last element of a list, if any
    /** @throw NotFound exception is thrown if the list is empty */
    // ============================================================================
    using ListBase<Val>::back;

    // ============================================================================
    /// returns the number of elements in the list.
    /** This method runs in constant time. */
    // ============================================================================
    using ListBase<Val>::size;

    // ============================================================================
    /// checks whether there exists a given element in the list.
    /** This method runs in linear time.
     * @param val the value of the element we wish to check the existence of.
     * Comparisons between Val instances are performed through == operators. */
    // ============================================================================
    using ListBase<Val>::exists;

    // ============================================================================
    /// erases the ith element of the List (the first one is in position 0)
    /** If the element cannot be found, the function returns without throwing any
     * exception. It runs in linear time in the size of the list.
     * @param i the position in the list of the element we wish to remove. */
    // ============================================================================
    void erase (unsigned int i);

    // ============================================================================
    /// erases the element of the List pointed to by the iterator
    /** If the element cannot be found, i.e., it has already been erased or the
     * iterator points to end/rend, the function returns without throwing any
     * exception. It runs in linear time in the size of the list. */
    // ============================================================================
    void erase (const iterator& iter);

    /// erases the element of the List pointed to by the const iterator
    /** If the element cannot be found, i.e., it has already been erased or the
     * iterator points to end/rend, the function returns without throwing any
     * exception. It runs in linear time in the size of the list. */
    void erase (const const_iterator& iter);

    // ============================================================================
    /// erases the first element encountered with a given value
    /** If no element equal to \e val can be found, the function
     * returns without throwing any exception. It runs in linear time both in
     * the size of the list and in the number of iterators referenced in the list.
     * Comparisons between Val instances are performed through == operators.
     * @param val the value of the element we wish to remove. */
    // ============================================================================
    void eraseByVal (const Val& val);

    // ============================================================================
    /// erases all the elements encountered with a given value
    /** If no element equal to \e val can be found, the function
     * returns without throwing any exception.
     * @param val the value of the element we wish to remove.
     * Comparisons between Val instances are performed through == operators. */
    // ============================================================================
    void eraseAllVal (const Val& val);

    // ============================================================================
    /// removes the last element of a List, if any
    /** When the list is empty, it does not do anything. */
    // ============================================================================
    void popBack();

    // ============================================================================
    /// removes the first element of a List, if any
    /** When the list is empty, it does not do anything. */
    // ============================================================================
    void popFront();

    // ============================================================================
    /// deletes all the elements of a chained list.
    /** All the iterators of the list will be resetted to rend. The method runs in
     * linear time of both the size of the list and the number of iterators
     * attached to the List. */
    // ============================================================================
    void clear();

    // ============================================================================
    /** @brief changes the iterator's list (this implies that all iterators
     * previously attached to the List will point to end/rend) */
    // ============================================================================
    void
    setIteratorList (const RefPtr< ListBase<ListConstIterator<Val>*> >& list);

    // ============================================================================
    /// returns the list containing the iterators pointing toward the List
    /** note that this list may also contain iterators pointing toward other Lists
     * as the iterator's lists may be shared by several Lists. */
    // ============================================================================
    const RefPtr< ListBase<ListConstIterator<Val>*> >& getIteratorList() const ;

    // ============================================================================
    /// returns a boolean indicating whether the chained list is empty
    // ============================================================================
    using ListBase<Val>::empty;

    // ============================================================================
    /// converts a list into a string
    // ============================================================================
    using ListBase<Val>::toString;

    // ============================================================================
    /// creates a list of mountains from a list of val
    /** @param f a function that maps any Val element into a Mount */
    // ============================================================================
    template <typename Mount> List<Mount> map (Mount (*f) (Val)) const;

    /// creates a list of mountains from a list of val
    /** @param f a function that maps any Val element into a Mount */
    template <typename Mount> List<Mount> map (Mount (*f) (Val&)) const;

    /// creates a list of mountains from a list of val
    /** @param f a function that maps any Val element into a Mount */
    template <typename Mount> List<Mount> map (Mount (*f) (const Val&)) const;

    // ============================================================================
    /// creates a list of mountains with a given value from a list of val
    /** @param mount the value taken by all the elements of the resulting list  */
    // ============================================================================
    template <typename Mount> List<Mount> map (const Mount& mount) const;

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator. The List keeps its own iterator's list.
    /** The new list and that which is copied do not share the
     * elements: the new list contains new instances of the values stored in the
     * list to be copied. Of course if these values are pointers, the new values
     * point toward the same elements. The List on which the operator is applied
     * keeps its iterator's list. Of course, if it previously contained some
     * elements, those are removed prior to the copy. This constructor runs in
     * linear time.
     * @warning If the current List previously contained iterators, those will
     * be resetted to rend().
     * @param from the list the content of which will be copied into the
     * current List */
    // ============================================================================
    List<Val>& operator= (const List<Val>& src);

    // ============================================================================
    /// inserts a new element at the end of the list (alias of pushBack).
    /** This enables writing code like \c list \c += \c xxx; to add element \c xxx
     * to the list.
     * @return a reference on the copy inserted into the list.
     * @warning Note that \e val is not actually inserted into the list. Rather, it
     * is a copy of val that is inserted. */
    // ============================================================================
    Val& operator+= (const Val& val);

    // ============================================================================
    /// checks whether two lists are identical (same elements in the same order)
    /** this method runs in time linear in the number of elements of the list */
    // ============================================================================
    bool operator== (const List<Val>& src) const;

    // ============================================================================
    /// checks whether two lists are different (different elements or orders)
    /** this method runs in time linear in the number of elements of the list */
    // ============================================================================
    bool operator!= (const List<Val>& src) const;

    // ============================================================================
    /// returns the ith element in the current chained list.
    /** The first of the list element has index 0.
     * This method runs in linear time.
     * @param i the position of the element in the list (0 = first element)
     * @throw NotFound exception is thrown if the element to be retrieved
     * does not exist
     * @return a reference on the element stored at the ith position in the list */
    // ============================================================================
    Val& operator[] (const unsigned int i) ;

    /// returns the const ith element in the current chained list.
    /** The first of the list element has index 0.
     * This method runs in linear time.
     * @param i the position of the element in the list (0 = first element)
     * @throw NotFound exception is thrown if the element to be retrieved
     * does not exist
     * @return a reference on the element stored at the ith position in the list */
    const Val& operator[] (const unsigned int i) const ;

    /// @}


  protected:
    // ============================================================================
    /// suppresses an element from a chained list.
    /** If parameter \e bucket is equal to 0, then the method does not perform
     * anything, else the bucket is deleted. In the latter case, no test is ever
     * performed to check that the bucket does actually belong to the List. The
     * method runs in linear time in the number of iterators on the list.
     * @param bucket a pointer on the bucket in the chained list we wish
     * to remove. */
    // ============================================================================
    void _erase (ListBucket<Val>* bucket);


  private:
    /// ListIterator should be a friend to optimize access to elements

    friend class ListIterator<Val>;

    friend class ListConstIterator<Val>;

#ifndef SWIG // SWIG cannot read these lines
    /// for friendly displaying the content of a list
    friend std::ostream& operator<< <> (std::ostream& stream, const List<Val>& list);
#endif // SWIG

    /** \brief the list of iterators attached to the current list. Note that this
     * list may be shared by other Lists. Indeed, during its construction, a
     * pointer to an external iterator's list may indeed be passed to the List.
     * This feature is particularly useful for complex classes which aggregate
     * several distinct lists as it speeds up significantly
     * iterating over all the elements of these complex classes. */
    RefPtr< ListBase<ListConstIterator<Val>*> > __iterator_list;
  };





  /* =========================================================================== */
  /* ===                            LIST ITERATORS                           === */
  /* =========================================================================== */
  /** @class ListConstIterator
   * @brief iterators for Lists.
   *
   * Class ListConstIterator implements iterators for List. However, developers
   * may consider using List<x>::iterator instead of ListConstIterator<x>. These
   * iterators ensure that whenever they point to an element that is being deleted
   * from memory, their accessing this element will never produce a segmentation
   * fault but rather throw an exception. Similarly, incrementing or decrementing
   * an iterator pointing to a deleted element is guaranteed to make the iterator
   * point on the next (or preceding) element that has not been deleted. This
   * enables safely writing code like:
   * \code
   * for (iter=mylist.begin(); iter != mylist.end(); ++iter)
   *   delete *iter;
   * \endcode
   *
   * @par Usage example:
   * @code
   * // create a list of strings
   * List<string> list;
   * list.pushBack ("toto"); list.pushBack ("titi");
   *
   * // parse all the elements of a list
   * for (List<string>::iterator iter = list.begin(); iter != list.end(); ++iter)
   *   cerr << *iter << endl;
   * for (List<string>::iterator iter = list.rbegin(); iter != list.rend(); --iter)
   *   cerr << *iter << endl;
   *
   * // use member size() of the strings
   * for (List<string>::iterator iter = list.begin(); iter != list.end(); ++iter)
   *   cerr << iter->size() << endl;
   * @endcode
   */
  /* =========================================================================== */

  template <typename Val> class ListConstIterator {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// basic constructor. returns an iterator pointing toward nothing
    // ============================================================================
    ListConstIterator() ;

    // ============================================================================
    /// copy constructor
    // ============================================================================
    ListConstIterator (const ListConstIterator<Val>& src);

    // ============================================================================
    /// Constructor for an iterator pointing to the \e ind_eltth element of a List
    // ============================================================================
    ListConstIterator (const List<Val>& theList, unsigned int ind_elt);

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~ListConstIterator() ;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// Makes the iterator point toward nothing
    /** A method for detaching the iterator from the List it is attached to.
     * It is mainly used by the List when the latter is deleted while the
     * iterator is still alive. After being detached, the iterator does not point
     * to any element, i.e., trying to access its content will raise an exception.
     */
    // ============================================================================
    void clear() ;

    // ============================================================================
    /// positions the iterator to the end of the list
    // ============================================================================
    void setToEnd() ;

    // ============================================================================
    /** @brief returns a bool indicating whether the iterator points to the end
     * of the list */
    // ============================================================================
    bool isEnd() const ;

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator
    /** The current iterator now points to the same element as iterator \e from. */
    // ============================================================================
    ListConstIterator<Val>& operator= (const ListConstIterator<Val>& src);

    // ============================================================================
    /// makes the iterator point to the next element in the List
    /** for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to parse
     * the whole List as long as no element is added to or deleted from the List
     * while being in the loop. Deleting elements during the loop is guaranteed
     * to never produce a segmentation fault. Runs in constant time. */
    // ============================================================================
    ListConstIterator<Val>& operator++() ;

    // ============================================================================
    /// makes the iterator point to the preceding element in the List
    /** for (iter=rbegin(); iter!=rend(); --iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from
     * the List while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. Runs in constant time. */
    // ============================================================================
    ListConstIterator<Val>& operator--() ;

    // ============================================================================
    /// checks whether two iterators point toward different elements
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    // ============================================================================
    bool operator!= (const ListConstIterator<Val> &src) const ;

    // ============================================================================
    /// checks whether two iterators point toward the same elements.
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    // ============================================================================
    bool operator== (const ListConstIterator<Val> &src) const ;

    // ============================================================================
    /// gives access to the content of the iterator
    /**
     * @throw UndefinedIteratorValue
     */
    // ============================================================================
    const Val& operator*() const;

    // ============================================================================
    /// dereferences the value pointed to by the iterator
    /**
     * @throw UndefinedIteratorValue
     */
    // ============================================================================
    const Val* operator->() const;

    /// @}


  private:
    /** class List must be a friend because it uses the getBucket method
     * to speed up some processes. */

    friend class List<Val>;

    /// the list the iterator is pointing to
    const List<Val> *__list;

    /// the bucket in the chained list pointed to by the iterator
    ListBucket<Val> *__bucket;

    /** \brief the bucket we should start from when we are pointing on a deleted
     * bucket and we decide to do a ++. */
    ListBucket<Val> *__next_current_bucket;

    /** \brief the bucket we should start from when we are pointing on a deleted
     * bucket and we decide to do a --. */
    ListBucket<Val> *__prev_current_bucket;

    /// indicates whether the bucket the iterator points to has been deleted
    bool __null_pointing;

    /// the bucket which contains this iterator in the list of iterators
    ListBucket<ListConstIterator<Val>*> *__container;

    // ============================================================================
    /// returns the bucket the iterator is pointing to.
    // ============================================================================
    ListBucket<Val>* __getBucket() const ;
  };








  /* =========================================================================== */
  /* ===                            LIST ITERATORS                           === */
  /* =========================================================================== */
  /** @class ListIterator
   * @brief iterators for Lists.
   *
   * Class ListIterator implements iterators for List. However, developers
   * may consider using List<x>::iterator instead of ListIterator<x>. These
   * iterators ensure that whenever they point to an element that is being deleted
   * from memory, their accessing this element will never produce a segmentation
   * fault but rather throw an exception. Similarly, incrementing or decrementing
   * an iterator pointing to a deleted element is guaranteed to make the iterator
   * point on the next (or preceding) element that has not been deleted. This
   * enables safely writing code like:
   * \code
   * for (iter=mylist.begin(); iter != mylist.end(); ++iter)
   *   delete *iter;
   * \endcode
   *
   * @par Usage example:
   * @code
   * // create a list of strings
   * List<string> list;
   * list.pushBack ("toto"); list.pushBack ("titi");
   *
   * // parse all the elements of a list
   * for (List<string>::iterator iter = list.begin(); iter != list.end(); ++iter)
   *   cerr << *iter << endl;
   * for (List<string>::iterator iter = list.rbegin(); iter != list.rend(); --iter)
   *   cerr << *iter << endl;
   *
   * // use member size() of the strings
   * for (List<string>::iterator iter = list.begin(); iter != list.end(); ++iter)
   *   cerr << iter->size() << endl;
   * @endcode
   */
  /* =========================================================================== */

  template <typename Val> class ListIterator : public ListConstIterator<Val> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// basic constructor. returns an iterator pointing toward nothing
    // ============================================================================
    ListIterator() ;

    // ============================================================================
    /// copy constructor
    // ============================================================================
    ListIterator (const ListIterator<Val>& src);

    // ============================================================================
    /// Constructor for an iterator pointing to the \e ind_eltth element of a List
    // ============================================================================
    ListIterator (const List<Val>& theList, unsigned int ind_elt);

    // ============================================================================
    /// Destructor
    // ============================================================================
    ~ListIterator() ;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// Makes the iterator point toward nothing
    /** A method for detaching the iterator from the List it is attached to.
     * It is mainly used by the List when the latter is deleted while the
     * iterator is still alive. After being detached, the iterator does not point
     * to any element, i.e., trying to access its content will raise an exception.
     */
    // ============================================================================
    void clear() ;

    // ============================================================================
    /// positions the iterator to the end of the list
    // ============================================================================
    void setToEnd() ;

    // ============================================================================
    /** @brief returns a bool indicating whether the iterator points to the end
     * of the list */
    // ============================================================================
    bool isEnd() const ;

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator
    /** The current iterator now points to the same element as iterator \e from. */
    // ============================================================================
    ListIterator<Val>& operator= (const ListIterator<Val>& src);

    // ============================================================================
    /// makes the iterator point to the next element in the List
    /** for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to parse
     * the whole List as long as no element is added to or deleted from the List
     * while being in the loop. Deleting elements during the loop is guaranteed
     * to never produce a segmentation fault. Runs in constant time. */
    // ============================================================================
    ListIterator<Val>& operator++() ;

    // ============================================================================
    /// makes the iterator point to the preceding element in the List
    /** for (iter=rbegin(); iter!=rend(); --iter) loops are guaranteed to
     * parse the whole List as long as no element is added to or deleted from
     * the List while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. Runs in constant time. */
    // ============================================================================
    ListIterator<Val>& operator--() ;

    // ============================================================================
    /// checks whether two iterators point toward different elements
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    // ============================================================================
    bool operator!= (const ListIterator<Val> &src) const ;

    // ============================================================================
    /// checks whether two iterators point toward the same elements.
    /** @warning the end and rend iterators are always equal, whatever the list
     * they belong to, i.e., \c list1.end() == \c list2.rend(). */
    // ============================================================================
    bool operator== (const ListIterator<Val> &src) const ;

    // ============================================================================
    /// gives access to the content of the iterator
    /**
     * @throw UndefinedIteratorValue
     */
    // ============================================================================
    Val& operator*();
    const Val& operator*() const;

    // ============================================================================
    /// dereferences the value pointed to by the iterator
    /**
     * @throw UndefinedIteratorValue
     */
    // ============================================================================
    Val* operator->();
    const Val* operator->() const;

    /// @}

  };


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // constructor and destructor for the iterator that represents end and rend
  template <> ListConstIterator<Debug>::ListConstIterator() ;
  template <> ListConstIterator<Debug>::~ListConstIterator() ;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/list.inl>
#endif /* GUM_NO_INLINE */

/// always include the implementation of the templates
#include <agrum/core/list.tcc>

#endif  /* GUM_LIST_H */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;
