// /***************************************************************************
//  *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
//  *   {prenom.nom}_at_lip6.fr                                               *
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   This program is distributed in the hope that it will be useful,       *
//  *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
//  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
//  *   GNU General Public License for more details.                          *
//  *                                                                         *
//  *   You should have received a copy of the GNU General Public License     *
//  *   along with this program; if not, write to the                         *
//  *   Free Software Foundation, Inc.,                                       *
//  *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
//  ***************************************************************************/
// /**
//  * @file
//  * @brief Headers of the PMDDOperatorStrategy planer class.
//  *
//  * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
//  */
//
// // =========================================================================
// #ifndef GUM_PMDDOperatorStrategy_H
// #define GUM_PMDDOperatorStrategy_H
// // =========================================================================
// /*#include <agrum/core/argMaxSet.h>
// #include <agrum/core/functors.h>
// #include <agrum/core/inline.h>
// #include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// // =========================================================================
// #include <agrum/multidim/multiDimFunctionGraph.h>
// #include
// <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>*/
// // =========================================================================
// #include <agrum/FMDP/fmdp.h>
// // =========================================================================
//
// namespace gum {
//
//
//   /**
//    * @class PMDDOperatorStrategy pspumdd.h <agrum/FMDP/planning/pspumdd.h>
//    * @brief A class to find optimal policy for a given FMDP.
//    * @ingroup fmdp_group
//    *
//    * Perform a SPUDD planning on given in parameter factored markov decision
//    * process
//    * Uses OpenMP to fully exploit modern processor capacity
//    *
//    */
//   template <typename GUM_SCALAR>
//   class PMDDOperatorStrategy : public MDDOperatorStrategy<GUM_SCALAR> {
//
//     public:
//     //
//     ==========================================================================
//     /// @name Constructor & destructor.
//     //
//     ==========================================================================
//     /// @{
//
//     /**
//      * Default constructor
//      */
//     PMDDOperatorStrategy( FMDP<GUM_SCALAR>* fmdp,
//                           GUM_SCALAR epsilon = 0.00001 );
//
//     /**
//      * Default destructor
//      */
//     ~PMDDOperatorStrategy();
//
//     /// @}
//
//     //
//     ==========================================================================
//     /// @name Planning Methods
//     //
//     ==========================================================================
//     /// @{
//
//     /**
//      * Initializes data structure needed for making the planning
//      * @warning No calling this methods before starting the first
//      makePlaninng
//      * will surely and definitely result in a crash
//      */
//     void initialize();
//
//     /// @}
//
//
//     protected:
//     /// Performs a single step of value iteration
//     MultiDimFunctionGraph<GUM_SCALAR>* _valueIteration();
//
//     MultiDimFunctionGraph<GUM_SCALAR>*
//     _evalQaction( const MultiDimFunctionGraph<GUM_SCALAR>*, Idx );
//
//     private:
//     /// A map to ensure all FMDP actions are correctly iterrated over during
//     /// value iteration
//     Bijection<Idx, Idx> __actionCpt2actionId;
//   };
//

//   extern template class PMDDOperatorStrategy<float>;
//   extern template class PMDDOperatorStrategy<double>;

// } /* namespace gum */
//
// #include <agrum/FMDP/planning/pspumdd.tcc>
//
// #endif  // GUM_PMDDOperatorStrategy_H
