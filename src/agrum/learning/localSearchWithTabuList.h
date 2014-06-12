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
#include <agrum/learning/graphChange.h>
#include <agrum/learning/graphChangesSelector.h>


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

      /// learns the structure of a Bayes net
      template <typename SCORE,
                typename STRUCT_CONSTRAINT,
                template <typename> class GRAPH_CHANGES_GENERATOR>
      DAG
      learnStructure
      ( GraphChangesSelector<SCORE,STRUCT_CONSTRAINT,GRAPH_CHANGES_GENERATOR>&
        selector,
        const std::vector<unsigned int>& modal,
        unsigned int N = 10,
        DAG initial_dag = DAG () );
     
      /// learns the structure and the parameters of a BN
      template <typename SCORE,
                typename STRUCT_CONSTRAINT,
                template <typename> class GRAPH_CHANGES_GENERATOR,
                typename PARAM_ESTIMATOR>
      BayesNet<float>
      learnBN
      ( GraphChangesSelector<SCORE,STRUCT_CONSTRAINT,GRAPH_CHANGES_GENERATOR>&
        selector,
        PARAM_ESTIMATOR& estimator,
        const std::vector<std::string>& names,
        const std::vector<unsigned int>& modal,
        unsigned int N = 10,
        DAG initial_dag = DAG () );

      /// @}

    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// always include templated methods
#include <agrum/learning/localSearchWithTabuList.tcc>


#endif /* GUM_LEARNING_LOCAL_SEARCH_WITH_TABU_LIST_H */

