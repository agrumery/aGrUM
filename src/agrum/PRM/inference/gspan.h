/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of gspan.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_GSPAN_H
#define GUM_GSPAN_H

#include <algorithm>
#include <list>
#include <ostream>
#include <string>
#include <vector>

#include <agrum/tools/core/timer.h>

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/PRM/gspan/DFSTree.h>

namespace gum {
  namespace prm {

    /**
     * @class GSpan
     * @headerfile gspan.h <agrum/PRM/gspan.h>
     *
     * @brief This class discovers pattern in a PRM<GUM_SCALAR>'s
     *PRMSystem<GUM_SCALAR>
     *to speed up structured
     *        inference.
     *
     * This class is not an inference algorithm for PRM<GUM_SCALAR>, however it
     *can
     *be used to
     * speed up structured inference as it will discover repeated patterns
     *including
     * more than one PRMInstance<GUM_SCALAR>.
     *
     * This algorithm proceeds in three main steps represented by the private
     * methods GSpan:: _sortNodesAndEdges_(), GSpan:: _subgraph_mining_() and
     * GSpan:: _sortPatterns_().
     */
    template < typename GUM_SCALAR >
    class GSpan {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /**
       * Default constructor.
       * @param prm The PRM<GUM_SCALAR> used by this class.
       * @param sys The PRMSystem<GUM_SCALAR> on which this class searches for
       * patterns.
       * @param strategy The search strategy used for pattern mining, the
       *                 default strategy is gspan::FrequenceSearch.
       */
      GSpan(const PRM< GUM_SCALAR >&             prm,
            const PRMSystem< GUM_SCALAR >&       sys,
            gspan::SearchStrategy< GUM_SCALAR >* strategy = 0);

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
      gspan::DFSTree< GUM_SCALAR >& tree();

      /**
       * Returns the DFSTree used to discover new patters.
       * @returns the DFSTree used to discover new patters.
       */
      const gspan::DFSTree< GUM_SCALAR >& tree() const;

      /**
       * Returns the InterfaceGraph used by this.
       * @returns the InterfaceGraph used by this.
       */
      gspan::InterfaceGraph< GUM_SCALAR >& interfaceGraph();

      /**
       * Returns the InterfaceGraph used by this.
       * @returns the InterfaceGraph used by this.
       */
      const gspan::InterfaceGraph< GUM_SCALAR >& interfaceGraph() const;

      /// @}
      // ========================================================================
      /// @name Pattern discovery methods.
      // ========================================================================
      /// @{

      /**
       * @brief This will methods will discover repeated patterns in the
       *        PRMSystem<GUM_SCALAR> assigned to this class.
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
      std::vector< gspan::Pattern* >& patterns();

      /**
       * Returns the Pattern mined by this class in a decreasing order of
       * interest.
       * @returns the Pattern mined by this class in a decreasing order of
       *          interest.
       */
      const std::vector< gspan::Pattern* >& patterns() const;

      /// Code alias.
      using MatchedInstances = Set< Sequence< PRMInstance< GUM_SCALAR >* >* >;

      /**
       * Returns a mapping between patterns and the sequence of instance in the
       * interface graph matching them.
       * @returns a mapping between patterns and the sequence of instance in the
       *          interface graph matching them.
       */
      MatchedInstances& matches(const gspan::Pattern& p);

      /**
       * Returns a mapping between patterns and the sequence of instance in the
       * interface graph matching them.
       * @returns a mapping between patterns and the sequence of instance in the
       *          interface graph matching them.
       */
      const MatchedInstances& matches(const gspan::Pattern& p) const;

      // /**
      //  * Returns the cumulative sum of all the cliques size created after
      //  lifting
      //  * the patterns.
      //  * @returns the cumulative sum of all the cliques size created after
      //  lifting
      //  *          the patterns.
      //  */
      // double patterns_cost() const;

      // /**
      //  * Returns the cumulative sum of all the cliques size created after
      //  lifting
      //  * the inner attributes.
      //  * @returns the cumulative sum of all the cliques size created after
      //  lifting
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
      gspan::InterfaceGraph< GUM_SCALAR >* _graph_;

      /// The DFSTree used to discover new patters.
      gspan::DFSTree< GUM_SCALAR > _tree_;

      /// The max depth allowed for the DSF tree
      Size _depth_stop_;

      /// The vector of discovered patters, in decreasing order of interest.
      std::vector< gspan::Pattern* > _patterns_;

      /// The vector of nodes in  _graph_, in decreasing order of interest.
      std::vector< gspan::LabelData* > _nodes_;

      /// The vector of edges in  _graph_, in decreasing order of interest.
      std::vector< gspan::LabelData* > _edges_;

      /// Mapping between labels and their cost.
      HashTable< gspan::LabelData*, Idx > _cost_;

      /// Mapping between a pattern and the multiset of instances matched
      /// to it.
      HashTable< gspan::Pattern*, MatchedInstances* > _matched_instances_;

      /// Contains all instance which belongs to a discovered and used pattern.
      Set< PRMInstance< GUM_SCALAR >* > _chosen_;

      /// @}
      // ========================================================================
      /// @name Private Methods
      // ========================================================================
      /// @{

      /// Sort the nodes and edges of  _graph_.
      void _sortNodesAndEdges_();

      /// Discovers new patterns by developing p.
      /// @param graph The interface graph used in this discovery process.
      /// @param p The pattern used as a base for discovery.
      void _subgraph_mining_(gspan::InterfaceGraph< GUM_SCALAR >& graph, gspan::Pattern& p);

      /// Returns the cost with respect to an interface size and its frequency.
      /// @param interface_size The size of all output nodes of a pattern.
      /// @param frequency The frequency of the pattern in the current interface
      ///        graph.
      /// @return the cost with respect to an interface size and its frequency.
      Size _cost_func_(Size interface_size, Size frequency);

      /// Sort the patterns and compute their respective costs.
      void _sortPatterns_();

      /// Returns true if e is an eligible root edge.
      /// @param e An EdgeData<GUM_SCALAR>.
      /// @return true if e is an eligible root edge.
      bool _isEdgeEligible_(typename gspan::EdgeData< GUM_SCALAR >* e);

      /// @}

      // ========================================================================
      /// @name Private classes
      // ========================================================================
      /// @{

      /**
       * @class LabelSort
       * @headerfile gspan.h <agrum/PRM/gspan.h>
       * Private class used to sort LabelData using STL sort algorithms.
       */
      struct LabelSort {
        /**
         * Default constructor.
         * @param my_gspan A pointer over the GSpan using this class.
         */
        LabelSort(GSpan* my_gspan);

        /**
         * Copy constructor.
         * @param source The copied LabelSort.
         */
        LabelSort(const LabelSort& source);

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
        bool operator()(gspan::LabelData* i, gspan::LabelData* j);

        /// A pointer over an instance of the GSpan class using this class.
        GSpan* gspan;
      };

      /**
       * @class PatternSort
       * @headerfile gspan.h <agrum/PRM/gspan.h>
       * Private class used to sort Pattern using STL sort algorithms.
       */
      struct PatternSort {
        /**
         * Default constructor.
         * @param my_gspan A pointer over the GSpan using this class.
         */
        PatternSort(GSpan* my_gspan);

        /**
         * Copy constructor.
         * @param source The copied PatternSort.
         */
        PatternSort(const PatternSort& source);

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
        bool operator()(gspan::Pattern* i, gspan::Pattern* j);

        /// A pointer over an instance of the GSpan class using this class.
        GSpan* gspan;
      };

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class GSpan< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/gspan_tpl.h>

#endif /* GUM_GSPAN_H */
