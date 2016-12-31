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

/**
 * @file
 * @brief Classes providing basic hash functions for hash tables.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_HASH_FUNC_H
#define GUM_HASH_FUNC_H

// utility provides the std::pair <> template
#include <climits>
#include <string>
#include <type_traits>
#include <utility>

#include <agrum/config.h>
#include <agrum/core/refPtr.h>

namespace gum {

  /**
   * @ingroup hashfunctions_group
   * @brief Returns the size in bits - 1 necessary to store the smallest power
   * of 2 greater or equal to nb.
   *
   * In aGrUM, the sizes of hash tables (number of slots) are powers of 2. This
   * is not actually compulsory for the hash function we use. However, as it
   * speeds up the computations of hashed values, we chose to impose this
   * restriction.
   *
   * @param nb The greatest number for wich this function will return a power
   * of 2.
   * @return Returns the size in bits - 1 necessary to store the smallest power
   * of 2 greater than or equal to nb.
   */
  unsigned int __hashTableLog2( const Size& nb );

  /**
   * @class HashFuncConst hashFunc.h <agrum/core/hashFunc.h>
   * @ingroup hashfunctions_group
   * @brief Constants for hash functions.
   *
   * Hash functions are of the form [M * ((k * A) mod 1)], where [] stands for
   * the integer part, M is equal to the number of slots in the hashtable, k is
   * the key to be hashed, and mod 1 retrieves the decimal part of (k * A). A
   * is an irrational number (currently either the gold number or pi/4). To
   * speed up computations, the hash function is implemented using only
   * unsigned longs.  Therefore pi/4 and the gold number are encoded as X *
   * 2^{-n} where n is the number of bits in an unsigned long. Consequently, we
   * should adapt X's definition to 32 and 64 bits architectures.
   * */
  struct HashFuncConst {
#if ULONG_MAX == 4294967295UL  // unsigned long = 32 bits
    static constexpr unsigned long gold = 2654435769UL;
    static constexpr unsigned long pi = 3373259426UL;
    static constexpr unsigned long mask = 4294967295UL;
    static constexpr size_t        offset = 32;
#else  // unsigned long = 64 bits
    static constexpr unsigned long gold = 11400714819323198486UL;
    static constexpr unsigned long pi = 14488038916154245684UL;
    static constexpr unsigned long mask = 18446744073709551615UL;
    static constexpr size_t        offset = 64;
#endif /* unsigned long = 32 bits */
  };

  // ===========================================================================
  // ===             BASE CLASS SHARED BY ALL THE HASH FUNCTIONS             ===
  // ===========================================================================
  /**
   * @class HashFuncBase hashFunc.h <agrum/core/hashFunc.h>
   * @brief All hash functions should inherit from this class.
   * @ingroup hashfunctions_group
   *
   * Whenever you create a new hash function you must inherit from this class.
   * Otherwise, your hash function will not compile because gum::HashTable will
   * refer directly to this class.
   *
   * The way gum::HashTable work, you do not need to define constructors,
   * destructors and assignment operators: the defaults created by C++ will
   * work correctly. However, if your hash function contains new attributes,
   * you must override the resize function to properly set these attributes.
   * You may even have to redefine the default constructor and/or the
   * assignment operator, but this should not occur very often.
   *
   * In fact, usually, when you create a new hash function, you will only need
   * to write something like:
   *
   * @code
   * template <> class HashFunc<MyObject> : public HashFuncBase<MyObject> {
   * public:
   *   Size operator() (const MyObject& key) const  {
   *      // here write the code using
   *      // HashFuncBase<MyObject>::_hash_size and/or
   *      // HashFuncBase<MyObject>::_hash_log2_size and/or
   *      // HashFuncBase<MyObject>::_hash_mask
   *   }
   * };
   * @endcode
   *
   * For instance, here is how HashFunc<string> is implemented:
   *
   * @code
   * template <> class HashFunc<std::string>: public HashFuncBase<std::string> {
   * public:
   *   Size operator() (const std::string& key) const  {
   *     Size h = 0;
   *     for (size_t i = 0, j = key.size(); i < j; ++i) {
   *       h = 19 * h + key[i];
   *     }
   *     return ((h * GUM_HASHTABLE_INT_GOLD) & _hash_mask);
   *   }
   * };
   * @endcode
   *
   * The gum::HashFunc::_hash_size attribute corresponds to the number of slots
   * in the gum::HashTable. Your function should always return a number in
   * [0,_hash_size). As the number of slots in a gum::HashTable is always a
   * power of 2, it may be convenient to know the number of bits used by the
   * hashed keys, this is precisely the information contained in
   * gum::HashFunc::_hash_log2_size. Finally, gum::HashFunc::_hash_mask is a
   * mask that can be used to ensure that the hashed key actually belongs to
   * [0,_hash_size). This is used in particular in the hash function for
   * hashing strings.
   *
   * @tparam Key The type hashed by this hash function.
   */
  template <typename Key>
  class HashFuncBase {
    public:
    /**
     * @brief Update the hash function to take into account a resize of the
     * hash table.
     *
     * When the user wishes to resize the gum::HashTable so that the array is
     * of size s, the gum::HashTable resizes itself to the smallest power of 2
     * greater than or equal to s. This new size is computed by function
     * gum::HashFuncBase::resize(gum::Size). Hence, s should be the size of the
     * array of lists, not the number of elements stored into the
     * gum::HashTable.
     *
     * @param s The hashtable's size wished by the user. Actually, a hashtable
     * of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    virtual void resize( Size s );

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()( const Key& key ) const = 0;

    /**
     * @brief Returns the hash table size as known by the hash function.
     * @return Returns the hash table size as known by the hash function.
     */
    Size size() const noexcept;

    protected:
    /// The size of the hash table.
    Size _hash_size{0};

    /// Log of the size of the hash table in base 2.
    unsigned int _hash_log2_size{0};

    /**
     * When you use this mask, you are guaranteed that hashed keys belong to
     * the set of indexes of the hash table.
     */
    Size _hash_mask{0};
  };

  // ===========================================================================
  // ===                  GENERIC MULTIPURPOSE HASH FUNCTIONS                ===
  // ===========================================================================

  /**
   * @class HashFuncSmallKey hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for keys smaller than or equal to long
   * integers.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template <typename Key>
  class HashFuncSmallKey : private HashFuncBase<Key> {
    public:
    /// This class use the HashFuncBase::size.
    using HashFuncBase<Key>::size;

    /**
     * @brief Update the hash function to take into account a resize of the
     * hash table.
     *
     * When the user wishes to resize the gum::HashTable so that the array is
     * of size s, the gum::HashTable resizes itself to the smallest power of 2
     * greater than or equal to s. This new size is computed by function
     * gum::HashFuncBase::resize(gum::Size). Hence, s should be the size of the
     * array of lists, not the number of elements stored into the
     * gum::HashTable.
     *
     * @param s The hashtable's size wished by the user. Actually, a hashtable
     * of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    void resize( Size s );

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const Key& key ) const;

    protected:
    /// The number of right shift to perform to get correct hashed values.
    unsigned int _right_shift;
  };

  /**
   * @class HashFuncSmallCastKey hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as unsigned longs and
   * whose size is strictly smaller than that of unsigned longs.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template <typename Key>
  class HashFuncSmallCastKey : private HashFuncBase<Key> {
    public:
    /**
     * @brief Class constructor.
     */
    HashFuncSmallCastKey();

    /**
     * @brief Update the hash function to take into account a resize of the
     * hash table.
     *
     * When the user wishes to resize the gum::HashTable so that the array is
     * of size s, the gum::HashTable resizes itself to the smallest power of 2
     * greater than or equal to s. This new size is computed by function
     * gum::HashFuncBase::resize(gum::Size). Hence, s should be the size of the
     * array of lists, not the number of elements stored into the
     * gum::HashTable.
     *
     * @param s The hashtable's size wished by the user. Actually, a hashtable
     * of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    void resize( Size s );

    /**
     * @brief Returns the value of a key as an unsigned long.
     * @param key The value to return as an unsigned long.
     * @return Returns the value of a key as an unsigned long.
     */
    Size castToSize( const Key& key ) const;

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const Key& key ) const;

    protected:
    /// The number of right shift to perform to get correct hashed values.
    unsigned int _right_shift;

    /**
     * An additional mask to ensure that keys with fewer bits than unsigned
     * longs are cast correctly.
     */
    unsigned long _small_key_mask;
  };

  /**
   * @class HashFuncMediumCastKey hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as unsigned longs and
   * whose size is precisely that of unsigned longs.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template <typename Key>
  class HashFuncMediumCastKey : private HashFuncBase<Key> {
    public:
    /// This class use the HashFuncBase::size.
    using HashFuncBase<Key>::size;

    /**
     * @brief Update the hash function to take into account a resize of the
     * hash table.
     *
     * When the user wishes to resize the gum::HashTable so that the array is
     * of size s, the gum::HashTable resizes itself to the smallest power of 2
     * greater than or equal to s. This new size is computed by function
     * gum::HashFuncBase::resize(gum::Size). Hence, s should be the size of the
     * array of lists, not the number of elements stored into the
     * gum::HashTable.
     *
     * @param s The hashtable's size wished by the user. Actually, a hashtable
     * of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    void resize( Size s );

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const Key& key ) const;

    /**
     * @brief Returns the value of a key as an unsigned long.
     * @param key The value to return as an unsigned long.
     * @return Returns the value of a key as an unsigned long.
     */
    Size castToSize( const Key& key ) const;


    protected:
    /// The number of right shift to perform to get correct hashed values.
    unsigned int _right_shift;
  };

  /**
   * @class HashFuncLargeCastKey hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as unsigned longs and
   * whose size is precisely twice that of unsigned longs.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template <typename Key>
  class HashFuncLargeCastKey : private HashFuncBase<Key> {
    public:
    /// This class use the HashFuncBase::size.
    using HashFuncBase<Key>::size;

    /**
     * @brief Update the hash function to take into account a resize of the
     * hash table.
     *
     * When the user wishes to resize the gum::HashTable so that the array is
     * of size s, the gum::HashTable resizes itself to the smallest power of 2
     * greater than or equal to s. This new size is computed by function
     * gum::HashFuncBase::resize(gum::Size). Hence, s should be the size of the
     * array of lists, not the number of elements stored into the
     * gum::HashTable.
     *
     * @param s The hashtable's size wished by the user. Actually, a hashtable
     * of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    void resize( Size s );

    /**
     * @brief Cast key to the exepcted type.
     * @param key The key to cast.
     * @return Returns the cast key to the exepcted type.
     */
    Size castToSize( const Key& key ) const;

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const Key& key ) const;

    protected:
    /// The number of right shift to perform to get correct hashed values.
    unsigned int _right_shift;
  };

  /**
   * @class HashFuncCastKey hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as unsigned longs whose
   * size is either smaller than unsigned long, or equal to that of one or two
   * unsigned longs.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template <typename T>
  struct HashFuncCastKey {

    /// The type used by this class.
    using type = typename std::conditional<
        sizeof( T ) < sizeof( long ),
        HashFuncSmallCastKey<T>,
        typename std::conditional<sizeof( T ) == 2 * sizeof( long ),
                                  HashFuncLargeCastKey<T>,
                                  HashFuncMediumCastKey<T>>::type>::type;
  };

  /**
   * @class HashFuncSmallKeyPair hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for pairs of, at most, two long integer
   * keys.
   * @ingroup hashfunctions_group
   * @tparam Key1 The type hashed of the first element in the pair.
   * @tparam Key2 The type hashed of the second element in the pair.
   */
  template <typename Key1, typename Key2>
  class HashFuncSmallKeyPair : public HashFuncBase<std::pair<Key1, Key2>> {
    public:
    /**
     * @brief Update the hash function to take into account a resize of the
     * hash table.
     *
     * When the user wishes to resize the gum::HashTable so that the array is
     * of size s, the gum::HashTable resizes itself to the smallest power of 2
     * greater than or equal to s. This new size is computed by function
     * gum::HashFuncBase::resize(gum::Size). Hence, s should be the size of the
     * array of lists, not the number of elements stored into the
     * gum::HashTable.
     *
     * @param s The hashtable's size wished by the user. Actually, a hashtable
     * of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    void resize( Size s );

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const std::pair<Key1, Key2>& key ) const;

    protected:
    /// The number of right shift to perform to get correct hashed values.
    unsigned int _right_shift;
  };

  /**
   * @class HashFuncAllCastKeyPair hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for pairs of keys whose sizes are precisely
   * twice that of unsigned longs and which can be cast into unsigned longs.
   * @ingroup hashfunctions_group
   * @tparam Key1 The type hashed of the first element in the pair.
   * @tparam Key2 The type hashed of the second element in the pair.
   * @tparam Func1 The function to hash Key1.
   * @tparam Func2 The function to hash Key2.
   */
  template <typename Key1, typename Key2, typename Func1, typename Func2>
  class HashFuncAllCastKeyPair : public HashFuncBase<std::pair<Key1, Key2>> {
    public:
    /**
     * @brief Update the hash function to take into account a resize of the
     * hash table.
     *
     * When the user wishes to resize the gum::HashTable so that the array is
     * of size s, the gum::HashTable resizes itself to the smallest power of 2
     * greater than or equal to s. This new size is computed by function
     * gum::HashFuncBase::resize(gum::Size). Hence, s should be the size of the
     * array of lists, not the number of elements stored into the
     * gum::HashTable.
     *
     * @param s The hashtable's size wished by the user. Actually, a hashtable
     * of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    void resize( Size s );

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const std::pair<Key1, Key2>& key ) const;

    protected:
    /// The number of right shift to perform to get correct hashed values.
    unsigned int _right_shift;

    private:
    /// The functions used to hash Key1.
    Func1 __func1;
    /// The functions used to hash Key2.
    Func2 __func2;
  };

  /**
   * @class HashFuncCastKeyPair hashFunc.h <agrum/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as unsigned longs whose
   * size is either smaller than unsigned long, or equal to that of one or two
   * unsigned longs.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template <typename T1, typename T2>
  struct HashFuncCastKeyPair {
    /// The casting function for T1.
    using Func1 = typename HashFuncCastKey<T1>::type;

    /// The casting function for T2.
    using Func2 = typename HashFuncCastKey<T2>::type;

    /// The expected type of this class.
    using type = HashFuncAllCastKeyPair<T1, T2, Func1, Func2>;
  };

  // ===========================================================================
  // ===                      WIDELY USED HASH FUNCTIONS                     ===
  // ===========================================================================

  /**
   * @class HashFunc hashFunc.h <agrum/core/hashFunc.h>
   * @brief This class should be useless as only its specializations should be
   * used.
   *
   * However it prevents to create hash functions on key types that are not yet
   * supported.
   *
   * @ingroup hashfunctions_group
   */
  template <typename key>
  class HashFunc {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for booleans.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<bool> : public HashFuncSmallKey<bool> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<int> : public HashFuncSmallKey<int> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for unsigned integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<unsigned int> : public HashFuncSmallKey<unsigned int> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for long integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<long> : public HashFuncSmallKey<long> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for unsigned long integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<unsigned long> : public HashFuncSmallKey<unsigned long> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for floats.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<float> : public HashFuncCastKey<float>::type {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for doubles.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<double> : public HashFuncCastKey<double>::type {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pointers.
   * @tparam The type for which the pointer is used to compute a hash.
   */
  template <typename Type>
  class HashFunc<Type*> : public HashFuncCastKey<Type*>::type {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<int, int>> : public HashFuncSmallKeyPair<int, int> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of unsigned integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<unsigned int, unsigned int>>
      : public HashFuncSmallKeyPair<unsigned int, unsigned int> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of long integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<long, long>> : public HashFuncSmallKeyPair<long, long> {
  };

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of unsigned long integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<unsigned long, unsigned long>>
      : public HashFuncSmallKeyPair<unsigned long, unsigned long> {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of float.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<float, float>>
      : public HashFuncCastKeyPair<float, float>::type {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of double.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<double, double>>
      : public HashFuncCastKeyPair<float, float>::type {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of double and long unsigned int.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<double, long unsigned int>>
      : public HashFuncCastKeyPair<double, long unsigned int>::type {};

  /**
   * @author JCM
   * @brief Modification : adding mirrored pair key of the one above
   * Though the question should be asked of whether we enforce only
   * one version for pair key or whether we authorize mirrored version
   * If one version is enforced sdyna.cpp must be modified!
   * @ingroup HashF
   */
  template <>
  class HashFunc<std::pair<long unsigned int, double>>
      : public HashFuncCastKeyPair<long unsigned int, double>::type {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of double and long int.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<double, long int>>
      : public HashFuncCastKeyPair<double, long int>::type {};

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for RefPtr.
   * @ingroup hashfunctions_group
   * @tparam Type The type of the RefPtr.
   */
  template <typename Type>
  class HashFunc<RefPtr<Type>> : public HashFunc<unsigned int*> {
    public:
    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const RefPtr<Type>& key ) const;
  };

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for strings.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::string> : public HashFuncBase<std::string> {
    public:
    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const std::string& key ) const;
  };

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for pairs of strings.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::pair<std::string, std::string>>
      : public HashFuncBase<std::pair<std::string, std::string>> {
    public:
    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const std::pair<std::string, std::string>& key ) const;
  };

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for vectors of gum::Idx.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<std::vector<Idx>> : public HashFuncBase<std::vector<Idx>> {
    public:
    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const std::vector<Idx>& key ) const;
  };

  /**
   * @headerfile hashFunc.h <agrum/core/hashFunc.h>
   * @brief Hash function for gum::Debug.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<Debug> : public HashFuncBase<Debug> {
    public:
    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    Size operator()( const Debug& key ) const;
  };

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/hashFunc_inl.h>
#endif /* GUM_NO_INLINE */

/// always include the implementation of the templates
#include <agrum/core/hashFunc_tpl.h>

#endif /* GUM_HASHFUNC_H */
