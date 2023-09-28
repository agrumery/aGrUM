//
// Created by Mahdi HADJALI on 06/09/2023.
//

#ifndef AGRUM_MEEKRULES_H
#define AGRUM_MEEKRULES_H

#include <agrum/agrum.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/graphs/DAG.h>

namespace gum {
  class MeekRules {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    MeekRules();

    /// destructor
    virtual ~MeekRules();

    /// @}

    /// Takes a partially oriented graph and orient all possible edges
    MixedGraph orientAllEdges(MixedGraph graph, std::vector< Arc > _latentCouples_);

    /// Takes a partially oriented graph and orient all the edges
    MixedGraph propagatesOrientations(MixedGraph graph, std::vector< Arc > _latentCouples_);

    private:
    /// Propagates the orientation from a node to its neighbours
    /** @param dag graph in which to which to propagate arcs
      * @param node node on which neighbours to propagate th orientation
      * @param force : true if an orientation has always to be found.
     */
    bool propagatesRemainingOrientableEdges_(MixedGraph& graph, NodeId xj, std::vector< Arc > _latentCouples_);

    /// heuristic for remaining edges when everything else has been tried
    void propagatesOrientationInChainOfRemainingEdges_(gum::MixedGraph& graph, std::vector< Arc > _latentCouples_);

    bool isOrientable_(const gum::MixedGraph& graph, gum::NodeId xi, gum::NodeId xj) const;

    static bool _existsDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);

  }; ///class MeekRules

} ///namespace gum


#endif   // AGRUM_MEEKRULES_H
