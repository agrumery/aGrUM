/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
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
 * @brief Detect barren nodes for inference in Bayesian networks
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_BARREN_NODES_FINDER_H
#define GUM_BARREN_NODES_FINDER_H


#include <agrum/config.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/BN/BayesNet.h>


namespace gum {

  
  /**
   * @class BarrenNodesFinder
   * @brief Detect barren nodes for inference in Bayesian networks
   */
  class BarrenNodesFinder {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    BarrenNodesFinder ( const DAG& dag );

    /// copy constructor
    BarrenNodesFinder ( const BarrenNodesFinder& from );

    /// move constructor
    BarrenNodesFinder ( BarrenNodesFinder&& from );

    /// destructor
    ~BarrenNodesFinder ();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    BarrenNodesFinder& operator= ( const BarrenNodesFinder& from );

    /// move operator
    BarrenNodesFinder& operator= ( BarrenNodesFinder&& from );

    /// @}
    

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new DAG
    void setDAG ( const DAG& new_dag );

    /// sets the observed nodes in the DAG
    void setEvidence ( const NodeSet& observed_nodes );

    /// returns the set of barren nodes in the messages sent in a junction tree
    ArcProperty<NodeSet> barrenNodes ( const CliqueGraph& junction_tree );

    /// returns the set of barren potentials in messages sent in a junction tree
    template <typename GUM_SCALAR>
    ArcProperty<Set<const Potential<GUM_SCALAR>*> >
    barrenPotentials ( const CliqueGraph& junction_tree,
                       const IBayesNet<GUM_SCALAR>& bn );

    /// @}

  private:

    /// the DAG on which we compute the barren nodes
    const DAG *__dag;

    /// the set of observed nodes
    NodeSet __observed_nodes;
    
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/BN/inference/barrenNodesFinder.inl>
#endif // GUM_NO_INLINE


#include <agrum/BN/inference/barrenNodesFinder.tcc>


#endif /* GUM_BARREN_NODES_FINDER_H */
