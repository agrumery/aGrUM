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
 * @brief Class providing generic hash tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides class HashTable. This class is both efficient and flexible:
 * efficient because the access to elements is usually computed using a small
 * amount of processor instructions, and flexible because several methods allow to
 * fine tune the behavior of each hash table. For instance, a hashtable can
 * allow or forbid different elements to have the same key. This behavior can be
 * modified at any time during the execution of the program. Functions for hashing
 * keys are defined in file HashFunc.h. Here again, these functions are quite
 * flexible as they can be overloaded by the user to support new kind of keys. In
 * addition to HashTable, the current file provides classes HashTableIteratorSafe
 * and HashTableConstIteratorSafe (a.k.a.  HashTable<>::iterator_safe and
 * HashTable<>::const_iterator_safe) that allow safe parsing of the hash tables.
 * By safe, we mean that whenever the element pointed to by such an iterator is
 * removed from the hashtable, accessing it through the iterator (*iter) does not
 * result in a segmentation fault but rather in an exception being thrown. This
 * safety is ensured at a very low cost (actually, our experiments show that our
 * HashTables and HashTable's safe iterators significantly outperform the standard
 * library unordered_maps). Of course, if there is no possibility for an iterator
 * to point to a deleted element, the user can use the "unsafe" iterators
 * HashTableIterator and HashTableConstIterator (a.k.a. HashTable<>::iterator and
 * HashTable<>::const_iterator). These iterators are slightly faster than their
 * safe counterparts. However, as in the standard library, accessing through them
 * a deleted element usually results in a mess (most probably a segfault).
 *
 * @warning HashTables @b guarantee that any element stored within them will have
 * the @b same @b location in memory until it is removed from the hashtable
 * (and this holds whatever operation is performed on the hashtable like new
 * insertions, deletions, resizing, etc.).
 *
 * @par Usage example:
 * @code
 * // creation of an empty hash table
 * HashTable<int,string> table1;
 *
 * // insert two elements into the hash table
 * table1.insert (10,"xxx");
 * table1.insert (20,"yyy");
 * table1.emplace (30,"zzz");
 *
 * // creation of a nonempty hashtable using initializer lists
 * HashTable<int,bool> table { std::make_pair (3,true), std::make_pair(2,false) };
 *
 * // display the content of the hash table
 * cerr << table1;
 *
 * // get the number of elements stored into the hash table
 * cerr << "number of elements in table1 = " << table1.size () << endl;
 *
 * // create two copies of the hash table
 * HashTable<int,string> table2, table3 = table1;
 * table2 = table3;
 *
 * // get the element whose key is 10
 * cerr << table1[10] << " = xxx" << endl;
 *
 * // check whether there exists an element with key 20
 * if (table1.exists (20)) cerr << "element found" << endl;
 *
 * // transform the hashtable of string into a hashtable of int assuming f is
 * // defined as: int f (const string& str) { return str.size (); }
 * HashTable<int,int> table = table1.map (f);
 *
 * // remove two elements from table1 and table2
 * table1.erase (10);         // key = 10
 * table1.eraseByVal ("yyy"); // val = "yyy"
 * table2.clear ();
 *
 * // check whether the hash table is empty
 * if (!table1.empty ()) cerr << "table not empty" << endl;
 *
 * // check wether hashtables contain the same elements
 * if ((table1 == table2) && (table1 != table3))
 *   cerr << "check for equality/inequality" << endl;
 *
 * // parse the content of a hashtable using an unsafe iterator
 * for (HashTable<int,string>::const_iterator iter = table1.cbegin();
 *       iter != table1.cend(); ++iter)
 *   cerr << *iter;
 * HashTable<int,string>::iterator iter = table1.begin();
 * iter += 2;
 * cerr << iter.key () << " " << iter.val ();
 *
 * // use an iterator to point the element we wish to erase
 * HashTable<int,string>::iterator_safe iterS = table1.beginSafe ();
 * table1.erase ( table1.beginSafe () + 4 );
 * table1.erase ( iterS ); // this is safe because the iterator is safe
 *
 * // check for iterator's safety
 * for (HashTable<int,string>::iterator_safe iter = table1.beginSafe ();
 *       iter != table1.endSafe (); ++iter )
 *   table1.eraseByVal ( *iter );
 * @endcode
 */

#ifndef GUM_HASH_TABLE_H
#define GUM_HASH_TABLE_H

#include <iostream>
#include <utility>
#include <cstddef>
#include <string>
#include <vector>
#include <initializer_list>

#include <agrum/config.h>

#include <agrum/core/hashFunc.h>

/* ############################################################################# */
/* #                           WARNING:  DEPENDENCIES                          # */
/* ############################################################################# */
/*
 * agrum/core/set.tcc: to speed-up accessors in sets, we rely on the fact (which
 * holds currently) that hashTable's iterators end are never modified by
 * insertions or deletions of elements in hash tables. If this property were to be
 * changed, set.tcc should be updated accordingly
 * agrum/core/bijection.tcc: same as set.tcc but, in addition, bijections assume
 * that once a pair (key,val) has been created in the hashtable, its location in
 * memory will never change, even if the hashtable is resized.
 * agrum/core/sequence.tcc: same as bijection.tcc
 * agrum/core/priorityQueue.tcc: same as bijection.tcc
 * agrum/core/heap.tcc: same as bijection.tcc
 */
/* ############################################################################# */

namespace gum {

  // parameters specifying the default behavior of the hashtables
  struct HashTableConst {
    // The default number of slots in hashtables. By default, hashtables can store
    // up to thrice the number of slots elements before their size is increased
    // To each slot corresponds a chained list of elements with the same hashed key
    static constexpr Size default_size{4};

    // the average number of elements admissible by slots. Once this number is
    // reached, the size of the hashtable is automatically doubled if we are in
    // automatic resize mode
    static constexpr unsigned int default_mean_val_by_slot{3};

    // a Boolean indicating whether inserting too many values into the hashtable
    // makes it resize itself automatically. true = automatic, false = manual
    static constexpr bool default_resize_policy{true};

    // a Boolean indicating the default behavior when trying to insert more than
    // once elements with identical keys. true = do not insert the elements but the
    // first one and throw an exception after the first insertion; false = insert
    // the elements without complaining
    static constexpr bool default_uniqueness_policy{true};
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // the templates defined by this file
  template <typename Key, typename Val, typename Alloc> class HashTable;
  template <typename Key, typename Val, typename Alloc> class HashTableList;
  template <typename Key, typename Val> class HashTableIterator;
  template <typename Key, typename Val> class HashTableConstIterator;
  template <typename Key, typename Val> class HashTableIteratorSafe;
  template <typename Key, typename Val> class HashTableConstIteratorSafe;
  template <typename T1, typename T2, typename Alloc> class Bijection;

  /// a << operator for HashTableList
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &, const HashTableList<Key, Val, Alloc> &);

  /// a << operator for HashTableList with pointer keys
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &, const HashTableList<Key *, Val, Alloc> &);

  /// a \c << operator for HashTable
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &, const HashTable<Key, Val, Alloc> &);

  /// a \c << operator for HashTable with pointer keys
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &, const HashTable<Key *, Val, Alloc> &);

  // a class used to create the static iterator used by HashTables. The aim of
  // using this class rather than just creating __HashTableIterEnd as a global
  // variable is to prevent other classes to access and modify __HashTableIterEnd
  class HashTableIteratorStaticEnd {
    private:
    // the unsafe iterator used by everyone
    static const HashTableIterator<int, int> *__HashTableIterEnd;

    // the safe iterator used by everyone
    static const HashTableIteratorSafe<int, int> *__HashTableIterEndSafe;

    // creates (if needed) and returns the iterator __HashTableIterEnd
    static const HashTableIterator<int, int> *end4Statics();

    // creates (if needed) and returns the iterator __HashTableIterEnd
    static const HashTableConstIterator<int, int> *constEnd4Statics();

    // creates (if needed) and returns the iterator __HashTableIterEndSafe
    static const HashTableIteratorSafe<int, int> *endSafe4Statics();

    // creates (if needed) and returns the iterator __HashTableIterEndSafe
    static const HashTableConstIteratorSafe<int, int> *constEndSafe4Statics();

    // friends that have access to the iterator
    template <typename Key, typename Val, typename Alloc> friend class HashTable;
  };

  /* =========================================================================== */
  /* ===          LISTS SPECIFIC FOR SAVING ELEMENTS IN HASHTABLES           === */
  /* =========================================================================== */

  /* =========================================================================== */
  /* in aGrUM, hashtables are vectors of chained lists. Each list corresponds to
   * the pairs (key,val) the keys of which have the same hashed value. Each box of
   * the list is called a bucket. Lists are doubly linked so as to enable efficient
   * begin/end iterators and efficient insert/erase operations. */
  /* =========================================================================== */
  template <typename Key, typename Val> struct HashTableBucket {
    std::pair<const Key, Val> pair;
    HashTableBucket<Key, Val> *prev{nullptr};
    HashTableBucket<Key, Val> *next{nullptr};

    // a dummy type for the emplace constructor
    /* this type is used to prevent the Bucket emplace (int,...) to compile */
    enum class Emplace { EMPLACE };

    HashTableBucket() {}
    HashTableBucket(const HashTableBucket<Key, Val> &from) : pair{from.pair} {}
    HashTableBucket(const Key &k, const Val &v) : pair{k, v} {}
    HashTableBucket(Key &&k, Val &&v) : pair{std::move(k), std::move(v)} {}
    HashTableBucket(const std::pair<const Key, Val> &p) : pair(p) {}
    HashTableBucket(std::pair<const Key, Val> &&p) : pair(std::move(p)) {}
    template <typename... Args>
    HashTableBucket(Emplace, Args &&... args)
        : // emplace (universal) constructor
          pair(std::forward<Args>(args)...) {}
    ~HashTableBucket() {}

    std::pair<const Key, Val> &elt() { return pair; }
    Key &key() { return const_cast<Key &>(pair.first); }
    Val &val() { return pair.second; }
  };

  /* =========================================================================== */
  /* ===       DOUBLY CHAINED LISTS FOR STORING ELEMENTS IN HASH TABLES      === */
  /* =========================================================================== */
  template <typename Key, typename Val, typename Alloc> class HashTableList {
    public:
    /// types for STL compliance
    /// @{
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const Key, Val>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = std::size_t;
    using allocator_type = Alloc;
    using Bucket = HashTableBucket<Key, Val>;
    /// @}

    /// the Bucket allocator
    using BucketAllocator = typename Alloc::template rebind<Bucket>::other;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /* basic constructor that creates an empty list. This is what is used
     * basically by hash tables
     * @warning if the allocator is not passed in argument, do not forget to use
     * method setAllocator after the creation */
    HashTableList(BucketAllocator *allocator = nullptr) noexcept;

    /// copy constructor.
    /** The new list and that which is copied do not share the
     * elements: the new list contains new instances of the keys and of values
     * stored in the list to be copied. Of course if these values are pointers,
     * the new values point toward the same elements.
     * @warning both from and this will share the same allocator. */
    HashTableList(const HashTableList<Key, Val, Alloc> &from);

    /// move constructor
    HashTableList(HashTableList<Key, Val, Alloc> &&from) noexcept;

    /// destructor
    ~HashTableList();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// assignment operator.
    /** The new list and that which is copied do not share their
     * elements: the new list contains new instances of the keys and of values
     * stored in the list to be copied. Of course if these values are pointers,
     * the new values point toward the same elements. If some allocation problem
     * occurs or if the copy of the Val elements cannot be performed properly,
     * exceptions may be raised. In this case, the function guarantees that no
     * memory leak occurs and that the list is kept in a coherent state (that of
     * an empty list).
     * @warning operator= does not change the current allocator of *this */
    HashTableList<Key, Val, Alloc> &
    operator=(const HashTableList<Key, Val, Alloc> &from);

    /** assignment operator. The new list and that which is copied do not share
     * their elements: the new list contains new instances of the keys and of
     * values stored in the list to be copied. Of course if these values are
     * pointers, the new values point toward the same elements. If some allocation
     * problem occurs or if the copy of the Val elements cannot be performed
     * properly, exceptions may be raised. In this case, the function guarantees
     * that no memory leak occurs and that the list is kept in a coherent state
     * (that of an empty list)
     * @warning operator= does not change the current allocator of *this */
    template <typename OtherAlloc>
    HashTableList<Key, Val, Alloc> &
    operator=(const HashTableList<Key, Val, OtherAlloc> &from);

    /// move operator
    /** @warning operator= does not change the current allocator of *this */
    HashTableList<Key, Val, Alloc> &
    operator=(HashTableList<Key, Val, Alloc> &&from) noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** function at returns the ith element in the current chained list. The first
     * element has index 0.
     * @throw NotFound if the list has fewer than i elements */
    value_type &at(unsigned int i);

    /** function at returns the ith element in the current chained list. The first
     * element has index 0.
     * @throw NotFound if the list has fewer than i elements */
    const value_type &at(unsigned int i) const;

    /// operator [] returns the value corresponding to a given key
    /**
     * @throw NotFound is raised if the element cannot be found
     */
    mapped_type &operator[](const key_type &key);

    /// operator [] returns the value corresponding to a given key
    /**
     * @throw NotFound is raised if the element cannot be found
     */
    const mapped_type &operator[](const key_type &key) const;

    /// check whether there exists an element with a given key in the list
    bool exists(const key_type &key) const;

    /** inserts a new element in the chained list. The element is inserted
     * at the beginning of the list. */
    void insert(Bucket *new_elt) noexcept;

    /// function erase removes an element from a chained list.
    void erase(Bucket *ptr);

    /// function for deleting all the elements of a chained list
    void clear();

    /// returns a boolean indicating whether the chained list is empty
    bool empty() const noexcept;

    /// a method to get the bucket corresponding to a given key. This enables
    /// efficient removals of buckets
    Bucket *bucket(const Key &key) const;

    /// sets a new allocator
    void setAllocator(BucketAllocator &alloc);

    /// @}

    private:
    // friends
    template <typename K, typename V, typename A> friend class HashTableList;
    friend class HashTable<Key, Val, Alloc>;
    friend class HashTableIterator<Key, Val>;
    friend class HashTableConstIterator<Key, Val>;
    friend class HashTableIteratorSafe<Key, Val>;
    friend class HashTableConstIteratorSafe<Key, Val>;
    friend std::ostream &operator<<<>(std::ostream &,
                                      const HashTableList<Key, Val, Alloc> &);
    friend std::ostream &operator<<<>(std::ostream &,
                                      const HashTableList<Key *, Val, Alloc> &);
    friend std::ostream &operator<<<>(std::ostream &,
                                      const HashTable<Key, Val, Alloc> &);
    friend std::ostream &operator<<<>(std::ostream &,
                                      const HashTable<Key *, Val, Alloc> &);

    // a pointer on the first element of the chained list
    HashTableBucket<Key, Val> *__deb_list{nullptr};

    // a pointer on the last element of the chained list
    HashTableBucket<Key, Val> *__end_list{nullptr};

    // the number of elements in the chained list
    unsigned int __nb_elements{0};

    // the allocator of the containing hashTable
    mutable BucketAllocator *__alloc_bucket;

    /* a function used to perform copies of HashTableLists. This code is
     * shared by the copy constructor and the copy operator. If it cannot perform
     * the necessary allocations, no memory leak occurs and the list is set to
     * the empty list. */
    template <typename OtherAlloc>
    void __copy(const HashTableList<Key, Val, OtherAlloc> &from);
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /* =========================================================================== */
  /* ===                          GENERIC HASH TABLES                        === */
  /* =========================================================================== */
  /**
   * @class HashTable
   * @brief The class for generic Hash Tables
   * @ingroup basicstruct_group
   *
   * In aGrUM, a hashtable is a vector of chained lists (collision problems are
   * fixed by chaining). Each slot of the vector contains a list of elements
   * sharing the same hashed value. To be computationally efficient, the hash table
   * should not contain too many elements as compared to its number of slots.
   * Therefore, it is sometimes useful to resize the chained lists vector. aGrUM's
   * hash tables are designed to automatically double their size when there is in
   * average more than 3 elements per slot. However, when memory consumption is a
   * concern, this feature can be turned off either by passing false as an optional
   * resize_pol argument to the constructor of the hash table or by using method
   * setResizePolicy when the instance of the class has already been constructed.
   * Similarly, the default number of slots of the hash table may be parameterized
   * as an optional argument of the constructor (size_param). Beware: when
   * inserting elements of a given class into a hash table, unless the element is
   * an r-value, only a copy of this element is stored into the table (this is
   * compulsory if the hashtable is to be generic and can be used to store both
   * complex classes and built-in types like integers). HashTable have different
   * kinds of iterators: HashTableIteratorSafe and HashTableConstIteratorSafe
   * (a.k.a.  HashTable<>::iterator_safe and HashTable<>::const_iterator_safe)
   * allow safe parsing of the hash tables. By safe, we mean that whenever the
   * element pointed to by such an iterator is removed from the hashtable,
   * accessing it through the iterator (*iter) does not result in a segmentation
   * fault but rather in an exception being thrown. This safety is ensured at a
   * very low cost (actually, our experiments show that our HashTables and
   * HashTable's safe iterators significantly outperform the standard library
   * unordered_maps). Of course, if there is no possibility for an iterator to
   * point to a deleted element, the user can use "unsafe" iterators
   * HashTableIterator and HashTableConstIterator (a.k.a. HashTable<>::iterator and
   * HashTable<>::const_iterator). These iterators are slightly faster than their
   * safe counterparts. However, as in the standard library, accessing through them
   * a deleted element usually results in a mess (most probably a segfault).
   *
   * @warning HashTables @b guarantee that any element stored within them will have
   * the @b same @b location in memory until it is removed from the hashtable
   * (and this holds whatever operation is performed on the hashtable like new
   * insertions, deletions, resizing, etc.).
   *
   * @par Usage example:
   * @code
   * // creation of an empty hash table
   * HashTable<int,string> table1;
   *
   * // insert two elements into the hash table
   * table1.insert (10,"xxx");
   * table1.insert (20,"yyy");
   * table1.emplace (30,"zzz");
   *
   * // creation of a nonempty hashtable using initializer lists
   * HashTable<int,bool> table { std::make_pair(3,true), std::make_pair(2,false) };
   *
   * // display the content of the hash table
   * cerr << table1;
   *
   * // get the number of elements stored into the hash table
   * cerr << "number of elements in table1 = " << table1.size () << endl;
   *
   * // create two copies of the hash table
   * HashTable<int,string> table2, table3 = table1;
   * table2 = table3;
   *
   * // get the element whose key is 10
   * cerr << table1[10] << " = xxx" << endl;
   *
   * // check whether there exists an element with key 20
   * if (table1.exists (20)) cerr << "element found" << endl;
   *
   * // transform the hashtable of string into a hashtable of int assuming f is
   * // defined as: int f (const string& str) { return str.size (); }
   * HashTable<int,int> table = table1.map (f);
   *
   * // remove two elements from table1 and table2
   * table1.erase (10);         // key = 10
   * table1.eraseByVal ("yyy"); // val = "yyy"
   * table2.clear ();
   *
   * // check whether the hash table is empty
   * if (!table1.empty ()) cerr << "table not empty" << endl;
   *
   * // check wether hashtables contain the same elements
   * if ((table1 == table2) && (table1 != table3))
   *   cerr << "check for equality/inequality" << endl;
   *
   * // parse the content of a hashtable using an unsafe iterator
   * for (HashTable<int,string>::const_iterator iter = table1.cbegin();
   *       iter != table1.cend(); ++iter)
   *   cerr << *iter;
   * HashTable<int,string>::iterator iter = table1.begin();
   * iter += 2;
   * cerr << iter.key () << " " << iter.val ();
   *
   * // use an iterator to point the element we wish to erase
   * HashTable<int,string>::iterator_safe iterS = table1.beginSafe ();
   * table1.erase ( table1.beginSafe () + 4 );
   * table1.erase ( iterS ); // this is safe because the iterator is safe
   *
   * // check for iterator's safety
   * for (HashTable<int,string>::iterator_safe iter = table1.beginSafe ();
   *       iter != table1.endSafe (); ++iter )
   *   table1.eraseByVal ( *iter );
   * @endcode
   */
  template <typename Key, typename Val,
            typename Alloc = std::allocator<std::pair<Key, Val>>>
  class HashTable {
    public:
    /// types for STL compliance
    /// @{
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const Key, Val>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = HashTableIterator<Key, Val>;
    using const_iterator = HashTableConstIterator<Key, Val>;
    using iterator_safe = HashTableIteratorSafe<Key, Val>;
    using const_iterator_safe = HashTableConstIteratorSafe<Key, Val>;
    /// @}

    /// the buckets where data are stored
    using Bucket = HashTableBucket<Key, Val>;

    /// the Bucket allocator
    using BucketAllocator = typename Alloc::template rebind<Bucket>::other;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################

    /// @{

    /// Default constructor
    /** The true capacity (vector's size) of the hashtable will be the
     * lowest number greater than or equal to size_param that is also a power of 2.
     * The second optional argument is the resizing policy. By default, each time
     * there is an average of 3 elements by node, the size of the hashtable is
     * automatically multiplied by 2. But the user may pass false as argument to
     * resize_pol to disable this feature.
     * @param size_param the size of the vector of chained lists
     * @param resize_pol the policy for resizing the hashtable when new elements
     * are added (possible values: true = automatic resize and false = manual
     * resize)
     * @param key_uniqueness_pol uniqueness policy : should we prevent inserting
     * the same key more than once in the table? */
    explicit HashTable(
        Size size_param = HashTableConst::default_size,
        bool resize_pol = HashTableConst::default_resize_policy,
        bool key_uniqueness_pol = HashTableConst::default_uniqueness_policy);

    /// initializer list constructor
    explicit HashTable(std::initializer_list<std::pair<Key, Val>> list);

    /// copy constructor
    /** This creates a new hashtable the content of which is
     * similar to that of the table passed in argument. Beware: similar does not
     * mean that both tables share the same objects, but rather that the objects
     * stored in the newly created table are copies of those of the table passed
     * in argument. In particular, the new hash table inherits the parameters
     * (resize policy, uniqueness policy) of table 'table' */
    HashTable(const HashTable<Key, Val, Alloc> &table);

    /// generalized copy constructor
    /** This creates a new hashtable the content of which is
     * similar to that of the table passed in argument. Beware: similar does not
     * mean that both tables share the same objects, but rather that the objects
     * stored in the newly created table are copies of those of the table passed
     * in argument. In particular, the new hash table inherits the parameters
     * (resize policy, uniqueness policy) of table 'table' */
    template <typename OtherAlloc>
    HashTable(const HashTable<Key, Val, OtherAlloc> &table);

    /// move constructor
    HashTable(HashTable<Key, Val, Alloc> &&table);

    /// destructor
    ~HashTable();

    /// @}

    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// returns the unsafe iterator pointing to the end of the hashtable
    /** unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower). */
    const iterator &end() noexcept;

    /// returns the unsafe const_iterator pointing to the end of the hashtable
    /** unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower). */
    const const_iterator &end() const noexcept;

    /// returns the unsafe const_iterator pointing to the end of the hashtable
    /** unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower). */
    const const_iterator &cend() const noexcept;

    /// returns an unsafe iterator pointing to the beginning of the hashtable
    /** unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower). */
    iterator begin();

    /// returns an unsafe const_iterator pointing to the beginning of the hashtable
    /** unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower). */
    const_iterator begin() const;

    /// returns an unsafe const_iterator pointing to the beginning of the hashtable
    /** unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower). */
    const_iterator cbegin() const;

    /// returns the safe iterator pointing to the end of the hashtable
    /** Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element. */
    const iterator_safe &endSafe() noexcept;

    /// returns the safe const_iterator pointing to the end of the hashtable
    /** Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element. */
    const const_iterator_safe &endSafe() const noexcept;

    /// returns the safe const_iterator pointing to the end of the hashtable
    /** Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element. */
    const const_iterator_safe &cendSafe() const noexcept;

    /// returns the safe iterator pointing to the beginning of the hashtable
    /** Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element. */
    iterator_safe beginSafe();

    /// returns the safe const_iterator pointing to the beginning of the hashtable
    /** Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element. */
    const_iterator_safe beginSafe() const;

    /// returns the safe const_iterator pointing to the beginning of the hashtable
    /** Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element. */
    const_iterator_safe cbeginSafe() const;

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce memory consumption of hash tables (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops,
     * end iterators are created just once as a static member of a non-template
     * hashtable. While this scheme is efficient and it works quite effectively
     * when manipulating hashtables, it has a drawback: other classes with static
     * members using the HashTable's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's
     * C++ FAQ for more details about this C++ feature). OK, so what is the
     * problem? Consider for instance class Set. A Set contains a hashtable that
     * stores all its elements in a convenient way. To reduce memory consumption,
     * Set::end iterator is a static member that is initialized with a
     * HashTable's end iterator. If the compiler decides to initialize Set::end
     * before initializing HashTable::end, then Set::end will be in an incoherent
     * state. Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we shall
     * enforce the fact that HashTable::end is initialized before Set::end. Using
     * method HashTable::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics initializes a global
     * variable that is the very end iterator used by all hashtables. Now, this
     * induces a small overhead. So, we also provide a HashTable::end() method
     * that returns the end iterator without this small overhead, but assuming that
     * function end4Statics has already been called once (which is always the case)
     * when a hashtable has been created.
     *
     * So, to summarize: when initializing static members, use end4Statics() rather
     * than end(). In all the other cases, use simply the usual method end(). */
    static const iterator &end4Statics();

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce memory consumption of hash tables (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops,
     * end iterators are created just once as a static member of a non-template
     * hashtable. While this scheme is efficient and it works quite effectively
     * when manipulating hashtables, it has a drawback: other classes with static
     * members using the HashTable's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's
     * C++ FAQ for more details about this C++ feature). OK, so what is the
     * problem? Consider for instance class Set. A Set contains a hashtable that
     * stores all its elements in a convenient way. To reduce memory consumption,
     * Set::end iterator is a static member that is initialized with a
     * HashTable's end iterator. If the compiler decides to initialize Set::end
     * before initializing HashTable::end, then Set::end will be in an incoherent
     * state. Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we shall
     * enforce the fact that HashTable::end is initialized before Set::end. Using
     * method HashTable::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics initializes a global
     * variable that is the very end iterator used by all hashtables. Now, this
     * induces a small overhead. So, we also provide a HashTable::end() method
     * that returns the end iterator without this small overhead, but assuming that
     * function end4Statics has already been called once (which is always the case)
     * when a hashtable has been created.
     *
     * So, to summarize: when initializing static members, use constEnd4Statics()
     * rather than cend(). In all the other cases, use simply the usual method
     * cend(). */
    static const const_iterator &constEnd4Statics();

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce memory consumption of hash tables (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops,
     * end iterators are created just once as a static member of a non-template
     * hashtable. While this scheme is efficient and it works quite effectively
     * when manipulating hashtables, it has a drawback: other classes with static
     * members using the HashTable's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's
     * C++ FAQ for more details about this C++ feature). OK, so what is the
     * problem? Consider for instance class Set. A Set contains a hashtable that
     * stores all its elements in a convenient way. To reduce memory consumption,
     * Set::end iterator is a static member that is initialized with a
     * HashTable's end iterator. If the compiler decides to initialize Set::end
     * before initializing HashTable::end, then Set::end will be in an incoherent
     * state. Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we shall
     * enforce the fact that HashTable::end is initialized before Set::end. Using
     * method HashTable::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics initializes a global
     * variable that is the very end iterator used by all hashtables. Now, this
     * induces a small overhead. So, we also provide a HashTable::end() method
     * that returns the end iterator without this small overhead, but assuming that
     * function end4Statics has already been called once (which is always the case)
     * when a hashtable has been created.
     *
     * So, to summarize: when initializing static members, use endSafe4Statics()
     * rather than endSafe(). In all the other cases, use simply the usual method
     * endSafe(). */
    static const iterator_safe &endSafe4Statics();

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce memory consumption of hash tables (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops,
     * end iterators are created just once as a static member of a non-template
     * hashtable. While this scheme is efficient and it works quite effectively
     * when manipulating hashtables, it has a drawback: other classes with static
     * members using the HashTable's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's
     * C++ FAQ for more details about this C++ feature). OK, so what is the
     * problem? Consider for instance class Set. A Set contains a hashtable that
     * stores all its elements in a convenient way. To reduce memory consumption,
     * Set::end iterator is a static member that is initialized with a
     * HashTable's end iterator. If the compiler decides to initialize Set::end
     * before initializing HashTable::end, then Set::end will be in an incoherent
     * state. Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we shall
     * enforce the fact that HashTable::end is initialized before Set::end. Using
     * method HashTable::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics initializes a global
     * variable that is the very end iterator used by all hashtables. Now, this
     * induces a small overhead. So, we also provide a HashTable::end() method
     * that returns the end iterator without this small overhead, but assuming that
     * function end4Statics has already been called once (which is always the case)
     * when a hashtable has been created.
     *
     * So, to summarize: when initializing static members, use
     * constEndSafe4Statics() rather than cendSafe(). In all the other cases, use
     * simply the usual method cendSafe(). */
    static const const_iterator_safe &constEndSafe4Statics();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** the copy operators ensures that whenever a memory allocation problem
     * occurs, no memory leak occurs as well and it also guarantees that in this
     * case the hashtable returned is in a coherent state (it is an empty
     * hashtable). Note that the copy not only involves copying pairs (key,value)
     * but also the copy of the resize and key uniqueness policies. */
    HashTable<Key, Val, Alloc> &operator=(const HashTable<Key, Val, Alloc> &from);

    /// generalized copy operator
    /** the copy operators ensures that whenever a memory allocation problem
     * occurs, no memory leak occurs as well and it also guarantees that in this
     * case the hashtable returned is in a coherent state (it is an empty
     * hashtable). Note that the copy not only involves copying pairs (key,value)
     * but also the copy of the resize and key uniqueness policies. */
    template <typename OtherAlloc>
    HashTable<Key, Val, Alloc> &
    operator=(const HashTable<Key, Val, OtherAlloc> &from);

    /// move operator
    HashTable<Key, Val, Alloc> &operator=(HashTable<Key, Val, Alloc> &&from);

    /// returns a reference on the value the key of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    Val &operator[](const Key &key);

    /// returns a reference on the value the key of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    const Val &operator[](const Key &key) const;

    /// checks whether two hashtables contain the same elements
    /** Two hashtables are considered equal if they contain the identical pairs
     * (key,val). Two pairs are identical if their keys have the same hashed value,
     * these two keys are equal in the sense of ==, and their val's are also equal
     * in the sense of ==. */
    template <typename OtherAlloc>
    bool operator==(const HashTable<Key, Val, OtherAlloc> &from) const;

    /// checks whether two hashtables contain different sets of elements
    /** Two hashtables are considered different if they contain different pairs
     * (key,val). Two pairs are different if their keys have different hashed
     * values, or if they are different in the sense of !=, or if their val's are
     * different in the sense of !=. */
    template <typename OtherAlloc>
    bool operator!=(const HashTable<Key, Val, OtherAlloc> &from) const;

    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    /// returns the number of slots in the 'nodes' vector of the hashtable
    /** The method runs in constant time. */
    Size capacity() const noexcept;

    /// changes the number of slots in the 'nodes' vector of the hash table
    /** Usually, method resize enables the user to resize manually the hashtable.
     * When in automatic resize mode, the function will actually resize the table
     * only if resizing policy is compatible with the new size, i.e., the new size
     * is not so small that there would be too many elements per slot in the table
     * (this would lead to a significant loss in performance). However, the
     * resizing policy may be changed by using method setResizePolicy.
     * The method runs in linear time in the size of the hashtable.
     * Upon memory allocation problem, the fuction guarantees that no data is
     * lost and that the hash table and its iterators are in a coherent state. In
     * such a case, a bad_alloc exception is thrown. */
    void resize(Size new_size);

    /// enables the user to change dynamically the resizing policy
    /** In most cases, this should be useless. However, when available memory
     * becomes rare, avoiding automatic resizing may speed-up new insertions in
     * the table.
     * @warning This function never resizes the hashtable by itself:
     * even if you set the new policy to be an automatic resizing and the number
     * of elements in the table is sufficiently high that we should resize the
     * table, function setResizePolicy won't perform this resizing. The resizing
     * will happen only if you insert a new element or if use method resize. */
    void setResizePolicy(const bool new_policy) noexcept;

    /// returns the current resizing policy
    bool resizePolicy() const noexcept;

    /** @brief enables the user to change dynamically the policy for checking
     * whether there can exist several elements in the table with identical keys
     *
     * By default, we should always check that there does not exist duplicate keys.
     * However, this test slows the insertion of elements in the table. So, when we
     * know for sure that no duplicate key will be entered into the table, we may
     * avoid uniqueness checks.
     * @warning When setting the key policy to "uniqueness", the function does not
     * check whether there are already different elements with identical keys in
     * the table. It thus only ensures that elements inserted from now on will have
     * unique keys. */
    void setKeyUniquenessPolicy(const bool new_policy) noexcept;

    /// returns the current checking policy
    bool keyUniquenessPolicy() const noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the number of elements stored into the hashtable
    /** The method runs in constant time. */
    Size size() const noexcept;

    /// checks whether there exists an element with a given key in the hashtable
    /** The method runs in average in constant time if the resizing policy
     * is set. */
    bool exists(const Key &key) const;

    /// adds a new element (actually a copy of this element) into the hash table
    /** If there already exists an element with the same key in the table and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return As only a copy of val is inserted into the hashtable, the method
     * returns a reference on a copy of the pair (key,val).
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set. */
    value_type &insert(const Key &key, const Val &val);

    /// moves a new element in the hash table
    /** If there already exists an element with the same key in the table and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return a reference to the pair (key,val) in the hashtable.
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set. */
    value_type &insert(Key &&key, Val &&val);

    /// adds a new element (actually a copy of this element) into the hash table
    /** If there already exists an element with the same key in the table and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return As only a copy of val is inserted into the hashtable, the method
     * returns a reference on a copy of the pair (key,val).
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set. */
    value_type &insert(const std::pair<Key, Val> &elt);

    /// moves a new element in the hash table
    /** If there already exists an element with the same key in the table and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return a reference to the pair (key,val) in the hashtable.
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set. */
    value_type &insert(std::pair<Key, Val> &&elt);

    /// emplace a new element into the hashTable
    /** If there already exists an element with the same key in the list and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return a reference to the pair (key,val) inserted in the hash table.
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set. */
    template <typename... Args> value_type &emplace(Args &&... args);

    /// returns a reference on the element the key of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * In case of not found key, (key,default_value) is inserted in *this. */
    mapped_type &getWithDefault(const Key &key, const Val &default_value);

    /// returns a reference on the element the key of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * In case of not found key, (key,default_value) is inserted in *this. */
    mapped_type &getWithDefault(Key &&key, Val &&default_value);

    /// add a new property or modify it if it already existed
    /** When used as a "dynamic property list", it may be convenient to use this
     * function. Function set inserts a new pair (key,val) if the key does not
     * already exists, or it changes the value associated with key if a pair
     * (key,val) already exists in the hash table. */
    void set(const Key &key, const Val &default_value);

    /// remove a property (i.e., remove an element)
    /** reset removes a property (i.e., a pair (key,val)) if it exists. This is an
     * alias for erase but it is quite convenient when dealing with "dynamic
     * property lists". */
    void reset(const Key &key);

    /// removes a given element from the hash table
    /** The element is the first one encountered in the list (from begin() to
     * end()) having the specified key. If no such element can be found, nothing
     * is done (in particular, it does not throw any exception). The function
     * never resizes the nodes vector (even if the resizing policy would enable to
     * decrease this size). The method runs in average in time linear to the
     * number of iterators pointing to the table if the automatic resizing policy
     * is set (else it is in linear time in the number of elements of the hash
     * table plus the number of iterators). */
    void erase(const Key &key);

    /// removes a given element from the hash table
    /** This method updates all the safe iterators pointing to the deleted element,
     * i.e., when trying to dereference those iterators, an exception will be
     * raised because they will know that the element they point to no
     * longer exists. */
    void erase(const iterator_safe &iter);

    /// removes a given element from the hash table
    /** This method updates all the safe iterators pointing to the deleted element,
     * i.e., when trying to dereference those iterators, an exception will be
     * raised because they will know that the element they point to no
     * longer exists. */
    void erase(const const_iterator_safe &iter);

    /// removes a given element from the hash table
    /** The element is the first one encountered in the list (from begin() to
     * end()) having the specified value. If no such element can be found, nothing
     * is done (in particular, it does not throw any exception). The function
     * never resizes the nodes vector (even if the resizing policy would enable to
     * decrease this size). Comparisons between Val instances are performed
     * through == operators. Logically, this method should have been named "erase",
     * however, this would have prevented creating hash tables where both keys and
     * vals have the same type. Hence we chose to add "ByVal" after erase to make
     * a difference between erasing by key and erasing by val. */
    void eraseByVal(const Val &val);

    /// returns a reference on the key the value of which is passed in argument
    /** In case of multiple identical values in the hash table, the first key
     * encountered is returned. The method runs in linear time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    const Key &keyByVal(const Val &val) const;

    /// returns a reference on a given key
    /** Some complex structures use pointers on keys of hashtables. These
     * structures thus require that we do not only get a copy of a given key, but
     * the key stored in the hashtable itself. This is the very purpose of this
     * function.
     * @throw NotFound is raised if the element cannot be found. */
    const Key &key(const Key &key) const;

    /// removes all the elements having a certain value from the hash table
    /** If no such element can be found, nothing is done (in
     * particular, it does not throw any exception). The function never resizes the
     * nodes vector (even if the resizing policy would enable to decrease
     * this size). Comparisons between Val instances are performed through ==
     * operators. */
    void eraseAllVal(const Val &val);

    /// removes all the elements in the hash table
    /** The function does not resize the nodes vector (even if the size of this one
     * has been increased after the creation of the hash table) and it resets the
     * iterators on the hash table to end. The method runs in linear time w.r.t.
     * the number of iterators pointing to the hash table. */
    void clear();

    /// indicates whether the hash table is empty
    bool empty() const noexcept;

    /// transforms a hashtable of vals into a hashtable of mountains
    /** @param f a function that maps any Val element into a Mount
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     * @warning Although the resulting hashtable has the same number of elements as
     * the original hashtable, by default, the size of the former may not be equal
     * to that of the latter. Hence iterators on the original hashtable may not
     * parse it in the same order as iterators on the resulting hashtable. To
     * guarrantee that both hashtables have the same size (and thus have the
     * elements in the same order), set the @e size argument to the size of the
     * original hashtable. */
    template <typename Mount, typename OtherAlloc = typename Alloc::template rebind<
                                  std::pair<Key, Mount>>::other>
    HashTable<Key, Mount, OtherAlloc>
    map(Mount (*f)(Val), Size size = 0,
        bool resize_pol = HashTableConst::default_resize_policy,
        bool key_uniqueness_pol = HashTableConst::default_uniqueness_policy) const;

    /// transforms a hashtable of vals into a hashtable of mountains
    /** @param f a function that maps any Val element into a Mount
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     * @warning Although the resulting hashtable has the same number of elements as
     * the original hashtable, by default, the size of the former may not be equal
     * to that of the latter. Hence iterators on the original hashtable may not
     * parse it in the same order as iterators on the resulting hashtable. To
     * guarrantee that both hashtables have the same size (and thus have the
     * elements in the same order), set the @e size argument to the size of the
     * original hashtable. */
    template <typename Mount, typename OtherAlloc = typename Alloc::template rebind<
                                  std::pair<Key, Mount>>::other>
    HashTable<Key, Mount, OtherAlloc>
    map(Mount (*f)(Val &), Size size = 0,
        bool resize_pol = HashTableConst::default_resize_policy,
        bool key_uniqueness_pol = HashTableConst::default_uniqueness_policy) const;

    /// transforms a hashtable of vals into a hashtable of mountains
    /** @param f a function that maps any Val element into a Mount
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     * @warning Although the resulting hashtable has the same number of elements as
     * the original hashtable, by default, the size of the former may not be equal
     * to that of the latter. Hence iterators on the original hashtable may not
     * parse it in the same order as iterators on the resulting hashtable. To
     * guarrantee that both hashtables have the same size (and thus have the
     * elements in the same order), set the @e size argument to the size of the
     * original hashtable. */
    template <typename Mount, typename OtherAlloc = typename Alloc::template rebind<
                                  std::pair<Key, Mount>>::other>
    HashTable<Key, Mount, OtherAlloc>
    map(Mount (*f)(const Val &), Size size = 0,
        bool resize_pol = HashTableConst::default_resize_policy,
        bool key_uniqueness_pol = HashTableConst::default_uniqueness_policy) const;

    /// creates a hashtable of mounts with a given value from a hashtable of vals
    /** @param val the value taken by all the elements of the resulting hashtable
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     * @warning Although the resulting hashtable has the same number of elements as
     * the original hashtable, by default, the size of the former may not be equal
     * to that of the latter. Hence iterators on the original hashtable may not
     * parse it in the same order as iterators on the resulting hashtable. To
     * guarrantee that both hashtables have the same size (and thus have the
     * elements in the same order), set the @e size argument to the size of the
     * original hashtable. */
    template <typename Mount, typename OtherAlloc = typename Alloc::template rebind<
                                  std::pair<Key, Mount>>::other>
    HashTable<Key, Mount, OtherAlloc>
    map(const Mount &val, Size size = 0,
        bool resize_pol = HashTableConst::default_resize_policy,
        bool key_uniqueness_pol = HashTableConst::default_uniqueness_policy) const;

    /// @}

    private:
    // friends
    /// to optimize the access to data, iterators must be friends
    template <typename K, typename V, typename A> friend class HashTable;
    friend class HashTableIterator<Key, Val>;
    friend class HashTableConstIterator<Key, Val>;
    friend class HashTableIteratorSafe<Key, Val>;
    friend class HashTableConstIteratorSafe<Key, Val>;

    /// for friendly displaying the content of the hashtable
    friend std::ostream &operator<<<>(std::ostream &,
                                      const HashTable<Key, Val, Alloc> &);
    friend std::ostream &operator<<<>(std::ostream &,
                                      const HashTable<Key *, Val, Alloc> &);

    /// for bijections to quickly access data
    template <typename T1, typename T2, typename A> friend class Bijection;

    /** @brief the hash table is represented as a vector of chained lists.
     * '__nodes' is this very vector. */
    std::vector<HashTableList<Key, Val, Alloc>> __nodes;

    /// the number of nodes in vector '__nodes'
    Size __size;

    /// number of elements of type Val stored in the hash table
    Size __nb_elements{0};

    /// the function used to hash keys (may change when the table is resized)
    HashFunc<Key> __hash_func;

    /// is resizing performed automatically?
    bool __resize_policy{true};

    /// shall we check for key uniqueness in the table?
    bool __key_uniqueness_policy{true};

    /// the __index begin() should have
    /** Beware: the beginning of a HashTable is the end of its __nodes vector,
     * i.e., the Bucket at the highest index in __nodes. This enables
     * a slightly faster parsing than if it were the lowest index.
     * @warning std::numeric_limits<Size>::max() means that we do not know where
     * the beginning of the table really is (this can mean either that there is
     * not yet any element in the hash table or that an erase operation has been
     * performed and that we lost track of the element that should correspond to
     * the begin(). */
    mutable Size __begin_index{std::numeric_limits<Size>::max()};

    /// the list of safe iterators pointing to the hash table
    mutable std::vector<HashTableConstIteratorSafe<Key, Val> *> __safe_iterators;

    /// the allocator for the buckets
    /** @warning the allocator field should compulsorily be the last of
     * field of the class. As such, for K and V fixed, all hashTable<K,V,A>
     * are the same (up to the allocator) for all allocators A. This feature
     * proves useful to avoid passing the allocator as a template parameter
     * to iterators. */
    BucketAllocator __alloc;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /// erases a given bucket
    void __erase(HashTableBucket<Key, Val> *bucket, Size index);

    /** a function used to perform copies of HashTables. This code is
     * shared by the copy constructor and the copy operator. The function ensures
     * that when a memory allocation problem occurs:
     * - no memory leak occurs
     * - the hashtable returned is empty but in a coherent state
     * - an exception is thrown
     * The function assumes that both this and table have arrays '__nodes' of the
     * same size. */
    template <typename OtherAlloc>
    void __copy(const HashTable<Key, Val, OtherAlloc> &table);

    /// a function used by all default constructors (general and specialized)
    void __create(Size size);

    /// clear all the safe iterators
    void __clearIterators();

    /// adds a new element (actually a copy of this element) in the hash table
    /** If there already exists an element with the same key in the list and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return the bucket inserted in the hash table.
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set. */
    void __insert(Bucket *bucket);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
  };

  /* =========================================================================== */
  /* ===                   SAFE HASH TABLES CONST ITERATORS                  === */
  /* =========================================================================== */
  /** @class HashTableConstIteratorSafe
   * @brief Safe Const Iterators for hashtables
   * @ingroup basicstruct_group
   *
   * HashTableConstIteratorSafe provides a safe way to parse HashTables. They are
   * safe because they are kept informed by the hashtable they belong to of the
   * elements deleted by the user. Hence, even if the user removes an element
   * pointed to by a HashTableConstIteratorSafe, using the latter to access this
   * element will never crash the application. Instead it will properly throw a
   * UndefinedIteratorValue exception.
   *
   * Developers may consider using HashTable<x,y>::const_iterator_safe instead of
   * HashTableConstIteratorSafe<x,y>.
   * @par Usage example:
   * @code
   * // creation of a hash table with 10 elements
   * HashTable<int,string> table;
   * for (int i = 0; i< 10; ++i)
   *   table.insert (i,"xxx" + string (i,'x'));
   *
   * // parse the hash table
   * for (HashTable<int,string>::const_iterator_safe iter = table.cbeginSafe ();
   *        iter != table.cendSafe (); ++iter) {
   *   // display the values
   *   cerr << "at " << iter.key () << " value = " << iter.val () << endl;
   *   const HashTable<int,string>::value_type& elt = *iter;
   *   const std::pair<const int, string>& xelt = *iter;
   * }
   *
   * // check whether two iterators point toward the same element
   * HashTable<int,string>::const_iterator_safe iter1 = table1.cbeginSafe ();
   * HashTable<int,string>::const_iterator_safe iter2 = table1.cendSafe ();
   * if (iter1 != iter) cerr << "iter1 and iter2 point toward different elements";
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   */
  template <typename Key, typename Val> class HashTableConstIteratorSafe {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const Key, Val>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor: creates an iterator pointing to nothing
    HashTableConstIteratorSafe();

    /// constructor for an iterator pointing to the first element of a hashtable
    template <typename Alloc>
    HashTableConstIteratorSafe(const HashTable<Key, Val, Alloc> &tab);

    /// constructor for an iterator pointing to the nth element of a hashtable
    /** The method runs in time linear to ind_elt.
     * @param tab the hash table to which the so-called element belongs
     * @param ind_elt the position of the element in the hash table (0 means the
     * first element).
     * @throw UndefinedIteratorValue exception is thrown if the element cannot
     * be found */
    template <typename Alloc>
    HashTableConstIteratorSafe(const HashTable<Key, Val, Alloc> &tab, Size ind_elt);

    /// copy constructor
    HashTableConstIteratorSafe(const HashTableConstIteratorSafe<Key, Val> &from);

    /// copy constructor
    explicit HashTableConstIteratorSafe(
        const HashTableConstIterator<Key, Val> &from);

    /// move constructor
    HashTableConstIteratorSafe(HashTableConstIteratorSafe<Key, Val> &&from);

    /// destructor
    ~HashTableConstIteratorSafe() noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the key pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    const key_type &key() const;

    /// returns the mapped value pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    const mapped_type &val() const;

    /** @brief makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current hash table)
     *
     * It is mainly used by the hashtable when the latter is deleted while the
     * iterator is still alive. */
    void clear() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    HashTableConstIteratorSafe<Key, Val> &
    operator=(const HashTableConstIteratorSafe<Key, Val> &from);

    /// copy operator
    HashTableConstIteratorSafe<Key, Val> &
    operator=(const HashTableConstIterator<Key, Val> &from);

    /// move operator
    HashTableConstIteratorSafe<Key, Val> &
    operator=(HashTableConstIteratorSafe<Key, Val> &&from) noexcept;

    /// makes the iterator point to the next element in the hash table
    /** for (iter = cbeginSafe(); iter != cendSafe (); ++iter) loops are
     * guaranteed to parse the whole hash table as long as no element is added to
     * or deleted from the hash table while being in the loop. Deleting elements
     * during the loop is guaranteed to never produce a segmentation fault. */
    HashTableConstIteratorSafe<Key, Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the hashtable
    HashTableConstIteratorSafe<Key, Val> &operator+=(unsigned int) noexcept;

    /// returns a new iterator
    HashTableConstIteratorSafe<Key, Val> operator+(unsigned int) const;

    /// checks whether two iterators are pointing toward different elements
    bool operator!=(const HashTableConstIteratorSafe<Key, Val> &from) const noexcept;

    /// checks whether two iterators are pointing toward the same element
    bool operator==(const HashTableConstIteratorSafe<Key, Val> &from) const noexcept;

    /// returns the element pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    const value_type &operator*() const;

    /// @}

    protected:
    /** class HashTable must be a friend because it stores iterator end
     * and this can be properly initialized only when the hashtable has been
     * fully allocated. Thus, proper initialization can only take place within
     * the constructor's code of the hashtable. */
    template <typename K, typename V, typename A> friend class HashTable;

    /// the hash table the iterator is pointing to
    const HashTable<Key, Val> *__table{nullptr};

    /** @brief the index of the chained list pointed to by the iterator in the
     * array __nodes of the hash table */
    Size __index{0};

    /// the bucket in the chained list pointed to by the iterator
    HashTableBucket<Key, Val> *__bucket{nullptr};

    /** @brief the bucket we should start from when we decide to do a ++. Usually
     * it should be equal to nullptr. However, if the user has deleted the object
     * pointed to by bucket, this will point to another bucket. When it is equal to
     * nullptr, it means that the bucket reached after a ++ belongs to another
     * slot of the hash table's '__node' vector. */
    HashTableBucket<Key, Val> *__next_bucket{nullptr};

    /// returns the current iterator's bucket
    HashTableBucket<Key, Val> *__getBucket() const noexcept;

    /// returns the index in the hashtable's node vector pointed to by the iterator
    Size __getIndex() const noexcept;

    /// remove the iterator from its hashtable' safe iterators list
    void __removeFromSafeList() const noexcept;

    /// insert the iterator into the hashtable's list of safe iterators
    void __insertIntoSafeList() const;
  };

  /* =========================================================================== */
  /* ===                         HASH TABLES ITERATORS                       === */
  /* =========================================================================== */
  /** @class HashTableIteratorSafe
   * @brief Safe Iterators for hashtables
   * @ingroup basicstruct_group
   *
   * HashTableIteratorSafe provides a safe way to parse HashTables. They are safe
   * because they are kept informed by the hashtable they belong to of the
   * elements deleted by the user. Hence, even if the user removes an element
   * pointed to by a HashTableIteratorSafe, using the latter to access this element
   * will never crash the application. Instead it will properly throw an
   * UndefinedIteratorValue exception.
   *
   * Developers may consider using HashTable<x,y>::iterator_safe instead of
   * HashTableIteratorSafe<x,y>.
   * @par Usage example:
   * @code
   * // creation of a hash table with 10 elements
   * HashTable<int,string> table;
   * for (int i = 0; i< 10; ++i)
   *   table.insert (i,"xxx" + string (i,'x'));
   *
   * // parse the hash table
   * for (HashTable<int,string>::iterator_safe iter = table.beginSafe ();
   *        iter != table.endSafe (); ++iter) {
   *   // display the values
   *   cerr << "at " << iter.key() << " value = " << iter.val () << endl;
   *   HashTable<int,string>::value_type& elt = *iter;
   *   std::pair<const int, string>& xelt = *iter;
   * }
   *
   * // check whether two iterators point toward the same element
   * HashTable<int,string>::iterator_safe iter1 = table1.beginSafe ();
   * HashTable<int,string>::iterator_safe iter2 = table1.endSafe ();
   * if (iter1 != iter) cerr << "iter1 and iter2 point toward different elements";
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   */
  template <typename Key, typename Val>
  class HashTableIteratorSafe : public HashTableConstIteratorSafe<Key, Val> {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const Key, Val>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor: creates an iterator pointing to nothing
    HashTableIteratorSafe();

    /// constructor for an iterator pointing to the first element of a hashtable
    template <typename Alloc>
    HashTableIteratorSafe(const HashTable<Key, Val, Alloc> &tab);

    /// constructor for an iterator pointing to the nth element of a hashtable
    /** The method runs in time linear to ind_elt.
     * @param tab the hash table to which the so-called element belongs
     * @param ind_elt the position of the element in the hash table (0 means the
     * first element).
     * @throw UndefinedIteratorValue exception is thrown if the element cannot
     * be found */
    template <typename Alloc>
    HashTableIteratorSafe(const HashTable<Key, Val, Alloc> &tab, Size ind_elt);

    /// copy constructor
    HashTableIteratorSafe(const HashTableIteratorSafe<Key, Val> &from);

    /// copy constructor
    explicit HashTableIteratorSafe(const HashTableIterator<Key, Val> &from);

    /// move constructor
    HashTableIteratorSafe(HashTableIteratorSafe<Key, Val> &&from) noexcept;

    /// destructor
    ~HashTableIteratorSafe() noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    using HashTableConstIteratorSafe<Key, Val>::key;
    using HashTableConstIteratorSafe<Key, Val>::val;
    using HashTableConstIteratorSafe<Key, Val>::clear;

    /// returns the mapped value pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    mapped_type &val();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    HashTableIteratorSafe<Key, Val> &
    operator=(const HashTableIteratorSafe<Key, Val> &from);

    /// copy operator
    HashTableIteratorSafe<Key, Val> &
    operator=(const HashTableIterator<Key, Val> &from);

    /// move operator
    HashTableIteratorSafe<Key, Val> &
    operator=(HashTableIteratorSafe<Key, Val> &&from) noexcept;

    /// makes the iterator point to the next element in the hash table
    /** for (iter = beginSafe (); iter != endSafe (); ++iter) loops are guaranteed
     * to parse the whole hash table as long as no element is added to or deleted
     * from the hash table while being in the loop. Deleting elements during the
     * loop is guaranteed to never produce a segmentation fault. */
    HashTableIteratorSafe<Key, Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the hashtable
    HashTableIteratorSafe<Key, Val> &operator+=(unsigned int) noexcept;

    /// returns a new iterator
    HashTableIteratorSafe<Key, Val> operator+(unsigned int) const;

    /// checks whether two iterators are pointing toward different elements
    bool operator!=(const HashTableIteratorSafe<Key, Val> &from) const noexcept;

    /// checks whether two iterators are pointing toward the same element
    bool operator==(const HashTableIteratorSafe<Key, Val> &from) const noexcept;

    /// returns the value pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    value_type &operator*();

    /// returns the value pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    const value_type &operator*() const;

    /// @}
  };

  /* =========================================================================== */
  /* ===                  UNSAFE HASH TABLES CONST ITERATORS                 === */
  /* =========================================================================== */
  /** @class HashTableConstIterator
   * @brief Unsafe Const Iterators for hashtables
   * @ingroup basicstruct_group
   *
   * HashTableConstIterator provides a fast but unsafe way to parse HashTables.
   * They should @b only be used when parsing hashtables in which no element is
   * removed from the hashtable. Removing an element where the iterator points to
   * will mess the iterator as it will most certainly point to an unallocated
   * memory. So, this kind of iterator should only be used when parsing "(key)
   * constant" hash tables, e.g., when we wish to display the content of a hash
   * table or when we wish to update the mapped values of some elements of the
   * hash table without ever modifying their keys.
   *
   * Developers may consider using HashTable<x,y>::const_iterator instead of
   * HashTableConstIterator<x,y>.
   * @par Usage example:
   * @code
   * // creation of a hash table with 10 elements
   * HashTable<int,string> table;
   * for (int i = 0; i< 10; ++i)
   *   table.insert (i,"xxx" + string (i,'x'));
   *
   * // parse the hash table
   * for (HashTable<int,string>::const_iterator iter = table.cbegin ();
   *        iter != table.cend (); ++iter) {
   *   // display the values
   *   cerr << "at " << iter.key() << " value = " << iter.val () << endl;
   *   const HashTable<int,string>::value_type& elt = *iter;
   *   const std::pair<const int, string>& xelt = *iter;
   * }
   *
   * // check whether two iterators point toward the same element
   * HashTable<int,string>::const_iterator iter1 = table1.cbegin();
   * HashTable<int,string>::const_iterator iter2 = table1.cend();
   * if (iter1 != iter) cerr << "iter1 and iter2 point toward different elements";
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   */
  template <typename Key, typename Val> class HashTableConstIterator {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const Key, Val>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor: creates an iterator pointing to nothing
    HashTableConstIterator() noexcept;

    /// constructor for an iterator pointing to the first element of a hashtable
    template <typename Alloc>
    HashTableConstIterator(const HashTable<Key, Val, Alloc> &tab) noexcept;

    /// constructor for an iterator pointing to the nth element of a hashtable
    /** The method runs in time linear to ind_elt.
     * @param tab the hash table to which the so-called element belongs
     * @param ind_elt the position of the element in the hash table (0 means the
     * first element).
     * @throw UndefinedIteratorValue exception is thrown if the element cannot
     * be found */
    template <typename Alloc>
    HashTableConstIterator(const HashTable<Key, Val, Alloc> &tab, Size ind_elt);

    /// copy constructor
    HashTableConstIterator(const HashTableConstIterator<Key, Val> &from) noexcept;

    /// move constructor
    HashTableConstIterator(HashTableConstIterator<Key, Val> &&from) noexcept;

    /// destructor
    ~HashTableConstIterator() noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the key corresponding to the element pointed to by the iterator
    /** @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure, use
     * a safe iterator instead of an unsafe one. */
    const key_type &key() const;

    /// returns the mapped value pointed to by the iterator
    /** @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure, use
     * a safe iterator instead of an unsafe one. */
    const mapped_type &val() const;

    /** @brief makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current hash table) */
    void clear() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    HashTableConstIterator<Key, Val> &
    operator=(const HashTableConstIterator<Key, Val> &from) noexcept;

    /// move operator
    HashTableConstIterator<Key, Val> &
    operator=(HashTableConstIterator<Key, Val> &&from) noexcept;

    /// makes the iterator point to the next element in the hash table
    /** for (iter = cbegin (); iter != cend(); ++iter ) loops are guaranteed to
     * parse the whole hash table as long as no element is added to or deleted
     * from the hash table while being in the loop.
     * @warning performing a ++ on an iterator that points to an element that has
     * been deleted will most certainly result in a segfault. */
    HashTableConstIterator<Key, Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the hashtable
    HashTableConstIterator<Key, Val> &operator+=(unsigned int) noexcept;

    /// returns a new iterator
    HashTableConstIterator<Key, Val> operator+(unsigned int) const noexcept;

    /// checks whether two iterators are pointing toward different elements
    bool operator!=(const HashTableConstIterator<Key, Val> &from) const noexcept;

    /// checks whether two iterators are pointing toward the same element
    bool operator==(const HashTableConstIterator<Key, Val> &from) const noexcept;

    /// returns the value pointed to by the iterator
    /** @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure, use
     * a safe iterator instead of an unsafe one. */
    const value_type &operator*() const;

    /// @}

    protected:
    /** class HashTable must be a friend because it stores iterator end
     * and this one can be properly initialized only when the hashtable has been
     * fully allocated. Thus, proper initialization can only take place within
     * the constructor's code of the hashtable. */
    template <typename K, typename V, typename A> friend class HashTable;

    /// for the safe copy constructor and operator
    friend class HashTableConstIteratorSafe<Key, Val>;

    /// the hash table the iterator is pointing to
    const HashTable<Key, Val> *__table{nullptr};

    /** @brief the index of the chained list pointed by the iterator in the
     * array of nodes of the hash table */
    Size __index{0};

    /// the bucket in the chained list pointed to by the iterator
    typename HashTable<Key, Val>::Bucket *__bucket{nullptr};

    /// returns the current iterator's bucket
    typename HashTable<Key, Val>::Bucket *__getBucket() const noexcept;

    /// returns the index in the hashtable's node vector pointed to by the iterator
    Size __getIndex() const noexcept;
  };

  /* =========================================================================== */
  /* ===                      UNSAFE HASH TABLES ITERATORS                   === */
  /* =========================================================================== */
  /** @class HashTableIterator
   * @brief Unsafe Iterators for hashtables
   * @ingroup basicstruct_group
   *
   * HashTableIterator provides a fast but unsafe way to parse HashTables.
   * They should @b only be used when parsing hashtables in which no element is
   * removed from the hashtable. Removing an element where the iterator points to
   * will mess the iterator as it will most certainly point to an unallocated
   * memory. So, this kind of iterator should only be used when parsing "(key)
   * constant" hash tables, e.g., when we wish to display the content of a hash
   * table or when we wish to update the mapped values of some elements of the
   * hash table without ever modifying their keys.
   *
   * Developers may consider using HashTable<x,y>::iterator instead of
   * HashTableIterator<x,y>.
   * @par Usage example:
   * @code
   * // creation of a hash table with 10 elements
   * HashTable<int,string> table;
   * for (int i = 0; i< 10; ++i)
   *   table.insert (i,"xxx" + string (i,'x'));
   *
   * // parse the hash table
   * for (HashTable<int,string>::iterator iter = table.begin ();
   *        iter != table.end (); ++iter) {
   *   // display the values
   *   cerr << "at " << iter.key() << " value = " << iter.val () << endl;
   *   HashTable<int,string>::value_type& elt = *iter;
   *   std::pair<const int, string>& xelt = *iter;
   * }
   *
   * // check whether two iterators point toward the same element
   * HashTable<int,string>::iterator iter1 = table1.begin();
   * HashTable<int,string>::iterator iter2 = table1.end();
   * if (iter1 != iter) cerr << "iter1 and iter2 point toward different elements";
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   */
  template <typename Key, typename Val>
  class HashTableIterator : public HashTableConstIterator<Key, Val> {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const Key, Val>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor: creates an iterator pointing to nothing
    HashTableIterator() noexcept;

    /// constructor for an iterator pointing to the first element of a hashtable
    template <typename Alloc>
    HashTableIterator(const HashTable<Key, Val, Alloc> &tab) noexcept;

    /// constructor for an iterator pointing to the nth element of a hashtable
    /** The method runs in time linear to ind_elt.
     * @param tab the hash table to which the so-called element belongs
     * @param ind_elt the position of the element in the hash table (0 means the
     * first element).
     * @throw UndefinedIteratorValue exception is thrown if the element cannot
     * be found */
    template <typename Alloc>
    HashTableIterator(const HashTable<Key, Val, Alloc> &tab, Size ind_elt);

    /// copy constructor
    HashTableIterator(const HashTableIterator<Key, Val> &from) noexcept;

    /// move constructor
    HashTableIterator(HashTableIterator<Key, Val> &&from) noexcept;

    /// destructor
    ~HashTableIterator() noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    using HashTableConstIterator<Key, Val>::key;
    using HashTableConstIterator<Key, Val>::val;
    using HashTableConstIterator<Key, Val>::clear;

    /// returns the mapped value pointed to by the iterator
    /** @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure, use
     * a safe iterator instead of an unsafe one. */
    mapped_type &val();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    HashTableIterator<Key, Val> &
    operator=(const HashTableIterator<Key, Val> &from) noexcept;

    /// move operator
    HashTableIterator<Key, Val> &
    operator=(HashTableIterator<Key, Val> &&from) noexcept;

    /// makes the iterator point to the next element in the hash table
    /** for (iter = begin(); iter != end(); ++iter) loops are guaranteed to parse
     * the whole hash table as long as no element is added to or deleted from the
     * hash table while being in the loop.
     * @warning performing a ++ on an iterator that points to an element that has
     * been deleted will most certainly result in a segfault. */
    HashTableIterator<Key, Val> &operator++() noexcept;

    /// makes the iterator point to i elements further in the hashtable
    HashTableIterator<Key, Val> &operator+=(unsigned int) noexcept;

    /// returns a new iterator
    HashTableIterator<Key, Val> operator+(unsigned int) const noexcept;

    /// checks whether two iterators are pointing toward different elements
    bool operator!=(const HashTableIterator<Key, Val> &from) const noexcept;

    /// checks whether two iterators are pointing toward the same element
    bool operator==(const HashTableIterator<Key, Val> &from) const noexcept;

    /// returns the value pointed to by the iterator
    /** @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure, use
     * a safe iterator instead of an unsafe one. */
    value_type &operator*();

    /// returns the value pointed to by the iterator
    /** @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure, use
     * a safe iterator instead of an unsafe one. */
    const value_type &operator*() const;

    /// @}
  };

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/core/hashTable.tcc>

#endif // GUM_HASHTABLE_H
