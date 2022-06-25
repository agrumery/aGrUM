/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/** @file
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing, 3off2 and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to help IDE parser
#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>
#include <agrum/tools/graphs/undiGraph.h>

namespace gum {

  namespace learning {

    // returns the row filter
    INLINE DBRowGeneratorParser& GenericBNLearner::Database::parser() { return *_parser_; }

    // returns the modalities of the variables
    INLINE const std::vector< std::size_t >& GenericBNLearner::Database::domainSizes() const {
      return _domain_sizes_;
    }

    // returns the names of the variables in the database
    INLINE const std::vector< std::string >& GenericBNLearner::Database::names() const {
      return _database_.variableNames();
    }

    /// assign new weight to the rows of the learning database
    INLINE void GenericBNLearner::Database::setDatabaseWeight(const double new_weight) {
      if (_database_.nbRows() == std::size_t(0)) return;
      const double weight = new_weight / double(_database_.nbRows());
      _database_.setAllRowsWeight(weight);
    }

    // returns the node id corresponding to a variable name
    INLINE NodeId GenericBNLearner::Database::idFromName(const std::string& var_name) const {
      try {
        const auto cols = _database_.columnsFromVariableName(var_name);
        return _nodeId2cols_.first(cols[0]);
      } catch (...) {
        GUM_ERROR(MissingVariableInDatabase,
                  "Variable " << var_name << " could not be found in the database");
      }
    }


    // returns the variable name corresponding to a given node id
    INLINE const std::string& GenericBNLearner::Database::nameFromId(NodeId id) const {
      try {
        return _database_.variableName(_nodeId2cols_.second(id));
      } catch (...) {
        GUM_ERROR(MissingVariableInDatabase,
                  "Variable of Id " << id << " could not be found in the database");
      }
    }


    /// returns the internal database table
    INLINE const DatabaseTable& GenericBNLearner::Database::databaseTable() const {
      return _database_;
    }


    /// returns the set of missing symbols taken into account
    INLINE const std::vector< std::string >& GenericBNLearner::Database::missingSymbols() const {
      return _database_.missingSymbols();
    }


    /// returns the mapping between node ids and their columns in the database
    INLINE const Bijection< NodeId, std::size_t >&
                 GenericBNLearner::Database::nodeId2Columns() const {
      return _nodeId2cols_;
    }


    /// returns the number of records in the database
    INLINE std::size_t GenericBNLearner::Database::nbRows() const { return _database_.nbRows(); }


    /// returns the number of records in the database
    INLINE std::size_t GenericBNLearner::Database::size() const { return _database_.size(); }


    /// sets the weight of the ith record
    INLINE void GenericBNLearner::Database::setWeight(const std::size_t i, const double weight) {
      _database_.setWeight(i, weight);
    }


    /// returns the weight of the ith record
    INLINE double GenericBNLearner::Database::weight(const std::size_t i) const {
      return _database_.weight(i);
    }


    /// returns the weight of the whole database
    INLINE double GenericBNLearner::Database::weight() const { return _database_.weight(); }


    // ===========================================================================

    // returns the node id corresponding to a variable name
    INLINE NodeId GenericBNLearner::idFromName(const std::string& var_name) const {
      return scoreDatabase_.idFromName(var_name);
    }

    // returns the variable name corresponding to a given node id
    INLINE const std::string& GenericBNLearner::nameFromId(NodeId id) const {
      return scoreDatabase_.nameFromId(id);
    }

    /// assign new weight to the rows of the learning database
    INLINE void GenericBNLearner::setDatabaseWeight(const double new_weight) {
      scoreDatabase_.setDatabaseWeight(new_weight);
    }

    /// assign new weight to the ith row of the learning database
    INLINE void GenericBNLearner::setRecordWeight(const std::size_t i, const double new_weight) {
      scoreDatabase_.setWeight(i, new_weight);
    }

    /// returns the weight of the ith record
    INLINE double GenericBNLearner::recordWeight(const std::size_t i) const {
      return scoreDatabase_.weight(i);
    }

    /// returns the weight of the whole database
    INLINE double GenericBNLearner::databaseWeight() const { return scoreDatabase_.weight(); }

    // sets an initial DAG structure
    INLINE void GenericBNLearner::setInitialDAG(const DAG& dag) { initialDag_ = dag; }

    INLINE DAG GenericBNLearner::initialDAG() { return initialDag_; }

    // indicate that we wish to use an AIC score
    INLINE void GenericBNLearner::useScoreAIC() {
      scoreType_ = ScoreType::AIC;
      checkScorePriorCompatibility();
    }

    // indicate that we wish to use a BD score
    INLINE void GenericBNLearner::useScoreBD() {
      scoreType_ = ScoreType::BD;
      checkScorePriorCompatibility();
    }

    // indicate that we wish to use a BDeu score
    INLINE void GenericBNLearner::useScoreBDeu() {
      scoreType_ = ScoreType::BDeu;
      checkScorePriorCompatibility();
    }

    // indicate that we wish to use a BIC score
    INLINE void GenericBNLearner::useScoreBIC() {
      scoreType_ = ScoreType::BIC;
      checkScorePriorCompatibility();
    }

    // indicate that we wish to use a K2 score
    INLINE void GenericBNLearner::useScoreK2() {
      scoreType_ = ScoreType::K2;
      checkScorePriorCompatibility();
    }

    // indicate that we wish to use a Log2Likelihood score
    INLINE void GenericBNLearner::useScoreLog2Likelihood() {
      scoreType_ = ScoreType::LOG2LIKELIHOOD;
      checkScorePriorCompatibility();
    }

    // sets the max indegree
    INLINE void GenericBNLearner::setMaxIndegree(Size max_indegree) {
      constraintIndegree_.setMaxIndegree(max_indegree);
    }

    // indicate that we wish to use 3off2
    INLINE void GenericBNLearner::use3off2() {
      selectedAlgo_ = AlgoType::THREE_OFF_TWO;
      algoMiic3off2_.set3of2Behaviour();
    }

    // indicate that we wish to use 3off2
    INLINE void GenericBNLearner::useMIIC() {
      selectedAlgo_ = AlgoType::MIIC;
      algoMiic3off2_.setMiicBehaviour();
    }

    /// indicate that we wish to use the NML correction for 3off2
    INLINE void GenericBNLearner::useNMLCorrection() {
      kmode3Off2_ = CorrectedMutualInformation::KModeTypes::NML;
    }

    /// indicate that we wish to use the MDL correction for 3off2
    INLINE void GenericBNLearner::useMDLCorrection() {
      kmode3Off2_ = CorrectedMutualInformation::KModeTypes::MDL;
    }

    /// indicate that we wish to use the NoCorr correction for 3off2
    INLINE void GenericBNLearner::useNoCorrection() {
      kmode3Off2_ = CorrectedMutualInformation::KModeTypes::NoCorr;
    }

    /// get the list of arcs hiding latent variables
    INLINE const std::vector< Arc > GenericBNLearner::latentVariables() const {
      return algoMiic3off2_.latentVariables();
    }

    // indicate that we wish to use a K2 algorithm
    INLINE void GenericBNLearner::useK2(const Sequence< NodeId >& order) {
      selectedAlgo_ = AlgoType::K2;
      algoK2_.setOrder(order);
    }

    // indicate that we wish to use a K2 algorithm
    INLINE void GenericBNLearner::useK2(const std::vector< NodeId >& order) {
      selectedAlgo_ = AlgoType::K2;
      algoK2_.setOrder(order);
    }

    // indicate that we wish to use a greedy hill climbing algorithm
    INLINE void GenericBNLearner::useGreedyHillClimbing() {
      selectedAlgo_ = AlgoType::GREEDY_HILL_CLIMBING;
    }

    // indicate that we wish to use a local search with tabu list
    INLINE void GenericBNLearner::useLocalSearchWithTabuList(Size tabu_size, Size nb_decrease) {
      selectedAlgo_        = AlgoType::LOCAL_SEARCH_WITH_TABU_LIST;
      nbDecreasingChanges_ = nb_decrease;
      constraintTabuList_.setTabuListSize(tabu_size);
      localSearchWithTabuList_.setMaxNbDecreasingChanges(nb_decrease);
    }

    /// use The EM algorithm to learn paramters
    INLINE void GenericBNLearner::useEM(const double epsilon) { epsilonEM_ = epsilon; }


    INLINE bool GenericBNLearner::hasMissingValues() const {
      return scoreDatabase_.databaseTable().hasMissingValues();
    }

    // assign a set of forbidden edges
    INLINE void GenericBNLearner::setPossibleEdges(const EdgeSet& set) {
      constraintPossibleEdges_.setEdges(set);
    }
    // assign a set of forbidden edges from an UndiGraph
    INLINE void GenericBNLearner::setPossibleSkeleton(const gum::UndiGraph& g) {
      setPossibleEdges(g.edges());
    }

    // assign a new possible edge
    INLINE void GenericBNLearner::addPossibleEdge(const Edge& edge) {
      constraintPossibleEdges_.addEdge(edge);
    }

    // remove a forbidden edge
    INLINE void GenericBNLearner::erasePossibleEdge(const Edge& edge) {
      constraintPossibleEdges_.eraseEdge(edge);
    }

    // assign a new forbidden edge
    INLINE void GenericBNLearner::addPossibleEdge(const NodeId tail, const NodeId head) {
      addPossibleEdge(Edge(tail, head));
    }

    // remove a forbidden edge
    INLINE void GenericBNLearner::erasePossibleEdge(const NodeId tail, const NodeId head) {
      erasePossibleEdge(Edge(tail, head));
    }

    // assign a new forbidden edge
    INLINE void GenericBNLearner::addPossibleEdge(const std::string& tail,
                                                  const std::string& head) {
      addPossibleEdge(Edge(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden edge
    INLINE void GenericBNLearner::erasePossibleEdge(const std::string& tail,
                                                    const std::string& head) {
      erasePossibleEdge(Edge(idFromName(tail), idFromName(head)));
    }

    // assign a set of forbidden arcs
    INLINE void GenericBNLearner::setForbiddenArcs(const ArcSet& set) {
      constraintForbiddenArcs_.setArcs(set);
    }

    // assign a new forbidden arc
    INLINE void GenericBNLearner::addForbiddenArc(const Arc& arc) {
      constraintForbiddenArcs_.addArc(arc);
    }

    // remove a forbidden arc
    INLINE void GenericBNLearner::eraseForbiddenArc(const Arc& arc) {
      constraintForbiddenArcs_.eraseArc(arc);
    }

    // assign a new forbidden arc
    INLINE void GenericBNLearner::addForbiddenArc(const NodeId tail, const NodeId head) {
      addForbiddenArc(Arc(tail, head));
    }

    // remove a forbidden arc
    INLINE void GenericBNLearner::eraseForbiddenArc(const NodeId tail, const NodeId head) {
      eraseForbiddenArc(Arc(tail, head));
    }

    // assign a new forbidden arc
    INLINE void GenericBNLearner::addForbiddenArc(const std::string& tail,
                                                  const std::string& head) {
      addForbiddenArc(Arc(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden arc
    INLINE void GenericBNLearner::eraseForbiddenArc(const std::string& tail,
                                                    const std::string& head) {
      eraseForbiddenArc(Arc(idFromName(tail), idFromName(head)));
    }

    // assign a set of forbidden arcs
    INLINE void GenericBNLearner::setMandatoryArcs(const ArcSet& set) {
      constraintMandatoryArcs_.setArcs(set);
    }

    // assign a new forbidden arc
    INLINE void GenericBNLearner::addMandatoryArc(const Arc& arc) {
      constraintMandatoryArcs_.addArc(arc);
    }

    // remove a forbidden arc
    INLINE void GenericBNLearner::eraseMandatoryArc(const Arc& arc) {
      constraintMandatoryArcs_.eraseArc(arc);
    }

    // assign a new forbidden arc
    INLINE void GenericBNLearner::addMandatoryArc(const std::string& tail,
                                                  const std::string& head) {
      addMandatoryArc(Arc(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden arc
    INLINE void GenericBNLearner::eraseMandatoryArc(const std::string& tail,
                                                    const std::string& head) {
      eraseMandatoryArc(Arc(idFromName(tail), idFromName(head)));
    }

    // assign a new forbidden arc
    INLINE void GenericBNLearner::addMandatoryArc(const NodeId tail, const NodeId head) {
      addMandatoryArc(Arc(tail, head));
    }

    // remove a forbidden arc
    INLINE void GenericBNLearner::eraseMandatoryArc(const NodeId tail, const NodeId head) {
      eraseMandatoryArc(Arc(tail, head));
    }

    // sets a partial order on the nodes
    INLINE void GenericBNLearner::setSliceOrder(const NodeProperty< NodeId >& slice_order) {
      constraintSliceOrder_ = StructuralConstraintSliceOrder(slice_order);
    }

    INLINE void
       GenericBNLearner::setSliceOrder(const std::vector< std::vector< std::string > >& slices) {
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

    // sets the prior weight
    INLINE void GenericBNLearner::_setPriorWeight_(double weight) {
      if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

      priorWeight_ = weight;
      checkScorePriorCompatibility();
    }

    // use the prior smoothing
    INLINE void GenericBNLearner::useNoPrior() {
      priorType_ = BNLearnerPriorType::NO_prior;
      checkScorePriorCompatibility();
    }

    // use the prior smoothing
    INLINE void GenericBNLearner::useSmoothingPrior(double weight) {
      if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

      priorType_ = BNLearnerPriorType::SMOOTHING;
      _setPriorWeight_(weight);

      checkScorePriorCompatibility();
    }

    // use the Dirichlet prior
    INLINE void GenericBNLearner::useDirichletPrior(const std::string& filename, double weight) {
      if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

      priorDbname_ = filename;
      priorType_   = BNLearnerPriorType::DIRICHLET_FROM_DATABASE;
      _setPriorWeight_(weight);

      checkScorePriorCompatibility();
    }


    // use the prior BDeu
    INLINE void GenericBNLearner::useBDeuPrior(double weight) {
      if (weight < 0) { GUM_ERROR(OutOfBounds, "the weight of the prior must be positive") }

      priorType_ = BNLearnerPriorType::BDEU;
      _setPriorWeight_(weight);

      checkScorePriorCompatibility();
    }


    // returns the type (as a string) of a given prior
    INLINE PriorType GenericBNLearner::getPriorType_() const {
      switch (priorType_) {
        case BNLearnerPriorType::NO_prior:
          return PriorType::NoPriorType;

        case BNLearnerPriorType::SMOOTHING:
          return PriorType::SmoothingPriorType;

        case BNLearnerPriorType::DIRICHLET_FROM_DATABASE:
          return PriorType::DirichletPriorType;

        case BNLearnerPriorType::BDEU:
          return PriorType::BDeuPriorType;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "GenericBNLearner getPriorType does "
                    "not support yet this prior");
      }
    }

    // returns the names of the variables in the database
    INLINE const std::vector< std::string >& GenericBNLearner::names() const {
      return scoreDatabase_.names();
    }

    // returns the modalities  of the variables in the database
    INLINE const std::vector< std::size_t >& GenericBNLearner::domainSizes() const {
      return scoreDatabase_.domainSizes();
    }

    // returns the modalities  of a variable in the database
    INLINE Size GenericBNLearner::domainSize(NodeId var) const {
      return scoreDatabase_.domainSizes()[var];
    }
    // returns the modalities  of a variables in the database
    INLINE Size GenericBNLearner::domainSize(const std::string& var) const {
      return scoreDatabase_.domainSizes()[idFromName(var)];
    }

    /// returns the current database rows' ranges used for learning
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
                 GenericBNLearner::databaseRanges() const {
      return ranges_;
    }

    /// reset the ranges to the one range corresponding to the whole database
    INLINE void GenericBNLearner::clearDatabaseRanges() { ranges_.clear(); }

    /// returns the database used by the BNLearner
    INLINE const DatabaseTable& GenericBNLearner::database() const {
      return scoreDatabase_.databaseTable();
    }

    INLINE Size GenericBNLearner::nbCols() const { return scoreDatabase_.domainSizes().size(); }

    INLINE Size GenericBNLearner::nbRows() const { return scoreDatabase_.databaseTable().size(); }

    // sets the number max of threads that can be used
    INLINE void GenericBNLearner::setNumberOfThreads(Size nb) {
      ThreadNumberManager::setNumberOfThreads(nb);
      if (score_ != nullptr) score_->setNumberOfThreads(nb);
    }

  } /* namespace learning */
} /* namespace gum */
