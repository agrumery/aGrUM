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
    MeekRules(const gum::MixedGraph graph);

    /// destructor
    virtual ~MeekRules();

    /// @}

    private:
    /// an empty vector of arcs
    std::vector< gum::Arc > _latentCouples_;
    const gum::MixedGraph&  _g_;

    /// maximum number of parents
    gum::Size _maxIndegree_;
    /// Graph that contains the mandatory arcs
    gum::DAG mandatoryGraph;
    /// Graph that contains the forbidden arcs
    gum::DiGraph forbiddenGraph;

    gum::DAG learnDAG(gum::MixedGraph graph);
    gum::MixedGraph learnPDAG(gum::MixedGraph graph);

    /// Propagates the orientation from a node to its neighbours
    bool propagatesRemainingOrientableEdges_(gum::MixedGraph& graph, gum::NodeId xj);

    /// heuristic for remaining edges when everything else has been tried
    void propagatesOrientationInChainOfRemainingEdges_(gum::MixedGraph& graph);

    bool isOrientable_(const gum::MixedGraph& graph, gum::NodeId xi, gum::NodeId xj) const;
    bool _isForbiddenArc_(MixedGraph graph, NodeId x, NodeId y) const;

    void setForbiddenGraph(gum::DiGraph forbidGraph);
    void setMandatoryGraph(gum::DAG mandaGraph);
    void setMaxIndegree(gum::Size n);

    static bool _existsDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);

  }; ///class MeekRules

} ///namespace gum


#endif   // AGRUM_MEEKRULES_H
