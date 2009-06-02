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
 * @brief Class providing generic double hash tables
 *
 * @author Jean-Philippe DUBUS
 */

#ifndef GUM_BIJECTION_H
#define GUM_BIJECTION_H

#include <agrum/core/hashTable.h>

namespace gum {

  /**
   * Iterators for bijection
   */
  template <typename FirstType, typename SecondType>
  class BijectionIterator {
  public:
    /**
     * Constructor
     */
    BijectionIterator(typename HashTable<FirstType, SecondType>::iterator& iter);

    /**
     * Destructor
     */
    virtual ~BijectionIterator();

    /**
     * Copy constructor
     */
    BijectionIterator(const BijectionIterator<FirstType, SecondType>& toCopy);

    /**
     * Copy operator
     */
    BijectionIterator<FirstType, SecondType>&
    operator=(const BijectionIterator<FirstType, SecondType>& toCopy);

    /**
     * return the first element of the current association
     */
    const FirstType& first() const;

    /**
     * return the second element of the current association
     */
    const SecondType& second() const;

    /**
     * Go to the next association (if exists)
     */
    BijectionIterator<FirstType, SecondType>& operator++();

    /**
     * Go to the previous association (if exists)
     */
    BijectionIterator<FirstType, SecondType>& operator--();

    /**
     * Comparaison iterators
     */
    bool operator!=(const BijectionIterator<FirstType, SecondType>& toCompare) const;
    bool operator==(const BijectionIterator<FirstType, SecondType>& toCompare) const;

  private:

    typename HashTable<FirstType, SecondType>::iterator __iter;


  };


  /**
   * This class is designed for modelling bijection between two sets, the idea is following :
   * - we want to create a bijection relation between the type FirstType and the type SecondType
   * - for x in FirstType, it exists only one y in SecondType associated to x
   * - for y in SecondType, it exists only one x in FirstType associated to y
   * - the user give all the (x, y) association and can search efficiently the values associated
   */
  template <typename FirstType, typename SecondType>
  class Bijection {
  public:
    /**
     * Default constructor
     */
    Bijection(Size size = GUM_HASHTABLE_DEFAULT_SIZE,
	      bool resize = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY);

    /**
     * destructor
     */
    virtual ~Bijection();

    /**
     * Copy constructor
     *
     * @param toCopy Bijection to copy
     */
    Bijection(const Bijection<FirstType, SecondType>& toCopy);

    /**
     * Copy operator
     *
     * @param toCopy Bijection to copy
     */
    Bijection<FirstType, SecondType>& operator=(const Bijection<FirstType, SecondType>& toCopy);



    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    typedef BijectionIterator<FirstType, SecondType> iterator;

    // ============================================================================
    /// returns the iterator at the beginning of the hashtable
    // ============================================================================
    iterator begin() const;

    // ============================================================================
    /// returns the iterator to the end of the hashtable
    // ============================================================================
    iterator end() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns a reference on the value the first element of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    FirstType& first(const SecondType& second);
    const FirstType& first(const SecondType& second) const;

    /**
     * Same method than first, but if the value is not found, a default value is returned
     */
    FirstType& firstWithDefault(const SecondType& second, const FirstType& val);
    const FirstType& firstWithDefault(const SecondType& second, const FirstType& val) const;
 
    // ============================================================================
    /// returns a reference on the value the second element of which is passed in argument
    /** In case of multiple identical keys in the hash table, the first value
     * encountered is returned. The method runs in constant time.
     * @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    SecondType& second(const FirstType& first);
    const SecondType& second(const FirstType& first) const;

    /**
     * Same method than second, but if the value is not found, a default value is returned
     */
    SecondType& secondWithDefault(const FirstType& first, const SecondType& val);
    const SecondType& secondWithDefault(const FirstType& first, const SecondType& val) const;

    /**
     * Test if the first value is existing in the bijection
     */
    bool existsFirst(const FirstType& first) const;
    
    /**
     * Test if the second value is existing in the bijection
     */
    bool existsSecond(const SecondType& second) const;

    /**
     * insert a new association in the bijection
     */
    void insert(const FirstType& first, const SecondType& second);

    /**
     * remove all the associations in the bijection
     */
    void clear();

    /**
     * return true if the bijection doesn't contains any relation
     */
    bool empty() const;

    /**
     * return the number of association in the bijection
     */
    Size size() const;

    /**
     * erase an association containing the given first element
     */
    void eraseFirst(const FirstType& first);

    /**
     * erase an association containing the given second element
     */
    void eraseSecond(const SecondType& second);

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

    /// @}

  private:

    HashTable<FirstType, SecondType> __firstToSecond;

    HashTable<SecondType, FirstType> __secondToFirst;

  };

  #include "bijection.tcc"

}

#endif
