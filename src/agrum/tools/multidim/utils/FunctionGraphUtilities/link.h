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
 * @brief Headers of the Link and LinkedList classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_LINK_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_LINK_H

// ============================================================================
#include <agrum/agrum.h>
// ============================================================================
#include <agrum/tools/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================

namespace gum {

  /**
   * @class Link
   * @headerfile link.h <agrum/tools/multidim/FunctionGraphUtilities/link.h>
   * @ingroup multidim_group
   *
   * @brief Link of a chain list allocated using the SmallObjectAllocator
   *
   * @tparam T The type stored by the chain list.
   */
  template < typename T >
  class Link {
    public:
    // ============================================================================
    /// @name Constructors and Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Constructor
     */
    explicit Link(const T& elem);

    /**
     * @brief Constructor that insert link before the given link
     */
    Link(const T& elem, Link< T >* nextLink);

    /**
     * @brief Destructor
     */
    ~Link();

    /**
     * @brief Operator new overload to use the SmallObjectAllocator
     */
    void* operator new(size_t s);

    /**
     * @brief Operator delete overload to use the SmallObjectAllocator
     */
    void operator delete(void* p);

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
    const Link< T >* nextLink() const;

    /**
     * @brief Returns next link.
     */
    Link< T >* nextLink();

    /**
     * @brief Sets the next link.
     */
    void setNextLink(Link< T >* newLink);

    /// @}

    private:
    /// The element embedded in this link
    T _element_;

    /// The next link in the list
    Link< T >* _nextLink_;
  };

  /**
   * @class LinkedList
   * @headerfile link.h <agrum/tools/multidim/FunctionGraphUtilities/link.h>
   * @ingroup multidim_group
   *
   * @brief Chain list allocated using the SmallObjectAllocator
   *
   * @tparam T The type stored by the chain list.
   */
  template < typename T >
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
    void* operator new(size_t s);

    // ============================================================================
    /// Operator delete overload to use the SmallObjectAllocator
    // ============================================================================
    void operator delete(void* p);

    /// @}
    // ============================================================================
    /// @name Misceleanous methods
    // ============================================================================
    /// @{

    /**
     * @brief Returns the first link in the chained list.
     */
    const Link< T >* list() const;

    /**
     * @brief Returns the first link in the chained list.
     */
    Link< T >* list();

    /**
     * @brief Clears the list.
     */
    void clear();

    /**
     * @brief Adds a link.
     */
    void addLink(const T& elem);

    /**
     * @brief Removes a element from the list.
     */
    void searchAndRemoveLink(const T& elem);

    /// @}

    private:
    /// The first link of our list
    Link< T >* _firstLink_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class Link< Idx >;
#endif

}   // End of namespace gum

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/link_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_LINK_H */
