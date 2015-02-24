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
 * @brief Headers of the Concrete Leaf class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_CONCRETE_LEAF_H
#define GUM_CONCRETE_LEAF_H
// =========================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
#include <agrum/core/multiPriorityQueue.h>
// =========================================================================
#include <agrum/graphs/graphElements.h>
// =========================================================================
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/nodeDatabase.h>
// =========================================================================

namespace gum {

  /**
   * @class ConcreteLeaf concreteLeaf.h <agrum/FMDP/learning/datastructure/leaves/concreteLeaf.h>
   * @brief A concrete leaf
   * @ingroup fmdp_group
   *
   */


  template<TESTNAME AttributeSelection, bool isScalar>
  class ConcreteLeaf : public AbstractLeaf {

    typedef typename ValueSelect<isScalar, double, long unsigned int>::type ValueType;

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        ConcreteLeaf ( NodeId leafId, NodeDatabase<AttributeSelection, isScalar>* n1, const Sequence<ValueType>* valueDomain )
          : AbstractLeaf(leafId),
            __n1(n1),
            __valueDomain(valueDomain) {
          GUM_CONSTRUCTOR(ConcreteLeaf)
        }

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~ConcreteLeaf(){
          GUM_DESTRUCTOR(ConcreteLeaf)
        }

      /// @}

        // ###################################################################
        /// Gaves the leaf effectif for given modality
        // ###################################################################
        virtual Idx effectif(Idx moda) const { return __n1->effectif(__valueDomain->atPos(moda)); }
        virtual Idx total() const { return __n1->nbObservation(); }

        Idx nbModa() { return __valueDomain->size(); }

      private :

        NodeDatabase<AttributeSelection, isScalar>* __n1;
        const Sequence<ValueType>* __valueDomain;
  };


} /* namespace gum */


#endif // GUM_CONCRETE_LEAF_H
