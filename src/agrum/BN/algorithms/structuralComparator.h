/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
 * @brief A basic pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_STRUCTURAL_COMPARATOR_H
#define GUM_LEARNING_STRUCTURAL_COMPARATOR_H

#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/BN/BayesNet.h>


namespace gum {

    /** @class StructuralComparator
     * @brief A class to compare the structure of graphs
     *
     * @ingroup learning_group
     */
    class StructuralComparator{

      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralComparator();

      /// destructor
      ~StructuralComparator();

      /// @}

      // ##########################################################################
      /// @name Accessors
      // ##########################################################################
      /// @{
      void compare( const DiGraph& ref, const DiGraph& test );
      void compare( const UndiGraph& ref, const UndiGraph& test );
      void compare( const MixedGraph& ref, const MixedGraph& test );
      template <typename GS1, typename GS2>
      void compare( const BayesNet<GS1>& ref, const BayesNet<GS2>& test );
      template <typename GUM_SCALAR>
      void compare( const BayesNet<GUM_SCALAR>& ref, const MixedGraph& test );
      template <typename GUM_SCALAR>
      void compare( const MixedGraph& ref, const BayesNet<GUM_SCALAR>& test );

      double precision_skeleton() const;
      double recall_skeleton() const;
      double f_score_skeleton() const;

      double precision() const;
      double recall() const;
      double f_score() const;
      /// @}

      protected:

      private:
      ///Confusion matrix
      /*   __________________________________________________
       *  |   \predicted |           |           |           |
       *  |true\         |    -->    |    ---    |     X     |
       *  |--------------|-----------|-----------|-----------|
       *  |      -->     | true arc  |wrong edge |wrong none |
       *  |      		 | mis. arc  |      arc  |      arc  |
       *  |--------------|-----------|-----------|-----------|
       *  |      ---     |wrong arc  | true edge |wrong none |
       *  |      		 |      edge |           |      edge |
       *  |--------------|-----------|-----------|-----------|
       *  |       X      |wrong arc  |wrong edge | true none |
       *  |      		 |      none |      none |           |
       *  |______________|___________|___________|___________|
       *
       */
      double __true_edge, __true_arc, __true_none;
      double __wrong_edge_arc, __wrong_edge_none;
      double __wrong_arc_edge, __wrong_arc_none, __misoriented_arc;
      double __wrong_none_edge, __wrong_none_arc;
      //double __precision, __recall, __f_score;
    };

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/algorithms/structuralComparator_tpl.h>

#endif /* GUM_LEARNING_STRUCTURAL_COMPARATOR_H */
