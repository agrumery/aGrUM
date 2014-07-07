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
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <algorithm>

#include <agrum/config.h>
#include <agrum/learning/BNLearner.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/BNLearner.inl>
#endif /* GUM_NO_INLINE */


namespace gum {


  namespace learning {

    
    /// Database default constructor
    BNLearner::Database::Database ( std::string filename ) :
      __database ( BNLearner::__readFile ( filename ) ),
      __generators ( RowGeneratorIdentity() ) {
      // create the RowFilter used for learning: we first generate a universal
      // filter that can parse any database. Then, we parse once the DB to
      // convert it into a compact int (an interval 0..N-1) so that we can
      // parse it very quickly
      __raw_translators.insertTranslator ( Col<0> (), __database.nbVariables() );

      auto raw_filter = make_DB_row_filter ( __database, __raw_translators,
                                             __generators );

      DBTransformCompactInt raw2fast_transfo;
      raw2fast_transfo.transform ( raw_filter );

      __translators.insertTranslator ( Col<0> (), __database.nbVariables() );
      
      __row_filter = new
        DBRowFilter< DatabaseVectInRAM::Handler,
                     DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                     FilteredRowGeneratorSet<RowGeneratorIdentity> >
        ( __database.handler (), __translators, __generators );

      __modalities = __row_filter->modalities();
    }

    
    /// Database default constructor
    BNLearner::Database::Database ( std::string filename,
                                    Database& score_database ) :
      __database ( BNLearner::__readFile ( filename ) ),
      __generators ( RowGeneratorIdentity() ) {
      // check that there are at least as many variables in the a priori
      // database as those in the score_database
      if ( __database.nbVariables () <
           score_database.__database.nbVariables () ) {
        GUM_ERROR ( InvalidArgument, "the a priori seems to have fewer variables "
                    "than the observed database" );
      }
      const std::vector<std::string>& score_vars =
        score_database.__database.variableNames ();
      const std::vector<std::string>& apriori_vars =
        __database.variableNames ();
      for ( unsigned int i = 0, size = score_vars.size (); i < size; ++i ) {
        if ( score_vars[i] != apriori_vars[i] ) {
          GUM_ERROR ( InvalidArgument, "some a priori variables do not match "
                      "their counterpart in the score database" );
        }
      }
      
      // create the RowFilter used for learning: we first generate a universal
      // filter that can parse any database. Then, we parse once the DB to
      // convert it into a compact int (an interval 0..N-1) so that we can
      // parse it very quickly. We first copy the raw translators of the score
      // database so that we take into account the values observed in this
      // database. Then we copy back these raw translators so that the score
      // filter knows about these values.
      __raw_translators = score_database.__raw_translators;
      
      auto raw_filter = make_DB_row_filter ( __database, __raw_translators,
                                             __generators );
      
      score_database.__raw_translators = __raw_translators;
      
      // create the fast translators
      DBTransformCompactInt raw2fast_transfo;
      raw2fast_transfo.transform ( raw_filter );

      __translators.insertTranslator ( Col<0> (),
                                       score_database.__database.nbVariables() );
      
      __row_filter = new
        DBRowFilter< DatabaseVectInRAM::Handler,
                     DBRowTranslatorSetDynamic<CellTranslatorCompactIntId>,
                     FilteredRowGeneratorSet<RowGeneratorIdentity> >
        ( __database.handler (), __translators, __generators );

      __modalities = __row_filter->modalities();
 
      // update the modalities of the score database
      score_database.__modalities = __modalities;
    }

    
    /// destructor
    BNLearner::Database::~Database () {
      delete __row_filter;
    }

    
    // ===========================================================================

    
    /// default constructor
    BNLearner::BNLearner() {
      // for debugging purposes
      __current_algorithm = nullptr;
      GUM_CONSTRUCTOR ( BNLearner );
    }


    /// copy constructor
    BNLearner::BNLearner ( const BNLearner& from ) :
      __score_type ( from.__score_type ),
      __score ( from.__score != nullptr ?
                from.__score->copyFactory() : nullptr ),
      __param_estimator_type ( from.__param_estimator_type ),
      __param_estimator ( from.__param_estimator != nullptr ?
                          from.__param_estimator->copyFactory() : nullptr ),
      __apriori_type ( from.__apriori_type ),
      __apriori ( from.__apriori != nullptr ?
                  from.__apriori->copyFactory() : nullptr ),
      __apriori_weight ( from.__apriori_weight ),
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
      __current_algorithm = nullptr;
      GUM_CONS_CPY ( BNLearner );
    }


    /// move constructor
    BNLearner::BNLearner ( BNLearner && from ) :
      __score_type ( from.__score_type ),
      __score ( from.__score ),
      __param_estimator_type ( from.__param_estimator_type ),
      __param_estimator ( from.__param_estimator ),
      __apriori_type ( from.__apriori_type ),
      __apriori ( from.__apriori ),
      __apriori_weight ( from.__apriori_weight ),
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
      from.__apriori = nullptr;

      __current_algorithm = nullptr;
      GUM_CONS_MOV ( BNLearner );
    }


    /// destructor
    BNLearner::~BNLearner() {
      if ( __score ) delete __score;

      if ( __param_estimator ) delete __param_estimator;

      if ( __apriori ) delete __apriori;

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

        if ( __apriori ) {
          delete __apriori;
          __apriori = nullptr;
        }

        __score_type = from.__score_type;
        __score = from.__score ? from.__score->copyFactory() : nullptr;
        __param_estimator_type = from.__param_estimator_type;
        __param_estimator = from.__param_estimator ?
                            from.__param_estimator->copyFactory() : nullptr;
        __apriori_type = from.__apriori_type;
        __apriori = from.__apriori ? from.__apriori->copyFactory() : nullptr;
        __apriori_weight = from.__apriori_weight;
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
        __current_algorithm = nullptr;

      }

      return *this;
    }


    /// move operator
    BNLearner&
    BNLearner::operator= ( BNLearner && from ) {
      if ( this != &from ) {
        if ( __score ) {
          delete __score;
          __score = nullptr;
        }

        if ( __param_estimator ) {
          delete __param_estimator;
          __param_estimator = nullptr;
        }

        if ( __apriori ) {
          delete __apriori;
          __apriori = nullptr;
        }

        __score_type = from.__score_type;
        __score = from.__score;
        __param_estimator_type = from.__param_estimator_type;
        __param_estimator = from.__param_estimator;
        __apriori_type = from.__apriori_type;
        __apriori = from.__apriori;
        __apriori_weight = from.__apriori_weight;
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
        from.__apriori = nullptr;
        __current_algorithm = nullptr;
      }

      return *this;
    }


    /// reads a file and returns a databaseVectInRam
    DatabaseVectInRAM
    BNLearner::__readFile ( const std::string& filename ) {
      // get the extension of the file
      int filename_size = filename.size();

      if ( filename_size < 4 ) {
        GUM_ERROR ( FormatNotFound, "BNLearner could not determine the "
                    "file type of the database" );
      }

      std::string extension = filename.substr ( filename.size() - 4 );
      std::transform ( extension.begin(), extension.end(),
                       extension.begin(), ::tolower );

      if ( extension == ".csv" ) {
        return DatabaseFromCSV ( filename );
      }

      GUM_ERROR ( OperationNotAllowed,
                  "BNLearner does not support yet this type of database file" );
    }

    
    /// read the database file for the score / parameter estimation
    void BNLearner::readDatabase ( const std::string& filename ) {
      if ( __score_database != nullptr ) {
        delete __score_database;
        __score_database = nullptr;
      }
      __score_database = new Database ( filename );
    }

    
    /// create the apriori used for learning
    void BNLearner::__createApriori() {
      // first, save the old apriori, to be delete if everything is ok
      Apriori<>* old_apriori = __apriori;

      // create the new apriori
      switch ( __apriori_type ) {
      case AprioriType::SMOOTHING:
        __apriori = new AprioriSmoothing<>;
        break;

      case AprioriType::DIRICHLET_FROM_DATABASE:
        if ( __score_database == nullptr ) {
          GUM_ERROR ( OperationNotAllowed, "the observation database is "
                      "needed to create the a priori" );
        }
        if ( __apriori_database != nullptr ) {
          delete __apriori_database;
          __apriori_database = nullptr;
        }
        __apriori_database = new Database ( __apriori_dbname, *__score_database );
  
        __apriori = new AprioriDirichletFromDatabase<>
          ( __apriori_database->rowFilter (),
            __apriori_database->modalities () );
        break;

      default:
        GUM_ERROR ( OperationNotAllowed,
                    "BNLearner does not support yet this apriori" );
      }

      // do not forget to assign a weight to the apriori
      __apriori->setWeight ( __apriori_weight );

      // remove the old apriori, if any
      if ( old_apriori != nullptr ) delete old_apriori;
    }


    /// create the score used for learning
    void BNLearner::__createScore () {
      // first, save the old score, to be delete if everything is ok
      Score<>* old_score = __score;

      // create the new scoring function
      switch ( __score_type ) {
        case ScoreType::AIC:
          __score = new ScoreAIC<> ( __score_database->rowFilter (),
                                     __score_database->modalities (),
                                     *__apriori );
          break;

        case ScoreType::BD:
          __score = new ScoreBD<> ( __score_database->rowFilter (),
                                    __score_database->modalities (),
                                    *__apriori );
          break;

        case ScoreType::BDeu:
          __score = new ScoreBDeu<> ( __score_database->rowFilter (),
                                      __score_database->modalities (),
                                      *__apriori );
          break;

        case ScoreType::BIC:
          __score = new ScoreBIC<> ( __score_database->rowFilter (),
                                     __score_database->modalities (),
                                     *__apriori );
          break;

        case ScoreType::K2:
          __score = new ScoreK2<> ( __score_database->rowFilter (),
                                    __score_database->modalities (),
                                    *__apriori );
          break;

        case ScoreType::LOG2LIKELIHOOD:
          __score = new ScoreLog2Likelihood<> ( __score_database->rowFilter (),
                                                __score_database->modalities (),
                                                *__apriori );
          break;

        default:
          GUM_ERROR ( OperationNotAllowed,
                      "BNLearner does not support yet this score" );
      }

      // remove the old score, if any
      if ( old_score != nullptr ) delete old_score;
    }


    /// create the parameter estimator used for learning
    void BNLearner::__createParamEstimator () {
      // first, save the old estimator, to be delete if everything is ok
      ParamEstimator<>* old_estimator = __param_estimator;
      
      // create the new estimator
      switch ( __param_estimator_type ) {
      case ParamEstimatorType::ML:
        __param_estimator =
          new ParamEstimatorML<> ( __score_database->rowFilter (),
                                   __score_database->modalities (),
                                   *__apriori );
        break;

      default:
        GUM_ERROR ( OperationNotAllowed,
                    "BNLearner does not support yet this parameter estimator" );
      }

      // remove the old estimator, if any
      if ( old_estimator != nullptr ) delete old_estimator;
    }

    
    /// learn a structure from a file
    DAG BNLearner::learnDAG () {
      // check that we have read a database
      if ( __score_database == nullptr ) {
        GUM_ERROR ( OperationNotAllowed, "you need to read a database before "
                    "learning from it" );
      }

      // create the score and the apriori
      __createApriori ();
      __createScore ();

      return __learnDAG ();
    }

    
    /// learn a structure from a file
    DAG BNLearner::__learnDAG () {
      // add the mandatory arcs to the initial dag and remove the forbidden ones
      // from the initial graph
      DAG init_graph = __initial_dag;

      const ArcSet& mandatory_arcs = __constraint_MandatoryArcs.arcs ();

      for ( const auto & arc : mandatory_arcs ) {
        if ( ! init_graph.exists ( arc.tail () ) )
          init_graph.addNode ( arc.tail () );

        if ( ! init_graph.exists ( arc.head () ) )
          init_graph.addNode ( arc.head () );

        init_graph.addArc ( arc.tail (), arc.head () );
      }

      const ArcSet& forbidden_arcs = __constraint_ForbiddenArcs.arcs ();

      for ( const auto & arc : forbidden_arcs ) {
        init_graph.eraseArc ( arc );
      }

      switch ( __selected_algo ) {
      // ========================================================================
      case AlgoType::GREEDY_HILL_CLIMBING: {
        BNLearnerListener listener ( this, __greedy_hill_climbing );
        StructuralConstraintSetStatic < StructuralConstraintMandatoryArcs,
                                        StructuralConstraintForbiddenArcs,
                                        StructuralConstraintSliceOrder >
          gen_constraint;
        static_cast<StructuralConstraintMandatoryArcs&> ( gen_constraint ) =
          __constraint_MandatoryArcs;
        static_cast<StructuralConstraintForbiddenArcs&> ( gen_constraint ) =
          __constraint_ForbiddenArcs;
        static_cast<StructuralConstraintSliceOrder&> ( gen_constraint ) =
          __constraint_SliceOrder;

        GraphChangesGenerator4DiGraph< decltype ( gen_constraint ) >
          op_set ( gen_constraint );

        StructuralConstraintSetStatic < StructuralConstraintIndegree,
                                        StructuralConstraintDAG > sel_constraint;
        static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
          __constraint_Indegree;

        GraphChangesSelector4DiGraph < Score<>,
                                       decltype ( sel_constraint ),
                                       decltype ( op_set ) >
          selector ( *__score, sel_constraint, op_set );

        return __greedy_hill_climbing.learnStructure
          ( selector, __score_database->modalities (), init_graph );
      }

      // ========================================================================
      case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST: {
        BNLearnerListener listener ( this, __local_search_with_tabu_list );
        StructuralConstraintSetStatic < StructuralConstraintMandatoryArcs,
                                        StructuralConstraintForbiddenArcs,
                                        StructuralConstraintSliceOrder >
          gen_constraint;
        static_cast<StructuralConstraintMandatoryArcs&> ( gen_constraint ) =
          __constraint_MandatoryArcs;
        static_cast<StructuralConstraintForbiddenArcs&> ( gen_constraint ) =
          __constraint_ForbiddenArcs;
        static_cast<StructuralConstraintSliceOrder&> ( gen_constraint ) =
          __constraint_SliceOrder;
        
        GraphChangesGenerator4DiGraph< decltype ( gen_constraint ) >
          op_set ( gen_constraint );

        StructuralConstraintSetStatic < StructuralConstraintTabuList,
                                        StructuralConstraintIndegree,
                                        StructuralConstraintDAG > sel_constraint;
        static_cast<StructuralConstraintTabuList&> ( sel_constraint ) =
          __constraint_TabuList;
        static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
          __constraint_Indegree;
        
        GraphChangesSelector4DiGraph < Score<>,
                                       decltype ( sel_constraint ),
                                       decltype ( op_set ) >
          selector ( *__score, sel_constraint, op_set );
        
        return __local_search_with_tabu_list.learnStructure
          ( selector, __score_database->modalities (), init_graph );
      }

      // ========================================================================
      case AlgoType::K2: {
        BNLearnerListener listener ( this, __K2.approximationScheme() );
        StructuralConstraintSetStatic < StructuralConstraintMandatoryArcs,
                                        StructuralConstraintForbiddenArcs >
          gen_constraint;
        static_cast<StructuralConstraintMandatoryArcs&> ( gen_constraint ) =
          __constraint_MandatoryArcs;
        static_cast<StructuralConstraintForbiddenArcs&> ( gen_constraint ) =
          __constraint_ForbiddenArcs;
        
        GraphChangesGenerator4K2< decltype ( gen_constraint ) >
          op_set ( gen_constraint );
        
        // if some mandatory arcs are incompatible with the order, use a DAG
        // constraint instead of a DiGraph constraint to avoid cycles
        const ArcSet& mandatory_arcs =
          static_cast<StructuralConstraintMandatoryArcs&>
          ( gen_constraint ).arcs ();
        const Sequence<NodeId>& order = __K2.order ();
        bool order_compatible = true;
        
        for ( const auto & arc : mandatory_arcs ) {
          if ( order.pos ( arc.tail () ) >= order.pos ( arc.head () ) ) {
            order_compatible = false;
            break;
          }
        }

        if ( order_compatible ) {
          StructuralConstraintSetStatic < StructuralConstraintIndegree,
                                          StructuralConstraintDiGraph >
            sel_constraint;
          static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
            __constraint_Indegree;
          
          GraphChangesSelector4DiGraph < Score<>,
                                         decltype ( sel_constraint ),
                                         decltype ( op_set ) >
            selector ( *__score, sel_constraint, op_set );
          
          return __K2.learnStructure
            ( selector, __score_database->modalities (), init_graph );
        }
        else {
          StructuralConstraintSetStatic < StructuralConstraintIndegree,
                                          StructuralConstraintDAG >
            sel_constraint;
          static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
            __constraint_Indegree;
          
          GraphChangesSelector4DiGraph < Score<>,
                                         decltype ( sel_constraint ),
                                         decltype ( op_set ) >
            selector ( *__score, sel_constraint, op_set );
          
          return __K2.learnStructure
            ( selector, __score_database->modalities (), init_graph );
        }
      }

      // ========================================================================
      default:
        GUM_ERROR ( OperationNotAllowed,
                    "the learnDAG method has not been implemented for this "
                    "learning algorithm" );
      }
    }


  } /* namespace learning */


} /* namespace gum */

