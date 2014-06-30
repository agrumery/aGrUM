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
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_K2_H
#define GUM_LEARNING_K2_H

#include <vector>
#include <string>

#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>
#include <agrum/learning/database/DBRowTranslatorSetDynamic.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/paramUtils/paramEstimatorMLwithUniformApriori.h>


namespace gum {

  
  namespace learning {


    
    /** @class K2
     * @brief The K2 algorithm
     * @ingroup learning_group
     */
    class K2 {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      K2 ();

      /// copy constructor
      K2 ( const K2& from );

      /// move constructor
      K2 ( K2&& from );

      /// destructor
      ~K2 ();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      K2& operator= ( const K2& from );

      /// move operator
      K2& operator= ( K2&& from );
      
      /// @}
      

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// learns the structure of a Bayes net
      template <typename SCORE, typename STRUCT_CONSTRAINT>
      DAG learnStructure ( SCORE& score,
                           STRUCT_CONSTRAINT& constraint,
                           const std::vector<unsigned int>& order,
                           DAG initial_dag = DAG () );
     
      /// learns the structure and the parameters of a BN
      template <typename GUM_SCALAR = float,
                typename SCORE,
                typename STRUCT_CONSTRAINT,
                typename PARAM_ESTIMATOR>
      BayesNet<GUM_SCALAR> learnBN ( SCORE& score,
                                     STRUCT_CONSTRAINT& constraint,
                                     PARAM_ESTIMATOR& estimator,
                                     const std::vector<std::string>& names,
                                     const std::vector<unsigned int>& modal,
                                     const std::vector<unsigned int>& order,
                                     DAG initial_dag = DAG () );

      /// basic learning of structure and parameters of a BN from a CSV
      template <typename GUM_SCALAR = float>
      BayesNet<GUM_SCALAR> learnBNFromCSV
      ( std::string filename,
        std::vector<unsigned int> order = std::vector<unsigned int> () );
      
    };

    
  } /* namespace learning */
  
  
} /* namespace gum */


/// always include templated methods
#include <agrum/learning/K2.tcc>


#endif /* GUM_LEARNING_K2_H */

