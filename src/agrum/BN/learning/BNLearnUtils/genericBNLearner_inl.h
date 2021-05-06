/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

// to help IDE parser
#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>
#include <agrum/tools/graphs/undiGraph.h>

namespace gum {

  namespace learning {

    // returns the row filter
    INLINE DBRowGeneratorParser<>& genericBNLearner::Database::parser() {
      return *parser__;
    }

    // returns the modalities of the variables
    INLINE const std::vector< std::size_t >&
                 genericBNLearner::Database::domainSizes() const {
      return domain_sizes__;
    }

    // returns the names of the variables in the database
    INLINE const std::vector< std::string >&
                 genericBNLearner::Database::names() const {
      return database__.variableNames();
    }

    /// assign new weight to the rows of the learning database
    INLINE void
       genericBNLearner::Database::setDatabaseWeight(const double new_weight) {
      if (database__.nbRows() == std::size_t(0)) return;
      const double weight = new_weight / double(database__.nbRows());
      database__.setAllRowsWeight(weight);
    }

    // returns the node id corresponding to a variable name
    INLINE NodeId
       genericBNLearner::Database::idFromName(const std::string& var_name) const {
      try {
        const auto cols = database__.columnsFromVariableName(var_name);
        return nodeId2cols__.first(cols[0]);
      } catch (...) {
        GUM_ERROR(MissingVariableInDatabase,
                  "Variable " << var_name
                              << " could not be found in the database");
      }
    }


    // returns the variable name corresponding to a given node id
    INLINE const std::string&
                 genericBNLearner::Database::nameFromId(NodeId id) const {
      try {
        return database__.variableName(nodeId2cols__.second(id));
      } catch (...) {
        GUM_ERROR(MissingVariableInDatabase,
                  "Variable of Id " << id
                                    << " could not be found in the database");
      }
    }


    /// returns the internal database table
    INLINE const DatabaseTable<>&
                 genericBNLearner::Database::databaseTable() const {
      return database__;
    }


    /// returns the set of missing symbols taken into account
    INLINE const std::vector< std::string >&
                 genericBNLearner::Database::missingSymbols() const {
      return database__.missingSymbols();
    }


    /// returns the mapping between node ids and their columns in the database
    INLINE const Bijection< NodeId, std::size_t >&
                 genericBNLearner::Database::nodeId2Columns() const {
      return nodeId2cols__;
    }


    /// returns the number of records in the database
    INLINE std::size_t genericBNLearner::Database::nbRows() const {
      return database__.nbRows();
    }


    /// returns the number of records in the database
    INLINE std::size_t genericBNLearner::Database::size() const {
      return database__.size();
    }


    /// sets the weight of the ith record
    INLINE void genericBNLearner::Database::setWeight(const std::size_t i,
                                                      const double      weight) {
      database__.setWeight(i, weight);
    }


    /// returns the weight of the ith record
    INLINE double genericBNLearner::Database::weight(const std::size_t i) const {
      return database__.weight(i);
    }


    /// returns the weight of the whole database
    INLINE double genericBNLearner::Database::weight() const {
      return database__.weight();
    }


    // ===========================================================================

    // returns the node id corresponding to a variable name
    INLINE NodeId genericBNLearner::idFromName(const std::string& var_name) const {
      return score_database__.idFromName(var_name);
    }

    // returns the variable name corresponding to a given node id
    INLINE const std::string& genericBNLearner::nameFromId(NodeId id) const {
      return score_database__.nameFromId(id);
    }

    /// assign new weight to the rows of the learning database
    INLINE void genericBNLearner::setDatabaseWeight(const double new_weight) {
      score_database__.setDatabaseWeight(new_weight);
    }

    /// assign new weight to the ith row of the learning database
    INLINE void genericBNLearner::setRecordWeight(const std::size_t i,
                                                  const double      new_weight) {
      score_database__.setWeight(i, new_weight);
    }

    /// returns the weight of the ith record
    INLINE double genericBNLearner::recordWeight(const std::size_t i) const {
      return score_database__.weight(i);
    }

    /// returns the weight of the whole database
    INLINE double genericBNLearner::databaseWeight() const {
      return score_database__.weight();
    }

    // sets an initial DAG structure
    INLINE void genericBNLearner::setInitialDAG(const DAG& dag) {
      initial_dag__ = dag;
    }

    // indicate that we wish to use an AIC score
    INLINE void genericBNLearner::useScoreAIC() {
      score_type__ = ScoreType::AIC;
      checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a BD score
    INLINE void genericBNLearner::useScoreBD() {
      score_type__ = ScoreType::BD;
      checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a BDeu score
    INLINE void genericBNLearner::useScoreBDeu() {
      score_type__ = ScoreType::BDeu;
      checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a BIC score
    INLINE void genericBNLearner::useScoreBIC() {
      score_type__ = ScoreType::BIC;
      checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a K2 score
    INLINE void genericBNLearner::useScoreK2() {
      score_type__ = ScoreType::K2;
      checkScoreAprioriCompatibility();
    }

    // indicate that we wish to use a Log2Likelihood score
    INLINE void genericBNLearner::useScoreLog2Likelihood() {
      score_type__ = ScoreType::LOG2LIKELIHOOD;
      checkScoreAprioriCompatibility();
    }

    // sets the max indegree
    INLINE void genericBNLearner::setMaxIndegree(Size max_indegree) {
      constraint_Indegree__.setMaxIndegree(max_indegree);
    }

    // indicate that we wish to use 3off2
    INLINE void genericBNLearner::use3off2() {
      selected_algo__ = AlgoType::MIIC_THREE_OFF_TWO;
      miic_3off2__.set3off2Behaviour();
    }

    // indicate that we wish to use 3off2
    INLINE void genericBNLearner::useMIIC() {
      selected_algo__ = AlgoType::MIIC_THREE_OFF_TWO;
      miic_3off2__.setMiicBehaviour();
    }

    /// indicate that we wish to use the NML correction for 3off2
    INLINE void genericBNLearner::useNML() {
      if (selected_algo__ != AlgoType::MIIC_THREE_OFF_TWO) {
        GUM_ERROR(OperationNotAllowed,
                  "You must use the 3off2 algorithm before selecting "
                     << "the NML score");
      }
      kmode_3off2__ = CorrectedMutualInformation<>::KModeTypes::NML;
    }

    /// indicate that we wish to use the MDL correction for 3off2
    INLINE void genericBNLearner::useMDL() {
      if (selected_algo__ != AlgoType::MIIC_THREE_OFF_TWO) {
        GUM_ERROR(OperationNotAllowed,
                  "You must use the 3off2 algorithm before selecting "
                     << "the MDL score");
      }
      kmode_3off2__ = CorrectedMutualInformation<>::KModeTypes::MDL;
    }

    /// indicate that we wish to use the NoCorr correction for 3off2
    INLINE void genericBNLearner::useNoCorr() {
      if (selected_algo__ != AlgoType::MIIC_THREE_OFF_TWO) {
        GUM_ERROR(OperationNotAllowed,
                  "You must use the 3off2 algorithm before selecting "
                     << "the NoCorr score");
      }
      kmode_3off2__ = CorrectedMutualInformation<>::KModeTypes::NoCorr;
    }

    /// get the list of arcs hiding latent variables
    INLINE const std::vector< Arc > genericBNLearner::latentVariables() const {
      if (selected_algo__ != AlgoType::MIIC_THREE_OFF_TWO) {
        GUM_ERROR(OperationNotAllowed,
                  "You must use the 3off2 algorithm before selecting "
                     << "the latentVariables method");
      }
      return miic_3off2__.latentVariables();
    }

    // indicate that we wish to use a K2 algorithm
    INLINE void genericBNLearner::useK2(const Sequence< NodeId >& order) {
      selected_algo__ = AlgoType::K2;
      K2__.setOrder(order);
    }

    // indicate that we wish to use a K2 algorithm
    INLINE void genericBNLearner::useK2(const std::vector< NodeId >& order) {
      selected_algo__ = AlgoType::K2;
      K2__.setOrder(order);
    }

    // indicate that we wish to use a greedy hill climbing algorithm
    INLINE void genericBNLearner::useGreedyHillClimbing() {
      selected_algo__ = AlgoType::GREEDY_HILL_CLIMBING;
    }

    // indicate that we wish to use a local search with tabu list
    INLINE void genericBNLearner::useLocalSearchWithTabuList(Size tabu_size,
                                                             Size nb_decrease) {
      selected_algo__ = AlgoType::LOCAL_SEARCH_WITH_TABU_LIST;
      constraint_TabuList__.setTabuListSize(tabu_size);
      local_search_with_tabu_list__.setMaxNbDecreasingChanges(nb_decrease);
    }

    /// use The EM algorithm to learn paramters
    INLINE void genericBNLearner::useEM(const double epsilon) {
      EMepsilon__ = epsilon;
    }


    INLINE bool genericBNLearner::hasMissingValues() const {
      return score_database__.databaseTable().hasMissingValues();
    }

    // assign a set of forbidden edges
    INLINE void genericBNLearner::setPossibleEdges(const EdgeSet& set) {
      constraint_PossibleEdges__.setEdges(set);
    }
    // assign a set of forbidden edges from an UndiGraph
    INLINE void genericBNLearner::setPossibleSkeleton(const gum::UndiGraph& g) {
      setPossibleEdges(g.edges());
    }

    // assign a new possible edge
    INLINE void genericBNLearner::addPossibleEdge(const Edge& edge) {
      constraint_PossibleEdges__.addEdge(edge);
    }

    // remove a forbidden edge
    INLINE void genericBNLearner::erasePossibleEdge(const Edge& edge) {
      constraint_PossibleEdges__.eraseEdge(edge);
    }

    // assign a new forbidden edge
    INLINE void genericBNLearner::addPossibleEdge(const NodeId tail,
                                                  const NodeId head) {
      addPossibleEdge(Edge(tail, head));
    }

    // remove a forbidden edge
    INLINE void genericBNLearner::erasePossibleEdge(const NodeId tail,
                                                    const NodeId head) {
      erasePossibleEdge(Edge(tail, head));
    }

    // assign a new forbidden edge
    INLINE void genericBNLearner::addPossibleEdge(const std::string& tail,
                                                  const std::string& head) {
      addPossibleEdge(Edge(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden edge
    INLINE void genericBNLearner::erasePossibleEdge(const std::string& tail,
                                                    const std::string& head) {
      erasePossibleEdge(Edge(idFromName(tail), idFromName(head)));
    }

    // assign a set of forbidden arcs
    INLINE void genericBNLearner::setForbiddenArcs(const ArcSet& set) {
      constraint_ForbiddenArcs__.setArcs(set);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addForbiddenArc(const Arc& arc) {
      constraint_ForbiddenArcs__.addArc(arc);
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseForbiddenArc(const Arc& arc) {
      constraint_ForbiddenArcs__.eraseArc(arc);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addForbiddenArc(const NodeId tail,
                                                  const NodeId head) {
      addForbiddenArc(Arc(tail, head));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseForbiddenArc(const NodeId tail,
                                                    const NodeId head) {
      eraseForbiddenArc(Arc(tail, head));
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addForbiddenArc(const std::string& tail,
                                                  const std::string& head) {
      addForbiddenArc(Arc(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseForbiddenArc(const std::string& tail,
                                                    const std::string& head) {
      eraseForbiddenArc(Arc(idFromName(tail), idFromName(head)));
    }

    // assign a set of forbidden arcs
    INLINE void genericBNLearner::setMandatoryArcs(const ArcSet& set) {
      constraint_MandatoryArcs__.setArcs(set);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addMandatoryArc(const Arc& arc) {
      constraint_MandatoryArcs__.addArc(arc);
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseMandatoryArc(const Arc& arc) {
      constraint_MandatoryArcs__.eraseArc(arc);
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addMandatoryArc(const std::string& tail,
                                                  const std::string& head) {
      addMandatoryArc(Arc(idFromName(tail), idFromName(head)));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseMandatoryArc(const std::string& tail,
                                                    const std::string& head) {
      eraseMandatoryArc(Arc(idFromName(tail), idFromName(head)));
    }

    // assign a new forbidden arc
    INLINE void genericBNLearner::addMandatoryArc(const NodeId tail,
                                                  const NodeId head) {
      addMandatoryArc(Arc(tail, head));
    }

    // remove a forbidden arc
    INLINE void genericBNLearner::eraseMandatoryArc(const NodeId tail,
                                                    const NodeId head) {
      eraseMandatoryArc(Arc(tail, head));
    }

    // sets a partial order on the nodes
    INLINE void
       genericBNLearner::setSliceOrder(const NodeProperty< NodeId >& slice_order) {
      constraint_SliceOrder__ = StructuralConstraintSliceOrder(slice_order);
    }

    INLINE void genericBNLearner::setSliceOrder(
       const std::vector< std::vector< std::string > >& slices) {
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

    // sets the apriori weight
    INLINE void genericBNLearner::setAprioriWeight__(double weight) {
      if (weight < 0) {
        GUM_ERROR(OutOfBounds, "the weight of the apriori must be positive")
      }

      apriori_weight__ = weight;
      checkScoreAprioriCompatibility();
    }

    // use the apriori smoothing
    INLINE void genericBNLearner::useNoApriori() {
      apriori_type__ = AprioriType::NO_APRIORI;
      checkScoreAprioriCompatibility();
    }

    // use the apriori smoothing
    INLINE void genericBNLearner::useAprioriSmoothing(double weight) {
      if (weight < 0) {
        GUM_ERROR(OutOfBounds, "the weight of the apriori must be positive")
      }

      apriori_type__ = AprioriType::SMOOTHING;
      setAprioriWeight__(weight);

      checkScoreAprioriCompatibility();
    }

    // use the Dirichlet apriori
    INLINE void genericBNLearner::useAprioriDirichlet(const std::string& filename,
                                                      double             weight) {
      if (weight < 0) {
        GUM_ERROR(OutOfBounds, "the weight of the apriori must be positive")
      }

      apriori_dbname__ = filename;
      apriori_type__   = AprioriType::DIRICHLET_FROM_DATABASE;
      setAprioriWeight__(weight);

      checkScoreAprioriCompatibility();
    }


    // use the apriori BDeu
    INLINE void genericBNLearner::useAprioriBDeu(double weight) {
      if (weight < 0) {
        GUM_ERROR(OutOfBounds, "the weight of the apriori must be positive")
      }

      apriori_type__ = AprioriType::BDEU;
      setAprioriWeight__(weight);

      checkScoreAprioriCompatibility();
    }


    // returns the type (as a string) of a given apriori
    INLINE const std::string& genericBNLearner::getAprioriType__() const {
      switch (apriori_type__) {
        case AprioriType::NO_APRIORI:
          return AprioriNoApriori<>::type::type;

        case AprioriType::SMOOTHING:
          return AprioriSmoothing<>::type::type;

        case AprioriType::DIRICHLET_FROM_DATABASE:
          return AprioriDirichletFromDatabase<>::type::type;

        case AprioriType::BDEU:
          return AprioriBDeu<>::type::type;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "genericBNLearner getAprioriType does "
                    "not support yet this apriori");
      }
    }

    // returns the names of the variables in the database
    INLINE const std::vector< std::string >& genericBNLearner::names() const {
      return score_database__.names();
    }

    // returns the modalities  of the variables in the database
    INLINE const std::vector< std::size_t >&
                 genericBNLearner::domainSizes() const {
      return score_database__.domainSizes();
    }

    // returns the modalities  of a variable in the database
    INLINE Size genericBNLearner::domainSize(NodeId var) const {
      return score_database__.domainSizes()[var];
    }
    // returns the modalities  of a variables in the database
    INLINE Size genericBNLearner::domainSize(const std::string& var) const {
      return score_database__.domainSizes()[idFromName(var)];
    }

    /// returns the current database rows' ranges used for learning
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
                 genericBNLearner::databaseRanges() const {
      return ranges__;
    }

    /// reset the ranges to the one range corresponding to the whole database
    INLINE void genericBNLearner::clearDatabaseRanges() { ranges__.clear(); }

    /// returns the database used by the BNLearner
    INLINE const DatabaseTable<>& genericBNLearner::database() const {
      return score_database__.databaseTable();
    }

    INLINE Size genericBNLearner::nbCols() const {
      return score_database__.domainSizes().size();
    }

    INLINE Size genericBNLearner::nbRows() const {
      return score_database__.databaseTable().size();
    }
  } /* namespace learning */
} /* namespace gum */
