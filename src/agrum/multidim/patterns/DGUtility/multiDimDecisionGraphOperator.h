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
* @brief Class used to compute the operation between two decision diagrams
*
* @author Jean-Christophe Magnan
*/

// =======================================================
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_OPERATOR_H
#define GUM_MULTI_DIM_DECISION_GRAPH_OPERATOR_H
// =======================================================
#include <tuple>
// =======================================================
#include <agrum/core/bijection.h>
// =======================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/patterns/DGUtility/o4DGContext.h>
// =======================================================

namespace gum {

    template <typename GUM_SCALAR, template <typename> class FUNCTOR >
    class MultiDimDecisionGraphOperator
    {
        public:
            MultiDimDecisionGraphOperator( const MultiDimDecisionGraph<GUM_SCALAR>* DG1, const MultiDimDecisionGraph<GUM_SCALAR>* DG2 );
            ~MultiDimDecisionGraphOperator();

            MultiDimDecisionGraph<GUM_SCALAR> *compute();


        private :

            void __establishVarOrder();
            Idx __distance(const MultiDimDecisionGraph<GUM_SCALAR> *, const DiscreteVariable*, const DiscreteVariable*);
            void __findRetrogradeVariables( const MultiDimDecisionGraph<GUM_SCALAR>* dg, HashTable<NodeId, short int*>& dgInstNeed);

            NodeId __compute(O4DGContext & currentSituation, Idx lastInstVarPos , std::string tab);
            NodeId __nodeRedundancyCheck( const DiscreteVariable* var, NodeId* sonsIds );

            const MultiDimDecisionGraph<GUM_SCALAR>* __DG1;
            const MultiDimDecisionGraph<GUM_SCALAR>* __DG2;
            MultiDimDecisionGraph<GUM_SCALAR>* __rd;
            Idx __nbVar;
            short int* __default;
            short int* __instNeeded;

            const FUNCTOR<GUM_SCALAR> __function;

            HashTable<double, NodeId> __explorationTable;

            HashTable<NodeId, short int*> __DG1InstantiationNeeded;
            HashTable<NodeId, short int*> __DG2InstantiationNeeded;


    };

} // namespace gum

#include <agrum/multidim/patterns/DGUtility/multiDimDecisionGraphOperator.tcc>

#endif // GUM_MULTI_DIM_DECISION_GRAPH_OPERATOR_H
