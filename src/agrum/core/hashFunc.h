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
 * @brief Classes providing basic hash functions for hash tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_HASH_FUNC_H
#define GUM_HASH_FUNC_H

// utility provides the std::pair <> template
#include <string>
#include <utility>

#include <agrum/config.h>

#include <agrum/core/refPtr.h>

namespace gum {

  /** in aGrUM, the sizes of hash tables (number of slots) are powers of 2. This
   * is not actually compulsory for the hash function we use. However, as it
   * speeds up the computations of hashed values, we chose to impose
   * this restriction. Function __hashTableLog2 thus returns the size in
   * bits - 1 necessary to store the smallest power of 2 greater than or
   * equal nb. */
  unsigned int __hashTableLog2( const Size& nb );

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  extern const Size               GUM_HASHTABLE_INT_GOLD;
  extern const Size               GUM_HASHTABLE_INT_PI;
  extern const Size               GUM_HASHTABLE_MASK;
  extern const size_t             GUM_HASHTABLE_OFFSET;

  //for pedantic  extern const unsigned long long GUM_HASHTABLE_LONG_GOLD;
  //for pedantic  extern const unsigned long long GUM_HASHTABLE_LONG_PI;


  // the hash function class

  template <typename Key> class HashFunc;

  template <typename Key> class HashFuncSmallKey;

  template <typename Key> class HashFuncBigKey;

  template <typename Key1, typename Key2> class HashFuncSmallKeyPair;

  template <typename Key1, typename Key2> class HashFuncBigKeyPair;

  template <typename Key, typename Val> class HashTable;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  /* =========================================================================== */
  /* ===             BASE CLASS SHARED BY ALL THE HASH FUNCTIONS             === */
  /* =========================================================================== */
  /** @class HashFuncBase
   * @brief All hash functions should inherit from this class
   *
   * Whenever you create a new hash function, make sure to inherit from this class,
   * else your hash function will not compile because HashTables will refer
   * directly to this class. The way HashTable work, you do not need to define
   * constructors, destructors and assignment operators: those synthesized by C++
   * will work correctly. However, if your hash function contains new attributes,
   * you shall override the resize function to properly set these attributes (you
   * may even have to redefine the default constructor and/or the assignment
   * operator, but this should not occur very often. In fact, usually, when you
   * create a new hash function, you will only need to write something like:
   * @code
   * template <> class HashFunc<MyObject> : public HashFuncBase<MyObject> {
   * public:
   *   Size operator() (const MyObject& key) const  {
   *      here write the code using HashFuncBase<MyObject>::_hash_size and/or
   *      HashFuncBase<MyObject>::_hash_log2_size and/or
   *      HashFuncBase<MyObject>::_hash_mask
   * };
   * @endcode
   * For instance, here is how HashFunc<string> is written:
   * @code
   * template <> class HashFunc<std::string>: public HashFuncBase<std::string> {
   * public:
   *   Size operator() (const std::string& key) const  {
   *     Size h = 0;
   *     for (size_t i = 0, j = key.size(); i < j; ++i)
   *       h = 19 * h + key[i];
   *     return ((h * GUM_HASHTABLE_INT_GOLD) & _hash_mask);
   *   }
   * };
   * @endcode
   *
   * _hash_size corresponds to the number of slots of the hash table. Your function
   * should always return a number in [0,_hash_size). As the number of slots in a
   * HashTable is always a power of 2, it may be convenient to know the number
   * of bits used by the hashed keys, this is precisely the information contained
   * in _hash_log2_size. Finally, _hash_mask is a mask that can be used to ensure
   * that the hashed key actually belongs to [0,_hash_size). This is used in
   * particular in the hash function for hashing strings. */
  /* =========================================================================== */

  template <typename Key> class HashFuncBase {
    public:
      /// update the hash function to take into account a resize of the hash table
      /**
       * @param s the hashtable's size wished by the user. Actually, a hashtable
       * of size n is an array of n lists. When the user wishes to resize the
       * hashtable so that the array is of size s, the hashtable resizes itself to
       * the smallest power of 2 greater than or equal to s. This new size is
       * precisely what is computed by function resize. Hence s should be the size
       * of the array of lists, not the number of elements stored into the hashtable.
       * @throw HashSize
       */
      virtual void resize( Size s );

      /// computes the hashed value of a key
      virtual Size operator()( const Key& ) const = 0;

      /// returns the hash table size as known by the hash function
      Size size() const;

    protected:
      /// the size of the hash table
      Size _hash_size;

      /// log of the size of the hash table in base 2
      unsigned int _hash_log2_size;

      /** @brief when you use this mask, you are guarranteed that hashed keys belong
       * to the set of indices of the hash table */
      Size _hash_mask;

      /** @brief constructors and destructors are protected so as to
       * guarrantee that user-defined HashFunc inherit from HashFuncBase */

      /// hash function for default size hash tables
      HashFuncBase() ;

      /// copy constructor
      HashFuncBase( const HashFuncBase& from ) ;

      /// copy operator
      HashFuncBase& operator=( const HashFuncBase& ) ;

      /// destructor
      virtual ~HashFuncBase() ;
  };



#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                  GENERIC MULTIPURPOSE HASH FUNCTIONS                === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** generic hash functions for keys smaller than long integer */
  template <typename Key> class HashFuncSmallKey :
    public HashFuncBase<Key> {
    public:
      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const Key& ) const;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;
  };


  /** generic hash functions for keys castable as unsigned longs and whose size
   * is precisely that of unsigned longs */
  template <typename Key> class HashFuncMediumCastKey :
    public HashFuncBase<Key> {
    public:
      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const Key& ) const;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;
  };


  /** generic hash functions for keys castable as unsigned longs and whose size
   * is smaller than that of unsigned longs */
  template <typename Key> class HashFuncSmallCastKey :
    public HashFuncBase<Key> {
    public:
      /// basic constructor
      HashFuncSmallCastKey();

      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const Key& ) const;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;

      // an additional mask to ensure that keys with fewer bits than unsigned longs
      // are cast correctly
      unsigned long _small_key_mask;
  };



  /** generic hash functions for pairs of at most long integer keys */

  template <typename Key1, typename Key2> class HashFuncSmallKeyPair :
    public HashFuncBase< std::pair<Key1,Key2> > {
    public:
      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const std::pair<Key1,Key2>& ) const ;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;
  };


  /** generic hash functions for pairs of keys whose sizes are precisely that
   * of unsigned longs and which can be cast into unsigned longs */
  template <typename Key1, typename Key2> class HashFuncMediumCastKeyPair :
    public HashFuncBase< std::pair<Key1,Key2> > {
    public:
      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const std::pair<Key1,Key2>& ) const ;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;
  };


  /** generic hash functions for pairs of at most long integer keys */
  template <typename Key1, typename Key2> class HashFuncSmallCastKeyPair :
    public HashFuncBase< std::pair<Key1,Key2> > {
    public:
      /// basic constructor
      HashFuncSmallCastKeyPair();

      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const std::pair<Key1,Key2>& ) const ;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;

      // some masks used to ensure that keys with fewer bits than unsigned longs
      // are cast correctly
      unsigned long _small_key_mask1;
      unsigned long _small_key_mask2;
  };


  /** generic hash functions for keys longer than long integers */

  template <typename Key> class HashFuncBigKey :
    public HashFuncBase<Key> {
    public:
      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const Key& ) const ;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;
  };


  /** generic hash functions for pairs of keys longer than long integers */

  template <typename Key1, typename Key2> class HashFuncBigKeyPair :
    public HashFuncBase< std::pair<Key1,Key2> > {
    public:
      /// update the hash function to take into account a resize of the hash table
      /** @throw HashSize */
      void resize( Size );

      /// computes the hashed value of a key
      Size operator()( const std::pair<Key1,Key2>& ) const ;

    protected:
      // the number of right shift to perform to get correct hashed values
      unsigned int _right_shift;
  };



  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                      WIDELY USED HASH FUNCTIONS                     === */
  /* =========================================================================== */
  /* =========================================================================== */
  /* =========================================================================== */
  /* this class should be useless as only its specializations should be used.
   * However it prevents to create hash functions on key types that are not yet
   * supported. */

  template <typename key> class HashFunc {};

  template <> class HashFunc<int> :
    public HashFuncSmallKey<int> {};

  template <> class HashFunc<unsigned int> :
    public HashFuncSmallKey<unsigned int> {};

  template <> class HashFunc<long> :
    public HashFuncSmallKey<long> {};

  template <> class HashFunc<unsigned long> :
    public HashFuncSmallKey<unsigned long> {};

  template <> class HashFunc<float> :
#if GUM_FLOAT_SIZE >= GUM_LONG_SIZE
    public HashFuncMediumCastKey<float> {};
#else
    public HashFuncSmallCastKey<float> {};
#endif

  template <> class HashFunc<double> :
#if GUM_DOUBLE_SIZE >= GUM_LONG_SIZE
    public HashFuncMediumCastKey<double> {};
#else
    public HashFuncSmallCastKey<double> {};
#endif


  //for pedantic    template <> class HashFunc<long long> :
  //for pedantic  public HashFuncBigKey<long long> {};

  //for pedantic  template<> class HashFunc<unsigned long long> :
  //for pedantic  public HashFuncBigKey<unsigned long long> {};

  template <> class HashFunc< std::pair<int, int> > :
    public HashFuncSmallKeyPair<int, int> {};

  template <> class HashFunc< std::pair<unsigned int, unsigned int> > :
    public HashFuncSmallKeyPair<unsigned int, unsigned int> {};

  template<> class HashFunc< std::pair<long, long> > :
    public HashFuncSmallKeyPair<long, long> {};

  template <> class HashFunc< std::pair<unsigned long, unsigned long> > :
    public HashFuncSmallKeyPair<unsigned long, unsigned long> {};

  template <> class HashFunc< std::pair<float,float> > :
#if GUM_FLOAT_SIZE >= GUM_LONG_SIZE
    public HashFuncMediumCastKeyPair<float,float> {};
#else
    public HashFuncSmallCastKeyPair<float,float> {};
#endif

  template <> class HashFunc< std::pair<double,double> > :
#if GUM_DOUBLE_SIZE >= GUM_LONG_SIZE
    public HashFuncMediumCastKeyPair<double,double> {};
#else
    public HashFuncSmallCastKeyPair<double,double> {};
#endif

  template <> class HashFunc< std::pair<double, long unsigned int> > :
    public HashFuncMediumCastKeyPair<double, long unsigned int> {
  };

  template <> class HashFunc< std::pair<double, long int> > :
    public HashFuncMediumCastKeyPair<double, long int> {
  };

  /* removed for pedantic reasons
     template <> class HashFunc< std::pair<long long, long long> > :
     public HashFuncBigKeyPair<long long, long long> {};

     template <>
     class HashFunc< std::pair<unsigned long long, unsigned long long> > :
     public HashFuncBigKeyPair<unsigned long long, unsigned long long> {};
  */

  template <typename Type> class HashFunc<Type*> :
    public HashFunc<unsigned long> {
    public:
      /// computes the hashed value of a key
      Size operator()( Type* const& key ) const ;
  };

  template <typename Type> class HashFunc< RefPtr<Type> > :
    public HashFunc<unsigned int*> {
    public:
      /// computes the hashed value of a key
      Size operator()( const RefPtr<Type>& key ) const ;
  };

  template <> class HashFunc<std::string> : public HashFuncBase<std::string> {
    public:
      /// computes the hashed value of a key
      Size operator()( const std::string& key ) const ;
  };

  template <> class HashFunc< std::pair<std::string, std::string> > :
    public HashFuncBase< std::pair<std::string, std::string> > {
    public:
      /// computes the hashed value of a key
      Size operator()( const std::pair<std::string, std::string>& key ) const ;
  };

  template <> class HashFunc< std::vector<Idx> > :
    public HashFuncBase< std::vector< Idx > > {
    public:
      /// computes the hashed value of a key
      Size operator()( const std::vector< Idx >& key ) const ;
  };

  template <> class HashFunc<Debug> : public HashFuncBase<Debug> {
    public:
      /// computes the hashed value of a key
      Size operator()( const Debug& key ) const ;
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/hashFunc.inl>
#endif /* GUM_NO_INLINE */


/// always include the implementation of the templates
#include <agrum/core/hashFunc.tcc>


#endif /* GUM_HASHFUNC_H */
