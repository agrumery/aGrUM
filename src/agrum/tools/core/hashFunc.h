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
 * @brief Classes providing basic hash functions for hash tables.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_HASH_FUNC_H
#define GUM_HASH_FUNC_H

// utility provides the std::pair <> template
#include <climits>
#include <string>
#include <type_traits>
#include <utility>

#include <agrum/agrum.h>
#include <agrum/tools/core/refPtr.h>

namespace gum {

  /**
   * @ingroup hashfunctions_group
   * @brief Returns the size in bits - 1 necessary to store the smallest power
   * of 2 greater than or equal to nb.
   *
   * In aGrUM, the sizes of hash tables (number of slots) are powers of 2. This
   * is not actually compulsory for the hash function we use. However, as it
   * speeds up the computations of hashed values, we chose to impose this
   * restriction.
   *
   * @param nb The greatest number for which this function will return a power
   * of 2.
   * @return Returns the size in bits - 1 necessary to store the smallest power
   * of 2 greater than or equal to nb.
   */
  unsigned int _hashTableLog2_(const Size nb);

  /**
   * @class HashFuncConst
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @ingroup hashfunctions_group
   * @brief Useful constants for hash functions.
   *
   * Hash functions are of the form [M * ((k * A) mod 1)], where [] stands for
   * the integer part, M is equal to the number of slots in the hashtable, k is
   * the key to be hashed, and mod 1 retrieves the decimal part of (k * A). A
   * is an irrational number (currently either the gold number or pi/4). To
   * speed up computations, the hash function is implemented using only
   * Size (a.k.a. std::size_t). Therefore pi/4 and the gold number are encoded
   * as X * 2^{-n} where n is the number of bits in a Size. Consequently, we
   * should adapt X's definition to 32 and 64 bits architectures.
   */
  struct HashFuncConst {
    static constexpr Size gold
       = sizeof(Size) == 4 ? Size(2654435769UL) : Size(11400714819323198486UL);
    static constexpr Size pi
       = sizeof(Size) == 4 ? Size(3373259426UL) : Size(14488038916154245684UL);
    static constexpr Size mask
       = sizeof(Size) == 4 ? Size(4294967295UL) : Size(18446744073709551615UL);
    static constexpr Size offset = sizeof(Size) == 4 ? Size(32) : Size(64);
  };


  // ===========================================================================
  // ===             BASE CLASS SHARED BY ALL THE HASH FUNCTIONS             ===
  // ===========================================================================
  /**
   * @class HashFuncBase
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
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
   *      // HashFuncBase<MyObject>::hash_size_ and/or
   *      // HashFuncBase<MyObject>::hash_log2_size_ and/or
   *      // HashFuncBase<MyObject>::hash_mask_
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
   *     return ((h * GUM_HASHTABLE_INT_GOLD) & hash_mask_);
   *   }
   * };
   * @endcode
   *
   * The gum::HashFunc::hash_size_ attribute corresponds to the number of slots
   * in the gum::HashTable. Your function should always return a number in
   * [0,_hash_size). As the number of slots in a gum::HashTable is always a
   * power of 2, it may be convenient to know the number of bits used by the
   * hashed keys, this is precisely the information contained in
   * gum::HashFunc::hash_log2_size_. Finally, gum::HashFunc::hash_mask_ is a
   * mask that can be used to ensure that the hashed key actually belongs to
   * [0,_hash_size). This is used in particular in the hash function for
   * hashing strings.
   *
   * @tparam Key The type hashed by this hash function.
   */
  template < typename Key >
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
     * @param new_size The hashtable's size wished by the user. Actually, a
     * hashtable of size n is an array of n lists.
     * @throw SizeError Raised if s is too small.
     */
    void resize(const Size new_size);

    /**
     * @brief Returns the hash table size as known by the hash function.
     * @return Returns the size of the hash table, i.e., the number of slots
     * (lists) it contains.
     */
    Size size() const;

    /**
     * @brief Computes the hashed value of a key.
     *
     * The classes inheriting from HashFuncBase should always declare
     * Operator() as follows:
     * @code
     * Size operator()(const Key& key) const override final;
     * @endcode
     * and its implementation should be something like:
     * @code
     * INLINE Size HashFunc< Key >::operator()(const Key& key) const {
     *   return (castToSize(key) * HashFuncConst::gold) >> this->right_shift_;
     * }
     * @endcode
     * By doing this, compilers optimize the code so that it is significantly
     * speeded-up because no virtual table will be used and everything is
     * most certainly inlined. Of course, you need to define a static method
     * castToSize which should take as argument a const Key& and return a Size
     *
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const Key& key) const = 0;

    protected:
    /// The size of the hash table.
    Size hash_size_{Size(0)};

    /// Log of the number of slots of the hash table in base 2.
    unsigned int hash_log2_size_{0};

    /**
     * @brief performing y = x & hash_mask_ guarantees that y is a slot index
     * of the hash table
     *
     * To transform a Size x into a slot index of the hash table, you can either
     * use x & hash_mask_ or x >> right_shift_ depending on whether you want
     * to exploit the least significant bits of x (&) or the most significant
     * one (>>).
     */
    Size hash_mask_{Size(0)};

    /**
     * @brief performing y = x >> right_shift_ guarantees that y is a slot
     * index of the hash table
     *
     * To transform a Size x into a slot index of the hash table, you can either
     * use x & hash_mask_ or x >> right_shift_ depending on whether you want
     * to exploit the least significant bits of x (&) or the most significant
     * one (>>).
     */
    unsigned int right_shift_{0};
  };


  // ===========================================================================
  // ===                GENERIC HASH FUNCTIONS FOR SIMPLE TYPES              ===
  // ===========================================================================

  /**
   * @class HashFuncSmallKey
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Generic hash functions for numeric keys smaller than or equal to Size
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template < typename Key >
  class HashFuncSmallKey: public HashFuncBase< Key > {
    public:
    /**
     * @brief Class constructor.
     */
    HashFuncSmallKey();

    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Key& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const Key& key) const override final;
  };


  /**
   * @class HashFuncSmallCastKey
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as Size and
   * whose size is strictly smaller than that of Size.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template < typename Key >
  class HashFuncSmallCastKey: public HashFuncBase< Key > {
    public:
    /**
     * @brief Class constructor.
     */
    HashFuncSmallCastKey();

    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Key& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const Key& key) const override final;

    protected:
    /**
     * An additional mask to ensure that keys with fewer bits than Size
     * are cast correctly.
     */
    static constexpr Size small_key_mask_{(Size(1) << (8 * sizeof(Key))) - Size(1)};
  };


  /**
   * @class HashFuncMediumCastKey
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as Size and
   * whose size is precisely that of Size.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template < typename Key >
  class HashFuncMediumCastKey: public HashFuncBase< Key > {
    public:
    /**
     * @brief Class constructor.
     */
    HashFuncMediumCastKey();

    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Key& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const Key& key) const override final;
  };


  /**
   * @class HashFuncLargeCastKey
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as Size and
   * whose size is precisely twice that of Size.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template < typename Key >
  class HashFuncLargeCastKey: public HashFuncBase< Key > {
    public:
    /**
     * @brief Class constructor.
     */
    HashFuncLargeCastKey();

    /**
     * @brief Cast key to the expected type.
     * @param key The key to cast.
     * @return Returns the cast key to the expected type.
     */
    static Size castToSize(const Key& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const Key& key) const override final;
  };


  /**
   * @class HashFuncCastKey
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Generic hash functions for keys castable as Size whose
   * size is either smaller than Size, or equal to that of one or two Size
   *
   * This class uses metaprogramming to select automatically which of classes
   * HashFuncSmallCastKey, HashFuncMediumCastKey or HashFuncLargeCastKey
   * you should inherit from.
   * @ingroup hashfunctions_group
   * @tparam Key The type hashed by this hash function.
   */
  template < typename Key >
  struct HashFuncCastKey {
    /// The type used by this class.
    using type = typename std::conditional<
       sizeof(Key) <= sizeof(Size) && std::is_integral< Key >::value,
       HashFuncSmallKey< Key >,
       typename std::conditional<
          sizeof(Key) < sizeof(Size),
          HashFuncSmallCastKey< Key >,
          typename std::conditional< sizeof(Key) == sizeof(Size),
                                     HashFuncMediumCastKey< Key >,
                                     typename std::conditional< sizeof(Key) == 2 * sizeof(Size),
                                                                HashFuncLargeCastKey< Key >,
                                                                void >::type >::type >::type >::
       type;
  };


  // ===========================================================================
  // ===   CLASSES FOR NOT DEFINING SEVERAL TIMES THE SAME HASH FUNCTIONS    ===
  // ===========================================================================

  // a dummy hash type
  template < typename Key >
  class dummyHash {};

  // the general type of the recursive type to select the appropriate hash function
  template < typename... >
  struct HashFuncConditionalType;

  // base of the recursive type to select the appropriate hash function
  template < typename Key >
  struct HashFuncConditionalType< Key > {
    using type = Key;
  };

  // base of the recursive type to select the appropriate hash function
  template < typename KEY_TYPE, typename TYPE >
  struct HashFuncConditionalType< KEY_TYPE, TYPE > {
    using type = typename std::
       conditional< std::is_same< KEY_TYPE, TYPE >::value, dummyHash< KEY_TYPE >, KEY_TYPE >::type;
  };

  /**
   * @class HashFuncConditionalType
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief This class enables to safely define hash functions for types
   * that may or may not already has defined hash functions
   * @ingroup hashfunctions_group
   *
   * There are types that are defined differently depending on the architecture
   * or the compiler you use. This is the case, for instance, of std::size_t
   * which is defined as an unsigned long by gcc and clang on 64 bits
   * architectures, but is defined as an unsigned int in 32 bits architectures by
   * theses compilers, and it is defined neither as an unsigned long nor as
   * an unsigned int by Visual Studio 15 MVSC on 64 bits architectures. To
   * enable to define the hash function of std::size_t appropriately in all these
   * cases, instead of defining directly a HasHunc of <std::size_t>, it is
   * sufficient to define a HashFunc of
   * <HashFuncConditionalType<std::size_t,unsigned int,unsigned long>::type>.
   * The latter will actually define a HasHunc of <std::size_t> if size_t
   * corresponds neither to an unsigned int nor to an unsigned long, else it
   * will not define the HasHunc of <std::size_t> (which would redefine an
   * already defined HashFunc, hence resulting in a compilation failure). */
  template < typename KEY_TYPE, typename FIRST_TYPE, typename... OTHER_TYPES >
  struct HashFuncConditionalType< KEY_TYPE, FIRST_TYPE, OTHER_TYPES... > {
    using type = typename std::conditional<
       std::is_same< KEY_TYPE, FIRST_TYPE >::value,
       dummyHash< KEY_TYPE >,
       typename HashFuncConditionalType< KEY_TYPE, OTHER_TYPES... >::type >::type;
  };


  // ===========================================================================
  // ===                  HASH FUNCTIONS FOR PAIRS OF KEYS                   ===
  // ===========================================================================

  /**
   * @class HashFunc
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief This class should be useless as only its specializations should be
   * used.
   *
   * However it prevents to create hash functions on key types that are not yet
   * supported.
   *
   * @ingroup hashfunctions_group
   */
  template < typename key >
  class HashFunc {};

  /**
   * @class HashFunc
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Generic hash functions for pairs of keys.
   * @ingroup hashfunctions_group
   * @tparam Key1 The type hashed of the first element in the pair.
   * @tparam Key2 The type hashed of the second element in the pair.
   */
  template < typename Key1, typename Key2 >
  class HashFunc< std::pair< Key1, Key2 > >: public HashFuncBase< std::pair< Key1, Key2 > > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const std::pair< Key1, Key2 >& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const std::pair< Key1, Key2 >& key) const override final;
  };


  // ===========================================================================
  // ===                      WIDELY USED HASH FUNCTIONS                     ===
  // ===========================================================================

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for booleans.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< bool >: public HashFuncSmallKey< bool > {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< int >: public HashFuncSmallKey< int > {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for unsigned integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< unsigned int >: public HashFuncSmallKey< unsigned int > {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for long integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< long >: public HashFuncSmallKey< long > {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for unsigned long integers.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< unsigned long >: public HashFuncSmallKey< unsigned long > {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for std::size_t.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc<
     typename HashFuncConditionalType< std::size_t, unsigned long, unsigned int, long, int >::
        type >: public HashFuncCastKey< std::size_t >::type {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for floats.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< float >: public HashFuncCastKey< float >::type {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for doubles.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< double >: public HashFuncCastKey< double >::type {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for pointers.
   * @tparam The type for which the pointer is used to compute a hash.
   */
  template < typename Type >
  class HashFunc< Type* >: public HashFuncCastKey< Type* >::type {};

  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for strings.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< std::string >: public HashFuncBase< std::string > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const std::string& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const std::string& key) const override final;
  };


  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for vectors of gum::Idx.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< std::vector< Idx > >: public HashFuncBase< std::vector< Idx > > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const std::vector< Idx >& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const std::vector< Idx >& key) const override final;
  };


  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for gum::Debug.
   * @ingroup hashfunctions_group
   */
  template <>
  class HashFunc< Debug >: public HashFuncBase< Debug > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Debug& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const Debug& key) const override final;

    template < typename OTHER_KEY >
    friend class HashFunc;
  };


  /**
   * @headerfile hashFunc.h <agrum/tools/core/hashFunc.h>
   * @brief Hash function for RefPtr.
   * @ingroup hashfunctions_group
   * @tparam Type The type of the RefPtr.
   */
  template < typename Type >
  class HashFunc< RefPtr< Type > >: public HashFuncBase< RefPtr< Type > > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const RefPtr< Type >& key);

    /**
     * @brief Computes the hashed value of a key.
     * @param key The key to compute the hashed value.
     * @return Returns the hashed value of a key.
     */
    virtual Size operator()(const RefPtr< Type >& key) const override final;
  };

} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/hashFunc_inl.h>
#endif /* GUM_NO_INLINE */

/// always include the implementation of the templates
#include <agrum/tools/core/hashFunc_tpl.h>

#endif /* GUM_HASHFUNC_H */
