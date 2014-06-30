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
/** @file
 * @brief The local search learning with tabu list algorithm (for directed graphs)
 *
 * The LocalSearchWithTabuList class implements a greedy search in which we allow
 * applying at most N consecutive graph changes that decrease the score. To
 * prevent infinite loops, when using local search, you should use a structural
 * constraint that includes a tabu list of at least N elements.
 * 
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_LOCAL_SEARCH_WITH_TABU_LIST_H
#define GUM_LEARNING_LOCAL_SEARCH_WITH_TABU_LIST_H


#include <vector>
#include <string>

#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>
#include <agrum/learning/database/DBRowTranslatorSetDynamic.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/learning/constraints/structuralConstraintSet.h>
#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/paramUtils/paramEstimatorMLwithUniformApriori.h>


namespace gum {

  
  namespace learning {

    
    /** @class LocalSearchWithTabuList
     * @brief The local search with tabu list learning algorithm (for
     * directed graphs)
     *
     * The LocalSearchWithTabuList class implements a greedy search in which we
     * allow applying at most N consecutive graph changes that decrease the score.
     * To prevent infinite loops, when using local search, you should use a
     * structural constraint that includes a tabu list of at least N elements.
     *
     * @ingroup learning_group
     */
    class LocalSearchWithTabuList {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      LocalSearchWithTabuList ();

      /// copy constructor
      LocalSearchWithTabuList ( const LocalSearchWithTabuList& from );

      /// move constructor
      LocalSearchWithTabuList ( LocalSearchWithTabuList&& from );

      /// destructor
      ~LocalSearchWithTabuList ();

      /// @}

      
      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      LocalSearchWithTabuList& operator= ( const LocalSearchWithTabuList& from );

      /// move operator
      LocalSearchWithTabuList& operator= ( LocalSearchWithTabuList&& from );

      /// @}
      
      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// set the max number of changes decreasing the score that we allow to apply
      void setMaxNbDecreasingChanges ( unsigned int nb );
      
      /// learns the structure of a Bayes net
      /** @param selector A class that computes the best changes that can be
       * applied and that enables the user to get them very easily
       * @param modal the domain sizes of the random variables observed in the
       * database
       * @param initial_dag the DAG we start from for our learning */
      template <typename SCORE,
                typename STRUCT_CONSTRAINT,
                typename GRAPH_CHANGES_SELECTOR>
      DAG
      learnStructure
      ( GraphChangesSelector4DiGraph<SCORE,STRUCT_CONSTRAINT,
                                     GRAPH_CHANGES_SELECTOR>& selector,
        const std::vector<unsigned int>& modal,
        DAG initial_dag = DAG () );

      
      /// learns the structure and the parameters of a BN
      template <typename GUM_SCALAR = float,
                typename SCORE,
                typename STRUCT_CONSTRAINT,
                typename GRAPH_CHANGES_SELECTOR,
                typename PARAM_ESTIMATOR>
      BayesNet<GUM_SCALAR>
      learnBN
      ( GraphChangesSelector4DiGraph<SCORE,STRUCT_CONSTRAINT,
                                     GRAPH_CHANGES_SELECTOR>& selector,
        PARAM_ESTIMATOR& estimator,
        const std::vector<std::string>& names,
        const std::vector<unsigned int>& modal,
        DAG initial_dag = DAG () );

      /// basic learning of structure and parameters of a BN from a CSV
      template <typename GUM_SCALAR = float>
      BayesNet<GUM_SCALAR> learnBNFromCSV ( std::string filename );

      /// @}


    private:

      /// the max number of changes decreasing the score that we allow to apply
      unsigned int __MaxNbDecreasing { 2 };

    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/localSearchWithTabuList.inl>
#endif /* GUM_NO_INLINE */


/// always include templated methods
#include <agrum/learning/localSearchWithTabuList.tcc>


#endif /* GUM_LEARNING_LOCAL_SEARCH_WITH_TABU_LIST_H */

