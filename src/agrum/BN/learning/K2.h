/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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
#ifndef GUM_K2_H
#define GUM_K2_H

// DELETE FROM DOC (see agrum.doxy.in)

// TODO Adapt to the new version of gumBayesNetTpl.
//
//
// #include <iostream>
// #include <vector>
//
// #include <agrum/config.h>
//
// #include <agrum/gumId.h>
// #include <agrum/gumBayesNetTpl.h>
// #include <agrum/gumDatabase.h>
// #include <agrum/learning/gumLearningTools.h>
//
//
//
//
//
// /* ============================================================================== */
// /* ============================================================================== */
// /* ===             GENERIC CLASS FOR COMPUTING K2-LIKE ALGORITHMS             === */
// /* ============================================================================== */
// /* ============================================================================== */
// /** @class gumK2AlgoBase
//   * @ingroup bn_group
//   */
// /* ============================================================================== */
// class gumK2AlgoBase {
// public:
//
//   // ##############################################################################
//   /// @name Constructors / Destructors
//   // ##############################################################################
//   /// @{
//
//   ==
//   /// basic constructor
//   ==
//   gumK2AlgoBase (const gumDatabase&, const std::vector<gumId>&);
//
//   ==
//   /// destructor
//   ==
//   virtual ~gumK2AlgoBase ();
//
//   /// @}
//
//
//
//   // ##############################################################################
//   /// @name Accessors / Modifiers
//   // ##############################################################################
//   /// @{
//
//   ==
//   /// returns the graph computed by the K2-like algorithm
//   ==
//   gumBayesNetTplStruct compute ();
//
//   /// @}
//
//
//
//   // ##############################################################################
//   /// @name Operators
//   // ##############################################################################
//   /// @{
//
//   ==
//   /// computes and return a BN resulting from K2-like algorithm
//   ==
//   operator gumBayesNetTplStruct ();
//
//   /// @}
//
//
//
// private:
//   ==
//   /// forbid copy constructor
//   ==
//   gumK2AlgoBase (const gumK2AlgoBase& from);
//
//   ==
//   /// forbid copy operator
//   ==
//   gumK2AlgoBase& operator= (const gumK2AlgoBase& from);
//
//
//
// protected:
//   /// keep track of the database
//   const gumDatabase& database;
//
//   /// the tree structure used to compute the score
//   gumLearnCounting tree;
//
//   /// the order in which the nodes should be parsed
//   std::vector<unsigned int> order;
//
//   /// the row number of the child the parents of which we look for
//   unsigned int child;
//
//   /// the current parent we check the possible dependence with child
//   unsigned int current_parent;
//
//   /// a vector containing the current parents of the child
//   std::vector<bool> parents;
//
//   /// the best score so far computed
//   float best_score;
//
//
//   ==
//   /// sets the (row of the) node that should be on the left of the conditioning mark
//   ==
//   virtual void setChild (unsigned int);
//
//   ==
//   /// perform the necessary updates after the insertion of a new parent
//   ==
//   virtual void updateNewParent (unsigned int) = 0;
//
//   ==
//   /// returns the row number of the next parent, if any, or throws an exception
//   ==
//   unsigned int getNextParent () throw (gumElementNotFound);
//
//   ==
//   /// compute the score of the K2 tree
//   ==
//   virtual float computeScore () throw () = 0;
// };
//
//
//
//
//
//
// /* ============================================================================== */
// /* ============================================================================== */
// /* ===        CLASS FOR COMPUTING A K2-LIKE ALGORITHM WITH A BIC SCORE        === */
// /* ============================================================================== */
// /* ============================================================================== */
// /** @class gumK2AlgoBase */
// /* ============================================================================== */
// class gumK2BICAlgo : private gumK2AlgoBase {
// public:
//   // ##############################################################################
//   /// @name Constructors / Destructors
//   // ##############################################################################
//   /// @{
//
//   ==
//   /// basic constructor
//   ==
//   gumK2BICAlgo (const gumDatabase&, const std::vector<gumId>&);
//
//   ==
//   /// destructor
//   ==
//   ~gumK2BICAlgo ();
//
//   /// @}
//
//
//
//   // ##############################################################################
//   /// @name Accessors / Modifiers
//   // ##############################################################################
//   /// @{
//
//   ==
//   /// returns the graph computed by the K2 like algorithm
//   ==
//   gumBayesNetTplStruct compute ();
//
//   /// @}
//
//
//
//   // ##############################################################################
//   /// @name Operators
//   // ##############################################################################
//   /// @{
//
//   ==
//   /// computes and return a BN resulting from K2-like algorithm
//   ==
//   operator gumBayesNetTplStruct ();
//
//   /// @}
//
//
//
// private:
//   /// vectors used to speed-up the computations of the log
//   std::vector<float> logVect;
//   std::vector<bool>  logComputed;
//
//   /// the dimension penalty
//   float logDim;
//
//   /// the log of the number of cases in the database
//   float logM;
//
//   ==
//   /// compute logarithms using a cache
//   ==
//   float computeLog (int n) throw ();
//
//   ==
//   /// compute the score of the K2 tree
//   ==
//   float computeScore () throw ();
//
//   ==
//   /// sets the (row of the) node that should be on the left of the conditioning mark
//   ==
//   void setChild (unsigned int);
//
//   ==
//   /// perform the necessary updates after the insertion of a new parent
//   ==
//   void updateNewParent (unsigned int);
//
// };
//
//
//
//
//
//
// /* ============================================================================== */
// /* ============================================================================== */
// /* ===                             IMPLEMENTATIONS                            === */
// /* ============================================================================== */
// /* ============================================================================== */
//
// #ifndef GUM_NO_INLINE
// #include "gumK2.inl"
// #endif /* GUM_NO_INLINE */
//
//
#endif /* GUM_K2_H */
