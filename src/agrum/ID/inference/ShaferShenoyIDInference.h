
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

#ifndef GUM_SHAFERSHENOY_INFLUENCE_DIAGRAM_INFERENCE_H
#define GUM_SHAFERSHENOY_INFLUENCE_DIAGRAM_INFERENCE_H

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
  class ShaferShenoyIDInference: public InfluenceDiagramInference< GUM_SCALAR > {
    public:
    // ====================================================================
    /// @name Constructor & destructor
    // ====================================================================
    /// @{

    /**
     * Default constructor.
     * @param infDiag the influence diagram we want to perform inference upon
     */
    explicit ShaferShenoyIDInference(
       const InfluenceDiagram< GUM_SCALAR >* infDiag);

    /**
     * Destructor.
     */
    virtual ~ShaferShenoyIDInference();

    const JunctionTree* junctionTree() const {
      GUM_ERROR(NotImplementedYet, "tbd asap")
    }

    void clear() override;
    ///@{
    /// No forgetting rule assumption
    bool isNoForgettingAssumption() const;

    GUM_SCALAR MEU();

    Idx optimalDecision(NodeId decisionId);
    Idx optimalDecision(std::string decisionName) {
      return optimalDecision(this->influenceDiagram().idFromName(decisionName));
    };

    std::vector<NodeSet> partialOrder();

    std::vector< std::pair< NodeId, Idx > > optimalDecisions();

    protected:
    void onStateChanged_() override;
    void onEvidenceAdded_(const NodeId id, bool isHardEvidence) override;
    void onEvidenceErased_(const NodeId id, bool isHardEvidence) override;
    void onAllEvidenceErased_(bool contains_hard_evidence) override;
    void onEvidenceChanged_(const NodeId id, bool hasChangedSoftHard) override;
    void onModelChanged_(const GraphicalModel* model) override;
    void updateOutdatedStructure_() override;
    void updateOutdatedPotentials_() override;
    void makeInference_() override;

    DAG reduce_;

    void createReduced_();
    std::vector<NodeSet> partialOrder_;
    ArcSet decisionConstraints_;
  };
} /* namespace gum */

#include <agrum/ID/inference/ShaferShenoyIDInference_tpl.h>

#endif /* GUM_SHAFERSHENOY_INFLUENCE_DIAGRAM_INFERENCE_H */
