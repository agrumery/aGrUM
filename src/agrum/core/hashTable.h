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
 * amount of processor instructions, and flexible because several methods allow
 * fine tuning the behavior of each hash table. For instance, a hashtable can
 * allow or forbid different elements to have the same key. This behavior can be
 * modified at any time during the execution of the program. Functions for hashing
 * keys are defined in file HashFunc.h. Here again, these functions are quite
 * flexible as they can be overloaded by the user to support new kind of keys. In
 * addition to HashTable, the current file provides class HashTableIterator that
 * allows safe parsing of the hash tables. By safe, we mean that whenever the
 * element pointed to by an iterator is removed from the hashtable, accessing it
 * through the iterator (*iter) does not result in a segmentation fault but rather
 * in an exception being thrown. This safety is ensured at a very low cost.
 * @par Usage example:
 * @code
 * // creation of an empty hash table
 * HashTable<int,string> table1;
 *
 * // insert two elements into the hash table
 * table1.insert (10,"xxx");
 * table1.insert (20,"yyy");
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
 * // checks whether there exists an element with key 20
 * if (table1.exists (20)) cerr << "element found" << endl;
 *
 * // transform the hashtable of string into a hashtable of int assuming f is
 * // defined as: int f (const string& str) { return str.size (); }
 * HashTable<int,char*> table = table1.map (f);
 *
 * // remove two elements from table1 and table2
 * table1.erase (10);
 * table1.eraseByVal ("yyy");
 * table2.clear ();
 *
 * // check whether the hash table is empty
 * if (!table1.empty ()) cerr << "table not empty" << endl;
 *
 * // checks wether hashtables contain the same elements
 * if ((table1 == table2) && (table1 != table3))
 *   cerr << "check for equality/inequality" << endl;
 *
 * // parse the content of a hashtable
 * for (HashTable<int,string>::iterator iter = table1.begin();
 *       iter != table1.end(); ++iter)
 *   cerr << *iter;
 *
 * // use an iterator to point the element we wish to erase
 * HashTable<int,string>::iterator iter = table1.begin();
 * table1.erase ( iter );
 *
 * // check for iterator's safety
 * for (HashTable<int,string>::iterator iter = table1.begin();
 *       iter != table1.end(); ++iter)
 *   table1.eraseByVal (*iter);
 * @endcode
 */

#ifndef GUM_HASH_TABLE_H
#define GUM_HASH_TABLE_H

#include <iostream>
#include <agrum/core/utils.h>
#include <agrum/core/hashFunc.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // parameters specifying the default behavior of the hashtables
  extern const Size         GUM_HASHTABLE_DEFAULT_SIZE;
  extern const unsigned int GUM_HASHTABLE_DEFAULT_MEAN_VAL_BY_SLOT;
  extern const bool         GUM_HASHTABLE_DEFAULT_RESIZE_POLICY;
  extern const bool         GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY;

  template <typename Key, typename Val> class HashTableList;
  template <typename Key, typename Val> class HashTable;
  template <typename Key, typename Val> class HashTableIterator;
  template <typename Key, typename Val> class HashTableConstIterator;
  template <typename T1,  typename T2>  class Bijection;

  
  // ==============================================================================
  /// a << operator for HashTableList
  // ==============================================================================
  template <typename Key, typename Val> std::ostream& operator<< 
  ( std::ostream&, const HashTableList<Key, Val>& );
  
  // ==============================================================================
  /// a << operator for HashTableList with pointer keys
  // ==============================================================================
  template <typename Key, typename Val> std::ostream& operator<<
  ( std::ostream&, const HashTableList<Key*, Val>& );

  // ==============================================================================
  /// a \c << operator for HashTable
  // ==============================================================================
  template <typename Key, typename Val> std::ostream& operator<<
  ( std::ostream&, const HashTable<Key, Val>& );

  // ==============================================================================
  /// a \c << operator for HashTable with pointer keys
  // ==============================================================================
  template <typename Key, typename Val> std::ostream& operator<<
  ( std::ostream&, const HashTable<Key*, Val>& );




  
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  
  // a class used to create the static iterator used by HashTables. The aim of
  // using this class rather than just creating __HashTableIterEnd as a global
  // variable is to prevent other classes to access and modify __HashTableIterEnd
  class HashTableIteratorStaticEnd {
  private:
    // the iterator used by everyone
    static const HashTableIterator<int,int>* __HashTableIterEnd;

    // creates (if needed) and returns the iterator __HashTableIterEnd
    static const HashTableIterator<int,int>* end4Statics ();

    // friends that have access to the iterator
    template<typename Key, typename Val> friend class HashTable;
  };
  
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===          LISTS SPECIFIC FOR SAVING ELEMENTS IN HASHTABLES           === */
  /* =========================================================================== */
  /* =========================================================================== */

  /* =========================================================================== */
  /* in aGrUM, hashtables are vectors of chained lists. Each list corresponds to
   * the pairs (key,val) the keys of which have the same hashed value. Each box of
   * the list is called a bucket. Lists are doubly linked so as to enable efficient
   * begin/end iterators. */
  /* =========================================================================== */
  template <typename Key, typename Val> struct HashTableBucket {
    HashTableBucket<Key, Val> *prev;
    HashTableBucket<Key, Val> *next;
    Key key;
    Val val;
    
    // ============================================================================
    HashTableBucket() : prev( 0 ), next( 0 ) {}

    HashTableBucket( const HashTableBucket<Key,Val>& from ) :
      prev( 0 ), next( 0 ), key( from.key ), val( from.val ) {}

    HashTableBucket( const Key& k, const Val& v ) :
      prev( 0 ), next( 0 ), key( k ), val( v ) {}
  };


  
  /* =========================================================================== */
  /* ===       DOUBLY CHAINED LISTS FOR STORING ELEMENTS IN HASH TABLES      === */
  /* =========================================================================== */
  template <typename Key, typename Val> class HashTableList {
  public:
    // ============================================================================
    /* basic constructor that creates an empty list. This is what is used
     * basically by hash tables */
    // ============================================================================
    HashTableList();
    
    // ============================================================================
    /** copy constructor. The new list and that which is copied do not share the
     * elements: the new list contains new instances of the keys and of values
     * stored in the list to be copied. Of course if these values are pointers,
     * the new values point toward the same elements. */
    // ============================================================================
    HashTableList( const HashTableList<Key, Val>& from );
    
    // ============================================================================
    /** assignment operator. The new list and that which is copied do not share the
     * elements: the new list contains new instances of the keys and of values
     * stored in the list to be copied. Of course if these values are pointers,
     * the new values point toward the same elements. If some allocation problem
     * occurs or if the copy of the Val elements cannot be performed properly,
     * exceptions may be raised. In this case, the function guarrantees that no
     * memory leak occurs and that the list is kept in a coherent state (that of
     * an empty list) */
    // ============================================================================
    HashTableList<Key, Val>& operator= ( const HashTableList<Key, Val>& from );
    
    // ============================================================================
    // destructor
    // ============================================================================
    virtual ~HashTableList();
    
    // ============================================================================
    /** function at returns the ith element in the current chained list. The first
     * element has index 0.
     * @throw NotFound */
    // ============================================================================
    Val& at( unsigned int i );
    const Val& at( unsigned int i ) const;
    
    // ============================================================================
    /// operator [] returns the value corresponding to a given key
    /**
     * @throw NotFound
     */
    // ============================================================================
    Val& operator[]( const Key& key );
    const Val& operator[]( const Key& key ) const ;
    
    // ============================================================================
    /// check whether there exists an element with a given key in the list
    // ============================================================================
    bool exists( const Key& key ) const  ;
    
    // ============================================================================
    /** inserts a new element in the chained list. The element is inserted
     * at the beginning of the list. When allocation cannot be performed, the
     * function raises a bad_alloc exception. */
    // ============================================================================
    Val& insert( const Key& key, const Val& val );
    
    // ============================================================================
    /** inserts a new element in the chained list. The element is inserted
     * at the beginning of the list. When allocation cannot be performed, the
     * function raises a bad_alloc exception. */
    // ============================================================================
    const Key& insertAndGetKey ( const Key& key, const Val& val );
    
    // ============================================================================
    /// function for deleting all the elements of a chained list
    // ============================================================================
    void clear();
    
    // ============================================================================
    /// returns a boolean indicating whether the chained list is empty
    // ============================================================================
    bool empty() const ;
    
  protected:
    // ============================================================================
    /* a function used to perform copies of HashTableLists. This code is
     * shared by the copy constructor and the copy operator. The function throws
     * a bad_alloc exception if it cannot perform the necessary allocations. In
     * this case, no memory leak occurs and the list is set to the empty list. */
    // ============================================================================
    void _copy( const HashTableList<Key, Val> &from );
    
    // ============================================================================
    // a method to get the bucket corresponding to a given key. This enables
    // efficient removals of buckets
    // ============================================================================
    HashTableBucket<Key, Val>* _getBucket( const Key& key ) const ;
    
    // ============================================================================
    /* function erase suppresses an element from a chained list. */
    // ============================================================================
    void _erase( const HashTableBucket<Key, Val>* ptr );

    // ============================================================================
    /** inserts a new element in the chained list. The element is inserted
     * at the beginning of the list. When allocation cannot be performed, the
     * function raises a bad_alloc exception. */
    // ============================================================================
    HashTableBucket<Key,Val>*
    _insertAndGetBucket ( const Key& key, const Val& val );
    
     
  private:
    // friends
    friend class HashTable<Key, Val>;
    friend class HashTableIterator<Key, Val>;
    friend class HashTableConstIterator<Key, Val>;
    friend std::ostream& operator<< <> ( std::ostream&,
                                         const HashTableList<Key,Val>& );
    friend std::ostream& operator<< <> ( std::ostream&,
                                         const HashTableList<Key*,Val>& );
    friend std::ostream& operator<< <> ( std::ostream&,
                                         const HashTable<Key,Val>& );
    friend std::ostream& operator<< <> ( std::ostream&,
                                         const HashTable<Key*,Val>& );
    
    // a pointer on the first element of the chained list
    HashTableBucket<Key, Val> *__deb_list;

    // a pointer on the last element of the chained list
    HashTableBucket<Key, Val> *__end_list;

    // the number of elements in the chained list
    unsigned int __nb_elements;
  };


#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  
  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                          GENERIC HASH TABLES                        === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class HashTable
   * @brief The class for generic Hash Tables
   * @ingroup basicstruct_group
   *
   * In aGrUM, a hashtable is a vector of chained lists (fix collision problems by
   * chaining). Each slot of the vector contains a list of elements sharing the
   * same hashed value. To be computationally efficient, the hash table should not
   * contain too many elements as compared to its number of slots. Therefore, it
   * is sometimes useful to resize the chained lists vector. aGrUM's hash tables
   * are designed to automatically double their size when there is in average more
   * than 3 nodes per slot. However, when memory consumption is a concern, this
   * feature can be turned off either by passing false as an optional resize_pol
   * argument to the constructor of the hash table or by using method
   * setResizePolicy when the instance of the class has already been constructed.
   * Similarly, the default number of slots of the hash table may be parameterized
   * as an optional argument of the constructor (size_param). Beware: when
   * inserting elements of a given class into a hash table, only a copy of this
   * element is stored into the table (this is compulsory if the hashtable is to
   * be generic and can be used to store both complex classes and built-in types
   * like integers). Note that HashTables can be parsed using "safe" iterators:
   * whenever the element pointed to by an iterator is removed from the hashtable,
   * accessing it through the iterator (*iter) does not result in a segmentation
   * fault but rather in an exception being thrown. This safety is ensured at a
   * very low cost.
   * @par Usage example:
   * @code
   * // creation of an empty hash table
   * HashTable<int,string> table1;
   *
   * // insert two elements into the hash table
   * table1.insert (10,"xxx");
   * table1.insert (20,"yyy");
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
   * // remove two elements from table1 and table2
   * table1.erase (10);
   * table1.eraseByVal ("yyy");
   * table2.clear ();
   *
   * // get the element whose key is 10
   * cerr << table1[10] << " = xxx" << endl;
   *
   * // checks whether there exists an element with key 20
   * if (table1.exists (20)) cerr << "element found" << endl;
   *
   * // transform the hashtable of string into a hashtable of int assuming f is
   * // defined as: int f (const string& str) { return str.size (); }
   * HashTable<int,char*> table = table1.map (f);
   *
   * // check whether the hash table is empty
   * if (!table1.empty ()) cerr << "table not empty" << endl;
   *
   * // checks wether hashtables contain the same elements
   * if ((table1 == table2) && (table1 != table3))
   *   cerr << "check for equality/inequality" << endl;
   *
   * // parse the content of a hashtable
   * for (HashTable<int,string>::iterator iter = table1.begin();
   *       iter != table1.end(); ++iter)
   *   cerr << *iter;
   *
   * // use an iterator to point the element we wish to erase
   * HashTable<int,string>::iterator iter = table1.begin();
   * table1.erase ( iter );
   *
   * // check for iterator's safety
   * for (HashTable<int,string>::iterator iter = table1.begin();
   *       iter != table1.end(); ++iter)
   *   table1.eraseByVal (*iter);
   * @endcode
   */
  /* =========================================================================== */
  template <typename Key, typename Val> class HashTable {
  public:
    /// iterators for hash tables (to be used with HashTable<X,Y>::iterator)
    typedef HashTableIterator<Key, Val> iterator;

    
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################

    /// @{
    
    // ===========================================================================
    /** @brief Default constructor: optionnally we may indicate the desired size
     * for the nodes vector.
     *
     * Actually, the true capacity (vector's size) of the hashtable will be the
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
    // ============================================================================
    explicit
    HashTable(Size size_param = GUM_HASHTABLE_DEFAULT_SIZE,
              bool resize_pol = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY,
              bool key_uniqueness_pol = GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY );

    #ifndef NDEBUG
    HashTable( std::string dummy,
               Size size_param = GUM_HASHTABLE_DEFAULT_SIZE,
               bool resize_pol = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY,
               bool key_uniqueness_pol = GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY );
    #endif //NDEBUG

    
    // ============================================================================
    /// copy constructor
    /** This creates a new hashtable the content of which is
     * similar to that of the table passed in argument. Beware: similar does not
     * mean that both tables share the same objects, but rather that the objects
     * stored in the newly created table are copies of those of the table passed
     * in argument. In particular, the new hash tables inherit the parameters
     * (resize policy, uniqueness policy) of table 'table' */
    // ============================================================================
    HashTable( const HashTable<Key,Val>& table );

    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~HashTable();

    /// @}


    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the iterator to the end of the hashtable
    // ============================================================================
    const iterator& end() const ;

    // ============================================================================
    /// returns the iterator at the beginning of the hashtable
    // ============================================================================
    iterator begin() const ;

    // ============================================================================
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
     * enfore the fact that HashTable::end is initialized before Set::end. Using
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
    // ============================================================================
    static const iterator& end4Statics ();
    
    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// copy operator
    /** the copy operators ensures that whenever a memory allocation problem
     * occurs, no memory leak occurs as well and it also guarantees that in this
     * case the hashtable returned is in a coherent state (it is an empty
     * hashtable). note that the copy not only involves copying pairs (key,value)
     * but also the copy of the resize and key uniqueness policies. */
    // ============================================================================
    HashTable<Key,Val>& operator= ( const HashTable<Key, Val> &from );

    // ============================================================================
    /// returns a reference on the value the key of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    Val& operator[]( const Key& key );
    const Val& operator[]( const Key& key ) const;

    // ============================================================================
    /// returns a reference on the value the key of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * In case of not found key, (key,default_value) is inserted in *this. */
    // ============================================================================
    Val& getWithDefault( const Key& key, const Val& default_value );
    const Val& getWithDefault( const Key& key, const Val& default_value ) const;

    // ============================================================================
    /// add or remove property
    /** When used as a "dynamic property list", it may be convenient to use these
     * functions. Function set inserts a new pair (key,val) if the key does not
     * already exists, or it changes the value associated with key if a pair
     * (key,val) already exists in the hash table.
     * reset removes a property (i.e., a pair (key,val)) if it exists. */
    // ============================================================================
    void set( const Key& key, const Val& default_value );
    void reset( const Key& key );

    // ============================================================================
    /// checks whether two hashtables contain the same elements
    /** Two hashtables are considered equal if they contain the identical pairs
     * (key,val). Two pairs are identical if their keys have the same hashed value,
     * these two keys are equal in the sense of ==, and their val's are also equal
     * in the sense of ==. */
    // ============================================================================
    bool operator== ( const HashTable<Key, Val> &from ) const ;
    
    // ============================================================================
    /// checks whether two hashtables contain different sets of elements
    /** Two hashtables are considered different if they contain different pairs
     * (key,val). Two pairs are different if heir keys have different hashed
     * values, or if they are different in the sense of !=, or if their val's are
     * different in the sense of !=. */
    // ============================================================================
    bool operator!= ( const HashTable<Key, Val> &from ) const ;

    /// @}


    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the number of slots in the 'nodes' vector of the hashtable
    /** The method runs in constant time. */
    // ============================================================================
    Size capacity() const ;

    // ============================================================================
    /// changes the number of slots in the 'nodes' vector of the hash table
    /** Usually, method resize enables the user to resize manually the hashtable.
     * When in automatic resize mode, the function will actually resize the table
     * only if resizing policy is compatible with the new size, i.e., the new size
     * is not so small that there would be too many elements by slot in the table
     * (this would lead to a significant loss in performance). However, the
     * resizing policy may be changed by using method setResizePolicy.
     * The method runs in linear time in the size of the hashtable.
     * Upon memory allocation problem, the fuction guarantees that no data is
     * lost and that the hash table and its iterators are in a coherent state. In
     * such a case, a bad_alloc exception is thrown. */
    // ============================================================================
    void resize( Size new_size );

    // ============================================================================
    /// enables the user to change dynamically the resizing policy
    /** In most cases, this should be useless. However, when available memory
     * becomes rare, avoiding automatic resizing may speed-up new insertions in
     * the table.
     * @warning This function never resizes the hashtable by itself:
     * even if you set the new policy to be an automatic resizing and the number
     * of elements in the table is sufficiently high that we should resize the
     * table, function setResizePolicy won't perform this resizing. The resizing
     * will happen only if you insert a new element or if use method resize. */
    // ============================================================================
    void setResizePolicy( const bool new_policy ) ;

    // ============================================================================
    /// returns the current resizing policy
    // ============================================================================
    bool resizePolicy() const ;

    // ============================================================================
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
    // ============================================================================
    void setKeyUniquenessPolicy( const bool new_policy ) ;

    // ============================================================================
    /// returns the current checking policy
    // ============================================================================
    bool keyUniquenessPolicy() const ;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the number of elements stored into the hashtable
    /** The method runs in constant time. */
    // ============================================================================
    Size size() const ;

    // ============================================================================
    /// checks whether there exists an element with a given key in the hashtable
    /** The method runs in average in constant time if the resizing policy
     * is set. */
    // ============================================================================
    bool exists( const Key& key ) const ;

    // ============================================================================
    /// adds a new element (actually a copy of this element) in the hash table
    /** If there already exists an element with the same key in the list and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return As only a copy of val is inserted into the hashtable, the method
     * returns a reference on this copy.
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set.
     * @throw bad_alloc exception is thrown when memory allocation problems occur.
     * In this case, the new element is of course not added to the hash table.
     * However, the latter is guaranteed to stay in a coherent state. */
    // ============================================================================
    Val& insert( const Key& key, const Val& val );

    // ============================================================================
    /// adds a new element (actually a copy of this element) in the hash table
    /** If there already exists an element with the same key in the list and the
     * uniqueness policy prevents multiple identical keys to belong to the same
     * hashtable, an exception DuplicateElement is thrown. If the uniqueness policy
     * is not set, the method runs in the worst case in constant time, else if
     * the automatic resizing policy is set, it runs in constant time in average
     * linear in the number of elements by slot.
     * @return a reference to the key inserted in the hash table.
     * @throw DuplicateElement is thrown when attempting to insert a pair
     * (key,val) in a hash table containing already a pair with the same key and
     * when the hash table's uniqueness policy is set.
     * @throw bad_alloc exception is thrown when memory allocation problems occur.
     * In this case, the new element is of course not added to the hash table.
     * However, the latter is guaranteed to stay in a coherent state. */
    // ============================================================================
    const Key& insertAndGetKey ( const Key& key, const Val& val );

    // ============================================================================
    /// removes a given element from the hash table
    /** The element is the first one encountered in the list (from begin() to
     * end()) having the specified key. If no such element can be found, nothing
     * is done (in particular, it does not throw any exception). The function
     * never resizes the nodes vector (even if the resizing policy would enable to
     * decrease this size). The method runs in average in time linear to the
     * number of iterators pointing to the table if the automatic resizing policy
     * is set (else it is in linear time in the number of elements of the hash
     * table plus the number of iterators). */
    // ============================================================================
    void erase( const Key& key );

    // ============================================================================
    /// removes a given element from the hash table
    // ============================================================================
    void erase( const iterator& iter );

    // ============================================================================
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
    // ============================================================================
    void eraseByVal( const Val& val );

    // ============================================================================
    /// returns a reference on the key the value of which is passed in argument
    /** In case of multiple identical values in the hash table, the first key
     * encountered is returned. The method runs in linear time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    const Key& keyByVal( const Val& key ) const;

    // ============================================================================
    /// returns a reference on a given key
    /** Some complex structures use pointers on keys of hashtables. These
     * structures thus require that we do not only get a copy of a given key, but
     * the key stored in the hashtable itself. This is the very purpose of this
     * function.
     * @throw NotFound is raised if the element cannot be found. */
    // ============================================================================
    const Key& key( const Key& key ) const;

    // ============================================================================
    /// removes all the elements having a certain value from the hash table
    /** If no such element can be found, nothing is done (in
     * particular, it does not throw any exception). The function never resizes the
     * nodes vector (even if the resizing policy would enable to decrease
     * this size). Comparisons between Val instances are performed through ==
     * operators. */
    // ============================================================================
    void eraseAllVal( const Val& val );

    // ============================================================================
    /// removes all the elements in the hash table
    /** The function does not resize the nodes vector (even if the size of this one
     * has been increased after the creation of the hash table) and it resets the
     * iterators on the hash table to end. The method runs in linear time w.r.t.
     * the number of iterators pointing to the hash table. */
    // ============================================================================
    void clear();

    // ============================================================================
    /// indicates whether the hash table is empty
    // ============================================================================
    bool empty() const ;

    // ============================================================================
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
    // ============================================================================
    template <typename Mount> HashTable<Key,Mount>
    map( Mount( *f )( Val ), Size size = 0,
         bool resize_pol = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY,
         bool key_uniqueness_pol = GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY ) const;
    template <typename Mount> HashTable<Key,Mount>
    map( Mount( *f )( Val& ), Size size = 0,
         bool resize_pol = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY,
         bool key_uniqueness_pol = GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY ) const;
    template <typename Mount> HashTable<Key,Mount>
    map( Mount( *f )( const Val& ), Size size = 0,
         bool resize_pol = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY,
         bool key_uniqueness_pol = GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY ) const;

    // ============================================================================
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
    // ============================================================================
    template <typename Mount> HashTable<Key,Mount>
    map( const Mount& val, Size size = 0,
         bool resize_pol = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY,
         bool key_uniqueness_pol = GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY ) const;

    /// @}

    
  protected:
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // ============================================================================
    /** a function used to perform copies of HashTables. This code is
     * shared by the copy constructor and the copy operator. The function ensures
     * that when a memory allocation problem occurs:
     * - no memory leak occurs
     * - the hashtable returned is empty but in a coherent state
     * - an exception is thrown
     * The function assumes that both this and table have arrays 'nodes' of the
     * same size. */
    // ============================================================================
    void _copy( const HashTable<Key, Val>& table );

    // ============================================================================
    /// a function used by all destructors (general and specialized)
    // ============================================================================
    void _destroy();

    // ============================================================================
    /// a function used by all default constructors (general and specialized)
    // ============================================================================
    void _create( Size size );

    // ============================================================================
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
     * when the hash table's uniqueness policy is set.
     * @throw bad_alloc exception is thrown when memory allocation problems occur.
     * In this case, the new element is of course not added to the hash table.
     * However, the latter is guaranteed to stay in a coherent state. */
    // ============================================================================
    HashTableBucket<Key,Val>*
    _insertAndGetBucket ( const Key& key, const Val& val );

    #endif /* DOXYGEN_SHOULD_SKIP_THIS */



  private:
    // friends
    /// to optimize the access to data, iterators mut be friends
    friend class HashTableIterator<Key, Val>;

    /// for friendly displaying the content of the hashtable
    friend std::ostream& operator<< <> ( std::ostream&,
                                         const HashTable<Key, Val>& );
    friend std::ostream& operator<< <> ( std::ostream&,
                                         const HashTable<Key*, Val>& );

    /// for bijections to quickly access data
    template <typename T1, typename T2> friend class Bijection;

    
    /** @brief the hash table is represented as a vector of chained lists. 'nodes'
     * is this very vector. */
    HashTableList<Key, Val> *__nodes;

    /// the number of nodes in vector 'nodes'
    Size __size;

    /// number of elements of type Val stored in the hash table
    Size __nb_elements;

    /// the function used to hash keys  (may change when the table is resized)
    HashFunc<Key> __hash_func;

    /// is resizing performed automatically?
    bool __resize_policy;

    /// shall we check for key uniqueness in the table?
    bool __key_uniqueness_policy;

    /// the list of iterators pointing to the hash table
    mutable iterator *__iterator_list;

    // ============================================================================
    /// erases a given bucket
    // ============================================================================
    void __erase ( HashTableBucket<Key, Val>* bucket, Size index );

  };



  
  /* =========================================================================== */
  /* ===                         HASH TABLES ITERATORS                       === */
  /* =========================================================================== */
  /** @class HashTableIterator
   * @brief Iterators for hashtables
   *
   * HashTableIterator provide a safe way to parse HashTable. They are safe
   * because they are kept informed by the hashtable they belong to of the
   * elements deleted by the user. Hence, even if the user removes an element
   * pointed to by a HashTableIterator, using the latter to access this element
   * will never crash the application. Instead it will properly throw a
   * UndefinedIteratorValue exception.
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
   *   cerr << "at " << iter.key() << " value = " << *iter << endl;
   *   // make the string pointed to by the iterator use method append
   *   iter->append ("yyy");
   * }
   *
   * // check whether two iterators point toward the same element
   * HashTable<int,string>::iterator iter1 = table1.begin();
   * HashTable<int,string>::iterator iter2 = table1.end();
   * if (iter1 != iter) cerr << "iter1 and iter2 point toward different elements
   *
   * // make iter1 point toward nothing
   * iter1.clear ();
   * @endcode
   */
  /* =========================================================================== */
  template <typename Key, typename Val> class HashTableIterator {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// basic constructor: creates an iterator pointing to nothing
    // ============================================================================
    HashTableIterator() ;

    // ============================================================================
    /// copy constructor
    // ============================================================================
    HashTableIterator( const HashTableIterator<Key, Val>& from ) ;

    // ============================================================================
    /// constructor for an iterator pointing to the nth element of a hashtable
    /** The method runs in time linear to ind_elt.
     * @param tab the hash table to which the so-called element belongs
     * @param ind_elt the position of the element in the hash table (0 means the
     * first element).
     * @throw UndefinedIteratorValue exception is thrown if the element cannot
     * be found */
    // ============================================================================
    HashTableIterator( const HashTable<Key, Val>& tab, Size ind_elt );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~HashTableIterator() ;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// returns the key corresponding to the value pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    // ============================================================================
    const Key& key() const;
    
    // ============================================================================
    /** @brief makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current hash table)
     *
     * It is mainly used by the hashtable when the latter is deleted while the
     * iterator is still alive. */
    // ============================================================================
    void clear() ;

    /// @}



    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// copy operator
    // ============================================================================
    HashTableIterator<Key,Val>& operator=
    ( const HashTableIterator<Key,Val>& from ) ;

    // ============================================================================
    /// makes the iterator point to the next element in the hash table
    /** for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to parse the
     * whole hash table as long as no element is added to or deleted from the
     * hash table while being in the loop. Deleting elements during the
     * loop is guaranteed to never produce a segmentation fault. */
    // ============================================================================
    HashTableIterator<Key, Val>& operator++() ;

    // ============================================================================
    /// checks whether two iterators are pointing toward different elements
    // ============================================================================
    bool operator!= ( const HashTableIterator<Key, Val> &from ) const ;

    // ============================================================================
    /// checks whether two iterators are pointing toward the same element
    // ============================================================================
    bool operator== ( const HashTableIterator<Key, Val> &from ) const ;

    // ============================================================================
    /// returns the value pointed to by the iterator
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid hash table element
     * @throw UndefinedIteratorValue */
    // ============================================================================
    Val& operator*() ;
    const Val& operator*() const;
    
    // ============================================================================
    /// dereferences the value pointed to by the iterator
    /** This operator allows developers to write code like @c iterator->member()
     * to run the member() method of the Val pointed to by the iterator
     * @throw UndefinedIteratorValue */
    // ============================================================================
    Val* operator-> () const;
    
    /// @}


  private:
    /** class HashTable must be a friend because it stores iterator end
     * and those can be properly initialized only when the hashtable has been
     * fully allocated. Thus, proper initialization can only take place within
     * the constructor's code of the hashtable. */
    friend class HashTable<Key, Val>;
    
    /// the hash table the iterator is pointing to
    const HashTable<Key, Val> *__table;

    /** @brief the index of the chained list pointed by the iterator in the
     * array of nodes of the hash table */
    Size __index;

    /// the bucket in the chained list pointed to by the iterator
    HashTableBucket<Key, Val> *__bucket;

    /** @brief the bucket we should start from when we decide to do a ++. Usually
     * it should be equal to bucket. However, if the user has deleted the object
     * pointed to by bucket, this will point to another bucket. When it is equal to
     * 0, it means that the bucket reached after a ++ belongs to another slot of
     * the hash table's 'node' vector. */
    HashTableBucket<Key, Val> *__next_bucket;

    /// next iterator attached to the hashtable
    HashTableIterator<Key, Val> *__next;

    /// preceding iterator of the hashtable registered list of iterators
    HashTableIterator<Key, Val> *__prev;

    // ============================================================================
    /// returns the current iterator's bucket
    // ============================================================================
    HashTableBucket<Key, Val> *__getBucket() const ;

    // ============================================================================
    /// returns the index in the hashtable's node vector pointed to by the iterator
    // ============================================================================
    Size __getIndex() const ;    

  };


#ifndef DOXYGEN_SHOULD_SKIP_THIS

  /// the end iterator for all hash tables
  extern const HashTableIterator<int,int>* HashTableIterEnd;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  
} /* namespace gum */


/// always include the implementation of the templates
#include <agrum/core/hashTable.tcc>
  
#endif  // GUM_HASHTABLE_H
