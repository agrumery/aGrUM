/****************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                                *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/
/**
* @file
* @brief Class used to manipulate o4DGContext in Decision Diagram Operations
*
* @author Pierre-Henri WUILLEMIN & Jean-Christophe Magnan
*/
// =======================================================
#ifndef O4DGCONTEXT_H
#define O4DGCONTEXT_H
// =======================================================
#include <math.h>
// =======================================================
#include <agrum/config.h>
// =======================================================
#include <agrum/core/sequence.h>
// =======================================================
#include <agrum/graphs/graphElements.h>
// =======================================================
#include <agrum/variables/discreteVariable.h>
// =======================================================

namespace gum {

  /**
   * @class O4DGContext o4DGContext.h <agrum/multidim/patterns/o4DGContext.h>
   * @brief Class used to manipulate context in Decision Diagram Operations
   * @ingroup multidim_group
   *
   */

  class O4DGContext {

    public:

      /// Table giving for each node of second diagram, the list of variables requiring instantiation before
      HashTable< NodeId, Set< const DiscreteVariable* >* >* retrogradeVarTable;

      // ===========================================================================
      /// @name Constructors, Destructors.
      // ===========================================================================
      /// @{

      /**
      * Default constructor.
      */
      O4DGContext();

      /**
      * Default destructor.
      */
      ~O4DGContext();

      /// @}

      // ===========================================================================
      /// @name Diagrams current nodes setters & getters.
      // ===========================================================================
      /// @{

      /// Set DG1 diagram current explored Node
      void setDG1Node( gum::NodeId exploredNode );

      /// Get DG1 diagram current explored Node
      gum::NodeId  getDG1Node() const { return __DG1ExploredNode; }

      /// Set DG2 diagram current explored Node
      void setDG2Node( gum::Idx exploredNode );

      /// Get DG2 diagram current explored Node
      gum::NodeId  getDG2Node() const { return __DG2ExploredNode; }

      // ===========================================================================
      /// @name Variables modalities handlers
      // ===========================================================================
      /// @{

      /// Returns true if variable is retrograde
      bool isRetrogradeVar( const gum::DiscreteVariable*  var ) const { return __varSeq.exists( var ); }

      /// Inserts a new retrograde variable
      void addRetrogradeVar( const gum::DiscreteVariable*  var );

      /// Changes given variable modality
      void chgVarModality( const gum::DiscreteVariable* var, gum::Idx newModality );

      /// Returns true if variable is retrograde
      gum::Idx variableModality( const gum::DiscreteVariable* var ) const { return __retrogradeVarInstantiation[ __varSeq.pos( var ) ]; }

      /// Returns current o4DGContext key
      const double contextKey() const;

      /// @}

      /// For displaying current o4DGContext
      std::string toString() const;

    private:

      /// DG1 Diagram current explored node
      gum::NodeId __DG1ExploredNode;
      double __DG1PrimeLog;

      /// DG2 Diagram current explored node
      gum::NodeId __DG2ExploredNode;
      double __DG2PrimeLog;

      /// Variable sequence for faster indexation
      gum::Sequence< const gum::DiscreteVariable* > __varSeq;

      /// Vector containing for each retrograde variable its current modality (starting from 0 meaning no instantiation done)
      std::vector< gum::Idx > __retrogradeVarInstantiation;

      /// vector containing for each variable its associated log2 prime number.
      std::vector< double > __var2PrimeLog;

//      /// Based on Goedl's factorization on prime number, a unique key matching current o4DGContext
//      double __contextKey;

      /// Table containing the log2 of prime numbers
      static const double __logPrime[];
      static const gum::Idx __nbLogPrime;
  };
}

//========================================================================
#ifndef GUM_NO_INLINE
#include <agrum/multidim/patterns/DGUtility/o4DGContext.inl>
#endif /* GUM_NO_INLINE */
//========================================================================

#endif // O4DGCONTEXT_H
