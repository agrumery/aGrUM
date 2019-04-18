
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief The 3off2 algorithm
 *
 * The ThreeOffTwo class implements the 3off2 algorithm as proposed by Affeldt and
 * al. in https://doi.org/10.1186/s12859-015-0856-x.
 * It starts by eliminating edges that correspond to independent variables to
 * build the skeleton of the graph, and then directs the remaining edges to get an
 * essential graph. Latent variables can be detected using bi-directed arcs.
 *
 * The variant MIIC is also implemented based on
 * https://doi.org/10.1371/journal.pcbi.1005662. Only the orientation phase differs
 * from 3off2, with a diffferent ranking method and different propagation rules.
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN and Maria-Virginia RUIZ
 * CUEVAS
 */
#ifndef GUM_LEARNING_3_OFF_2_H
#define GUM_LEARNING_3_OFF_2_H

#include <string>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/config.h>
#include <agrum/core/approximations/IApproximationSchemeConfiguration.h>
#include <agrum/core/approximations/approximationScheme.h>
#include <agrum/core/heap.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/learning/scores_and_tests/correctedMutualInformation.h>

namespace gum {

  namespace learning {

    class GreaterPairOn2nd {
      public:
      bool operator()(
         const std::pair<
            std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
            double >& e1,
         const std::pair<
            std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
            double >& e2) const;
    };

    class GreaterAbsPairOn2nd {
      public:
      bool operator()(
         const std::pair< std::tuple< NodeId, NodeId, NodeId >*, double >& e1,
         const std::pair< std::tuple< NodeId, NodeId, NodeId >*, double >& e2)
         const;
    };

    class GreaterTupleOnLast {
      public:
      bool operator()(
         const std::
            tuple< std::tuple< NodeId, NodeId, NodeId >*, double, double, double >&
               e1,
         const std::
            tuple< std::tuple< NodeId, NodeId, NodeId >*, double, double, double >&
               e2) const;
    };
    /**
     * @class Miic
     * @brief The miic learning algorithm
     *
     * The miic class implements the miic algorithm based on
     * https://doi.org/10.1371/journal.pcbi.1005662.
     * It starts by eliminating edges that correspond to independent variables to
     * build the skeleton of the graph, and then directs the remaining edges to get
     * an
     * essential graph. Latent variables can be detected using bi-directed arcs.
     *
     * The variant 3off2 is also implemented as proposed by Affeldt and
     * al. in https://doi.org/10.1186/s12859-015-0856-x.  Only the orientation
     * phase differs
     * from miic, with a different ranking method and different propagation rules.
     *
     * @ingroup learning_group
     */
    class Miic : public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      Miic();

      /// default constructor with maxLog
      Miic(int maxLog);

      /// copy constructor
      Miic(const Miic& from);

      /// move constructor
      Miic(Miic&& from);

      /// destructor
      ~Miic();

      /// @}

      /// copy operator
      Miic& operator=(const Miic& from);

      /// move operator
      Miic& operator=(Miic&& from);

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{


      /// learns the structure of an Essential Graph
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param graph the MixedGraph we start from for the learning
       * */
      MixedGraph learnMixedStructure(CorrectedMutualInformation<>& I,
                                     MixedGraph                    graph);

      /// learns the structure of an Bayesian network, ie a DAG, by first learning
      /// an Essential graph and then directing the remaining edges.
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database
       * @param graph the MixedGraph we start from for the learning
       */
      DAG learnStructure(CorrectedMutualInformation<>& I, MixedGraph graph);

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

      /// Sets the orientation phase to follow the one of the MIIC algorithm
      void setMiicBehaviour();
      /// Sets the orientation phase to follow the one of the 3off2 algorithm
      void set3off2Behaviour();

      /// Set a ensemble of constraints for the orientation phase
      void addConstraints(
         HashTable< std::pair< NodeId, NodeId >, char > constraints);
      /// @}

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
       * @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param graph the MixedGraph we start from for the learning
       * @param sep_set the separation set for independent couples, here set to {}
       * @param _rank the heap of ranks of the algorithm
       */
      void _initiation(
         CorrectedMutualInformation<>&                                    I,
         MixedGraph&                                                      graph,
         HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sep_set,
         Heap< std::pair<
                  std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                  double >,
               GreaterPairOn2nd >&                                        _rank);

      /// Iteration phase
      /**
       * As long as we find important nodes for edges, we go over them to see if
       * we can assess the conditional independence of the variables.
       *
       * @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param graph the MixedGraph returned from the previous phase
       * @param sep_set the separation set for independent couples, built during
       * the iterations of the phase
       * @param _rank the heap of ranks of the algorithm
       */
      void _iteration(
         CorrectedMutualInformation<>&                                    I,
         MixedGraph&                                                      graph,
         HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sep_set,
         Heap< std::pair<
                  std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                  double >,
               GreaterPairOn2nd >&                                        _rank);

      /// Orientation phase from the 3off2 algorithm, returns a CPDAG
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param graph the MixedGraph returned from the previous phase
       * @param sep_set the separation set for independent couples, built during
       * the previous phase
       */
      void _orientation_3off2(CorrectedMutualInformation<>&             I,
                              MixedGraph&                               graph,
                              const HashTable< std::pair< NodeId, NodeId >,
                                               std::vector< NodeId > >& sep_set);

      /// Modified version of the orientation phase that tries to propagate
      /// orientations from both orientations in case of a bidirected arc, not used
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param graph the MixedGraph returned from the previous phase
       * @param sep_set the separation set for independent couples, built during
       * the previous phase
       */
      void _orientation_latents(CorrectedMutualInformation<>&             I,
                                MixedGraph&                               graph,
                                const HashTable< std::pair< NodeId, NodeId >,
                                                 std::vector< NodeId > >& sep_set);

      /// Orientation phase from the MIIC algorithm, returns a mixed graph that
      /// may contain circles
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param graph the MixedGraph returned from the previous phase
       * @param sep_set the separation set for independent couples, built during
       * the previous phase
       */
      void _orientation_miic(CorrectedMutualInformation<>&             I,
                             MixedGraph&                               graph,
                             const HashTable< std::pair< NodeId, NodeId >,
                                              std::vector< NodeId > >& sep_set);
      /// @}

      /// finds the best contributor node for a pair given a conditioning set
      /**@param x first node
       * @param y second node
       * @param ui conditioning set
       * @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param graph containing the assessed nodes
       * @param _rank the heap of ranks of the algorithm
       */
      void _findBestContributor(
         NodeId                        x,
         NodeId                        y,
         const std::vector< NodeId >&  ui,
         const MixedGraph&             graph,
         CorrectedMutualInformation<>& I,
         Heap< std::pair<
                  std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                  double >,
               GreaterPairOn2nd >&     _rank);

      /// gets the list of unshielded triples in the graph in decreasing value of
      ///|I'(x, y, z|{ui})|
      /*@param graph graph in which to find the triples
       *@param I mutual information object to compute the scores
       *@param sep_set hashtable storing the separation sets for pairs of variables
       */
      std::vector< std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > >
         _getUnshieldedTriples(const MixedGraph&                         graph,
                               CorrectedMutualInformation<>&             I,
                               const HashTable< std::pair< NodeId, NodeId >,
                                                std::vector< NodeId > >& sep_set);

      /// gets the list of unshielded triples in the graph in decreasing value of
      ///|I'(x, y, z|{ui})|, prepares the orientation matrix for MIIC
      /*@param graph graph in which to find the triples
       *@param I mutual information object to compute the scores
       *@param sep_set hashtable storing the separation sets for pairs of variables
       * @param marks hashtable containing the orientation marks for edges
       */
      std::vector< std::tuple< std::tuple< NodeId, NodeId, NodeId >*,
                               double,
                               double,
                               double > >
         _getUnshieldedTriplesMIIC(
            const MixedGraph&             graph,
            CorrectedMutualInformation<>& I,
            const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >&
                                                            sep_set,
            HashTable< std::pair< NodeId, NodeId >, char >& marks);

      /// Gets the orientation probabilities like MIIC for the orientation phase
      /*@param graph graph in which to find the triples
       *@param proba_triples probabilities for the different triples to update
       */
      std::vector< std::tuple< std::tuple< NodeId, NodeId, NodeId >*,
                               double,
                               double,
                               double > >
         _updateProbaTriples(
            const MixedGraph&                   graph,
            std::vector< std::tuple< std::tuple< NodeId, NodeId, NodeId >*,
                                     double,
                                     double,
                                     double > > proba_triples);

      /// Propagates the orientation from a node to its neighbours
      /*@param dag graph in which to which to propagate arcs
       *@param node node on which neighbours to propagate th orientation
       *@todo : avoid exception driven programmation in circle detection
       */
      void _propagatesHead(MixedGraph& graph, NodeId node);


      private:
      /// Fixes the maximum log that we accept in exponential computations
      int __maxLog = 100;
      /// an empty conditioning set
      const std::vector< NodeId > __empty_set;
      /// an empty vector of arcs
      std::vector< Arc > __latent_couples;

      /// size of the database
      Size __N;
      /// wether to use the miic algorithm or not
      bool __usemiic{false};

      /// Storing the propabilities for each arc set in the graph
      ArcProperty< double > __arc_probas;

      /// Initial marks for the orientation phase, used to convey constraints
      HashTable< std::pair< NodeId, NodeId >, char > __initial_marks;

      /// checks for directed paths in a graph, consider double arcs like edges
      /*@param graph MixedGraph in which to search the path
       *@param n1 tail of the path
       *@param n2 head of the path
       */
      const bool __existsDirectedPath(const MixedGraph& graph,
                                      const NodeId      n1,
                                      const NodeId      n2) const;
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
//#include <agrum/learning/threeOffTwo_tpl.h>

#endif /* GUM_LEARNING_3_OFF_2_H */
