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
 * @brief Class representing Bayesian networks
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 *
 */
#ifndef GUM_BAYES_NET_FRAGMENT_H
#define GUM_BAYES_NET_FRAGMENT_H

#include <agrum/config.h>

namespace gum {
  /**
   * @class BayesNetFragment BayesNetFragment.h <agrum/BN/BayesNetFragment.h>
   * @brief portion of a BN identified by the list of nodes inserted and an BayesNet source.
   *
   * @author Pierre-Henri WUILLEMIN
   *
   * This class is a decorator of a BayesNet implementing the IBayesNet interface.
   * CPTs can be shared with the BN or can be specific to the Fragment if different.
   */

  template<typename GUM_SCALAR>
  class BayesNetFragment : public IBayesNet<GUM_SCALAR> {
    private:
      const BayesNet<GUM_SCALAR>& __bn;
      
      /// Mapping between the variable's id and their CPT specific to this Fragment.
      //Property< Potential< GUM_SCALAR >* >::onNodes __probaMap;
      HashTable<NodeId, Potential<GUM_SCALAR>* > __probaMap;
      
    public:

    public:
      // ===========================================================================
      /// @name Constructors / Destructors
      // ===========================================================================
      /// @{
      /// @}
      
      
      
    public:
      using IBayesNet<GUM_SCALAR>::dag;
      using IBayesNet<GUM_SCALAR>::size;
      using IBayesNet<GUM_SCALAR>::dim;
      
      using IBayesNet<GUM_SCALAR>::jointProbability;
      using IBayesNet<GUM_SCALAR>::log2JointProbability;
      
      using IBayesNet<GUM_SCALAR>::toDot;
      using IBayesNet<GUM_SCALAR>::toString;

      using IBayesNet<GUM_SCALAR>::property;
      using IBayesNet<GUM_SCALAR>::setProperty;
      using IBayesNet<GUM_SCALAR>::nbrArcs;
      using IBayesNet<GUM_SCALAR>::empty;
      using IBayesNet<GUM_SCALAR>::completeInstantiation;      
      using IBayesNet<GUM_SCALAR>::endNodes;
      using IBayesNet<GUM_SCALAR>::beginNodes;
      using IBayesNet<GUM_SCALAR>::endArcs;
      using IBayesNet<GUM_SCALAR>::beginArcs;
      using IBayesNet<GUM_SCALAR>::moralGraph;
      using IBayesNet<GUM_SCALAR>::topologicalOrder;
      using IBayesNet<GUM_SCALAR>::log10DomainSize;
  };

}// namespace gum

#endif // GUM_BAYES_NET_FRAGMENT_H
