
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of an influence diagram inference algorithm based upon
 * Shaffer-Shenoy's one for bayes net inferences
 */

#ifndef GUM_SHAFERSHENOY_LIMIDS_H
#define GUM_SHAFERSHENOY_LIMIDS_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/list.h>

#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/tools/multidim/implementations/multiDimBucket.h>
#include <agrum/tools/multidim/implementations/multiDimSparse.h>

#include <agrum/ID/inference/tools/influenceDiagramInference.h>

namespace gum {

  /**
   * @class InfluenceDiagramInference influenceDiagramInference.h
   *<agrum/ID/inference/influenceDiagramInference.h>
   * @brief This class implements an algorithm for inference
   * in influence diagrams based upon  Shaffer-Shenoy's one for bayes net
   *inferences
   * @ingroup id_group
   *
   * The class used for the triangulation is partialOrderedTriangulation.
   */
  template < typename GUM_SCALAR >
  class ShaferShenoyLIMIDSInference:
      public InfluenceDiagramInference< GUM_SCALAR > {
    public:
    // ====================================================================
    /// @name Constructor & destructor
    // ====================================================================
    /// @{

    /**
     * Default constructor.
     * @param infDiag the influence diagram we want to perform inference upon
     */
    explicit ShaferShenoyLIMIDSInference(
       const InfluenceDiagram< GUM_SCALAR >* infDiag);

    /**
     * Destructor.
     */
    virtual ~ShaferShenoyLIMIDSInference();

    const JunctionTree* junctionTree() const {
      GUM_ERROR(NotImplementedYet, "tbd asap")
    }

    void clear() override;

    ///@{
    /// No forgetting rule assumption
    void addNoForgettingAssumption(const std::vector< NodeId >& ids);
    void addNoForgettingAssumption(const std::vector< std::string >& names);
    bool hasNoForgettingAssumption() const;
    ///@}

    DAG  reducedGraph() const { return reduced_; };

    GUM_SCALAR MEU();

    Idx optimalDecision(NodeId decisionId);
    Idx optimalDecision(std::string decisionName);

    std::vector< NodeSet > reversePartialOrder() const;

    std::vector< std::pair< NodeId, Idx > > optimalDecisions();

    InfluenceDiagram< GUM_SCALAR > reducedLIMID() const;

    bool isSolvable() const;

    protected:
    void onStateChanged_() override;
    void onEvidenceAdded_(NodeId id, bool isHardEvidence) override;
    void onEvidenceErased_(NodeId id, bool isHardEvidence) override;
    void onAllEvidenceErased_(bool contains_hard_evidence) override;
    void onEvidenceChanged_(NodeId id, bool hasChangedSoftHard) override;
    void onModelChanged_(const GraphicalModel* model) override;
    void updateOutdatedStructure_() override;
    void updateOutdatedPotentials_() override;
    void makeInference_() override;

    /// Returns the set of non-requisite for node d
    NodeSet nonRequisiteNodes_(NodeId d) const;

    DAG reduced_;

    void                   createReduced_();
    std::vector< NodeSet > reversePartialOrder_;
    std::vector< NodeId >  solvabiltyOrder_;
    std::vector< NodeId >  noForgettingOrder_;

    private:
    void completingNoForgettingAssumption__();
    void reducingLIMID__();
    void creatingPartialOrder__(const NodeSet& utilities);
    void checkingSolvability__(const NodeSet& utilities);
  };
} /* namespace gum */

#include <agrum/ID/inference/ShaferShenoyLIMIDSInference_tpl.h>

#endif /* GUM_SHAFERSHENOY_LIMIDS_H */
