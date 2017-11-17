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
 * @brief The greedy hill learning algorithm (for directed graphs)
 *
 * The ThreeOffTwo class implements a greedy search in which the only
 * the graph changes that increase the global score are applied. Those that
 * increase it the more are applied first. The algorithm stops when no single
 * change can increase the score anymore.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_3_OFF_2_H
#define GUM_LEARNING_3_OFF_2_H

#include <string>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/config.h>
//#include <agrum/core/approximations/approximationScheme.h>
#include <agrum/learning/scores_and_tests/correctedMutualInformation.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/DAG.h>
#include <agrum/core/heap.h>
#include <agrum/core/approximations/IApproximationSchemeConfiguration.h>

namespace gum {

  namespace learning {

    class GreaterPairOn2nd{
      public:
      bool operator()( const std::pair<std::tuple<Idx, Idx, Idx, std::vector<Idx>>, double>& e1,
    		  const std::pair<std::tuple<Idx, Idx, Idx, std::vector<Idx>>, double>& e2 ) const;
    };

    class GreaterAbsPairOn2nd{
      public:
      bool operator()( const std::pair<std::tuple<Idx, Idx, Idx>, double>& e1,
    		  const std::pair<std::tuple<Idx, Idx, Idx>, double>& e2 ) const;
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
    class ThreeOffTwo : IApproximationSchemeConfiguration{
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

        /// default constructor
        ThreeOffTwo();

        /// default constructor with maxLog
        ThreeOffTwo( int maxLog );
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
      MixedGraph learnMixedStructure( CorrectedMutualInformation<>&  I,
                          MixedGraph          graph);//= MixedGraph() ?

      /// learns the structure of an Bayesian network, ie a DAG, from an Essential
      /// graph.
      /** @param I A mutual information instance that will do the computations
       * and has loaded the database.
       * @param detectLatent wether or not to detect latent variables
       * @param graph the MixedGraph we start from for our learning
       * @param topologicalOrder a topological order of the variables to sort out
       * oritation problems*/
      DAG learnStructure( CorrectedMutualInformation<>&  I,
				 	 	  MixedGraph            graph );

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
      template <typename GUM_SCALAR = double,
                typename GRAPH_CHANGES_SELECTOR,
                typename PARAM_ESTIMATOR,
                typename CELL_TRANSLATORS>
      BayesNet<GUM_SCALAR> learnBN( GRAPH_CHANGES_SELECTOR&         selector,
                                    PARAM_ESTIMATOR&                estimator,
                                    const std::vector<std::string>& names,
                                    const std::vector<Size>&        modal,
                                    const CELL_TRANSLATORS&         translator,
                                    DAG initial_dag = DAG() );

      ///get the list of arcs hiding latent variables
      const std::vector<Arc> getLatent() const;

      /// @}
      protected:
      /*
      MixedGraph initiation( MixedGraph graph );
      MixedGraph iteration( MixedGraph graph);
      MixedGraph orientation( MixedGraph graph );
      */
      ///finds the best contributor node for a pair given a conditioning set
      /**@param x first node
       * @param y second node
       * @param ui conditioning set
       */
      void _findBestContributor( Idx x, Idx y, const std::vector<Idx>& ui,
										const MixedGraph&             graphe,
										CorrectedMutualInformation<>&          I,
Heap<std::pair<std::tuple<Idx, Idx, Idx, std::vector<Idx>>, double>,
												   GreaterPairOn2nd>&  _rank );

      ///gets the list of unshielded triples in the graph in decreasing value of
      ///|I'(x, y, z|{ui})|
      /*@param graph graph in which to find the triples
       *@param I mutual information object to compute the scores
       *@param sep_set hashtable storing the separation sets for pairs of variables
       */
      Heap<std::pair<std::tuple<Idx, Idx, Idx>, double>, GreaterAbsPairOn2nd>
      	  	  	  	  	  	 _getUnshieldedTriples( const MixedGraph& graph,
      	  	  	  	  	  			 	CorrectedMutualInformation<>& I,
		      const HashTable<std::pair<Idx, Idx>, std::vector<Idx>>& sep_set  );

      ///Propagates the orientation from a node to its neighbours
      /*@param dag graph in which to which to propagate arcs
       *@param node node on which neighbours to propagate th orientation
       *@todo : avoid exception driven programmation in circle detection
       */
      void _propagatesHead( MixedGraph& graph, NodeId node );
      ///Fixes the maximum log that we accept in exponential computations
      int __maxLog = 100;

      private:
      /// an empty conditioning set
      const std::vector<Idx> __empty_set;
      /// an empty vector of arcs
      std::vector<Arc> __latent_couples;

      ///members for the second implementation of the orientation phase
      ///whether to use the first or second form of the orientation algorithm
      bool __orient_first{true};
      ///vector of triples to sort
      std::vector<std::pair<std::tuple<Idx, Idx, Idx>, double>> __triples2;
      ///size of the database
      Size __N;
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
//#include <agrum/learning/threeOffTwo_tpl.h>

#endif /* GUM_LEARNING_3_OFF_2_H */
