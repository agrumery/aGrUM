
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
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_LINK_H
#define GUM_MULTI_DIM_DECISION_GRAPH_LINK_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================

namespace gum {

  /**
   * @class Link link.h <agrum/multidim/decisionGraphUtilities/link.h>
   *
   * @brief Link of a chain list allocated using the SmallObjectAllocator
   *
   * @ingroup multidim_group
   */
  template<typename T>
  class Link {

    public :

      // #############################################################################################
      /// Constructors and Destructors
      // #############################################################################################
      /// @{

        // ============================================================================
        /// Constructor
        // ============================================================================
        Link(const T& elem);

        // ============================================================================
        /// Constructor that insert link before the given link
        // ============================================================================
        Link(const T& elem, Link<T> *nextLink );

        // ============================================================================
        /// Destructor
        // ============================================================================
        ~Link();

        // ============================================================================
        /// Operator new overload to use the SmallObjectAllocator
        // ============================================================================
        void* operator new(size_t s){ return SmallObjectAllocator::instance().allocate(s);}

        // ============================================================================
        /// Operator delete overload to use the SmallObjectAllocator
        // ============================================================================
        void operator delete(void* p){ SmallObjectAllocator::instance().deallocate(p, sizeof(Link<T>));}

      /// @}

      // #############################################################################################
      /// Misceleanous methods
      // #############################################################################################
      /// @{

        const T& element() const { return __element; }
        T& element() { return __element; }

        const Link<T>* nextLink() const { return __nextLink; }
        void setNextLink( Link<T>* newLink ) { __nextLink = newLink; }
        Link<T>* nextLink() { return __nextLink; }

      /// @}

    private :

      /// The element embedded in this link
      T __element;

      /// The next link in the list
      Link<T>* __nextLink;
  };

  /**
   * @class LinkedList link.h <agrum/multidim/decisionGraphUtilities/link.h>
   *
   * @brief Chain list allocated using the SmallObjectAllocator
   *
   * @ingroup multidim_group
   */
  template<typename T>
  class LinkedList {

    public :

      // #############################################################################################
      /// Constructors and Destructors
      // #############################################################################################
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
        void* operator new(size_t s){ return SmallObjectAllocator::instance().allocate(s);}

        // ============================================================================
        /// Operator delete overload to use the SmallObjectAllocator
        // ============================================================================
        void operator delete(void* p){ SmallObjectAllocator::instance().deallocate(p, sizeof(LinkedList<T>));}

      /// @}

      // #############################################################################################
      /// Misceleanous methods
      // #############################################################################################
      /// @{

        const Link<T>* list() const { return __firstLink; }
        Link<T>* list() { return __firstLink; }

        void clear();

        void addLink( const T& elem );

        void searchAndRemoveLink( const T& elem );

      /// @}

    private :

      /// The first link of our list
      Link<T>* __firstLink;
  };

} // End of namespace gum

#include <agrum/multidim/decisionGraphUtilities/link.tcc>

#endif /* GUM_MULTI_DIM_DECISION_GRAPH_LINK_H */
