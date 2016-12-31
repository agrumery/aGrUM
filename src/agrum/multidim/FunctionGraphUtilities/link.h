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
 * @brief Headers of the Link and LinkedList classes.
 *
 * @author Jean-Christophe Magnan
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_LINK_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_LINK_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================

namespace gum {

  /**
   * @class Link link.h <agrum/multidim/FunctionGraphUtilities/link.h>
   * @ingroup multidim_group
   *
   * @brief Link of a chain list allocated using the SmallObjectAllocator
   *
   * @tparam T The type stored by the chain list.
   */
  template <typename T>
  class Link {

    public:
    // ============================================================================
    /// @name Constructors and Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Constructor
     */
    Link( const T& elem );

    /**
     * @brief Constructor that insert link before the given link
     */
    Link( const T& elem, Link<T>* nextLink );

    /**
     * @brief Destructor
     */
    ~Link();

    /**
     * @brief Operator new overload to use the SmallObjectAllocator
     */
    void* operator new( size_t s );

    /**
     * @brief Operator delete overload to use the SmallObjectAllocator
     */
    void operator delete( void* p );

    /// @}
    // ============================================================================
    /// @name Getters and setters
    // ============================================================================
    /// @{

    /**
     * @brief Returns the element stored in this link.
     */
    const T& element() const;

    /**
     * @brief Returns the element stored in this link.
     */
    T& element();

    /**
     * @brief Returns next link.
     */
    const Link<T>* nextLink() const;

    /**
     * @brief Returns next link.
     */
    Link<T>* nextLink();

    /**
     * @brief Sets the next link.
     */
    void setNextLink( Link<T>* newLink );

    /// @}

    private:
    /// The element embedded in this link
    T __element;

    /// The next link in the list
    Link<T>* __nextLink;
  };

  /**
   * @class LinkedList link.h <agrum/multidim/FunctionGraphUtilities/link.h>
   * @ingroup multidim_group
   *
   * @brief Chain list allocated using the SmallObjectAllocator
   *
   * @tparam T The type stored by the chain list.
   */
  template <typename T>
  class LinkedList {

    public:
    // ============================================================================
    /// @name Constructors and Destructors
    // ============================================================================
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    LinkedList();

    // ============================================================================
    /// Destructor
    // ============================================================================
    ~LinkedList();

    // ============================================================================
    /// Operator new overload to use the SmallObjectAllocator
    // ============================================================================
    void* operator new( size_t s );

    // ============================================================================
    /// Operator delete overload to use the SmallObjectAllocator
    // ============================================================================
    void operator delete( void* p );

    /// @}
    // ============================================================================
    /// @name Misceleanous methods
    // ============================================================================
    /// @{

    /**
     * @brief Returns the first link in the chained list.
     */
    const Link<T>* list() const;

    /**
     * @brief Returns the first link in the chained list.
     */
    Link<T>* list();

    /**
     * @brief Clears the list.
     */
    void clear();

    /**
     * @brief Adds a link.
     */
    void addLink( const T& elem );

    /**
     * @brief Removes a element from the list.
     */
    void searchAndRemoveLink( const T& elem );

    /// @}

    private:
    /// The first link of our list
    Link<T>* __firstLink;
  };

  extern template class Link<Idx>;

}  // End of namespace gum

#include <agrum/multidim/FunctionGraphUtilities/link_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_LINK_H */
