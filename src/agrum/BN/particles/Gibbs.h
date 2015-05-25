/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief This file contains gibbs sampling (for BNs) class definitions
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_GIBBS_H
#define GUM_GIBBS_H

#include <vector>

#include <agrum/BN/IBayesNet.h>

namespace gum {

  namespace particle {

    /**
     * @class Gibbs Gibbs.h <agrum/BN/particle/Gibbs.h>
     * @brief class for particle using GIBBS sampling
     * @ingroup bn_group
     *
     */
    template <typename GUM_SCALAR> class Gibbs {

      public:
      /**
       * Default constructor
       */
      Gibbs(const IBayesNet<GUM_SCALAR> &BN);

      /**
       * Destructor.
       */
      virtual ~Gibbs();

      /** Evidence management
      * @{
      */

      /**
          * Insert new evidence in the graph.
          * @warning if an evidence already w.r.t. a given node and a new
          * evidence w.r.t. this node is onserted, the old evidence is removed.
          */
      virtual void
      insertEvidence(const List<const Potential<GUM_SCALAR> *> &pot_list);

      /**
       * Remove a given evidence from the graph.
       */
      virtual void eraseEvidence(const Potential<GUM_SCALAR> *e);

      /**
       * Remove all evidence from the graph.
       */
      virtual void eraseAllEvidence();

      /** @} */

      /// initialize the particle using nodes_array as the list of nodes that could
      /// be drawn.
      /// @warning nodes_array is assumed to be compatible with a topological order
      void initParticle();

      /// compute the next particle
      void nextParticle();

      /// getters and setters
      /// @{
      void setNbrDrawnBySample(Size s);
      Size nbrDrawnBySample();

      const Instantiation &particle();

      const IBayesNet<GUM_SCALAR> &bn();
      ///@}

      private:
      void __setValVar(NodeId id, Idx choice);

      void __drawVar(NodeId id);
      void __GibbsSample(NodeId id);
      void __MonteCarloSample();

      Size __nbr_drawn_by_sample;

      /// nbr of iteration by sampling
      Size __nbr_of_iterations;

      /// the actual values of sample
      Instantiation __particle;

      /// std::vector of ids in the bN to update during sampling
      std::vector<NodeId> __nodes_array;

      /// a table of instantiation for each cpt
      NodeProperty<Instantiation *> __cpt_idx;

      /// a table of children for each node
      NodeProperty<std::vector<NodeId> *> __node_children;

      /// a table of potential for posterior computed in the markov blanket
      NodeProperty<Potential<GUM_SCALAR> *> __sampling_posterior;

      /// a list of all the evidence stored into the graph
      NodeProperty<const Potential<GUM_SCALAR> *> __evidences;

      /// a table of instantiation for direct access on _sampling_nbr
      NodeProperty<Instantiation *> __sampling_idx;

      /// a list of hard evidence (not sampled)
      NodeProperty<Idx> __hard_evidences;

      /// The Bayes net we draw particle on
      const IBayesNet<GUM_SCALAR> &__bayesNet;
    };

    extern template class Gibbs<float>;
    extern template class Gibbs<double>;
  } // namespace particle
} // namespace gum

#include <agrum/BN/particles/Gibbs.tcc>

#endif // GUM_GIBBS_H
