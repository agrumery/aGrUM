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
 * @brief Headers of gspan.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_GSPAN_H
#define GUM_GSPAN_H

#include <string>
#include <ostream>
#include <vector>
#include <list>
#include <algorithm>

#include <agrum/core/timer.h>

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/gspan/interfaceGraph.h>
#include <agrum/PRM/gspan/DFSTree.h>

namespace gum {
  namespace prm {

    /**
     * @class GSpan gspan.h <agrum/PRM/gspan.h>
     *
     * @brief This class discovers pattern in a PRM<GUM_SCALAR>'s System<GUM_SCALAR>
     *to speed up structured
     *        inference.
     *
     * This class is not an inference algorithm for PRM<GUM_SCALAR>, however it can
     *be used to
     * speed up structured inference as it will discover repeated patterns including
     * more than one Instance<GUM_SCALAR>.
     *
     * This algorithm proceeds in three main steps represented by the private
     * methods GSpan::__sortNodesAndEdges(), GSpan::__subgraph_mining() and
     * GSpan::__sortPatterns().
     */
    template <typename GUM_SCALAR> class GSpan {

      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /**
       * Default constructor.
       * @param prm The PRM<GUM_SCALAR> used by this class.
       * @param sys The System<GUM_SCALAR> on which this class searches for patterns.
       * @param strategy The search strategy used for pattern mining, the
       *                 default strategy is gspan::FrequenceSearch.
       */
      GSpan(const PRM<GUM_SCALAR> &prm, const System<GUM_SCALAR> &sys,
            gspan::SearchStrategy<GUM_SCALAR> *strategy = 0);

      /// Destructor.
      ~GSpan();

      /// @}
      // ========================================================================
      /// @name Getters and setters.
      // ========================================================================
      /// @{

      /**
       * Returns the maximal depth of the DFSTree used to discover new patterns.
       *
       * @return the maximal depth of the DFSTree used to discover new patterns.
       */
      Size getMaxDFSDepth() const;

      /**
       * Defines the maximal depth of the DFSTree used by this class to discover
       * new patterns.
       *
       * @param depth The new maximal DFSTree depth.
       */
      void setMaxDFSDepth(Size depth);

      /**
       * Returns the DFSTree used to discover new patters.
       * @returns the DFSTree used to discover new patters.
       */
      gspan::DFSTree<GUM_SCALAR> &tree();

      /**
       * Returns the DFSTree used to discover new patters.
       * @returns the DFSTree used to discover new patters.
       */
      const gspan::DFSTree<GUM_SCALAR> &tree() const;

      /**
       * Returns the InterfaceGraph used by this.
       * @returns the InterfaceGraph used by this.
       */
      gspan::InterfaceGraph<GUM_SCALAR> &interfaceGraph();

      /**
       * Returns the InterfaceGraph used by this.
       * @returns the InterfaceGraph used by this.
       */
      const gspan::InterfaceGraph<GUM_SCALAR> &interfaceGraph() const;

      /// @}
      // ========================================================================
      /// @name Pattern discovery methods.
      // ========================================================================
      /// @{

      /**
       * @brief This will methods will discover repeated patterns in the
       *        System<GUM_SCALAR> assigned to this class.
       *
       * The results are saved in a vector of Patterns which can be obtained
       * by calling GSpan::patterns().
       */
      void discoverPatterns();

      /**
       * Returns the Pattern mined by this class in a decreasing order of
       * interest.
       * @returns the Pattern mined by this class in a decreasing order of
       *          interest.
       */
      std::vector<gspan::Pattern *> &patterns();

      /**
       * Returns the Pattern mined by this class in a decreasing order of
       * interest.
       * @returns the Pattern mined by this class in a decreasing order of
       *          interest.
       */
      const std::vector<gspan::Pattern *> &patterns() const;

      /// Code alias.
      typedef Set<Sequence<Instance<GUM_SCALAR> *> *> MatchedInstances;

      /**
       * Returns a mapping between patterns and the sequence of instance in the
       * interface graph matching them.
       * @returns a mapping between patterns and the sequence of instance in the
       *          interface graph matching them.
       */
      MatchedInstances &matches(const gspan::Pattern &p);

      /**
       * Returns a mapping between patterns and the sequence of instance in the
       * interface graph matching them.
       * @returns a mapping between patterns and the sequence of instance in the
       *          interface graph matching them.
       */
      const MatchedInstances &matches(const gspan::Pattern &p) const;

      // /**
      //  * Returns the cumulative sum of all the cliques size created after lifting
      //  * the patterns.
      //  * @returns the cumulative sum of all the cliques size created after lifting
      //  *          the patterns.
      //  */
      // double patterns_cost() const;

      // /**
      //  * Returns the cumulative sum of all the cliques size created after lifting
      //  * the inner attributes.
      //  * @returns the cumulative sum of all the cliques size created after lifting
      //  *          the inner attributes.
      //  */
      // double sve_cost() const;

      ///@}
      private:
      // ========================================================================
      /// @name Private Members.
      // ========================================================================
      /// @{

      /// The interface graph used by this class.
      gspan::InterfaceGraph<GUM_SCALAR> *__graph;

      /// The DFSTree used to discover new patters.
      gspan::DFSTree<GUM_SCALAR> __tree;

      /// The max depth allowed for the DSF tree
      Size __depth_stop;

      /// The vector of discovered patters, in decreasing order of interest.
      std::vector<gspan::Pattern *> __patterns;

      /// The vector of nodes in __graph, in decreasing order of interest.
      std::vector<gspan::LabelData *> __nodes;

      /// The vector of edges in __graph, in decreasing order of interest.
      std::vector<gspan::LabelData *> __edges;

      /// Mapping between labels and their cost.
      HashTable<gspan::LabelData *, unsigned long> __cost;

      /// Mapping between a pattern and the multiset of instances matched
      /// to it.
      HashTable<gspan::Pattern *, MatchedInstances *> __matched_instances;

      /// Contains all instance which belongs to a discovered and used pattern.
      Set<Instance<GUM_SCALAR> *> __chosen;

      /// @}
      // ========================================================================
      /// @name Private Methods
      // ========================================================================
      /// @{

      /// Sort the nodes and edges of __graph.
      void __sortNodesAndEdges();

      /// Discovers new patterns by developing p.
      /// @param graph The interface graph used in this discovery process.
      /// @param p The pattern used as a base for discovery.
      void __subgraph_mining(gspan::InterfaceGraph<GUM_SCALAR> &graph,
                             gspan::Pattern &p);

      /// Returns the cost with respect to an interface size and its frequency.
      /// TODO replace this by a class to enable different cost policies.
      /// @param interface The size of all output nodes of a pattern.
      /// @param frequency The frequency of the pattern in the current interface
      ///        graph.
      /// @return the cost with respect to an interface size and its frequency.
      unsigned long __cost_func(unsigned int interface, unsigned int frequency);

      /// Sort the patterns and compute their respective costs.
      void __sortPatterns();

      // /// Returns the cost of an instance.
      // /// @param i An instance.
      // /// @return the cost of an instance.
      // double __instance_cost(Instance<GUM_SCALAR>* i) const;

      /// Print an iso map. For debug purpose.
      void __printIsoMap(gspan::Pattern &p);

      /// Returns true if e is an eligible root edge.
      /// @param e An EdgeData<GUM_SCALAR>.
      /// @return true if e is an eligible root edge.
      bool __isEdgeEligible(gspan::EdgeData<GUM_SCALAR> *e);

      /// @}

      // ========================================================================
      /// @name Private classes
      // ========================================================================
      /// @{

      /**
       * @class LabelSort gspan.h <agrum/PRM/gspan.h>
       * Private class used to sort LabelData using STL sort algorithms.
       */
      struct LabelSort {
        /**
         * Default constructor.
         * @param my_gspan A pointer over the GSpan using this class.
         */
        LabelSort(GSpan *my_gspan);

        /**
         * Copy constructor.
         * @param source The copied LabelSort.
         */
        LabelSort(const LabelSort &source);

        /**
         * Destructor.
         */
        ~LabelSort();

        /**
         * Returns true if i's cost is lesser than j's.
         * @param i A LabelData.
         * @param j A LabelData.
         * @return true if i's cost is above than j's cost.
         */
        bool operator()(gspan::LabelData *i, gspan::LabelData *j);

        /// A pointer over an instance of the GSpan class using this class.
        GSpan *gspan;
      };

      /**
       * @class PatternSort gspan.h <agrum/PRM/gspan.h>
       * Private class used to sort Pattern using STL sort algorithms.
       */
      struct PatternSort {

        /**
         * Default constructor.
         * @param my_gspan A pointer over the GSpan using this class.
         */
        PatternSort(GSpan *my_gspan);

        /**
         * Copy constructor.
         * @param source The copied PatternSort.
         */
        PatternSort(const PatternSort &source);

        /**
         * Destructor.
         */
        ~PatternSort();

        /**
         * Returns true if i's cost is lesser than j's.
         * @param i A Pattern.
         * @param j A Pattern.
         * @return true if i's cost is above j's cost.
         */
        bool operator()(gspan::Pattern *i, gspan::Pattern *j);

        /// A pointer over an instance of the GSpan class using this class.
        GSpan *gspan;
      };

      /// @}
    };

    extern template class GSpan<double>;
  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/gspan.tcc>

#endif /* GUM_GSPAN_H */
