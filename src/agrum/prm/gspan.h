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
 * @author Lionel TORTI
 */
// ============================================================================
#include <string>
#include <ostream>
#include <vector>
#include <list>
#include <algorithm>
// ============================================================================
#include <agrum/core/timer.h>
// ============================================================================
#include <agrum/BN/inference/variableElimination.h>
// ============================================================================
#include <agrum/prm/PRM.h>
#include <agrum/prm/gspan/interfaceGraph.h>
#include <agrum/prm/gspan/DFSTree.h>
// ============================================================================
#ifndef GUM_GSPAN_H
#define GUM_GSPAN_H
// ============================================================================
namespace gum {
namespace prm {

/**
 * @class GSpan gspan.h <agrum/prm/gspan.h>
 *
 * @brief This class discovers pattern in a PRM's System to speed up structured
 *        inference.
 *
 * This class is not an inference algorithm for PRM, however it can be used to
 * speed up structured inference as it will discover repeated patterns including
 * more than one Instance.
 *
 * This algorithm proceeds in three main steps represented by the private
 * methods GSpan::__sortNodesAndEdges(), GSpan::__subgraph_mining() and
 * GSpan::__sortPatterns().
 */
class GSpan {

  public:
  // ========================================================================
  /// @name Constructors & destructor.
  // ========================================================================
    /// @{

    /**
     * Default constructor.
     * @param prm The PRM used by this class.
     * @param sys The System on which this class searches for patterns.
     * @param min_freq The minimal frequency value used by this class.
     * @param depth_stop The maximal depth of the gspan::DFSTree used by
     *                   this class.
     * @param strategy The search strategy used for pattern mining, the
     *                 default strategy is gspan::FrequenceSearch.
     */
    GSpan(const PRM& prm, const System& sys, gspan::SearchStrategy* strategy = 0);

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
    gspan::DFSTree& tree();

    /**
     * Returns the DFSTree used to discover new patters.
     * @returns the DFSTree used to discover new patters.
     */
    const gspan::DFSTree& tree() const;

    /**
     * Returns the InterfaceGraph used by this.
     * @returns the InterfaceGraph used by this.
     */
    gspan::InterfaceGraph& interfaceGraph();

    /**
     * Returns the InterfaceGraph used by this.
     * @returns the InterfaceGraph used by this.
     */
    const gspan::InterfaceGraph& interfaceGraph() const;

    /// @}
  // ========================================================================
  /// @name Pattern discovery methods.
  // ========================================================================
    /// @{

    /**
     * @brief This will methods will discover repeated patterns in the
     *        System assigned to this class.
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
    std::vector<gspan::Pattern*>& patterns();

    /**
     * Returns the Pattern mined by this class in a decreasing order of
     * interest.
     * @returns the Pattern mined by this class in a decreasing order of
     *          interest.
     */
    const std::vector<gspan::Pattern*>& patterns() const;

    /// Code alias.
    typedef Set< Sequence<Instance*>* > MatchedInstances;

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
    gspan::InterfaceGraph* __graph;

    /// The DFSTree used to discover new patters.
    gspan::DFSTree __tree;

    /// The max depth allowed for the DSF tree
    Size __depth_stop;

    /// The vector of discovered patters, in decreasing order of interest.
    std::vector<gspan::Pattern*> __patterns;

    /// The vector of nodes in __graph, in decreasing order of interest.
    std::vector<gspan::LabelData*> __nodes;

    /// The vector of edges in __graph, in decreasing order of interest.
    std::vector<gspan::LabelData*> __edges;

    /// Mapping between labels and their cost.
    HashTable<gspan::LabelData*, unsigned long> __cost;

    /// Mapping between a pattern and the multiset of instances matched
    /// to it.
    HashTable<gspan::Pattern*, MatchedInstances* > __matched_instances;

    /// Contains all instance which belongs to a discovered and used pattern.
    Set<Instance*> __chosen;

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
    void __subgraph_mining(gspan::InterfaceGraph& graph, gspan::Pattern& p);

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
    // double __instance_cost(Instance* i) const;

    /// Print an iso map. For debug purpose.
    void __printIsoMap(gspan::Pattern& p);

    /// Returns true if e is an eligible root edge.
    /// @param e An EdgeData.
    /// @return true if e is an eligible root edge.
    bool __isEdgeEligible(gspan::EdgeData* e);

    /// @}
//    // ========================================================================
//    /// @name Private junk code
//    // ========================================================================
//    /// @{
//    /// Junk code.
//    void generateBN(size_t number);
//
//    /// Junk code.
//    BayesNet<prm_float>& baseBN(gspan::Pattern& p);
//
//    /// Junk code.
//    void infer(size_t number);
//
//    /// Print in out all the references in p.
//    void __flushReferences(gspan::Pattern* p, std::ostream& out) const;
//
//    void __flushVariables(gspan::Pattern* p, std::ostream& out) const;
//
//    void __flushAttribute(gspan::Pattern* p, std::ostream& out,
//                          const Instance* inst, const Attribute* attr) const;
//
//    /// This is junk code and will be erased.
//    HashTable<gspan::Pattern*, BayesNet<prm_float>*> __bn_map;
//
//    /// This is junk code and will be erased.
//    BayesNet<prm_float>* __generateBNTopology(Sequence<Instance*>& seq);
//
//    /// This is junk code and will be erased.
//    void __declare_variables(BayesNetFactory<prm_float>& factory,
//                             Sequence<Instance*>& seq);
//
//    /// This is junk code and will be erased.
//    void __declare_parents(BayesNetFactory<prm_float>& factory,
//                           Sequence<Instance*>& seq);
//
//    /// This is junk code and will be erased.
//    void __fill_inner_nodes(BayesNet<prm_float>& bn, Sequence<Instance*>& seq);
//
//    /// This is junk code and will be erased.
//    void __fill_input_nodes(BayesNet<prm_float>& bn, Sequence<Instance*>& seq);
//
//    /// @}
    // ========================================================================
    /// @name Private classes
    // ========================================================================
    /// @{

    /**
     * @class LabelSort gspan.h <agrum/prm/gspan.h>
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
      bool operator() (gspan::LabelData* i, gspan::LabelData* j);

      /// A pointer over an instance of the GSpan class using this class.
      GSpan* gspan;
    };

    /**
     * @class PatternSort gspan.h <agrum/prm/gspan.h>
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
      bool operator() (gspan::Pattern* i, gspan::Pattern* j);

      /// A pointer over an instance of the GSpan class using this class.
      GSpan* gspan;
    };

    /// @}
};

} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/gspan.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_GSPAN_H */
// ============================================================================
