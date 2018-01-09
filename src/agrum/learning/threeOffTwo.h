/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief The 3off12 algorithm
 *
 * The ThreeOffTwo class implements the 3off2 algorithm as proposed by Affeldt and
 * al. in https://doi.org/10.1186/s12859-015-0856-x.
 * It starts by eliminating edges that correspond to independent vaiables to build
 * the skeleton of the graph, and then directs the remaining edges to get an
 * essential graph. Latent variables can be detected using bi-directed arcs.
 *
 * @author Quentin FALCAND
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
      bool
      operator()(const std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*,
                                  double >& e1,
                 const std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*,
                                  double >& e2) const;
    };

    class GreaterAbsPairOn2nd {
      public:
      bool operator()(
        const std::pair< std::tuple< Idx, Idx, Idx >*, double >& e1,
        const std::pair< std::tuple< Idx, Idx, Idx >*, double >& e2) const;
    };

    class GreaterTupleOnLast {
      public:
      bool operator()(
        const std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double >&
          e1,
        const std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double >&
          e2) const;
    };

    /**
     * @class ThreeOffTwo
     * @brief The 3off2 learning algorithm
     *
     * The ThreeOffTwo class implements a novel structure learning algorithm that
     * uses the mutual information between variables to assess the conditional
     * independence between the nodes.
     * @ingroup learning_group
     */
    class ThreeOffTwo : public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      ThreeOffTwo();

      /// default constructor with maxLog
      ThreeOffTwo(int maxLog);
      /*
            /// copy constructor
            ThreeOffTwo( const ThreeOffTwo& from );

            /// move constructor
            ThreeOffTwo( ThreeOffTwo&& from );
      */
      /// destructor
      ~ThreeOffTwo();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{
      /*
            /// copy operator
            ThreeOffTwo& operator=( const ThreeOffTwo& from );

            /// move operator
            ThreeOffTwo& operator=( ThreeOffTwo&& from );
      */
      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{


      /// learns the structure of an Essential Graph
      /// @todo : avoid exception driven programmation in the orientation part to
      ///		  detect cycles
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param detectLatent wether or not to detect latent variables
       * @param graph the MixedGraph we start from for our learning
       * */
      MixedGraph learnMixedStructure(CorrectedMutualInformation<>& I,
                                     MixedGraph                    graph);

      /// learns the structure of an Bayesian network, ie a DAG, from an Essential
      /// graph.
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param detectLatent wether or not to detect latent variables
       * @param graph the MixedGraph we start from for our learning
       * @param topologicalOrder a topological order of the variables to sort out
       * oritation problems*/
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
                 typename PARAM_ESTIMATOR,
                 typename CELL_TRANSLATORS >
      BayesNet< GUM_SCALAR > learnBN(GRAPH_CHANGES_SELECTOR&           selector,
                                     PARAM_ESTIMATOR&                  estimator,
                                     const std::vector< std::string >& names,
                                     const std::vector< Size >&        modal,
                                     const CELL_TRANSLATORS&           translator,
                                     DAG initial_dag = DAG());

      /// get the list of arcs hiding latent variables
      const std::vector< Arc > latentVariables() const;

      /// Sets the orientation phase to follow the one of the MIIC algorithm
      void orientMIIC();
      /// Sets the orientation phase to follow the one of the 3off2 algorithm
      void orient3off2();

      /// @}
      // ##########################################################################
      /// @name Signalers and Listeners
      // ##########################################################################
      /// @{

      /**
       * @brief Returns the approximation scheme message.
       * @return Returns the approximation scheme message.
       */
      // std::string messageApproximationScheme() const;

      /// @}

      protected:
      // ##########################################################################
      /// @name Main phases
      // ##########################################################################
      /// @{

      /// Initiation phase
      /**
       * We go over all edges and test if the variables are independent. If they
       * are,
       * the edge is deleted. If not, the best contributor is found.
       */
      void _initiation(
        CorrectedMutualInformation<>& I,
        MixedGraph&                   graph,
        HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set,
        Heap<
          std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >,
          GreaterPairOn2nd >& _rank);

      /// Iteration phase
      /**
       * As long as we find important nodes for edges, we go over them to see if
       * we can assess the independence of the variables.
       */
      void _iteration(
        CorrectedMutualInformation<>& I,
        MixedGraph&                   graph,
        HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set,
        Heap<
          std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >,
          GreaterPairOn2nd >& _rank);

      /// Orientation phase
      void _orientation(
        CorrectedMutualInformation<>& I,
        MixedGraph&                   graph,
        const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set);

      void _orientation_latents(
        CorrectedMutualInformation<>& I,
        MixedGraph&                   graph,
        const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set);

      void _orientation_miic(
        CorrectedMutualInformation<>& I,
        MixedGraph&                   graph,
        const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set);
      /// @}

      /// finds the best contributor node for a pair given a conditioning set
      /**@param x first node
       * @param y second node
       * @param ui conditioning set
       */
      void _findBestContributor(
        Idx                           x,
        Idx                           y,
        const std::vector< Idx >&     ui,
        const MixedGraph&             graphe,
        CorrectedMutualInformation<>& I,
        Heap<
          std::pair< std::tuple< Idx, Idx, Idx, std::vector< Idx > >*, double >,
          GreaterPairOn2nd >& _rank);

      /// gets the list of unshielded triples in the graph in decreasing value of
      ///|I'(x, y, z|{ui})|
      /*@param graph graph in which to find the triples
       *@param I mutual information object to compute the scores
       *@param sep_set hashtable storing the separation sets for pairs of variables
       */
      std::vector< std::pair< std::tuple< Idx, Idx, Idx >*, double > >
      _getUnshieldedTriples(
        const MixedGraph&             graph,
        CorrectedMutualInformation<>& I,
        const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set);

      std::vector<
        std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > >
      _getUnshieldedTriplesMIIC(
        const MixedGraph&             graph,
        CorrectedMutualInformation<>& I,
        const HashTable< std::pair< Idx, Idx >, std::vector< Idx > >& sep_set,
        HashTable< std::pair< Idx, Idx >, char >&                     marks);

      /// Gets the orientation probabilities like MIIC for the orientation phase
      std::vector<
        std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > >
      _updateProbaTriples(
        const MixedGraph& graph,
        std::vector<
          std::tuple< std::tuple< Idx, Idx, Idx >*, double, double, double > >
          proba_triples);

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
      const std::vector< Idx > __empty_set;
      /// an empty vector of arcs
      std::vector< Arc > __latent_couples;

      /// size of the database
      Size __N;

      /// wether to use the miic algorithm or not
      bool __usemiic{false};

      public:
      /// checks for directed paths in a graph, consider double arcs like edges
      const bool __existsDirectedPath(const MixedGraph& graph,
                                      const NodeId      n1,
                                      const NodeId      n2) const;
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
//#include <agrum/learning/threeOffTwo_tpl.h>

#endif /* GUM_LEARNING_3_OFF_2_H */
