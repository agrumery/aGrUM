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

#include <agrum/tools/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/tools/multidim/implementations/multiDimBucket.h>
#include <agrum/tools/multidim/implementations/multiDimSparse.h>
#include <agrum/ID/inference/tools/decisionPotential.h>

#include <agrum/ID/inference/tools/influenceDiagramInference.h>
#include <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>


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
  class ShaferShenoyLIMIDInference: public InfluenceDiagramInference< GUM_SCALAR > {
    using PhiNodeProperty = NodeProperty< DecisionPotential< GUM_SCALAR > >;
    using PsiArcProperty  = ArcProperty< DecisionPotential< GUM_SCALAR > >;
    using SetOfVars       = Set< const DiscreteVariable* >;


    public:
    // ====================================================================
    /// @name Constructor & destructor
    // ====================================================================
    /// @{

    /**
     * Default constructor.
     * @param infDiag the influence diagram we want to perform inference upon
     */
    explicit ShaferShenoyLIMIDInference(const InfluenceDiagram< GUM_SCALAR >* infDiag);

    /**
     * Destructor.
     */
    virtual ~ShaferShenoyLIMIDInference();

    const JunctionTree* junctionTree() const;

    void clear() override;

    ///@{
    /// No forgetting rule assumption
    void addNoForgettingAssumption(const std::vector< NodeId >& ids);
    void addNoForgettingAssumption(const std::vector< std::string >& names);
    bool hasNoForgettingAssumption() const;
    ///@}

    DAG reducedGraph() const { return reduced_; };

    std::vector< NodeSet > reversePartialOrder() const;

    InfluenceDiagram< GUM_SCALAR > reducedLIMID() const;

    bool isSolvable() const;


    gum::Potential< GUM_SCALAR > optimalDecision(NodeId decisionId) final;
    gum::Potential< GUM_SCALAR > optimalDecision(const std::string& decisionName) final {
      return optimalDecision(this->influenceDiagram().idFromName(decisionName));
    };

    /**
     * Return the posterior probability of a node
     *
     * @param node
     * @return the posterior probability
     */
    virtual const Potential< GUM_SCALAR >& posterior(NodeId node) final;
    const Potential< GUM_SCALAR >&         posterior(const std::string& name) final {
      return posterior(this->influenceDiagram().idFromName(name));
    };

    /**
     * Return the posterior utility of a node
     *
     * @param node
     * @return the posterior utility of a node
     */
    virtual const Potential< GUM_SCALAR >& posteriorUtility(NodeId node) final;
    virtual const Potential< GUM_SCALAR >& posteriorUtility(const std::string& name) final {
      return posteriorUtility(this->influenceDiagram().idFromName(name));
    };

    /**
     * Return the pair (mean,variance) for a node
     *
     * @param node
     * @return the pair (mean,variance) for a node
     */
    virtual std::pair< GUM_SCALAR, GUM_SCALAR > meanVar(NodeId node) final;
    std::pair< GUM_SCALAR, GUM_SCALAR >         meanVar(const std::string& name) final {
      return meanVar(this->influenceDiagram().idFromName(name));
    };

    /**
     * Return the pair (mean,variance) for the total utility (MEU)
     *
     * @return the pair (mean,variance) for MEU
     */
    std::pair< GUM_SCALAR, GUM_SCALAR > MEU() final;

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

    DAG                                             reduced_;
    CliqueGraph                                     reducedJunctionTree_;
    NodeProperty< NodeId >                          node_to_clique_;
    EdgeProperty< SetOfVars >                       varsSeparator_;
    NodeProperty< Potential< GUM_SCALAR > >         strategies_;
    NodeProperty< DecisionPotential< GUM_SCALAR > > posteriors_;
    NodeProperty< DecisionPotential< GUM_SCALAR > > unconditionalDecisions_;

    void                   createReduced_();
    std::vector< NodeSet > reversePartialOrder_;
    std::vector< NodeId >  solvabilityOrder_;
    std::vector< NodeId >  noForgettingOrder_;

    private:
    void _completingNoForgettingAssumption_();
    void _reducingLIMID_();
    void _creatingPartialOrder_(const NodeSet& utilities);
    void _checkingSolvability_(const NodeSet& utilities);
    void _creatingJunctionTree_();
    void _findingCliqueForEachNode_(DefaultTriangulation& triangulation);

    void initializingInference_(PhiNodeProperty& phi, PsiArcProperty& psi);
    void collectingMessage_(PhiNodeProperty& phi, PsiArcProperty& psi, NodeId rootClique);
    void collectingToFollowingRoot_(PhiNodeProperty& phi,
                                    PsiArcProperty&  psi,
                                    NodeId           fromClique,
                                    NodeId           toClique);
    void deciding_(PhiNodeProperty& phi, PsiArcProperty& psi, NodeId decisionNode);
    void transmittingMessage_(PhiNodeProperty& phi,
                              PsiArcProperty&  psi,
                              NodeId           fromClique,
                              NodeId           toClique);
    void transmittingFinalMessage_(PhiNodeProperty& phi,
                                   PsiArcProperty&  psi,
                                   NodeId           fromClique,
                                   NodeId           toClique);
    void distributingMessage_(PhiNodeProperty& phi, PsiArcProperty& psi, NodeId rootClique);
    void computingPosteriors_(const PhiNodeProperty& phi, const PsiArcProperty& psi);
    DecisionPotential< double > integrating_(const PhiNodeProperty& phi,
                                             const PsiArcProperty&  psi,
                                             NodeId                 clique,
                                             NodeId                 except) const;
    DecisionPotential< double >
         integrating_(const PhiNodeProperty& phi, const PsiArcProperty& psi, NodeId clique) const;
    void binarizingMax_(const Potential< GUM_SCALAR >& decision,
                        const Potential< GUM_SCALAR >& proba) const;
  };
} /* namespace gum */

#include <agrum/ID/inference/ShaferShenoyLIMIDInference_tpl.h>

#endif /* GUM_SHAFERSHENOY_LIMIDS_H */
