/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief The Miic algorithm
 *
 * The Miic class implements the miic algorithm based on
 * https://doi.org/10.1371/journal.pcbi.1005662.
 * It starts by eliminating edges that correspond to independent variables to
 * build the skeleton of the graph, and then directs the remaining edges to get
 * an essential graph. Latent variables can be detected using bi-directed arcs.
 *
 * Miic allows the option of adding constraints on the skeleton construction
 * such as: a maximum number of parents, mandatory arcs, forbidden arcs or an order between the
 * variables.
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN(_at_LIP6) and Maria Virginia
 * RUIZ CUEVAS
 */
#ifndef GUM_LEARNING_MIIC_H
#define GUM_LEARNING_MIIC_H

#include <string>
#include <vector>

#include <agrum/config.h>

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/heap.h>
#include <agrum/base/graphs/PDAG.h>
#include <agrum/BN/learning/correctedMutualInformation.h>

#include "agrum/base/graphs/algorithms/MeekRules.h"

#define GUM_SL_EMIT(x, y, action, explain)                                                \
  {                                                                                       \
    std::ostringstream action_stream;                                                     \
    action_stream << action;                                                              \
    std::ostringstream explain_stream;                                                    \
    explain_stream << explain;                                                            \
    GUM_EMIT4(onStructuralModification, x, y, action_stream.str(), explain_stream.str()); \
  }

namespace gum {

  namespace learning {
    using CondThreePoints = std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >;
    using CondRanking     = std::pair< CondThreePoints*, double >;

    using ThreePoints = std::tuple< NodeId, NodeId, NodeId >;
    using Ranking     = std::pair< ThreePoints*, double >;

    using ProbabilisticRanking = std::tuple< ThreePoints*, double, double, double >;

    class GreaterPairOn2nd {
      public:
      bool operator()(const CondRanking& e1, const CondRanking& e2) const;
    };

    class GreaterAbsPairOn2nd {
      public:
      bool operator()(const Ranking& e1, const Ranking& e2) const;
    };

    class GreaterTupleOnLast {
      public:
      bool operator()(const ProbabilisticRanking& e1, const ProbabilisticRanking& e2) const;
    };

    /**
     * @class Miic
     * @brief The Miic learning algorithm
     *
     * The Miic class implements the miic algorithm based on
     * https://doi.org/10.1371/journal.pcbi.1005662.
     * It starts by eliminating edges that correspond to independent variables to
     * build the skeleton of the graph, and then directs the remaining edges to get
     * an essential graph. Latent variables can be detected using bi-directed arcs.
     *
     * Miic allows the option of adding constraints on the skeleton construction
     * such as: a maximum number of parents, mandatory arcs, forbidden arcs or an order between the
     * variables.
     *
     *
     * @ingroup learning_group
     */
    class Miic: public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      Miic();

      /// default constructor with maxLog
      explicit Miic(int maxLog);

      /// copy constructor
      Miic(const Miic& from);

      /// move constructor
      Miic(Miic&& from);

      /// destructor
      ~Miic() override;

      /// @}

      /// copy operator
      Miic& operator=(const Miic& from);

      /// move operator
      Miic& operator=(Miic&& from);

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// learns the skeleton of a MixedGraph (no orientation).
      /** @param mutualInformation A mutual information instance that will do the
       * computations and has loaded the database.
       * @param graph the MixedGraph we start from for the learning
       * */
      MixedGraph learnSkeleton(CorrectedMutualInformation& mutualInformation, MixedGraph graph);

      /// learns the structure of a MixedGraph (Meek rules not used here).
      /** @param mutualInformation A mutual information instance that will do the
       * computations and has loaded the database.
       * @param graph the MixedGraph we start from for the learning
       * */
      MixedGraph learnMixedStructure(CorrectedMutualInformation& mutualInformation,
                                     MixedGraph                  graph);

      /// learns the structure of an Essential Graph
      /** @param mutualInformation A mutual information instance that will do the
       * computations and has loaded the database.
       * @param graph the MixedGraph we start from for the learning
       * */
      PDAG learnPDAG(CorrectedMutualInformation& mutualInformation, MixedGraph graph);

      /// learns the structure of a Bayesian network, i.e. a DAG, by first learning
      /// an Essential graph and then directing the remaining edges.
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database
       * @param graph the MixedGraph we start from for the learning
       */
      DAG learnStructure(CorrectedMutualInformation& I, MixedGraph graph);

      /// learns the structure and the parameters of a BN
      /** @param selector A selector class that computes the best changes that
       * can be applied and that enables the user to get them very easily.
       * Typically, the selector is a GraphChangesSelector4DiGraph<SCORE,
       * STRUCT_CONSTRAINT, GRAPH_CHANGES_GENERATOR>.
       * @param estimator A estimator.
       * @param names The variables names.
       * @param modal the domain sizes of the random variables observed in the
       * database
       * @param translator The cell translator to use.
       * @param initial_dag the DAG we start from for our learning */
      template < typename GUM_SCALAR = double,
                 typename GRAPH_CHANGES_SELECTOR,
                 typename PARAM_ESTIMATOR >
      BayesNet< GUM_SCALAR > learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                     PARAM_ESTIMATOR&        estimator,
                                     DAG                     initial_dag = DAG());

      /// get the list of arcs hiding latent variables
      const std::vector< Arc > latentVariables() const;

      /// Set a ensemble of constraints for the orientation phase
      void addConstraints(HashTable< std::pair< NodeId, NodeId >, char > constraints);
      /// @}

      /// Set ForbiddenGraph (resp. MadatoryGraph) which contains the forbidden (resp. mandatory)
      /// arcs.
      void setForbiddenGraph(gum::DiGraph forbidGraph);
      void setMandatoryGraph(gum::DAG mandaGraph);
      void setMaxIndegree(gum::Size n);

      /// Set a ensemble of constraints for the learning/orientation phase
      // void testNodeProperty(const NodeProperty< NodeId >& order);

      protected:
      // ##########################################################################
      /// @name Main phases
      // ##########################################################################
      /// @{

      /// Initiation phase
      /**
       * We go over all edges and test if the variables are marginally independent.
       * If they are, the edge is deleted. If not, the best contributor is found.
       *
       * @param mutualInformation A mutual information instance that will do the
       * computations and has loaded the database.
       * @param graph the MixedGraph we start from for the learning
       * @param sepSet the separation set for independent couples, here set to {}
       * @param rank the heap of ranks of the algorithm
       */
      void initiation_(CorrectedMutualInformation& mutualInformation,
                       MixedGraph&                 graph,
                       HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                       Heap< CondRanking, GreaterPairOn2nd >&                           rank);

      /// Iteration phase
      /**
       * As long as we find important nodes for edges, we go over them to see if
       * we can assess the conditional independence of the variables.
       *
       * @param mutualInformation A mutual information instance that will do the
       * computations and has loaded the database.
       * @param graph the MixedGraph returned from the previous phase
       * @param sepSet the separation set for independent couples, built during
       * the iterations of the phase
       * @param rank the heap of ranks of the algorithm
       */
      void iteration_(CorrectedMutualInformation& mutualInformation,
                      MixedGraph&                 graph,
                      HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                      Heap< CondRanking, GreaterPairOn2nd >&                           rank);

      /// Orientation phase from the MIIC algorithm, returns a mixed graph that
      /// may contain circles
      /** @param mutualInformation A mutual information instance that will do the
       * computations and has loaded the database.
       * @param graph the MixedGraph returned from the previous phase
       * @param sepSet the separation set for independent couples, built during
       * the previous phase
       */
      void orientationMiic_(
          CorrectedMutualInformation&                                            mutualInformation,
          MixedGraph&                                                            graph,
          const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet);
      /// @}

      /// finds the best contributor node for a pair given a conditioning set
      /**@param x first node
       * @param y second node
       * @param ui conditioning set
       * @param mutualInformation A mutual information instance that will do the
       * computations and has loaded the database.
       * @param graph containing the assessed nodes
       * @param rank the heap of ranks of the algorithm
       */
      void findBestContributor_(NodeId                                 x,
                                NodeId                                 y,
                                const std::vector< NodeId >&           ui,
                                const MixedGraph&                      graph,
                                CorrectedMutualInformation&            mutualInformation,
                                Heap< CondRanking, GreaterPairOn2nd >& rank);

      /// gets the list of unshielded triples in the graph in decreasing value of
      ///|I'(x, y, z|{ui})|
      /**@param graph graph in which to find the triples
       *@param I mutual information object to compute the scores
       *@param sep_set hashtable storing the separation sets for pairs of variables
       */
      std::vector< Ranking > unshieldedTriples_(
          const MixedGraph&                                                      graph,
          CorrectedMutualInformation&                                            mutualInformation,
          const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet);

      /// gets the list of unshielded triples in the graph in decreasing value of
      ///|I'(x, y, z|{ui})|, prepares the orientation matrix for MIIC
      /**@param graph graph in which to find the triples
       *@param I mutual information object to compute the scores
       *@param sep_set hashtable storing the separation sets for pairs of variables
       * @param marks hashtable containing the orientation marks for edges
       */
      std::vector< ProbabilisticRanking > unshieldedTriplesMiic_(
          const MixedGraph&                                                      graph,
          CorrectedMutualInformation&                                            mutualInformation,
          const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
          HashTable< std::pair< NodeId, NodeId >, char >&                        marks);

      /// Gets the orientation probabilities like MIIC for the orientation phase
      /**@param graph graph in which to find the triples
       *@param proba_triples probabilities for the different triples to update
       */
      std::vector< ProbabilisticRanking >
          updateProbaTriples_(const MixedGraph&                   graph,
                              std::vector< ProbabilisticRanking > probaTriples);

      /// Orient double headed arcs to avoid cycles
      /**@param mg the MixedGraph from which the double headed arcs will be oriented.
       */
      void orientDoubleHeadedArcs_(MixedGraph& mg);

      /// Object that can propagates orientations to non-oriented edges.
      gum::MeekRules meekRules_;

      /// Check constraints
      bool isForbiddenArc_(NodeId x, NodeId y) const;
      bool isForbiddenEdge_(NodeId x, NodeId y);
      bool isMaxIndegree_(MixedGraph graph, NodeId x);
      bool isArcValid_(MixedGraph graph, NodeId x, NodeId y);

      private:
      /// Fixes the maximum log that we accept in exponential computations
      int _maxLog_ = 100;
      /// an empty conditioning set
      const std::vector< NodeId > _emptySet_;
      /// an empty vector of arcs
      std::vector< Arc > _latentCouples_;

      /// maximum number of parents
      gum::Size _maxIndegree_;

      /// size of the database
      Size _size_;

      /// Storing the propabilities for each arc set in the graph
      ArcProperty< double > _arcProbas_;

      /// Initial marks for the orientation phase, used to convey constraints
      HashTable< std::pair< NodeId, NodeId >, char > _initialMarks_;

      /// Graph that contains the mandatories arcs
      gum::DAG _mandatoryGraph_;

      /// Graph that contains the forbidden arcs
      gum::DiGraph _forbiddenGraph_;

      /** @brief checks for directed paths in a graph, considering double arcs like
       * edges, not considering arc as a directed path.
       * @param graph MixedGraph in which to search the path
       * @param n1 tail of the path
       * @param n2 head of the path
       * @param countArc bool to know if we
       */
      static bool _existsNonTrivialDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);

      /** checks for directed paths in a graph, consider double arcs like edges
       *@param graph MixedGraph in which to search the path
       *@param n1 tail of the path
       *@param n2 head of the path
       */
      static bool _existsDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);

      void _orientingVstructureMiic_(MixedGraph&                                     graph,
                                     HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                     NodeId                                          x,
                                     NodeId                                          y,
                                     NodeId                                          z,
                                     double                                          p1,
                                     double                                          p2);

      void _propagatingOrientationMiic_(MixedGraph&                                     graph,
                                        HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                        NodeId                                          x,
                                        NodeId                                          y,
                                        NodeId                                          z,
                                        double                                          p1,
                                        double                                          p2);

      bool _isNotLatentCouple_(NodeId x, NodeId y);

      public:
      Signaler4< gum::NodeId, gum::NodeId, std::string, std::string > onStructuralModification;
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_CONSTRAINT_MIIC_H */
