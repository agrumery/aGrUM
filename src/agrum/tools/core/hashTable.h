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
 * @brief Class hash tables iterators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_HASHTABLE_H
#define GUM_HASHTABLE_H

#include <limits>

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/hashFunc.h>
#include <agrum/tools/core/staticInitializer.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // the templates used by this file
  template < typename Key, typename Val >
  class HashTable;
  template < typename Key, typename Val >
  class HashTableList;
  template < typename Key, typename Val >
  class HashTableIterator;
  template < typename Key, typename Val >
  class HashTableConstIterator;
  template < typename Key, typename Val >
  class HashTableIteratorSafe;
  template < typename Key, typename Val >
  class HashTableConstIteratorSafe;
  template < typename T1, typename T2 >
  class Bijection;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /**
   * @class HashTableConst
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief Parameters specifying the default behavior of the hashtables.
   * @ingroup hashtable_group
   */
  struct HashTableConst {
    /**
     * The default number of slots in hashtables. By default, hashtables can
     * store up to thrice the number of slots elements before their size is
     * increased To each slot corresponds a chained list of elements with the
     * same hashed key.
     */
    static constexpr Size default_size{Size(4)};

    /**
     * The average number of elements admissible by slots. Once this number is
     * reached, the size of the hashtable is automatically doubled if we are in
     * automatic resize mode.
     */
    static constexpr Size default_mean_val_by_slot{Size(3)};

    /**
     * A Boolean indicating whether inserting too many values into the
     * hashtable makes it resize itself automatically. true = automatic, false
     * = manual.
     */
    static constexpr bool default_resize_policy{true};

    /**
     * A Boolean indicating the default behavior when trying to insert more
     * than once elements with identical keys. true = do not insert the
     * elements but the first one and throw an exception after the first
     * insertion; false = insert the elements without complaining.
     */
    static constexpr bool default_uniqueness_policy{true};
  };

  // Doxygen raises warning with the following comment bloc
  // @brief Prints the content of a gum::HashTableList in the stream.
  // @ingroup hashtable_group
  // @param s The s used to print the gum::HashTableList.
  // @param list The gum::HashTableList to print.
  // @return Returns the std::ostream s.
  // @tparam Key The type of keys in the gum::HashTableList.
  // @tparam Val The type of values in the gum::HashTableList.

  /**
   * @brief Prints the content of a gum::HashTableList in the stream.
   * @ingroup hashtable_group
   */
  template < typename Key, typename Val >
  std::ostream& operator<<(std::ostream& s, const HashTableList< Key, Val >& list);

  // Doxygen raises warning with the following comment bloc
  // @brief Prints the content of a gum::HashTableList with pointers key in the
  // stream.
  // @ingroup hashtable_group
  // @param s The s used to print the gum::HashTableList.
  // @param list The gum::HashTableList to print.
  // @return Returns the std::ostream s.
  // @tparam Key The type of keys in the gum::HashTableList.
  // @tparam Val The type of values in the gum::HashTableList.
  //

  /**
   * @brief Prints the content of a gum::HashTableList with pointers key in
   * the stream.
   * @ingroup hashtable_group
   */
  template < typename Key, typename Val >
  std::ostream& operator<<(std::ostream& s, const HashTableList< Key*, Val >& list);

  // Doxygen raises warning with the following comment bloc
  // @brief Prints the content of a gum::HashTable in the stream.
  // @ingroup hashtable_group
  // @param s The stream used to print the gum::HashTable.
  // @param table The gum::HashTable to print.
  // @return Returns the std::ostream s.
  // @tparam Key The type of keys in the gum::HashTable.
  // @tparam Val The type of values in the gum::HashTable.

  /**
   * @brief Prints the content of a gum::HashTable in the stream.
   * @ingroup hashtable_group
   */
  template < typename Key, typename Val >
  std::ostream& operator<<(std::ostream& s, const HashTable< Key, Val >& table);

  // Doxygen raises warning with the following comment bloc
  // @brief Prints the content of a gum::HashTable with pointers key in the
  // stream.
  // @ingroup hashtable_group
  // @param s The stream used to print the gum::HashTable.
  // @param table The gum::HashTable to print.
  // @return Returns the std::ostream s.
  // @tparam Key The type of keys in the gum::HashTable.
  // @tparam Val The type of values in the gum::HashTable.

  /**
   * @brief Prints the content of a gum::HashTable with pointers key in the
   * stream.
   * @ingroup hashtable_group
   */
  template < typename Key, typename Val >
  std::ostream& operator<<(std::ostream& s, const HashTable< Key*, Val >& table);

  // ===========================================================================
  // ===          LISTS SPECIFIC FOR SAVING ELEMENTS IN HASHTABLES           ===
  // ===========================================================================

  /**
   * @class HashTableBucket
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief A recipient for a pair of key value in a gum::HashTableList.
   * @ingroup hashtable_group
   *
   * In aGrUM, hashtables are vectors of chained lists. Each list corresponds
   * to the pairs (key,val) the keys of which have the same hashed value. Each
   * box of the list is called a bucket. Lists are doubly linked so as to
   * enable efficient begin/end iterators and efficient insert/erase
   * operations.
   *
   * @tparam Key The type for keys in a gum::HashTable.
   * @tparam Val The type for values in a gum::HashTable.
   */
  template < typename Key, typename Val >
  struct HashTableBucket {
    /// The pair stored in this bucket.
    std::pair< const Key, Val > pair;

    /// A pointer toward the previous bucket in the gum::HashTableList.
    HashTableBucket< Key, Val >* prev{nullptr};

    /// A pointer toward the next bucket in the gum::HashTableList.
    HashTableBucket< Key, Val >* next{nullptr};

    /**
     * @brief A dummy type for the emplace constructor.
     * This type is used to prevent the Bucket emplace (int,...) to compile.
     */
    enum class Emplace {
      EMPLACE
    };

    /**
     * Class constructor.
     */
    HashTableBucket() {}

    /**
     * Copy constructor.
     * @param from The gum::HashTableBucket to copy.
     */
    HashTableBucket(const HashTableBucket< Key, Val >& from) : pair{from.pair} {}

    /**
     * Constructor.
     * @param k The key part of the pair.
     * @param v The value part of the pair.
     */
    HashTableBucket(const Key& k, const Val& v) : pair{k, v} {}

    /**
     * Constructor.
     * @param k The key part of the pair.
     * @param v The value part of the pair.
     */
    HashTableBucket(Key&& k, Val&& v) : pair{std::move(k), std::move(v)} {}

    /**
     * Constructor.
     * @param p The pair to store.
     */
    HashTableBucket(const std::pair< const Key, Val >& p) : pair(p) {}

    /**
     * Constructor.
     * @param p The pair to store.
     */
    HashTableBucket(std::pair< const Key, Val >&& p) : pair(std::move(p)) {}

    /**
     * The emplace constructor.
     * @param e The emplace.
     * @param args A construction list.
     * @tparam args The types in the construction list.
     */
    template < typename... Args >
    HashTableBucket(Emplace e, Args&&... args) :
        // emplace (universal) constructor
        pair(std::forward< Args >(args)...) {}

    /**
     * Class destructor.
     */
    ~HashTableBucket() {}

    /**
     * @brief Returns the pair stored in this bucket.
     * @return Returns the pair stored in this bucket.
     */
    std::pair< const Key, Val >& elt() { return pair; }

    /**
     * @brief Returns the key part of the pair.
     * @return Returns the key part of the pair.
     */
    Key& key() { return const_cast< Key& >(pair.first); }

    /**
     * @brief Returns the value part of the pair.
     * @return Returns value key part of the pair.
     */
    Val& val() { return pair.second; }
  };

  // ===========================================================================
  // ===       DOUBLY CHAINED LISTS FOR STORING ELEMENTS IN HASH TABLES      ===
  // ===========================================================================

  /**
   * @class HashTableList
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief A chained list used by gum::HashTable.
   * @ingroup hashtable_group
   *
   * @tparam Key The type for keys in a gum::HashTable.
   * @tparam Val The type for values in a gum::HashTable.
   */
  template < typename Key, typename Val >
  class HashTableList {
    public:
    /// types for STL compliance
    /// @{
    using key_type        = Key;
    using mapped_type     = Val;
    using value_type      = std::pair< const Key, Val >;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using size_type       = Size;
    using Bucket          = HashTableBucket< Key, Val >;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor that creates an empty list.
     *
     * This is what is used basically by gum::HashTable.
     */
    HashTableList() noexcept;

    /**
     * @brief Copy constructor.
     *
     * The new list and that which is copied do not share elements: the new
     * list contains new instances of the keys and values stored in the copied
     * list. Of course, if these values are pointers, the new values point
     * toward the same elements.
     *
     * @param from The gum::HashTableList to copy.
     */
    HashTableList(const HashTableList< Key, Val >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::HashTableList to move.
     */
    HashTableList(HashTableList< Key, Val >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~HashTableList();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Assignment operator.
     *
     * The new list and that which is copied do not share elements: the new
     * list contains new instances of the keys and values stored in the copied
     * list. Of course, if these values are pointers, the new values point
     * toward the same elements.
     *
     * If some allocation problem occurs or if copying the Val elements cannot
     * be performed properly, exceptions may be raised. In this case, the
     * function guarantees that no memory leak occurs and that the list is kept
     * in a coherent state (that of an empty list).
     *
     * @param from The gum::HashTableList to copy.
     * @return Returns this gum::HashTableList.
     */
    HashTableList< Key, Val >& operator=(const HashTableList< Key, Val >& from);

    /**
     * @brief Move operator.
     * @param from The gum::HashTableList to copy.
     * @return Returns this gum::HashTableList.
     */
    HashTableList< Key, Val >& operator=(HashTableList< Key, Val >&& from) noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Function at returns the ith element in the current chained list.
     *
     * The first element has index 0.
     * @param i The index to look up.
     * @return Returns the value at index i.
     * @throw NotFound Raised if the list has fewer than i elements.
     */
    value_type& at(Size i);

    /**
     * @brief Function at returns the ith element in the current chained list.
     *
     * The first element has index 0.
     * @param i The index to look up.
     * @return Returns the value at index i.
     * @throw NotFound Raised if the list has fewer than i elements.
     */
    const value_type& at(Size i) const;

    /**
     * @brief Returns the value corresponding to a given key.
     * @param key The key for which a value is returned.
     * @return Returns the value corresponding to a given key.
     * @throw NotFound is raised if the element cannot be found
     */
    mapped_type& operator[](const key_type& key);

    /**
     * @brief Returns the value corresponding to a given key.
     * @param key The key for which a value is returned.
     * @return Returns the value corresponding to a given key.
     * @throw NotFound is raised if the element cannot be found
     */
    const mapped_type& operator[](const key_type& key) const;

    /**
     * @brief Returns true if a value with the given key exists.
     *
     * Checks whether there exists an element with a given key in the list.
     * @param key The key to test for existence.
     * @return Returns true if a value with the given key exists.
     */
    bool exists(const key_type& key) const;

    /**
     * @brief Inserts a new element in the chained list.
     *
     * The element is inserted at the beginning of the list.
     * @param new_elt The element to add in the gum::HashTableList.
     */
    void insert(Bucket* new_elt) noexcept;

    /**
     * @brief Removes an element from this chained list.
     * @param ptr The element to remove.
     */
    void erase(Bucket* ptr);

    /**
     * @brief Removes all the elements of this chained list.
     */
    void clear();

    /**
     * @brief Returns true if this chained list is empty.
     * @return Returns true if this chained list is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief A method to get the bucket corresponding to a given key.
     *
     * This enables efficient removals of buckets.
     * @param key The key of the bucket to return.
     * @return Returns the buckket matching key.
     */
    Bucket* bucket(const Key& key) const;

    /// @}

    private:
    /// Friend for faster access.
    /// @{
    friend class HashTable< Key, Val >;
    friend class HashTableIterator< Key, Val >;
    friend class HashTableConstIterator< Key, Val >;
    friend class HashTableIteratorSafe< Key, Val >;
    friend class HashTableConstIteratorSafe< Key, Val >;
    friend std::ostream& operator<< <>(std::ostream&, const HashTableList< Key, Val >&);
    friend std::ostream& operator<< <>(std::ostream&, const HashTableList< Key*, Val >&);
    friend std::ostream& operator<< <>(std::ostream&, const HashTable< Key, Val >&);
    friend std::ostream& operator<< <>(std::ostream&, const HashTable< Key*, Val >&);
    /// @}

    /// A pointer on the first element of the chained list.
    HashTableBucket< Key, Val >* _deb_list_{nullptr};

    /// A pointer on the last element of the chained list.
    HashTableBucket< Key, Val >* _end_list_{nullptr};

    /// The number of elements in the chained list.
    Size _nb_elements_{Size(0)};

    /**
     * @brief A function used to perform copies of HashTableLists.
     *
     * This code is shared by the copy constructor and the copy operator. If it
     * cannot perform the necessary allocations, no memory leak occurs and the
     * list is set to the empty list.
     *
     * @param from The gum::HashTableList to copy.
     */
    void _copy_(const HashTableList< Key, Val >& from);
  };

  // ===========================================================================
  // ===                          GENERIC HASH TABLES                        ===
  // ===========================================================================
  /**
   * @class HashTable
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief The class for generic Hash Tables.
   * @ingroup basicstruct_group
   * @ingroup hashtable_group
   *
   * In aGrUM, a hashtable is a vector of chained lists (collision problems are
   * fixed by chaining). Each slot of the vector contains a list of elements
   * sharing the same hashed value. To be computationally efficient, the hash
   * table should not contain too many elements as compared to its number of
   * slots.  Therefore, it is sometimes useful to resize the chained lists
   * vector.  aGrUM's hash tables are designed to automatically double their
   * size when there is in average more than 3 elements per slot. However, when
   * memory consumption is a concern, this feature can be turned off either by
   * passing false as an optional resize_pol argument to the constructor of the
   * hash table or by using method setResizePolicy when the instance of the
   * class has already been constructed.  Similarly, the default number of
   * slots of the hash table may be parameterized as an optional argument of
   * the constructor (size_param). Beware: when inserting elements of a given
   * class into a hash table, unless the element is an r-value, only a copy of
   * this element is stored into the table (this is compulsory if the hashtable
   * is to be generic and can be used to store both complex classes and
   * built-in types like integers). HashTable have different kinds of
   * iterators: HashTableIteratorSafe and HashTableConstIteratorSafe (a.k.a.
   * HashTable<>::iterator_safe and HashTable<>::const_iterator_safe) allow
   * safe parsing of the hash tables. By safe, we mean that whenever the
   * element pointed to by such an iterator is removed from the hashtable,
   * accessing it through the iterator (*iter) does not result in a
   * segmentation fault but rather in an exception being thrown. This safety is
   * ensured at a very low cost (actually, our experiments show that our
   * HashTables and HashTable's safe iterators significantly outperform the
   * standard library unordered_maps). Of course, if there is no possibility
   * for an iterator to point to a deleted element, the user can use "unsafe"
   * iterators HashTableIterator and HashTableConstIterator (a.k.a.
   * HashTable<>::iterator and HashTable<>::const_iterator). These iterators
   * are slightly faster than their safe counterparts. However, as in the
   * standard library, accessing through them a deleted element usually results
   * in a mess (most probably a segfault).
   *
   * @warning HashTables @b guarantee that any element stored within them will
   * have the @b same @b location in memory until it is removed from the
   * hashtable (and this holds whatever operation is performed on the hashtable
   * like new insertions, deletions, resizing, etc.).
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
   * HashTable<int,bool> table { std::make_pair(3,true), std::make_pair(2,false)
   * };
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
   * // check whether hashtables contain the same elements
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
   *
   * @tparam Key The type for keys in a gum::HashTable.
   * @tparam Val The type for values in a gum::HashTable.
   */
  template < typename Key, typename Val >
  class HashTable {
    public:
    /// Types for STL compliance.
    /// @{
    using key_type            = Key;
    using mapped_type         = Val;
    using value_type          = std::pair< const Key, Val >;
    using reference           = value_type&;
    using const_reference     = const value_type&;
    using pointer             = value_type*;
    using const_pointer       = const value_type*;
    using size_type           = Size;
    using difference_type     = std::ptrdiff_t;
    using iterator            = HashTableIterator< Key, Val >;
    using const_iterator      = HashTableConstIterator< Key, Val >;
    using iterator_safe       = HashTableIteratorSafe< Key, Val >;
    using const_iterator_safe = HashTableConstIteratorSafe< Key, Val >;
    /// @}

    /// The buckets where data are stored.
    using Bucket = HashTableBucket< Key, Val >;

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * The true capacity (vector's size) of the hashtable will be the lowest
     * number greater than or equal to size_param that is also a power of 2.
     * The second optional argument is the resizing policy. By default, each
     * time there is an average of 3 elements by node, the size of the
     * hashtable is automatically multiplied by 2. But the user may pass false
     * as argument to resize_pol to disable this feature.
     *
     * @param size_param The initial size of the gum::HashTable.
     * @param resize_pol The policy for resizing the hashtable when new
     * elements are added (possible values: true = automatic resize and false =
     * manual resize).
     * @param key_uniqueness_pol Uniqueness policy : should we prevent inserting
     * the same key more than once in the table?
     */
    explicit HashTable(Size size_param         = HashTableConst::default_size,
                       bool resize_pol         = HashTableConst::default_resize_policy,
                       bool key_uniqueness_pol = HashTableConst::default_uniqueness_policy);

    /**
     * @brief Initializer list constructor.
     * @param list The initialized list.
     */
    explicit HashTable(std::initializer_list< std::pair< Key, Val > > list);

    /**
     * @brief Copy constructor.
     *
     * This creates a new hashtable the content of which is similar to that of
     * the table passed in argument. Beware: similar does not mean that both
     * tables share the same objects, but rather that the objects stored in the
     * newly created table are copies of those of the table passed in argument.
     * In particular, the new hash table inherits the parameters (resize
     * policy, uniqueness policy) of table 'from'.
     *
     * @param from The gum::HashTable to copy.
     */
    HashTable(const HashTable< Key, Val >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::HashTable to move.
     */
    HashTable(HashTable< Key, Val >&& from);

    /**
     * @brief Class destructor.
     */
    ~HashTable();

    /// @}
    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /**
     * @brief Returns the unsafe iterator pointing to the end of the hashtable.
     *
     * Unsafe iterators are slightly faster than safe iterators. However, BE
     * CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower).
     *
     * @return Returns the unsafe iterator pointing to the end of the hashtable.
     */
    const iterator& end() noexcept;

    /**
     * @brief Returns the unsafe const_iterator pointing to the end of the
     * hashtable.
     *
     * Unsafe iterators are slightly faster than safe iterators. However, BE
     * CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower).
     *
     * @return Returns the unsafe const_iterator pointing to the end of the
     * hashtable.
     */
    const const_iterator& end() const noexcept;

    /**
     * @brief Returns the unsafe const_iterator pointing to the end of the
     * hashtable.
     *
     * Unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower).
     *
     * @return Returns the unsafe const_iterator pointing to the end of the
     * hashtable.
     */
    const const_iterator& cend() const noexcept;

    /**
     * @brief Returns an unsafe iterator pointing to the beginning of the
     * hashtable.
     *
     * Unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower).
     *
     * @return Returns an unsafe iterator pointing to the beginning of the
     * hashtable.
     */
    iterator begin();

    /**
     * @brief Returns an unsafe const_iterator pointing to the beginning of the
     * hashtable.
     *
     * Unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower).
     *
     * @return Returns an unsafe const_iterator pointing to the beginning of
     * the hashtable.
     */
    const_iterator begin() const;

    /**
     * @brief Returns an unsafe const_iterator pointing to the beginning of the
     * hashtable.
     *
     * Unsafe iterators are slightly faster than safe iterators. However,
     * BE CAREFUL when using them: they should ONLY be used when you have the
     * guarantee that they will never point to a deleted element. If unsure,
     * prefer using the safe iterators (those are only slightly slower).
     *
     * @return Returns an unsafe const_iterator pointing to the beginning of the
     * hashtable.
     */
    const_iterator cbegin() const;

    /**
     * @brief Returns the safe iterator pointing to the end of the hashtable.
     *
     * Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element.
     *
     * @return Returns the safe iterator pointing to the end of the hashtable.
     */
    const iterator_safe& endSafe() noexcept;

    /**
     * @brief Returns the safe const_iterator pointing to the end of the
     * hashtable.
     *
     * Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element.
     *
     * @return Returns the safe const_iterator pointing to the end of the
     * hashtable.
     */
    const const_iterator_safe& endSafe() const noexcept;

    /**
     * @brief Returns the safe const_iterator pointing to the end of the
     * hashtable.
     *
     * Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element.
     *
     * @return Returns the safe const_iterator pointing to the end of the
     * hashtable.
     */
    const const_iterator_safe& cendSafe() const noexcept;

    /**
     * @brief Returns the safe iterator pointing to the beginning of the
     * hashtable.
     *
     * Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element.
     *
     * @return Returns the safe iterator pointing to the beginning of the
     * hashtable.
     */
    iterator_safe beginSafe();

    /**
     * @brief Returns the safe const_iterator pointing to the beginning of the
     * hashtable.
     *
     * Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element.
     *
     * @return Returns the safe const_iterator pointing to the beginning of the
     * hashtable.
     */
    const_iterator_safe beginSafe() const;

    /**
     * @brief Returns the safe const_iterator pointing to the beginning of the
     * hashtable.
     *
     * Safe iterators are slightly slower than unsafe ones but they guarantee
     * that you will never get a segfault if they try to access to a deleted
     * element or if they try a ++ operation from a deleted element.
     *
     * @return Returns the safe const_iterator pointing to the beginning of the
     * hashtable.
     */
    const_iterator_safe cbeginSafe() const;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * The copy operators ensures that whenever a memory allocation problem
     * occurs, no memory leak occurs as well and it also guarantees that in
     * this case the hashtable returned is in a coherent state (it is an empty
     * hashtable). Note that the copy not only involves copying pairs
     * (key,value) but also the copy of the resize and key uniqueness policies.
     *
     * @param from The gum::HashTable to copy.
     * @return Returns this gum::HashTable.
     */
    HashTable< Key, Val >& operator=(const HashTable< Key, Val >& from);

    /**
     * Move operator.
     *
     * @param from The gum::HashTable to move.
     * @return Returns this gum::HashTable.
     */
    HashTable< Key, Val >& operator=(HashTable< Key, Val >&& from);

    /**
     * @brief Returns a reference on the value the key of which is passed in
     * argument.
     *
     * In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     *
     * @param key The key of the value to return.
     * @return Returns the value matching the given key.
     * @throws NotFound exception is thrown if the element cannot be found.
     */
    Val& operator[](const Key& key);

    /// returns a reference on the value the key of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    const Val& operator[](const Key& key) const;

    /**
     * @brief Checks whether two hashtables contain the same elements.
     *
     * Two hashtables are considered equal if they contain the identical pairs
     * (key,val). Two pairs are identical if their keys have the same hashed
     * value, these two keys are equal in the sense of ==, and their val's are
     * also equal in the sense of ==.
     *
     * @param from The gum::HashTable to test for equality.
     * @return True if this and from are equal.
     */
    bool operator==(const HashTable< Key, Val >& from) const;

    ///
    /**
     * @brief Checks whether two hashtables contain different sets of elements.
     *
     * Two hashtables are considered different if they contain different pairs
     * (key,val). Two pairs are different if their keys have different hashed
     * values, or if they are different in the sense of !=, or if their val's
     * are different in the sense of !=.
     *
     * @param from The gum::HashTable to test for inequality.
     * @return True if this and from are not equal.
     */
    bool operator!=(const HashTable< Key, Val >& from) const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of slots in the 'nodes' vector of the
     * hashtable.
     *
     * The method runs in constant time.
     *
     * @return Returns the number of slots in the 'nodes' vector of the
     * hashtable.
     */
    Size capacity() const noexcept;

    ///
    /**
     * @brief Changes the number of slots in the 'nodes' vector of the hash
     * table.
     *
     * Usually, method resize enables the user to resize manually the
     * hashtable.  When in automatic resize mode, the function will actually
     * resize the table only if resizing policy is compatible with the new
     * size, i.e., the new size is not so small that there would be too many
     * elements per slot in the table (this would lead to a significant loss in
     * performance). However, the resizing policy may be changed by using
     * method setResizePolicy.  The method runs in linear time in the size of
     * the hashtable.  Upon memory allocation problem, the fuction guarantees
     * that no data is lost and that the hash table and its iterators are in a
     * coherent state.  In such a case, a bad_alloc exception is thrown.
     *
     * @param new_size The new number of slots in the gum::HashTable.
     */
    void resize(Size new_size);

    /**
     * @brief Enables the user to change dynamically the resizing policy.
     *
     * In most cases, this should be useless. However, when available memory
     * becomes rare, avoiding automatic resizing may speed-up new insertions in
     * the table.
     *
     * @warning This function never resizes the hashtable by itself: even if
     * you set the new policy to be an automatic resizing and the number of
     * elements in the table is sufficiently high that we should resize the
     * table, function setResizePolicy won't perform this resizing. The
     * resizing will happen only if you insert a new element or if use method
     * resize.
     *
     * @param new_policy The new resizing policy, true implies automatic
     * resizing.
     */
    void setResizePolicy(const bool new_policy) noexcept;

    /**
     * @brief Returns the current resizing policy.
     * @return Returns the current resizing policy.
     */
    bool resizePolicy() const noexcept;

    /**
     * @brief Enables the user to change dynamically the policy for checking
     * whether there can exist several elements in the table with identical
     * keys.
     *
     * By default, we should always check that there does not exist duplicate
     * keys.  However, this test slows the insertion of elements in the table.
     * So, when we know for sure that no duplicate key will be entered into the
     * table, we may avoid uniqueness checks.
     *
     * @warning When setting the key policy to "uniqueness", the function does
     * not check whether there are already different elements with identical
     * keys in the table. It thus only ensures that elements inserted from now
     * on will have unique keys.
     */
    void setKeyUniquenessPolicy(const bool new_policy) noexcept;

    /**
     * @brief Returns the current checking policy.
     * @return Returns the current checking policy.
     */
    bool keyUniquenessPolicy() const noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of elements stored into the hashtable.
     *
     * The method runs in constant time.
     *
     * @return Returns the number of elements stored into the hashtable.
     */
    Size size() const noexcept;

    /**
     * @brief Checks whether there exists an element with a given key in the
     * hashtable.
     *
     * The method runs in average in constant time if the resizing policy
     * is set.
     *
     * @param key The key to test for existence.
     * @return True if key is in this gum::HashTable.
     */
    bool exists(const Key& key) const;

    /**
     * @brief Adds a new element (actually a copy of this element) into the
     * hash table.
     *
     * If there already exists an element with the same key in the table and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness
     * policy is not set, the method runs in the worst case in constant time,
     * else if the automatic resizing policy is set, it runs in constant time in
     * average linear in the number of elements by slot.  @return As only a copy
     * of val is inserted into the hashtable, the method returns a reference on
     * a copy of the pair (key,val).  @throw DuplicateElement is thrown when
     * attempting to insert a pair (key,val) in a hash table containing already
     * a pair with the same key and when the hash table's uniqueness policy is
     * set.
     *
     * @param key The key to add.
     * @param val The value to add.
     * @return The value added by copy to this gum::HashTable.
     */
    value_type& insert(const Key& key, const Val& val);

    /**
     * @brief Moves a new element in the hash table.
     *
     * If there already exists an element with the same key in the table and
     * the uniqueness policy prevents multiple identical keys to belong to the
     * same hashtable, an exception DuplicateElement is thrown. If the
     * uniqueness policy is not set, the method runs in the worst case in
     * constant time, else if the automatic resizing policy is set, it runs in
     * constant time in average linear in the number of elements by slot.
     * @return a reference to the pair (key,val) in the hashtable.  @throw
     * DuplicateElement is thrown when attempting to insert a pair (key,val) in
     * a hash table containing already a pair with the same key and when the
     * hash table's uniqueness policy is set.
     *
     * @param key The key to move.
     * @param val The value to move.
     * @return The value moved to this gum::HashTable.
     */
    value_type& insert(Key&& key, Val&& val);

    /**
     * @brief Adds a new element (actually a copy of this element) into the
     * hash table.
     *
     * If there already exists an element with the same key in the table and
     * the uniqueness policy prevents multiple identical keys to belong to the
     * same hashtable, an exception DuplicateElement is thrown. If the
     * uniqueness policy is not set, the method runs in the worst case in
     * constant time, else if the automatic resizing policy is set, it runs in
     * constant time in average linear in the number of elements by slot.
     * @return As only a copy of val is inserted into the hashtable, the method
     * returns a reference on a copy of the pair (key,val).  @throw
     * DuplicateElement is thrown when attempting to insert a pair (key,val) in
     * a hash table containing already a pair with the same key and when the
     * hash table's uniqueness policy is set.
     *
     * @param elt The pair of key value to add.
     * @return The value added by copy to this gum::HashTable.
     */
    value_type& insert(const std::pair< Key, Val >& elt);

    /**
     * @brief Moves a new element in the hash table.
     *
     * If there already exists an element with the same key in the table and
     * the uniqueness policy prevents multiple identical keys to belong to the
     * same hashtable, an exception DuplicateElement is thrown. If the
     * uniqueness policy is not set, the method runs in the worst case in
     * constant time, else if the automatic resizing policy is set, it runs in
     * constant time in average linear in the number of elements by slot.
     * @return a reference to the pair (key,val) in the hashtable.  @throw
     * DuplicateElement is thrown when attempting to insert a pair (key,val) in
     * a hash table containing already a pair with the same key and when the
     * hash table's uniqueness policy is set.
     *
     * @param elt The pair of key value to move in this gum::HashTable.
     * @return The value moved to this gum::HashTable.
     */
    value_type& insert(std::pair< Key, Val >&& elt);

    /**
     * @brief Emplace a new element into the hashTable.
     *
     * If there already exists an element with the same key in the list and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness
     * policy is not set, the method runs in the worst case in constant time,
     * else if the automatic resizing policy is set, it runs in constant time
     * in average linear in the number of elements by slot.  @return a
     * reference to the pair (key,val) inserted in the hash table.  @throw
     * DuplicateElement is thrown when attempting to insert a pair (key,val) in
     * a hash table containing already a pair with the same key and when the
     * hash table's uniqueness policy is set.
     *
     * @param args The element to emplace.
     */
    template < typename... Args >
    value_type& emplace(Args&&... args);

    /**
     * @brief Returns a reference on the element the key of which is passed in
     * argument.
     *
     * In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * In case of not found key, (key,default_value) is inserted in *this.
     *
     * @param key The key for wich we want the value.
     * @param default_value The default value to return if key does not match
     * any value.
     * @return Returns a reference on the element the key of which is passed in
     * argument.
     */
    mapped_type& getWithDefault(const Key& key, const Val& default_value);

    /**
     * @brief Returns a reference on the element the key of which is passed in
     * argument.
     *
     * In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.  In case of
     * not found key, (key,default_value) is inserted in *this.
     *
     * @param key The key for wich we want the value.
     * @param default_value The default value to return if key does not match
     * any value.
     * @return Returns a reference on the element the key of which is passed in
     * argument.
     */
    mapped_type& getWithDefault(Key&& key, Val&& default_value);

    /**
     * @brief Add a new property or modify it if it already existed.
     *
     * When used as a "dynamic property list", it may be convenient to use this
     * function. Function set inserts a new pair (key,val) if the key does not
     * already exists, or it changes the value associated with key if a pair
     * (key,val) already exists in the hash table.
     *
     * @param key The key of the value to add or set.
     * @param default_value The value to set or add.
     */
    void set(const Key& key, const Val& default_value);

    /**
     * @brief Removes a property (i.e., remove an element).
     *
     * Reset removes a property (i.e., a pair (key,val)) if it exists. This is
     * an alias for erase but it is quite convenient when dealing with "dynamic
     * property lists".
     *
     * @param key The property to remove.
     */
    void reset(const Key& key);

    /**
     * @brief Removes a given element from the hash table.
     *
     * The element is the first one encountered in the list (from begin() to
     * end()) having the specified key. If no such element can be found,
     * nothing is done (in particular, it does not throw any exception). The
     * function never resizes the nodes vector (even if the resizing policy
     * would enable to decrease this size). The method runs in average in time
     * linear to the number of iterators pointing to the table if the automatic
     * resizing policy is set (else it is in linear time in the number of
     * elements of the hash table plus the number of iterators).
     *
     * @param key The key of the element to remove.
     */
    void erase(const Key& key);

    /**
     * @brief Removes a given element from the hash table.
     *
     * This method updates all the safe iterators pointing to the deleted
     * element, i.e., when trying to dereference those iterators, an exception
     * will be raised because they will know that the element they point to no
     * longer exists.
     *
     * @param iter An iterator over the element to remove.
     */
    void erase(const iterator_safe& iter);

    /**
     * @brief Removes a given element from the hash table.
     *
     * This method updates all the safe iterators pointing to the deleted
     * element, i.e., when trying to dereference those iterators, an exception
     * will be raised because they will know that the element they point to no
     * longer exists.
     *
     * @param iter An iterator over the element to remove.
     */
    void erase(const const_iterator_safe& iter);

    /**
     * @brief Removes a given element from the hash table.
     *
     * The element is the first one encountered in the list (from begin() to
     * end()) having the specified value. If no such element can be found,
     * nothing is done (in particular, it does not throw any exception). The
     * function never resizes the nodes vector (even if the resizing policy
     * would enable to decrease this size). Comparisons between Val instances
     * are performed through == operators. Logically, this method should have
     * been named "erase", however, this would have prevented creating hash
     * tables where both keys and vals have the same type. Hence we chose to
     * add "ByVal" after erase to make a difference between erasing by key and
     * erasing by val.
     *
     * @param val The value to remove.
     */
    void eraseByVal(const Val& val);

    /**
     * @brief Returns a reference on the key given a value.
     *
     * In case of multiple identical values in the hash table, the first key
     * encountered is returned. The method runs in linear time.
     *
     * @param val The value for which the key is returned.
     * @return Returns a reference on the key given a value.
     * @throws NotFound Raised if the element cannot be found.
     */
    const Key& keyByVal(const Val& val) const;

    /**
     * @brief Returns a reference on a given key.
     *
     * Some complex structures use pointers on keys of hashtables. These
     * structures thus require that we do not only get a copy of a given key,
     * but the key stored in the hashtable itself. This is the very purpose of
     * this function.
     *
     * @param key The key to return.
     * @return Returns a reference on a given key.
     * @throw NotFound Raised if the element cannot be found.
     */
    const Key& key(const Key& key) const;

    /**
     * @brief Removes all the elements having a certain value from the hash
     * table.
     *
     * If no such element can be found, nothing is done (in particular, it does
     * not throw any exception). The function never resizes the nodes vector
     * (even if the resizing policy would enable to decrease this size).
     * Comparisons between Val instances are performed through == operators.
     *
     * @param val The value to remove.
     *
     */
    void eraseAllVal(const Val& val);

    /**
     * @brief Removes all the elements in the hash table.
     *
     * The function does not resize the nodes vector (even if the size of this
     * one has been increased after the creation of the hash table) and it
     * resets the iterators on the hash table to end. The method runs in linear
     * time w.r.t.  the number of iterators pointing to the hash table.
     */
    void clear();

    /**
     * @brief Indicates whether the hash table is empty.
     * @return Returns true if the gum::HashTable is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Transforms a hashtable of vals into a hashtable of mountains.
     *
     * @warning Although the resulting hashtable has the same number of
     * elements as the original hashtable, by default, the size of the former
     * may not be equal to that of the latter. Hence iterators on the original
     * hashtable may not parse it in the same order as iterators on the
     * resulting hashtable. To guarrantee that both hashtables have the same
     * size (and thus have the elements in the same order), set the @e size
     * argument to the size of the original hashtable.
     *
     * @param f A function that maps any Val element into a Mount.
     * @param size The size of the resulting hashtable.  When equal to 0, a
     * default size is computed that is a good trade-off between space
     * consumption and efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     *
     * @return Returns the gum::HashTable of mountains.
     */
    template < typename Mount >
    HashTable< Key, Mount > map(Mount (*f)(Val),
                                Size size       = Size(0),
                                bool resize_pol = HashTableConst::default_resize_policy,
                                bool key_uniqueness_pol
                                = HashTableConst::default_uniqueness_policy) const;

    /**
     * @brief Transforms a hashtable of vals into a hashtable of mountains.
     *
     * @warning Although the resulting hashtable has the same number of
     * elements as the original hashtable, by default, the size of the former
     * may not be equal to that of the latter. Hence iterators on the original
     * hashtable may not parse it in the same order as iterators on the
     * resulting hashtable. To guarrantee that both hashtables have the same
     * size (and thus have the elements in the same order), set the @e size
     * argument to the size of the original hashtable.
     *
     * @param f A function that maps any Val element into a Mount.
     * @param size The size of the resulting hashtable.  When equal to 0, a
     * default size is computed that is a good trade-off between space
     * consumption and efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     *
     * @return Returns the gum::HashTable of mountains.
     */
    template < typename Mount >
    HashTable< Key, Mount > map(Mount (*f)(Val&),
                                Size size       = Size(0),
                                bool resize_pol = HashTableConst::default_resize_policy,
                                bool key_uniqueness_pol
                                = HashTableConst::default_uniqueness_policy) const;

    /**
     * @brief Transforms a hashtable of vals into a hashtable of mountains.
     *
     * @warning Although the resulting hashtable has the same number of
     * elements as the original hashtable, by default, the size of the former
     * may not be equal to that of the latter. Hence iterators on the original
     * hashtable may not parse it in the same order as iterators on the
     * resulting hashtable. To guarrantee that both hashtables have the same
     * size (and thus have the elements in the same order), set the @e size
     * argument to the size of the original hashtable.
     *
     * @param f A function that maps any Val element into a Mount.
     * @param size The size of the resulting hashtable.  When equal to 0, a
     * default size is computed that is a good trade-off between space
     * consumption and efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     *
     * @return Returns the gum::HashTable of mountains.
     */
    template < typename Mount >
    HashTable< Key, Mount > map(Mount (*f)(const Val&),
                                Size size       = Size(0),
                                bool resize_pol = HashTableConst::default_resize_policy,
                                bool key_uniqueness_pol
                                = HashTableConst::default_uniqueness_policy) const;

    /**
     * @brief Creates a hashtable of mounts with a given value from a hashtable
     * of vals.
     *
     * @warning Although the resulting hashtable has the same number of
     * elements as the original hashtable, by default, the size of the former
     * may not be equal to that of the latter. Hence iterators on the original
     * hashtable may not parse it in the same order as iterators on the
     * resulting hashtable. To guarrantee that both hashtables have the same
     * size (and thus have the elements in the same order), set the @e size
     * argument to the size of the original hashtable.
     *
     * @param val The value taken by all the elements of the resulting
     * hashtable.
     * @param size The size of the resulting hashtable.  When equal to 0, a
     * default size is computed that is a good trade-off between space
     * consumption and efficiency of new elements insertions
     * @param resize_pol the resizing policy (automatic or manual resizing)
     * @param key_uniqueness_pol uniqueness policy
     *
     * @return Returns the gum::HashTable of mountains.
     */
    template < typename Mount >
    HashTable< Key, Mount > map(const Mount& val,
                                Size         size       = Size(0),
                                bool         resize_pol = HashTableConst::default_resize_policy,
                                bool         key_uniqueness_pol
                                = HashTableConst::default_uniqueness_policy) const;

    /// @}

    private:
    /// Friends to optimize the access to data, iterators must be friends
    /// @{
    friend class HashTableIterator< Key, Val >;
    friend class HashTableConstIterator< Key, Val >;
    friend class HashTableIteratorSafe< Key, Val >;
    friend class HashTableConstIteratorSafe< Key, Val >;

    friend std::ostream& operator<< <>(std::ostream&, const HashTable< Key, Val >&);
    friend std::ostream& operator<< <>(std::ostream& s, const HashTable< Key*, Val >& table);

    /// For bijections to quickly access data.
    template < typename T1, typename T2 >
    friend class Bijection;
    /// @}

    /**
     * The hash table is represented as a vector of chained lists.  ' __nodes'
     * is this very vector.
     */
    std::vector< HashTableList< Key, Val > > _nodes_;

    /// The number of nodes in vector ' __nodes'.
    Size _size_;

    /// Number of elements of type Val stored in the hash table.
    Size _nb_elements_{Size(0)};

    /// The function used to hash keys (may change when the table is resized).
    HashFunc< Key > _hash_func_;

    /// Is resizing performed automatically?
    bool _resize_policy_{true};

    /// Shall we check for key uniqueness in the table?
    bool _key_uniqueness_policy_{true};

    /**
     * @brief Returns where the begin index should be.
     *
     * Beware: the beginning of a HashTable is the end of its  _nodes_ vector,
     * i.e., the Bucket at the highest index in  _nodes_. This enables a
     * slightly faster parsing than if it were the lowest index.  @warning
     * std::numeric_limits<Size>::max() means that we do not know where the
     * beginning of the table really is (this can mean either that there is not
     * yet any element in the hash table or that an erase operation has been
     * performed and that we lost track of the element that should correspond
     * to the begin().
     *
     * @return Returns where the begin index should be.
     */
    mutable Size _begin_index_{std::numeric_limits< Size >::max()};

    /// The list of safe iterators pointing to the hash table.
    mutable std::vector< HashTableConstIteratorSafe< Key, Val >* > _safe_iterators_;


    /// Erases a given bucket.
    void _erase_(HashTableBucket< Key, Val >* bucket, Size index);

    /**
     * @brief A function used to perform copies of HashTables.
     *
     * This code is shared by the copy constructor and the copy operator. The
     * function ensures that when a memory allocation problem occurs:
     *  - no memory leak occurs
     *  - the hashtable returned is empty but in a coherent state
     *  - an exception is thrown
     *
     * The function assumes that both this and table have arrays ' __nodes' of
     * the same size.
     *
     * @param table The gum::HashTable to copy.
     */
    void _copy_(const HashTable< Key, Val >& table);

    /**
     * @brief Used by all default constructors (general and specialized).
     * @param size The size of the gum::HashTable to create.
     */
    void _create_(Size size);

    /**
     * @brief Clear all the safe iterators.
     */
    void _clearIterators_();

    /**
     * @brief Adds a new element (actually a copy of this element) in the hash
     * table.
     *
     * If there already exists an element with the same key in the list and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness
     * policy is not set, the method runs in the worst case in constant time,
     * else if the automatic resizing policy is set, it runs in constant time
     * in average linear in the number of elements by slot.
     *
     * @param bucket The bucket inserted in the hash table.
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key
     * and when the hash table's uniqueness policy is set.
     */
    void _insert_(Bucket* bucket);
  };


  // ===========================================================================
  // ===                   SAFE HASH TABLES CONST ITERATORS                  ===
  // ===========================================================================
  /**
   * @class HashTableConstIteratorSafe
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief Safe Const Iterators for hashtables.
   * @ingroup hashtable_group
   *
   * HashTableConstIteratorSafe provides a safe way to parse HashTables. They
   * are safe because they are kept informed by the hashtable they belong to of
   * the elements deleted by the user. Hence, even if the user removes an
   * element pointed to by a HashTableConstIteratorSafe, using the latter to
   * access this element will never crash the application. Instead it will
   * properly throw a UndefinedIteratorValue exception.
   *
   * Developers may consider using HashTable<x,y>::const_iterator_safe instead
   * of HashTableConstIteratorSafe<x,y>.
   *
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
   * if (iter1 != iter) {
   *   cerr << "iter1 and iter2 point toward different elements";
   * }
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   */
  template < typename Key, typename Val >
  class HashTableConstIteratorSafe {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type          = Key;
    using mapped_type       = Val;
    using value_type        = std::pair< const Key, Val >;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor: creates an iterator pointing to nothing.
     */
    explicit HashTableConstIteratorSafe();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static cendSafe/crendSafe iterator
    // only hashTable.cpp should use this constructor
    explicit consteval HashTableConstIteratorSafe(StaticInitializer init) noexcept {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for an iterator pointing to the first element of a
     * hashtable.
     * @param tab A gum::HashTable to iterate over.
     */
    HashTableConstIteratorSafe(const HashTable< Key, Val >& tab);

    ///
    /**
     * @brief Constructor for an iterator pointing to the nth element of a
     * hashtable.
     *
     * The method runs in time linear to ind_elt.
     *
     * @param tab the hash table to which the so-called element belongs
     * @param ind_elt the position of the element in the hash table (0 means
     * the first element).
     * @throw UndefinedIteratorValue Raised if the element cannot be found.
     */
    HashTableConstIteratorSafe(const HashTable< Key, Val >& tab, Size ind_elt);

    /**
     * @brief Copy constructor.
     * @param from The gum::HashTableConstIteratorSafe to copy.
     */
    HashTableConstIteratorSafe(const HashTableConstIteratorSafe< Key, Val >& from);

    /**
     * @brief Copy constructor.
     * @param from The gum::HashTableConstIterator to copy.
     */
    explicit HashTableConstIteratorSafe(const HashTableConstIterator< Key, Val >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::HashTableConstIteratorSafe to move.
     */
    HashTableConstIteratorSafe(HashTableConstIteratorSafe< Key, Val >&& from);

    /**
     * @brief Destructor.
     */
    ~HashTableConstIteratorSafe() noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the key pointed to by the iterator.
     * @return Returns the key pointed to by the iterator.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid hash table element
     */
    const key_type& key() const;

    ///
    /**
     * @brief Returns the mapped value pointed to by the iterator.
     * @return Returns the mapped value pointed to by the iterator.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid hash table element.
     */
    const mapped_type& val() const;

    /**
     * @brief Makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current hash table).
     *
     * It is mainly used by the hashtable when the latter is deleted while the
     * iterator is still alive.
     */
    void clear() noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param from The gum::HashTableConstIteratorSafe to copy.
     * @return Returns this gum::HashTableConstIteratorSafe.
     */
    HashTableConstIteratorSafe< Key, Val >&
       operator=(const HashTableConstIteratorSafe< Key, Val >& from);

    /**
     * @brief Copy operator.
     * @param from The gum::HashTableConstIterator to copy.
     * @return Returns this gum::HashTableConstIteratorSafe.
     */
    HashTableConstIteratorSafe< Key, Val >&
       operator=(const HashTableConstIterator< Key, Val >& from);

    /**
     * @brief Move operator.
     * @param from The gum::HashTableConstIteratorSafe to move.
     * @return Returns this gum::HashTableConstIteratorSafe.
     */
    HashTableConstIteratorSafe< Key, Val >&
       operator=(HashTableConstIteratorSafe< Key, Val >&& from) noexcept;

    /**
     * @brief Makes the iterator point to the next element in the hash table.
     *
     * @code
     * for (iter = hash.beginSafe(); iter != hash.endSafe (); ++iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole hash table as long as no
     * element is added to or deleted from the hash table while being in the
     * loop. Deleting elements during the loop is guaranteed to never produce a
     * segmentation fault.
     *
     * @return Returns this gum::HashTableConstIteratorSafe pointing to the
     * next element in the gum::HashTable it's iterating over.
     */
    HashTableConstIteratorSafe< Key, Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the hashtable.
     * @param i The number of steps to increment the
     * gum::HashTableConstIteratorSafe.
     * @return Returns this gum::HashTableConstIteratorSafe.
     */
    HashTableConstIteratorSafe< Key, Val >& operator+=(Size i) noexcept;

    /**
     * @brief Returns a new iterator poiting to i elements further in the
     * hashtable.
     * @param i The number of steps to increment the
     * gum::HashTableConstIteratorSafe.
     * @return Returns a new gum::HashTableConstIteratorSafe.
     */
    HashTableConstIteratorSafe< Key, Val > operator+(Size i) const;

    /**
     * @brief Checks whether two iterators are not equal.
     * @param from from The iterator to test for inequality.
     * @return Returns true if from and this iterator are inequal.
     */
    bool operator!=(const HashTableConstIteratorSafe< Key, Val >& from) const noexcept;

    /**
     * @brief Checks whether two iterators are equal.
     * @param from from The iterator to test for equality.
     * @return Returns true if from and this iterator are equal.
     */
    bool operator==(const HashTableConstIteratorSafe< Key, Val >& from) const noexcept;

    /**
     * @brief Returns the element pointed to by the iterator.
     * @return Returns the element pointed to by the iterator.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid hash table element.
     */
    const value_type& operator*() const;

    /// @}

    protected:
    /**
     * Class HashTable must be a friend because it stores iterator end and this
     * can be properly initialized only when the hashtable has been fully
     * allocated. Thus, proper initialization can only take place within the
     * constructor's code of the hashtable.
     */
    friend class HashTable< Key, Val >;

    /// The hash table the iterator is pointing to.
    const HashTable< Key, Val >* _table_{nullptr};

    /**
     * @brief the index of the chained list pointed to by the iterator in the
     * array  _nodes_ of the hash table.
     */
    Size _index_{Size(0)};

    /// The bucket in the chained list pointed to by the iterator.
    HashTableBucket< Key, Val >* _bucket_{nullptr};

    /**
     * @brief the bucket we should start from when we decide to do a ++.
     *
     * Usually it should be equal to nullptr. However, if the user has deleted
     * the object pointed to by bucket, this will point to another bucket. When
     * it is equal to nullptr, it means that the bucket reached after a ++
     * belongs to another slot of the hash table's ' __node' vector.
     */
    HashTableBucket< Key, Val >* _next_bucket_{nullptr};

    /// Returns the current iterator's bucket.
    HashTableBucket< Key, Val >* _getBucket_() const noexcept;

    /**
     * @brief  Returns the index in the hashtable's node vector pointed to by
     * the iterator.
     * @return  Returns the index in the hashtable's node vector pointed to by
     * the iterator.
     */
    Size _getIndex_() const noexcept;

    /**
     * @brief Removes the iterator from its hashtable' safe iterators list.
     */
    void _removeFromSafeList_() const;

    /**
     * @brief Insert the iterator into the hashtable's list of safe iterators.
     */
    void _insertIntoSafeList_() const;
  };

  // ===========================================================================
  // ===                         HASH TABLES ITERATORS                       ===
  // ===========================================================================

  /**
   * @class HashTableIteratorSafe
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief Safe Iterators for hashtables.
   * @ingroup hashtable_group
   *
   * HashTableIteratorSafe provides a safe way to parse HashTables. They are
   * safe because they are kept informed by the hashtable they belong to of the
   * elements deleted by the user. Hence, even if the user removes an element
   * pointed to by a HashTableIteratorSafe, using the latter to access this
   * element will never crash the application. Instead it will properly throw
   * an UndefinedIteratorValue exception.
   *
   * Developers may consider using HashTable<x,y>::iterator_safe instead of
   * HashTableIteratorSafe<x,y>.
   *
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
   * if (iter1 != iter) {
   *   cerr << "iter1 and iter2 point toward different elements";
   * }
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcodesetset
   *
   * @tparam Key The gum::HashTable key.
   * @tparam Val The gum::HashTable Value.
   */
  template < typename Key, typename Val >
  class HashTableIteratorSafe: public HashTableConstIteratorSafe< Key, Val > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type          = Key;
    using mapped_type       = Val;
    using value_type        = std::pair< const Key, Val >;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor: creates an iterator pointing to nothing.
     */
    explicit HashTableIteratorSafe();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static endSafe/rendSafe iterator
    // only hashTable.cpp should use this constructor
    explicit consteval HashTableIteratorSafe(StaticInitializer init) noexcept :
        HashTableConstIteratorSafe< Key, Val >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for an iterator pointing to the first element of a
     * hashtable.
     */
    HashTableIteratorSafe(const HashTable< Key, Val >& tab);

    /**
     * @brief Constructor for an iterator pointing to the nth element of a
     * hashtable.
     *
     * The method runs in time linear to ind_elt.
     *
     * @param tab the hash table to which the so-called element belongs
     * @param ind_elt the position of the element in the hash table (0 means
     * the first element).
     * @throw UndefinedIteratorValue Raised if the element cannot be found
     */
    HashTableIteratorSafe(const HashTable< Key, Val >& tab, Size ind_elt);

    /**
     * @brief Copy constructor.
     * @param from the gum::HashTableIteratorSafe to copy.
     * @return This gum::HashTableIteratorSafe.
     */
    HashTableIteratorSafe(const HashTableIteratorSafe< Key, Val >& from);

    /**
     * @brief Copy constructor.
     * @param from the gum::HashTableIterator to copy.
     * @return This gum::HashTableIteratorSafe.
     */
    explicit HashTableIteratorSafe(const HashTableIterator< Key, Val >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::HashTableIteratorSafe to move.
     * @return Returns this gum::HashTableIteratorSafe.
     */
    HashTableIteratorSafe(HashTableIteratorSafe< Key, Val >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~HashTableIteratorSafe() noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /// Usefull Alias
    /// @{
    using HashTableConstIteratorSafe< Key, Val >::key;
    using HashTableConstIteratorSafe< Key, Val >::val;
    using HashTableConstIteratorSafe< Key, Val >::clear;
    /// @}

    /**
     * @brief Returns the mapped value pointed to by the iterator.
     * @return Returns the mapped value pointed to by the iterator.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid hash table element.
     */
    mapped_type& val();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param from The gum::HashTableIteratorSafe to copy.
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIteratorSafe< Key, Val >& operator=(const HashTableIteratorSafe< Key, Val >& from);

    /**
     * @brief Copy operator.
     * @param from The gum::HashTableIterator to copy.
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIteratorSafe< Key, Val >& operator=(const HashTableIterator< Key, Val >& from);

    /**
     * @brief Move operator.
     * @param from The gum::HashTableIteratorSafe to move.
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIteratorSafe< Key, Val >& operator=(HashTableIteratorSafe< Key, Val >&& from) noexcept;

    /**
     * @brief Makes the iterator point to the next element in the hash table.
     *
     * @code
     * for (iter = hash.beginSafe (); iter != hash.endSafe (); ++iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole hash table as long as no
     * element is added to or deleted from the hash table while being in the
     * loop. Deleting elements during the loop is guaranteed to never produce a
     * segmentation fault.
     *
     * @return This gum::HashTableIteratorSafe.
     */
    HashTableIteratorSafe< Key, Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the hashtable.
     * @param i The number of increments.
     * @return Return this gum::HashTableIteratorSafe.
     */
    HashTableIteratorSafe< Key, Val >& operator+=(Size i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i elements further in the
     * hashtable.
     * @param i The number of increments.
     * @return Returns this gum::HashTableIteratorSafe.
     */
    HashTableIteratorSafe< Key, Val > operator+(Size i) const;

    /**
     * @brief Checks whether two iterators are pointing toward different
     * elements.
     * @param from The gum::HashTableIteratorSafe to test for inequality.
     * @return Returns true if this and from are not equal.
     */
    bool operator!=(const HashTableIteratorSafe< Key, Val >& from) const noexcept;

    /**
     * @brief Checks whether two iterators are pointing toward equal
     * elements.
     * @param from The gum::HashTableIteratorSafe to test for equality.
     * @return Returns true if this and from are equal.
     */
    bool operator==(const HashTableIteratorSafe< Key, Val >& from) const noexcept;

    /**
     * @brief Returns the value pointed to by the iterator.
     * @return Returns the value pointed to by the iterator.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid hash table element
     */
    value_type& operator*();

    /**
     * @brief Returns the value pointed to by the iterator.
     * @return Returns the value pointed to by the iterator.
     *
     * @throws UndefinedIteratorValue Raised when the iterator
     * does not point to a valid hash table element.
     */
    const value_type& operator*() const;

    /// @}
  };

  // ===========================================================================
  // ===                  UNSAFE HASH TABLES CONST ITERATORS                 ===
  // ===========================================================================

  /**
   * @class HashTableConstIterator
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief Unsafe Const Iterators for hashtables
   * @ingroup hashtable_group
   *
   * HashTableConstIterator provides a fast but unsafe way to parse HashTables.
   * They should @b only be used when parsing hashtables in which no element is
   * removed from the hashtable. Removing an element where the iterator points
   * to will mess the iterator as it will most certainly point to an
   * unallocated memory. So, this kind of iterator should only be used when
   * parsing "(key) constant" hash tables, e.g., when we wish to display the
   * content of a hash table or when we wish to update the mapped values of
   * some elements of the hash table without ever modifying their keys.
   *
   * Developers may consider using HashTable<x,y>::const_iterator instead of
   * HashTableConstIterator<x,y>.
   *
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
   * if (iter1 != iter) {
   *   cerr << "iter1 and iter2 point toward different elements";
   * }
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   *
   * @tparam Key The gum::HashTable key.
   * @tparam Val The gum::HashTable Value.
   */
  template < typename Key, typename Val >
  class HashTableConstIterator {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type          = Key;
    using mapped_type       = Val;
    using value_type        = std::pair< const Key, Val >;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor: creates an iterator pointing to nothing.
     */
    explicit HashTableConstIterator() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static cend/crend iterator
    // only hashTable.cpp should use this constructor
    explicit consteval HashTableConstIterator(StaticInitializer init) noexcept {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for an iterator pointing to the first element of a
     * hashtable.
     * @param tab The gum::HashTable to iterate over.
     */
    HashTableConstIterator(const HashTable< Key, Val >& tab) noexcept;

    /**
     * @brief Constructor for an iterator pointing to the nth element of a
     * hashtable.
     *
     * The method runs in time linear to ind_elt.
     *
     * @param tab The hash table to which the so-called element belongs.
     * @param ind_elt The position of the element in the hash table (0 means
     * the first element).
     * @throw UndefinedIteratorValue Raised if the element cannot be found.
     */
    HashTableConstIterator(const HashTable< Key, Val >& tab, Size ind_elt);

    /**
     * @brief Copy constructor.
     * @param from The gum::HashTableConstIterator to copy.
     */
    HashTableConstIterator(const HashTableConstIterator< Key, Val >& from) noexcept;

    /**
     * @brief Move constructor.
     * @param from The gum::HashTableConstIterator to move.
     */
    HashTableConstIterator(HashTableConstIterator< Key, Val >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~HashTableConstIterator() noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the key corresponding to the element pointed to by the
     * iterator.
     *
     * @warning Using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure,
     * use a safe iterator instead of an unsafe one.
     *
     * @return Returns the key corresponding to the element pointed to by the
     * iterator.
     */
    const key_type& key() const;

    /**
     * @brief Returns the mapped value pointed to by the iterator.
     *
     * @warning Using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure, use
     * a safe iterator instead of an unsafe one.
     *
     * @return Returns the mapped value pointed to by the iterator.
     */
    const mapped_type& val() const;

    /**
     * @brief Makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current hash table).
     */
    void clear() noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param from The gum::HashTableConstIterator to copy.
     * @return Returns this gum::HashTableConstIterator.
     */
    HashTableConstIterator< Key, Val >&
       operator=(const HashTableConstIterator< Key, Val >& from) noexcept;

    /**
     * @brief Move operator.
     * @param from The gum::HashTableConstIterator to move.
     * @return Returns this gum::HashTableConstIterator.
     */
    HashTableConstIterator< Key, Val >&
       operator=(HashTableConstIterator< Key, Val >&& from) noexcept;

    ///
    /**
     * @brief Makes the iterator point to the next element in the hash table.
     *
     * @code
     * for (iter = cbegin (); iter != cend(); ++iter ) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole hash table as long as no
     * element is added to or deleted from the hash table while being in the
     * loop.
     *
     * @warning performing a ++ on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault.
     *
     * @return Returns this gum::HashTableConstIterator.
     */
    HashTableConstIterator< Key, Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the hashtable.
     * @param i The number of increments.
     * @return Returns this gum::HashTableConstIterator.
     */
    HashTableConstIterator< Key, Val >& operator+=(Size i) noexcept;

    /**
     * @brief Returns a new iterator pointing to i elements further in the
     * hashtable.
     * @param i The number of increments.
     * @return Returns a new iterator pointing to i elements further in the
     * hashtable.
     */
    HashTableConstIterator< Key, Val > operator+(Size i) const noexcept;

    /**
     * @brief Checks whether two iterators are pointing toward different
     * elements.
     * @param from The gum::HashTableConstIterator to test for inequality.
     * @return Returns true if this and from are not equal.
     */
    bool operator!=(const HashTableConstIterator< Key, Val >& from) const noexcept;

    /**
     * @brief Checks whether two iterators are pointing toward equal
     * elements.
     * @param from The gum::HashTableConstIterator to test for equality.
     * @return Returns true if this and from are equal.
     */
    bool operator==(const HashTableConstIterator< Key, Val >& from) const noexcept;


    /**
     * @brief Returns the value pointed to by the iterator.
     *
     * @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure,
     * use a safe iterator instead of an unsafe one.
     *
     * @return Returns the value pointed to by the iterator.
     */
    const value_type& operator*() const;

    /// @}

    protected:
    /**
     * Class HashTable must be a friend because it stores iterator end and this
     * one can be properly initialized only when the hashtable has been fully
     * allocated. Thus, proper initialization can only take place within the
     * constructor's code of the hashtable.
     */
    friend class HashTable< Key, Val >;

    /// For the safe copy constructor and operator.
    friend class HashTableConstIteratorSafe< Key, Val >;

    /// The hash table the iterator is pointing to.
    const HashTable< Key, Val >* _table_{nullptr};

    /**
     * @brief The index of the chained list pointed by the iterator in the
     * array of nodes of the hash table.
     */
    Size _index_{Size(0)};

    /// The bucket in the chained list pointed to by the iterator.
    typename HashTable< Key, Val >::Bucket* _bucket_{nullptr};

    /**
     * @brief Returns the current iterator's bucket.
     * @return Returns the current iterator's bucket.
     */
    typename HashTable< Key, Val >::Bucket* _getBucket_() const noexcept;

    /**
     * @brief Returns the index in the hashtable's node vector pointed to by
     * the iterator.
     * @return Returns the index in the hashtable's node vector pointed to by
     * the iterator.
     */
    Size _getIndex_() const noexcept;
  };

  // ===========================================================================
  // ===                      UNSAFE HASH TABLES ITERATORS                   ===
  // ===========================================================================

  /**
   * @class HashTableIterator
   * @headerfile hashTable.h <agrum/tools/core/hashTable.h>
   * @brief Unsafe Iterators for hashtables
   * @ingroup hashtable_group
   *
   * HashTableIterator provides a fast but unsafe way to parse HashTables.
   * They should @b only be used when parsing hashtables in which no element is
   * removed from the hashtable. Removing an element where the iterator points
   * to will mess the iterator as it will most certainly point to an
   * unallocated memory. So, this kind of iterator should only be used when
   * parsing "(key) constant" hash tables, e.g., when we wish to display the
   * content of a hash table or when we wish to update the mapped values of
   * some elements of the hash table without ever modifying their keys.
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
   * if (iter1 != iter) {
   *   cerr << "iter1 and iter2 point toward different elements";
   * }
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   *
   * @tparam Key The gum::HashTable key.
   * @tparam Val The gum::HashTable Value.
   */
  template < typename Key, typename Val >
  class HashTableIterator: public HashTableConstIterator< Key, Val > {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using key_type          = Key;
    using mapped_type       = Val;
    using value_type        = std::pair< const Key, Val >;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /**
     * @brief Basic constructor: creates an iterator pointing to nothing.
     */
    explicit HashTableIterator() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static end/rend iterator
    // only hashTable.cpp should use this constructor
    explicit consteval HashTableIterator(StaticInitializer init) noexcept :
        HashTableConstIterator< Key, Val >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Constructor for an iterator pointing to the first element of a
     * hashtable.
     * @param tab The gum::HashTable to iterate over.
     */
    HashTableIterator(const HashTable< Key, Val >& tab) noexcept;

    ///
    /**
     * @brief Constructor for an iterator pointing to the nth element of a
     * hashtable.
     *
     * The method runs in time linear to ind_elt.
     *
     * @param tab The hash table to which the so-called element belongs.
     * @param ind_elt The position of the element in the hash table (0 means
     * the first element).
     * @throw UndefinedIteratorValue Raised if the element cannot be found.
     */
    HashTableIterator(const HashTable< Key, Val >& tab, Size ind_elt);

    /**
     * @brief Copy constructor.
     * @param from The gum::HashTableIterator to copy.
     */
    HashTableIterator(const HashTableIterator< Key, Val >& from) noexcept;

    /**
     * @brief Move constructor.
     * @param from The gum::HashTableIterator to move.
     */
    HashTableIterator(HashTableIterator< Key, Val >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~HashTableIterator() noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /// Usefull alias.
    /// @{
    using HashTableConstIterator< Key, Val >::key;
    using HashTableConstIterator< Key, Val >::val;
    using HashTableConstIterator< Key, Val >::clear;
    /// @}

    /**
     * @brief Returns the mapped value pointed to by the iterator.
     *
     * @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure,
     * use a safe iterator instead of an unsafe one.
     *
     * @return Returns the mapped value pointed to by the iterator.
     */
    mapped_type& val();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param from The gum::HashTableIterator to copy.
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIterator< Key, Val >& operator=(const HashTableIterator< Key, Val >& from) noexcept;

    /**
     * @brief Move operator.
     * @param from The gum::HashTableIterator to move.
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIterator< Key, Val >& operator=(HashTableIterator< Key, Val >&& from) noexcept;

    /**
     * @brief Makes the iterator point to the next element in the hash table.
     *
     * @code
     * for (iter = begin(); iter != end(); ++iter) { }
     * @endcode
     *
     * The above loop is guaranteed to parse the whole hash table as long as no
     * element is added to or deleted from the hash table while being in the
     * loop.
     *
     * @warning performing a ++ on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault.
     *
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIterator< Key, Val >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the hashtable.
     * @param i The number of increments.
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIterator< Key, Val >& operator+=(Size i) noexcept;

    /**
     * @brief Returns a new iterator.
     * @param i The number of increments.
     * @return Returns this gum::HashTableIterator.
     */
    HashTableIterator< Key, Val > operator+(Size i) const noexcept;

    /**
     * @brief Checks whether two iterators are pointing toward different
     * elements.
     * @param from The gum::HashTableIterator to test for inequality.
     * @return Returns true if this and from are not equal.
     */
    bool operator!=(const HashTableIterator< Key, Val >& from) const noexcept;

    /**
     * @brief Checks whether two iterators are pointing toward equal
     * elements.
     * @param from The gum::HashTableIterator to test for equality.
     * @return Returns true if this and from are equal.
     */
    bool operator==(const HashTableIterator< Key, Val >& from) const noexcept;

    /**
     * @brief Returns the value pointed to by the iterator.
     *
     * @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure,
     * use a safe iterator instead of an unsafe one.
     *
     * @return Returns the value pointed to by the iterator.
     */
    value_type& operator*();

    /**
     * @brief Returns the value pointed to by the iterator.
     *
     * @warning using this method on an iterator that points to an element that
     * has been deleted will most certainly result in a segfault. If unsure,
     * use a safe iterator instead of an unsafe one.
     *
     * @return Returns the value pointed to by the iterator.
     */
    const value_type& operator*() const;

    /// @}
  };


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // _static_HashTable_end_ is a 'constant' iterator initialized at compile time
  // that represents the end/rend iterators for all hash tables (whatever their
  // type). This global variable avoids creating the same iterators within every
  // HashTable instance (this would be quite inefficient as end is precisely
  // identical for all AVL trees). The same hold for const and safe end iterators.
  // The type of _HashTable_end_ is a pointer to void because C++ allows
  // pointers to void to be cast into pointers to other types (and conversely).
  // This avoids the painful strict-aliasing rule warning
  extern const HashTableIterator< int, int >          _static_HashTable_end_;
  extern const HashTableConstIterator< int, int >     _static_HashTable_cend_;
  extern const HashTableIteratorSafe< int, int >      _static_HashTable_end_safe_;
  extern const HashTableConstIteratorSafe< int, int > _static_HashTable_cend_safe_;

  inline constexpr void* const _HashTable_end_       = (void* const)&_static_HashTable_end_;
  inline constexpr void* const _HashTable_cend_      = (void* const)&_static_HashTable_cend_;
  inline constexpr void* const _HashTable_end_safe_  = (void* const)&_static_HashTable_end_safe_;
  inline constexpr void* const _HashTable_cend_safe_ = (void* const)&_static_HashTable_cend_safe_;
#endif   // DOXYGEN_SHOULD_SKIP_THIS

}   // namespace gum


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::HashTable< int, int >;
extern template class gum::HashTable< int, std::string >;
extern template class gum::HashTable< std::string, std::string >;
extern template class gum::HashTable< std::string, int >;
#endif


// always include the implementation of the templates
#include <agrum/tools/core/hashTable_tpl.h>

#endif   // GUM_HASHTABLE_H
