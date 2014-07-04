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
 *   GNU General Public License __for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <algorithm>

#include <agrum/config.h>
#include <agrum/learning/BNLearner.h>
#include <agrum/learning/BNLearnerListener.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/BNLearner.inl>
#endif /* GUM_NO_INLINE */


namespace gum {


  namespace learning {


    /// default constructor
    BNLearner::BNLearner () {
      // for debugging purposes
      GUM_CONSTRUCTOR ( BNLearner );

      //__forK2=new BNLearnerListener ( this,__K2 );
      __forGreedyHillClimbing=new BNLearnerListener ( this,__greedy_hill_climbing );
      __forLocalSearch=new BNLearnerListener ( this,__local_search_with_tabu_list );
    }


    /// copy constructor
    BNLearner::BNLearner ( const BNLearner& from ) :
      __score_type ( from.__score_type ),
      __score ( from.__score != nullptr ?
                from.__score->copyFactory () : nullptr ),
      __param_estimator_type ( from.__param_estimator_type ),
      __param_estimator ( from.__param_estimator != nullptr ?
                          from.__param_estimator->copyFactory () : nullptr ),
      __constraint_SliceOrder ( from.__constraint_SliceOrder ),
      __constraint_Indegree ( from.__constraint_Indegree ),
      __constraint_TabuList ( from.__constraint_TabuList ),
      __constraint_ForbiddenArcs ( from.__constraint_ForbiddenArcs ),
      __constraint_MandatoryArcs ( from.__constraint_MandatoryArcs ),
      __selected_algo ( from.__selected_algo ),
      __K2 ( from.__K2 ),
      __greedy_hill_climbing ( from.__greedy_hill_climbing ),
      __local_search_with_tabu_list ( from.__local_search_with_tabu_list ),
      __initial_dag ( from.__initial_dag ) {
      // for debugging purposes
      GUM_CONS_CPY ( BNLearner );

      //__forK2=new BNLearnerListener ( this,__K2 );
      __forGreedyHillClimbing=new BNLearnerListener ( this,__greedy_hill_climbing );
      __forLocalSearch=new BNLearnerListener ( this,__local_search_with_tabu_list );
    }


    /// move constructor
    BNLearner::BNLearner ( BNLearner&& from ) :
      __score_type ( from.__score_type ),
      __score ( from.__score ),
      __param_estimator_type ( from.__param_estimator_type ),
      __param_estimator ( from.__param_estimator ),
      __constraint_SliceOrder ( std::move ( from.__constraint_SliceOrder ) ),
      __constraint_Indegree ( std::move ( from.__constraint_Indegree ) ),
      __constraint_TabuList ( std::move ( from.__constraint_TabuList ) ),
      __constraint_ForbiddenArcs ( std::move ( from.__constraint_ForbiddenArcs ) ),
      __constraint_MandatoryArcs ( std::move ( from.__constraint_MandatoryArcs ) ),
      __selected_algo ( from.__selected_algo ),
      __K2 ( std::move ( from.__K2 ) ),
      __greedy_hill_climbing ( std::move ( from.__greedy_hill_climbing ) ),
      __local_search_with_tabu_list
      ( std::move ( from.__local_search_with_tabu_list ) ),
      __initial_dag ( std::move ( from.__initial_dag ) ) {
      from.__score = nullptr;
      from.__param_estimator = nullptr;

      GUM_CONS_MOV ( BNLearner );

      //__forK2=new BNLearnerListener ( this,__K2 );
      __forGreedyHillClimbing=new BNLearnerListener ( this,__greedy_hill_climbing );
      __forLocalSearch=new BNLearnerListener ( this,__local_search_with_tabu_list );
    }


    /// destructor
    BNLearner::~BNLearner () {
      if ( __score ) delete __score;
      if ( __param_estimator ) delete __param_estimator;

      //delete __forK2;
      delete __forGreedyHillClimbing;
      delete __forLocalSearch;

      GUM_DESTRUCTOR ( BNLearner );
    }


    /// copy operator
    BNLearner&
    BNLearner::operator= ( const BNLearner& from ) {
      if ( this != &from ) {
        if ( __score ) {
          delete __score;
          __score = nullptr;
        }

        if ( __param_estimator ) {
          delete __param_estimator;
          __param_estimator = nullptr;
        }

        __score_type = from.__score_type;
        __score = from.__score ? from.__score->copyFactory () : nullptr;
        __param_estimator_type = from.__param_estimator_type;
        __param_estimator = from.__param_estimator ?
                            from.__param_estimator->copyFactory () : nullptr;
        __constraint_SliceOrder    = from.__constraint_SliceOrder;
        __constraint_Indegree      = from.__constraint_Indegree;
        __constraint_TabuList      = from.__constraint_TabuList;
        __constraint_ForbiddenArcs = from.__constraint_ForbiddenArcs;
        __constraint_MandatoryArcs = from.__constraint_MandatoryArcs;
        __selected_algo = from.__selected_algo;
        __K2 = from.__K2;
        __greedy_hill_climbing = from.__greedy_hill_climbing;
        __local_search_with_tabu_list = from.__local_search_with_tabu_list;
        __initial_dag = from.__initial_dag;

        //__forK2=new BNLearnerListener ( this,__K2 );
        __forGreedyHillClimbing=new BNLearnerListener ( this,__greedy_hill_climbing );
        __forLocalSearch=new BNLearnerListener ( this,__local_search_with_tabu_list );
      }

      return *this;
    }


    /// move operator
    BNLearner&
    BNLearner::operator= ( BNLearner&& from ) {
      if ( this != &from ) {
        if ( __score ) {
          delete __score;
          __score = nullptr;
        }

        if ( __param_estimator ) {
          delete __param_estimator;
          __param_estimator = nullptr;
        }

        __score_type = from.__score_type;
        __score = from.__score;
        __param_estimator_type = from.__param_estimator_type;
        __param_estimator = from.__param_estimator;
        __constraint_SliceOrder    = std::move ( from.__constraint_SliceOrder );
        __constraint_Indegree      = std::move ( from.__constraint_Indegree );
        __constraint_TabuList      = std::move ( from.__constraint_TabuList );
        __constraint_ForbiddenArcs = std::move ( from.__constraint_ForbiddenArcs );
        __constraint_MandatoryArcs = std::move ( from.__constraint_MandatoryArcs );
        __selected_algo = from.__selected_algo;
        __K2 = from.__K2;
        __greedy_hill_climbing = std::move ( from.__greedy_hill_climbing );
        __local_search_with_tabu_list =
          std::move ( from.__local_search_with_tabu_list );
        __initial_dag = std::move ( from.__initial_dag );
        from.__score = nullptr;
        from.__param_estimator = nullptr;

        //__forK2=new BNLearnerListener ( this,__K2 );
        __forGreedyHillClimbing=new BNLearnerListener ( this,__greedy_hill_climbing );
        __forLocalSearch=new BNLearnerListener ( this,__local_search_with_tabu_list );
      }

      return *this;
    }


    /// reads a file and returns a databaseVectInRam
    DatabaseVectInRAM
    BNLearner::__readFile ( const std::string& filename ) {
      // get the extension of the file
      int filename_size = filename.size ();
      if ( filename_size < 4 ) {
        GUM_ERROR ( FormatNotFound, "BNLearner could not determine the "
                    "file type of the database" );
      }
      std::string extension = filename.substr ( filename.size () - 4 );
      std::transform ( extension.begin (), extension.end (),
                       extension.begin (), ::tolower );

      if ( extension == ".csv" ) {
        return DatabaseFromCSV ( filename );
      }

      GUM_ERROR ( OperationNotAllowed,
                  "BNLearner does not support yet this type of database file" );
    }


    /// learn a structure from a file
    DAG BNLearner::learnDAG ( std::string filename ) {
      // read the database
      DatabaseVectInRAM database = __readFile ( filename );

      // create the RowFilter used for learning: we first generate a universal
      // filter that can parse any database. Then, we parse once the DB to
      // convert it into a compact int (an interval 0..N-1) so that we can
      // parse it very quickly
      DBRowTranslatorSetDynamic<CellTranslatorUniversal> raw_translators;
      raw_translators.insertTranslator ( Col<0> (), database.nbVariables () );

      auto generators = make_generators ( RowGeneratorIdentity () );

      auto raw_filter = make_DB_row_filter ( database, raw_translators,
                                             generators );

      DBTransformCompactInt raw2fast_transfo;
      raw2fast_transfo.transform ( raw_filter );

      DBRowTranslatorSetDynamic<CellTranslatorCompactIntId> fast_translators;
      fast_translators.insertTranslator ( Col<0> (), database.nbVariables () );

      auto fast_filter = make_DB_row_filter ( database, fast_translators,
                                              generators );

      // get the modalities and create the score
      std::vector<unsigned int> modalities = raw_filter.modalities ();
      __createScore ( fast_filter, modalities );

      return __learnDAG ( fast_filter, modalities );
    }

  } /* namespace learning */


} /* namespace gum */

