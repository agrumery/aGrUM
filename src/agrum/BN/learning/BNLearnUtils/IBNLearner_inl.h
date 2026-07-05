/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/** @file
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing, MIIC and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to help IDE parser
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/BN/learning/BNLearnUtils/IBNLearner.h>

namespace gum::learning {


  // returns the row filter
  INLINE DBRowGeneratorParser& IBNLearner::Database::parser() { return *_parser_; }

  // returns the modalities of the variables
  INLINE const std::vector< std::size_t >& IBNLearner::Database::domainSizes() const {
    return _domain_sizes_;
  }

  // returns the names of the variables in the database
  INLINE const std::vector< std::string >& IBNLearner::Database::names() const {
    return _database_.variableNames();
  }

  /// assign new weight to the rows of the learning database
  INLINE void IBNLearner::Database::setDatabaseWeight(const double new_weight) {
    if (_database_.nbRows() == std::size_t(0)) return;
    const double weight = new_weight / double(_database_.nbRows());
    _database_.setAllRowsWeight(weight);
  }

  // returns the node id corresponding to a variable name
  INLINE NodeId IBNLearner::Database::idFromName(std::string_view var_name) const {
    try {
      const auto cols = _database_.columnsFromVariableName(var_name);
      return _nodeId2cols_.first(cols[0]);
    } catch (...) {
      GUM_ERROR(MissingVariableInDatabase,
                "Variable " << var_name << " could not be found in the database")
    }
  }

  // returns the variable name corresponding to a given node id
  INLINE const std::string& IBNLearner::Database::nameFromId(NodeId id) const {
    try {
      return _database_.variableName(_nodeId2cols_.second(id));
    } catch (...) {
      GUM_ERROR(MissingVariableInDatabase,
                "Variable of Id " << id << " could not be found in the database")
    }
  }

  /// returns the internal database table
  INLINE const DatabaseTable& IBNLearner::Database::databaseTable() const { return _database_; }

  /// returns the set of missing symbols taken into account
  INLINE const std::vector< std::string >& IBNLearner::Database::missingSymbols() const {
    return _database_.missingSymbols();
  }

  /// returns the mapping between node ids and their columns in the database
  INLINE const Bijection< NodeId, std::size_t >& IBNLearner::Database::nodeId2Columns() const {
    return _nodeId2cols_;
  }

  /// returns the number of records in the database
  INLINE std::size_t IBNLearner::Database::nbRows() const { return _database_.nbRows(); }

  /// returns the number of records in the database
  INLINE std::size_t IBNLearner::Database::size() const { return _database_.size(); }

  /// sets the weight of the ith record
  INLINE void IBNLearner::Database::setWeight(const std::size_t i, const double weight) {
    _database_.setWeight(i, weight);
  }

  /// returns the weight of the ith record
  INLINE double IBNLearner::Database::weight(const std::size_t i) const {
    return _database_.weight(i);
  }

  /// returns the weight of the whole database
  INLINE double IBNLearner::Database::weight() const { return _database_.weight(); }

  // ===========================================================================

  // returns the node id corresponding to a variable name
  INLINE NodeId IBNLearner::idFromName(std::string_view var_name) const {
    return scoreDatabase_.idFromName(var_name);
  }

  // returns the variable name corresponding to a given node id
  INLINE const std::string& IBNLearner::nameFromId(NodeId id) const {
    return scoreDatabase_.nameFromId(id);
  }

  /// assign new weight to the rows of the learning database
  INLINE void IBNLearner::setDatabaseWeight(const double new_weight) {
    scoreDatabase_.setDatabaseWeight(new_weight);
  }

  /// assign new weight to the ith row of the learning database
  INLINE void IBNLearner::setRecordWeight(const std::size_t i, const double new_weight) {
    scoreDatabase_.setWeight(i, new_weight);
  }

  /// returns the weight of the ith record
  INLINE double IBNLearner::recordWeight(const std::size_t i) const {
    return scoreDatabase_.weight(i);
  }

  /// returns the weight of the whole database
  INLINE double IBNLearner::databaseWeight() const { return scoreDatabase_.weight(); }

  // sets an initial DAG structure
  INLINE void IBNLearner::setInitialDAG(const DAG& dag) { initialDag_ = dag; }

  INLINE DAG IBNLearner::initialDAG() { return initialDag_; }

  // indicate that we wish to use an AIC score
  INLINE void IBNLearner::useScoreAIC() {
    scoreType_ = ScoreType::AIC;
    checkScorePriorCompatibility();
  }

  // indicate that we wish to use a BD score
  INLINE void IBNLearner::useScoreBD() {
    scoreType_ = ScoreType::BD;
    checkScorePriorCompatibility();
  }

  // indicate that we wish to use a BDeu score
  INLINE void IBNLearner::useScoreBDeu() {
    scoreType_ = ScoreType::BDeu;
    checkScorePriorCompatibility();
  }

  // indicate that we wish to use a BIC score
  INLINE void IBNLearner::useScoreBIC() {
    scoreType_ = ScoreType::BIC;
    checkScorePriorCompatibility();
  }

  // indicate that we wish to use a fNML score
  INLINE void IBNLearner::useScorefNML() {
    scoreType_ = ScoreType::fNML;
    checkScorePriorCompatibility();
  }

  // indicate that we wish to use a K2 score
  INLINE void IBNLearner::useScoreK2() {
    scoreType_ = ScoreType::K2;
    checkScorePriorCompatibility();
  }

  // indicate that we wish to use a Log2Likelihood score
  INLINE void IBNLearner::useScoreLog2Likelihood() {
    scoreType_ = ScoreType::LOG2LIKELIHOOD;
    checkScorePriorCompatibility();
  }

  // indicate that we wish to use a MDL score
  INLINE void IBNLearner::useScoreMDL() {
    scoreType_ = ScoreType::MDL;
    checkScorePriorCompatibility();
  }

  // sets the max indegree
  INLINE void IBNLearner::setMaxIndegree(Size max_indegree) {
    constraintIndegree_.setMaxIndegree(max_indegree);
  }

  // indicate that we wish to use MIIC with constraints
  INLINE void IBNLearner::useMIIC() { selectedAlgo_ = AlgoType::MIIC; }

  // indicate that we wish to use the PC algorithm
  INLINE void IBNLearner::usePC() { selectedAlgo_ = AlgoType::PC; }

  // indicate that we wish to use the FCI algorithm
  INLINE void IBNLearner::useFCI() { selectedAlgo_ = AlgoType::FCI; }

  // select Chi2 independence test for FCI (default)
  INLINE void IBNLearner::useFCIChi2Test() {
    if (selectedAlgo_ != AlgoType::FCI) {
      GUM_ERROR(OperationNotAllowed, "useFCIChi2Test() is only valid when using the FCI algorithm")
    }
    indepTestTypeFCI_ = IndepTestType::Chi2;
  }

  // select G2 independence test for FCI
  INLINE void IBNLearner::useFCIG2Test() {
    if (selectedAlgo_ != AlgoType::FCI) {
      GUM_ERROR(OperationNotAllowed, "useFCIG2Test() is only valid when using the FCI algorithm")
    }
    indepTestTypeFCI_ = IndepTestType::G2;
  }

  // set alpha threshold for FCI
  INLINE void IBNLearner::setFCIAlpha(double alpha) {
    if (selectedAlgo_ != AlgoType::FCI) {
      GUM_ERROR(OperationNotAllowed, "setFCIAlpha() is only valid when using the FCI algorithm")
    }
    alphaFci_ = alpha;
  }

  // set max discriminating-path length for FCI R4 (Size(-1) = unlimited)
  INLINE void IBNLearner::setFCIMaxPathLength(Size max_len) {
    if (selectedAlgo_ != AlgoType::FCI) {
      GUM_ERROR(OperationNotAllowed,
                "setFCIMaxPathLength() is only valid when using the FCI algorithm")
    }
    maxPathLengthFci_ = max_len;
  }

  INLINE void IBNLearner::setFCIExhaustiveSepSet(bool exhaustive) {
    if (selectedAlgo_ != AlgoType::FCI) {
      GUM_ERROR(OperationNotAllowed,
                "setFCIExhaustiveSepSet() is only valid when using the FCI algorithm")
    }
    exhaustiveSepSetFci_ = exhaustive;
  }

  INLINE bool IBNLearner::fciExhaustiveSepSet() const {
    if (selectedAlgo_ != AlgoType::FCI) {
      GUM_ERROR(OperationNotAllowed,
                "fciExhaustiveSepSet() is only valid when using the FCI algorithm")
    }
    return exhaustiveSepSetFci_;
  }

  // select Chi2 independence test for PC (default)
  INLINE void IBNLearner::useChi2Test() {
    if (selectedAlgo_ != AlgoType::PC) {
      GUM_ERROR(OperationNotAllowed, "useChi2Test() is only valid when using the PC algorithm")
    }
    indepTestTypePC_ = IndepTestType::Chi2;
  }

  // select G2 independence test for PC
  INLINE void IBNLearner::useG2Test() {
    if (selectedAlgo_ != AlgoType::PC) {
      GUM_ERROR(OperationNotAllowed, "useG2Test() is only valid when using the PC algorithm")
    }
    indepTestTypePC_ = IndepTestType::G2;
  }

  // set the alpha threshold for PC
  INLINE void IBNLearner::setPCAlpha(double alpha) {
    if (selectedAlgo_ != AlgoType::PC) {
      GUM_ERROR(OperationNotAllowed, "setPCAlpha() is only valid when using the PC algorithm")
    }
    alphaPc_ = alpha;
  }

  // set stable mode for PC
  INLINE void IBNLearner::setPCStable(bool stable) {
    if (selectedAlgo_ != AlgoType::PC) {
      GUM_ERROR(OperationNotAllowed, "setPCStable() is only valid when using the PC algorithm")
    }
    stablePc_ = stable;
  }

  // set max conditioning set size for PC (Size(-1) = unlimited)
  INLINE void IBNLearner::setPCMaxCondSetSize(Size max_k) {
    if (selectedAlgo_ != AlgoType::PC) {
      GUM_ERROR(OperationNotAllowed,
                "setPCMaxCondSetSize() is only valid when using the PC algorithm")
    }
    maxCondSetSizePc_ = max_k;
  }

  // set unshielded-collider ordering for PC
  INLINE void IBNLearner::setPCUnshieldedColliderSorted(bool sorted) {
    if (selectedAlgo_ != AlgoType::PC) {
      GUM_ERROR(OperationNotAllowed,
                "setPCUnshieldedColliderSorted() is only valid when using the PC algorithm")
    }
    sortedUCPc_ = sorted;
  }

  /// indicate that we wish to use the NML correction for MIIC
  INLINE void IBNLearner::useNMLCorrection() {
    kmodeMiic_ = CorrectedMutualInformation::KModeTypes::NML;
  }

  /// indicate that we wish to use the MDL correction for MIIC
  INLINE void IBNLearner::useMDLCorrection() {
    kmodeMiic_ = CorrectedMutualInformation::KModeTypes::MDL;
  }

  /// indicate that we wish to use the NoCorr correction for MIIC
  INLINE void IBNLearner::useNoCorrection() {
    kmodeMiic_ = CorrectedMutualInformation::KModeTypes::NoCorr;
  }

  /// get the list of arcs hiding latent variables
  INLINE std::vector< Arc > IBNLearner::latentVariables() const {
    return algoMiic_.latentVariables();
  }

  // indicate that we wish to use a K2 algorithm
  INLINE void IBNLearner::useK2(const Sequence< NodeId >& order) {
    selectedAlgo_ = AlgoType::K2;
    algoK2_.setOrder(order);
  }

  // indicate that we wish to use a K2 algorithm
  INLINE void IBNLearner::useK2(const std::vector< NodeId >& order) {
    selectedAlgo_ = AlgoType::K2;
    algoK2_.setOrder(order);
  }

  // indicate that we wish to use a greedy hill climbing algorithm
  INLINE void IBNLearner::useGreedyHillClimbing() {
    selectedAlgo_ = AlgoType::GREEDY_HILL_CLIMBING;
  }

  // indicate that we wish to use a greedy hill climbing algorithm
  INLINE void IBNLearner::useExtendedGreedyHillClimbing() {
    selectedAlgo_ = AlgoType::EXTENDED_GREEDY_HILL_CLIMBING;
  }

  // indicate that we wish to use greedy thick-thinning
  INLINE void IBNLearner::useGreedyThickThinning() {
    selectedAlgo_ = AlgoType::GREEDY_THICK_THINNING;
  }

  // enable or disable arc reversals in the thin phase of greedy thick-thinning
  INLINE void IBNLearner::setGreedyThickThinningReversals(bool allow) {
    greedyThickThinning_.setAllowReversalsInThinPhase(allow);
  }

  // returns whether arc reversals are allowed in the thin phase
  INLINE bool IBNLearner::greedyThickThinningReversals() const {
    return greedyThickThinning_.allowReversalsInThinPhase();
  }

  // indicate that we wish to use a local search with tabu list
  INLINE void IBNLearner::useLocalSearchWithTabuList(Size tabu_size, Size nb_decrease) {
    selectedAlgo_        = AlgoType::LOCAL_SEARCH_WITH_TABU_LIST;
    nbDecreasingChanges_ = nb_decrease;
    constraintTabuList_.setTabuListSize(tabu_size);
    localSearchWithTabuList_.setMaxNbDecreasingChanges(nb_decrease);
  }

  /// use The EM algorithm to learn parameters
  INLINE void IBNLearner::useEM(const double epsilon, const double noise) {
    if (epsilon < 0.0)
      GUM_ERROR(OutOfBounds, "EM's min log-likelihood evolution rate must be non-negative");
    if ((noise < 0.0) || (noise > 1.0))
      GUM_ERROR(OutOfBounds, "EM's noise must belong to interval [0,1]");
    if (epsilon > 0) {
      useEM_ = true;
      dag2BN_.setMinEpsilonRate(epsilon);
      dag2BN_.setNoise(noise);
      noiseEM_ = noise;
    } else {
      useEM_ = false;   // epsilon == 0
    }
  }

  /// use The EM algorithm to learn parameters with the rate stopping criterion
  INLINE void IBNLearner::useEMWithRateCriterion(const double epsilon, const double noise) {
    if (epsilon <= 0.0)
      GUM_ERROR(OutOfBounds, "EM's min log-likelihood evolution rate must be positive");
    useEM(epsilon, noise);
  }

  /// use The EM algorithm to learn parameters with the diff stoppîng criterion
  INLINE void IBNLearner::useEMWithDiffCriterion(const double epsilon, const double noise) {
    if (epsilon <= 0.0)
      GUM_ERROR(OutOfBounds, "EM's min log-likelihood differences must be positive");
    if ((noise < 0.0) || (noise > 1.0))
      GUM_ERROR(OutOfBounds, "EM's noise must belong to interval [0,1]");
    useEM_ = true;
    dag2BN_.setEpsilon(epsilon);
    dag2BN_.setNoise(noise);
    noiseEM_ = noise;
  }

  /// forbid to use EM
  INLINE void IBNLearner::forbidEM() { useEM_ = false; }

  /// indicates whether we use EM for parameter learning
  INLINE bool IBNLearner::isUsingEM() const { return useEM_; }

  /// returns the EM parameter learning approximation scheme
  INLINE EMApproximationScheme& IBNLearner::EM() {
    if (useEM_) return dag2BN_;
    else GUM_ERROR(NotFound, "EM is currently forbidden. Please enable it with useEM()")
  }

  /// returns the state of the last EM algorithm executed
  INLINE IApproximationSchemeConfiguration::ApproximationSchemeSTATE IBNLearner::EMState() const {
    if (useEM_) return dag2BN_.stateApproximationScheme();
    else return IApproximationSchemeConfiguration::ApproximationSchemeSTATE::Undefined;
  }

  /// returns the state of the EM algorithm
  INLINE std::string IBNLearner::EMStateMessage() const {
    if (useEM_) return dag2BN_.messageApproximationScheme();
    else return "EM is currently forbidden. Please enable it with useEM()";
  }

  // allow (true)/forbid (false) to add arcs during learning
  INLINE void IBNLearner::allowArcAdditions(bool allow) { allowArcAdditions_ = allow; }

  // allow (true)/forbid (false) to delete arcs during learning
  INLINE void IBNLearner::allowArcDeletions(bool allow) { allowArcDeletions_ = allow; }

  // allow (true)/forbid (false) to reverse arcs during learning
  INLINE void IBNLearner::allowArcReversals(bool allow) { allowArcReversals_ = allow; }

  // allow (true)/forbid (false) to delete arc triangles during learning
  INLINE void IBNLearner::allowArcTriangleDeletions(bool allow) {
    allowArcTriangleDeletions_ = allow;
  }

  INLINE bool IBNLearner::hasMissingValues() const {
    return scoreDatabase_.databaseTable().hasMissingValues();
  }

  // assign a set of forbidden edges
  INLINE void IBNLearner::setPossibleEdges(const EdgeSet& set) {
    constraintPossibleEdges_.setEdges(set);
  }

  // assign a set of forbidden edges from an UndiGraph
  INLINE void IBNLearner::setPossibleSkeleton(const gum::UndiGraph& g) {
    setPossibleEdges(g.edges());
  }

  // assign a new possible edge
  INLINE void IBNLearner::addPossibleEdge(const Edge& edge) {
    constraintPossibleEdges_.addEdge(edge);
  }

  // remove a forbidden edge
  INLINE void IBNLearner::erasePossibleEdge(const Edge& edge) {
    constraintPossibleEdges_.eraseEdge(edge);
  }

  // assign a new forbidden edge
  INLINE void IBNLearner::addPossibleEdge(const NodeId tail, const NodeId head) {
    addPossibleEdge(Edge(tail, head));
  }

  // remove a forbidden edge
  INLINE void IBNLearner::erasePossibleEdge(const NodeId tail, const NodeId head) {
    erasePossibleEdge(Edge(tail, head));
  }

  // assign a new forbidden edge
  INLINE void IBNLearner::addPossibleEdge(std::string_view tail, std::string_view head) {
    addPossibleEdge(Edge(idFromName(tail), idFromName(head)));
  }

  // remove a forbidden edge
  INLINE void IBNLearner::erasePossibleEdge(std::string_view tail, std::string_view head) {
    erasePossibleEdge(Edge(idFromName(tail), idFromName(head)));
  }

  // assign a set of forbidden arcs
  INLINE void IBNLearner::setForbiddenArcs(const ArcSet& set) {
    constraintForbiddenArcs_.setArcs(set);
  }

  // assign a new forbidden arc
  INLINE void IBNLearner::addForbiddenArc(const Arc& arc) { constraintForbiddenArcs_.addArc(arc); }

  // remove a forbidden arc
  INLINE void IBNLearner::eraseForbiddenArc(const Arc& arc) {
    constraintForbiddenArcs_.eraseArc(arc);
  }

  // assign a new forbidden arc
  INLINE void IBNLearner::addForbiddenArc(const NodeId tail, const NodeId head) {
    addForbiddenArc(Arc(tail, head));
  }

  // remove a forbidden arc
  INLINE void IBNLearner::eraseForbiddenArc(const NodeId tail, const NodeId head) {
    eraseForbiddenArc(Arc(tail, head));
  }

  // assign a new forbidden arc
  INLINE void IBNLearner::addForbiddenArc(std::string_view tail, std::string_view head) {
    addForbiddenArc(Arc(idFromName(tail), idFromName(head)));
  }

  // remove a forbidden arc
  INLINE void IBNLearner::eraseForbiddenArc(std::string_view tail, std::string_view head) {
    eraseForbiddenArc(Arc(idFromName(tail), idFromName(head)));
  }

  // assign a set of forbidden arcs
  INLINE void IBNLearner::setMandatoryArcs(const ArcSet& set) {
    constraintMandatoryArcs_.setArcs(set);
  }

  // assign a new forbidden arc
  INLINE void IBNLearner::addMandatoryArc(const Arc& arc) { constraintMandatoryArcs_.addArc(arc); }

  // remove a forbidden arc
  INLINE void IBNLearner::eraseMandatoryArc(const Arc& arc) {
    constraintMandatoryArcs_.eraseArc(arc);
  }

  INLINE void IBNLearner::addNoParentNode(NodeId node) { constraintNoParentNodes_.addNode(node); }

  INLINE void IBNLearner::addNoParentNode(std::string_view name) {
    addNoParentNode(idFromName(name));
  }

  INLINE void IBNLearner::eraseNoParentNode(NodeId node) {
    constraintNoParentNodes_.eraseNode(node);
  }

  INLINE void IBNLearner::eraseNoParentNode(std::string_view name) {
    eraseNoParentNode(idFromName(name));
  }

  INLINE void IBNLearner::addNoChildrenNode(NodeId node) {
    constraintNoChildrenNodes_.addNode(node);
  }

  INLINE void IBNLearner::addNoChildrenNode(std::string_view name) {
    addNoChildrenNode(idFromName(name));
  }

  INLINE void IBNLearner::eraseNoChildrenNode(NodeId node) {
    constraintNoChildrenNodes_.eraseNode(node);
  }

  INLINE void IBNLearner::eraseNoChildrenNode(std::string_view name) {
    eraseNoChildrenNode(idFromName(name));
  }

  // assign a new forbidden arc
  INLINE void IBNLearner::addMandatoryArc(std::string_view tail, std::string_view head) {
    addMandatoryArc(Arc(idFromName(tail), idFromName(head)));
  }

  // remove a forbidden arc
  INLINE void IBNLearner::eraseMandatoryArc(std::string_view tail, std::string_view head) {
    eraseMandatoryArc(Arc(idFromName(tail), idFromName(head)));
  }

  // assign a new forbidden arc
  INLINE void IBNLearner::addMandatoryArc(NodeId tail, NodeId head) {
    addMandatoryArc(Arc(tail, head));
  }

  // remove a forbidden arc
  INLINE void IBNLearner::eraseMandatoryArc(NodeId tail, NodeId head) {
    eraseMandatoryArc(Arc(tail, head));
  }

  // sets a partial order on the nodes
  INLINE void IBNLearner::setSliceOrder(const NodeProperty< NodeId >& slice_order) {
    constraintSliceOrder_ = StructuralConstraintSliceOrder(slice_order);
  }

  INLINE void IBNLearner::setSliceOrder(const std::vector< std::vector< std::string > >& slices) {
    NodeProperty< NodeId > slice_order;
    NodeId                 rank = 0;
    for (const auto& slice: slices) {
      for (const auto& name: slice) {
        slice_order.insert(idFromName(name), rank);
      }
      rank++;
    }
    setSliceOrder(slice_order);
  }

  INLINE void IBNLearner::unsetSliceOrder() { setSliceOrder(NodeProperty< NodeId >()); }

  // sets a total order over some nodes
  INLINE void IBNLearner::setTotalOrder(const Sequence< NodeId >& order) {
    constraintTotalOrder_ = StructuralConstraintTotalOrder(order);
  }

  // sets a total order over some nodes
  INLINE void IBNLearner::setTotalOrder(const std::vector< std::string >& order) {
    Sequence< NodeId > sequence;
    for (const auto& name: order) {
      sequence.insert(idFromName(name));
    }
    setTotalOrder(sequence);
  }

  // removes the current total ordering constraint, if any
  INLINE void IBNLearner::unsetTotalOrder() { setTotalOrder(Sequence< NodeId >()); }

  // sets the prior weight
  INLINE void IBNLearner::_setPriorWeight_(double weight) {
    if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

    priorWeight_ = weight;
    checkScorePriorCompatibility();
  }

  // use the prior smoothing
  INLINE void IBNLearner::useNoPrior() {
    priorType_ = BNLearnerPriorType::NO_prior;
    checkScorePriorCompatibility();
  }

  // use the prior smoothing
  INLINE void IBNLearner::useSmoothingPrior(double weight) {
    if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

    priorType_ = BNLearnerPriorType::SMOOTHING;
    _setPriorWeight_(weight);

    checkScorePriorCompatibility();
  }

  // use the Dirichlet prior
  INLINE void IBNLearner::useDirichletPrior(std::string_view filename, double weight) {
    if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

    priorDbname_ = filename;
    priorType_   = BNLearnerPriorType::DIRICHLET_FROM_DATABASE;
    _setPriorWeight_(weight);

    checkScorePriorCompatibility();
  }

  // use the prior BDeu
  INLINE void IBNLearner::useBDeuPrior(double weight) {
    if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

    priorType_ = BNLearnerPriorType::BDEU;
    _setPriorWeight_(weight);

    checkScorePriorCompatibility();
  }

  // returns the type (as a string) of a given prior
  INLINE PriorType IBNLearner::getPriorType_() const {
    switch (priorType_) {
      using enum gum::learning::IBNLearner::BNLearnerPriorType;
      case NO_prior : return PriorType::NoPriorType;
      case SMOOTHING : return PriorType::SmoothingPriorType;
      case DIRICHLET_FROM_DATABASE :
      case DIRICHLET_FROM_BAYESNET : return PriorType::DirichletPriorType;
      case BDEU : return PriorType::BDeuPriorType;
      default :
        GUM_ERROR(OperationNotAllowed,
                  "IBNLearner getPriorType does "
                  "not support yet this prior")
    }
  }

  // returns the names of the variables in the database
  INLINE const std::vector< std::string >& IBNLearner::names() const {
    return scoreDatabase_.names();
  }

  // returns the modalities  of the variables in the database
  INLINE const std::vector< std::size_t >& IBNLearner::domainSizes() const {
    return scoreDatabase_.domainSizes();
  }

  // returns the modalities  of a variable in the database
  INLINE Size IBNLearner::domainSize(NodeId var) const { return scoreDatabase_.domainSizes()[var]; }

  // returns the modalities  of a variables in the database
  INLINE Size IBNLearner::domainSize(std::string_view var) const {
    return scoreDatabase_.domainSizes()[idFromName(var)];
  }

  /// returns the current database rows' ranges used for learning
  INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
               IBNLearner::databaseRanges() const {
    return ranges_;
  }

  /// reset the ranges to the one range corresponding to the whole database
  INLINE void IBNLearner::clearDatabaseRanges() { ranges_.clear(); }

  /// returns the database used by the BNLearner
  INLINE const DatabaseTable& IBNLearner::database() const {
    return scoreDatabase_.databaseTable();
  }

  INLINE Size IBNLearner::nbCols() const { return scoreDatabase_.domainSizes().size(); }

  INLINE Size IBNLearner::nbRows() const { return scoreDatabase_.databaseTable().size(); }

  // sets the number max of threads that can be used
  INLINE void IBNLearner::setNumberOfThreads(Size nb) {
    ThreadNumberManager::setNumberOfThreads(nb);
    if (score_ != nullptr) score_->setNumberOfThreads(nb);
  }

  // ===========================================================================
  // IBNLearner — inline implementations migrated from class body
  // ===========================================================================

  INLINE bool IBNLearner::isConstraintBased() const {
    switch (selectedAlgo_) {
      case AlgoType::K2 :
      case AlgoType::GREEDY_HILL_CLIMBING :
      case AlgoType::EXTENDED_GREEDY_HILL_CLIMBING :
      case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST :
      case AlgoType::GREEDY_THICK_THINNING : return false;
      case AlgoType::MIIC :
      case AlgoType::PC :
      case AlgoType::FCI : return true;
      default : throw OperationNotAllowed("Unknown algorithm");
    }
  }

  INLINE bool IBNLearner::isScoreBased() const { return !isConstraintBased(); }

  INLINE void
      IBNLearner::setCurrentApproximationScheme(const ApproximationScheme* approximationScheme) {
    currentAlgorithm_ = approximationScheme;
  }

  INLINE void IBNLearner::distributeProgress(const ApproximationScheme* approximationScheme,
                                             Size                       pourcent,
                                             double                     error,
                                             double                     time) {
    setCurrentApproximationScheme(approximationScheme);
    if (onProgress.hasListener()) GUM_EMIT3(onProgress, pourcent, error, time);
  }

  INLINE void IBNLearner::distributeStop(const ApproximationScheme* approximationScheme,
                                         std::string_view           message) {
    setCurrentApproximationScheme(approximationScheme);
    if (onStop.hasListener()) GUM_EMIT1(onStop, message);
  }

  INLINE void IBNLearner::setEpsilon(double eps) {
    algoK2_.approximationScheme().setEpsilon(eps);
    greedyHillClimbing_.setEpsilon(eps);
    localSearchWithTabuList_.setEpsilon(eps);
    dag2BN_.setEpsilon(eps);
  }

  INLINE double IBNLearner::epsilon() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->epsilon();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::disableEpsilon() {
    algoK2_.approximationScheme().disableEpsilon();
    greedyHillClimbing_.disableEpsilon();
    localSearchWithTabuList_.disableEpsilon();
    dag2BN_.disableEpsilon();
  }

  INLINE void IBNLearner::enableEpsilon() {
    algoK2_.approximationScheme().enableEpsilon();
    greedyHillClimbing_.enableEpsilon();
    localSearchWithTabuList_.enableEpsilon();
    dag2BN_.enableEpsilon();
  }

  INLINE bool IBNLearner::isEnabledEpsilon() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->isEnabledEpsilon();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::setMinEpsilonRate(double rate) {
    algoK2_.approximationScheme().setMinEpsilonRate(rate);
    greedyHillClimbing_.setMinEpsilonRate(rate);
    localSearchWithTabuList_.setMinEpsilonRate(rate);
    dag2BN_.setMinEpsilonRate(rate);
  }

  INLINE double IBNLearner::minEpsilonRate() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->minEpsilonRate();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::disableMinEpsilonRate() {
    algoK2_.approximationScheme().disableMinEpsilonRate();
    greedyHillClimbing_.disableMinEpsilonRate();
    localSearchWithTabuList_.disableMinEpsilonRate();
    dag2BN_.disableMinEpsilonRate();
  }

  INLINE void IBNLearner::enableMinEpsilonRate() {
    algoK2_.approximationScheme().enableMinEpsilonRate();
    greedyHillClimbing_.enableMinEpsilonRate();
    localSearchWithTabuList_.enableMinEpsilonRate();
    dag2BN_.enableMinEpsilonRate();
  }

  INLINE bool IBNLearner::isEnabledMinEpsilonRate() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->isEnabledMinEpsilonRate();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::setMaxIter(Size max) {
    algoK2_.approximationScheme().setMaxIter(max);
    greedyHillClimbing_.setMaxIter(max);
    localSearchWithTabuList_.setMaxIter(max);
    dag2BN_.setMaxIter(max);
  }

  INLINE Size IBNLearner::maxIter() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->maxIter();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::disableMaxIter() {
    algoK2_.approximationScheme().disableMaxIter();
    greedyHillClimbing_.disableMaxIter();
    localSearchWithTabuList_.disableMaxIter();
    dag2BN_.disableMaxIter();
  }

  INLINE void IBNLearner::enableMaxIter() {
    algoK2_.approximationScheme().enableMaxIter();
    greedyHillClimbing_.enableMaxIter();
    localSearchWithTabuList_.enableMaxIter();
    dag2BN_.enableMaxIter();
  }

  INLINE bool IBNLearner::isEnabledMaxIter() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->isEnabledMaxIter();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::setMaxTime(double timeout) {
    algoK2_.approximationScheme().setMaxTime(timeout);
    greedyHillClimbing_.setMaxTime(timeout);
    localSearchWithTabuList_.setMaxTime(timeout);
    dag2BN_.setMaxTime(timeout);
  }

  INLINE double IBNLearner::maxTime() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->maxTime();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE double IBNLearner::currentTime() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->currentTime();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::disableMaxTime() {
    algoK2_.approximationScheme().disableMaxTime();
    greedyHillClimbing_.disableMaxTime();
    localSearchWithTabuList_.disableMaxTime();
    dag2BN_.disableMaxTime();
  }

  INLINE void IBNLearner::enableMaxTime() {
    algoK2_.approximationScheme().enableMaxTime();
    greedyHillClimbing_.enableMaxTime();
    localSearchWithTabuList_.enableMaxTime();
    dag2BN_.enableMaxTime();
  }

  INLINE bool IBNLearner::isEnabledMaxTime() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->isEnabledMaxTime();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::setPeriodSize(Size p) {
    algoK2_.approximationScheme().setPeriodSize(p);
    greedyHillClimbing_.setPeriodSize(p);
    localSearchWithTabuList_.setPeriodSize(p);
    dag2BN_.setPeriodSize(p);
  }

  INLINE Size IBNLearner::periodSize() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->periodSize();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE void IBNLearner::setVerbosity(bool v) {
    algoK2_.approximationScheme().setVerbosity(v);
    greedyHillClimbing_.setVerbosity(v);
    localSearchWithTabuList_.setVerbosity(v);
    dag2BN_.setVerbosity(v);
  }

  INLINE bool IBNLearner::verbosity() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->verbosity();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE IApproximationSchemeConfiguration::ApproximationSchemeSTATE
         IBNLearner::stateApproximationScheme() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->stateApproximationScheme();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE Size IBNLearner::nbrIterations() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->nbrIterations();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  INLINE const std::vector< double >& IBNLearner::history() const {
    if (currentAlgorithm_ != nullptr) return currentAlgorithm_->history();
    else GUM_ERROR(FatalError, "No chosen algorithm for learning")
  }

  // EM methods

  INLINE void IBNLearner::EMsetEpsilon(double eps) { dag2BN_.setEpsilon(eps); }

  INLINE double IBNLearner::EMEpsilon() const { return dag2BN_.epsilon(); }

  INLINE void IBNLearner::EMdisableEpsilon() { dag2BN_.disableEpsilon(); }

  INLINE void IBNLearner::EMenableEpsilon() { dag2BN_.enableEpsilon(); }

  INLINE bool IBNLearner::EMisEnabledEpsilon() const { return dag2BN_.isEnabledEpsilon(); }

  INLINE void IBNLearner::EMsetMinEpsilonRate(double rate) { dag2BN_.setMinEpsilonRate(rate); }

  INLINE double IBNLearner::EMMinEpsilonRate() const { return dag2BN_.minEpsilonRate(); }

  INLINE void IBNLearner::EMdisableMinEpsilonRate() { dag2BN_.disableMinEpsilonRate(); }

  INLINE void IBNLearner::EMenableMinEpsilonRate() { dag2BN_.enableMinEpsilonRate(); }

  INLINE bool IBNLearner::EMisEnabledMinEpsilonRate() const {
    return dag2BN_.isEnabledMinEpsilonRate();
  }

  INLINE void IBNLearner::EMsetMaxIter(Size max) { dag2BN_.setMaxIter(max); }

  INLINE Size IBNLearner::EMMaxIter() const { return dag2BN_.maxIter(); }

  INLINE void IBNLearner::EMdisableMaxIter() { dag2BN_.disableMaxIter(); }

  INLINE void IBNLearner::EMenableMaxIter() { dag2BN_.enableMaxIter(); }

  INLINE bool IBNLearner::EMisEnabledMaxIter() const { return dag2BN_.isEnabledMaxIter(); }

  INLINE void IBNLearner::EMsetMaxTime(double timeout) { dag2BN_.setMaxTime(timeout); }

  INLINE double IBNLearner::EMMaxTime() const { return dag2BN_.maxTime(); }

  INLINE double IBNLearner::EMCurrentTime() const { return dag2BN_.currentTime(); }

  INLINE void IBNLearner::EMdisableMaxTime() { dag2BN_.disableMaxTime(); }

  INLINE void IBNLearner::EMenableMaxTime() { dag2BN_.enableMaxTime(); }

  INLINE bool IBNLearner::EMisEnabledMaxTime() const { return dag2BN_.isEnabledMaxTime(); }

  INLINE void IBNLearner::EMsetPeriodSize(Size p) { dag2BN_.setPeriodSize(p); }

  INLINE Size IBNLearner::EMPeriodSize() const { return dag2BN_.periodSize(); }

  INLINE void IBNLearner::EMsetVerbosity(bool v) { dag2BN_.setVerbosity(v); }

  INLINE bool IBNLearner::EMVerbosity() const { return dag2BN_.verbosity(); }

  INLINE IApproximationSchemeConfiguration::ApproximationSchemeSTATE
         IBNLearner::EMStateApproximationScheme() const {
    return dag2BN_.stateApproximationScheme();
  }

  INLINE Size IBNLearner::EMnbrIterations() const { return dag2BN_.nbrIterations(); }

  INLINE const std::vector< double >& IBNLearner::EMHistory() const { return dag2BN_.history(); }

  /* namespace learning */
}   // namespace gum::learning
