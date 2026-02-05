/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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
#include <agrum/base/core/smallobjectallocator/smallObjectAllocator.h>

// ============================================================================

namespace gum {

  /**
   * @class Link
   * @headerfile link.h <agrum/base/multidim/FunctionGraphUtilities/link.h>
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
   * @headerfile link.h <agrum/base/multidim/FunctionGraphUtilities/link.h>
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

#include <agrum/base/multidim/utils/FunctionGraphUtilities/link_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_LINK_H */
