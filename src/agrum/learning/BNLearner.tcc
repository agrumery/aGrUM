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
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

    
    /// create the score used for learning
    template <typename FILTER>
    void
    BNLearner::__createScore ( FILTER& filter,
                               std::vector<unsigned int>& modalities ) {
      // first, save the old score, to be delete if everything is ok
      Score<>* old_score = __score;

      // create the new scoring function
      switch ( __score_type ) {
      case ScoreType::AIC:
        __score = new ScoreAIC<> ( filter, modalities, *__apriori );
        break;

        /*
      case ScoreType::BD:
        __score = new ScoreBD<> ( filter, modalities, *__apriori );
        break;

      case ScoreType::BDEU:
        __score = new ScoreBDeu<> ( filter, modalities, *__apriori );
        break;
        */
        
      case ScoreType::BIC:
        __score = new ScoreBIC<> ( filter, modalities, *__apriori );
        break;
        
      case ScoreType::K2:
        __score = new ScoreK2<> ( filter, modalities, *__apriori );
        break;
        
      case ScoreType::LOG2LIKELIHOOD:
        __score = new ScoreLog2Likelihood<> ( filter, modalities, *__apriori );
        break;

      default:
        GUM_ERROR ( OperationNotAllowed,
                    "BNLearner does not support yet this score" );
      }

      // remove the old score, if any
      if ( old_score != nullptr ) delete old_score;
    }

    
    /// create the parameter estimator used for learning
    template <typename FILTER>
    void BNLearner::__createParamEstimator
    ( FILTER& filter,
      std::vector<unsigned int>& modalities ) {
      // first, save the old estimator, to be delete if everything is ok
      ParamEstimator<>* old_estimator = __param_estimator;
      
      // create the new estimator
      switch ( __param_estimator_type ) {
      case ParamEstimatorType::ML:
        __param_estimator = new ParamEstimatorML<> ( filter, modalities );
        break;

      case ParamEstimatorType::MLwithUniformApriori:
        __param_estimator =
          new ParamEstimatorMLwithUniformApriori<> ( filter, modalities );
        break;

      default:
         GUM_ERROR ( OperationNotAllowed,
                    "BNLearner does not support yet this parameter estimator" );
      }

      // remove the old estimator, if any
      if ( old_estimator != nullptr ) delete old_estimator;
    }

    
    /// learn a structure from a file
    template <typename FILTER>
    DAG BNLearner::__learnDAG ( FILTER& filter,
                                std::vector<unsigned int>& modal ) {
      // add the mandatory arcs to the initial dag and remove the forbidden ones
      // from the initial graph
      DAG init_graph = __initial_dag;
      
      const ArcSet& mandatory_arcs = __constraint_MandatoryArcs.arcs ();
      for ( const auto& arc : mandatory_arcs ) {
        if ( ! init_graph.exists ( arc.tail () ) )
          init_graph.addNode ( arc.tail () );
        if ( ! init_graph.exists ( arc.head () ) )
          init_graph.addNode ( arc.head () );
        init_graph.addArc ( arc.tail (), arc.head () );
      }

      const ArcSet& forbidden_arcs = __constraint_ForbiddenArcs.arcs ();
      for ( const auto& arc : forbidden_arcs ) {
        init_graph.eraseArc ( arc );
      }
      
      switch ( __selected_algo ) {
      // ========================================================================
      case AlgoType::GREEDY_HILL_CLIMBING:
        {
          StructuralConstraintSetStatic<StructuralConstraintMandatoryArcs,
                                        StructuralConstraintForbiddenArcs,
                                        StructuralConstraintSliceOrder>
            gen_constraint;
          static_cast<StructuralConstraintMandatoryArcs&> ( gen_constraint ) =
            __constraint_MandatoryArcs;
          static_cast<StructuralConstraintForbiddenArcs&> ( gen_constraint ) =
            __constraint_ForbiddenArcs;
          static_cast<StructuralConstraintSliceOrder&> ( gen_constraint ) =
            __constraint_SliceOrder;
          
          GraphChangesGenerator4DiGraph< decltype ( gen_constraint ) >
            op_set ( gen_constraint );
        
          StructuralConstraintSetStatic<StructuralConstraintIndegree,
                                        StructuralConstraintDAG> sel_constraint;
          static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
            __constraint_Indegree;
          
          GraphChangesSelector4DiGraph< Score<>,
                                        decltype ( sel_constraint ),
                                        decltype ( op_set ) >
            selector ( *__score, sel_constraint, op_set );
                         
          return __greedy_hill_climbing.learnStructure ( selector, modal,
                                                         init_graph );
        }
        
       // ========================================================================
       case AlgoType::LOCAL_SEARCH_WITH_TABU_LIST:
        {
          StructuralConstraintSetStatic<StructuralConstraintMandatoryArcs,
                                        StructuralConstraintForbiddenArcs,
                                        StructuralConstraintSliceOrder>
            gen_constraint;
          static_cast<StructuralConstraintMandatoryArcs&> ( gen_constraint ) =
            __constraint_MandatoryArcs;
          static_cast<StructuralConstraintForbiddenArcs&> ( gen_constraint ) =
            __constraint_ForbiddenArcs;
          static_cast<StructuralConstraintSliceOrder&> ( gen_constraint ) =
            __constraint_SliceOrder;
          
          GraphChangesGenerator4DiGraph< decltype ( gen_constraint ) >
            op_set ( gen_constraint );
          
          StructuralConstraintSetStatic<StructuralConstraintTabuList,
                                        StructuralConstraintIndegree,
                                        StructuralConstraintDAG> sel_constraint;
          static_cast<StructuralConstraintTabuList&> ( sel_constraint ) =
            __constraint_TabuList;
          static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
            __constraint_Indegree;
          
          GraphChangesSelector4DiGraph< Score<>,
                                        decltype ( sel_constraint ),
                                        decltype ( op_set ) >
            selector ( *__score, sel_constraint, op_set );
          
          return __local_search_with_tabu_list.learnStructure ( selector, modal,
                                                                init_graph );
        }
        
      // ========================================================================
      case AlgoType::K2:
        {
          StructuralConstraintSetStatic<StructuralConstraintMandatoryArcs,
                                        StructuralConstraintForbiddenArcs>
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
          for ( const auto& arc : mandatory_arcs ) {
            if ( order.pos ( arc.tail () ) >= order.pos ( arc.head () ) ) {
              order_compatible = false;
              break;
            }
          }

          if ( order_compatible ) {
            StructuralConstraintSetStatic<StructuralConstraintIndegree,
                                          StructuralConstraintDiGraph>
              sel_constraint;
            static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
              __constraint_Indegree;
          
            GraphChangesSelector4DiGraph< Score<>,
                                          decltype ( sel_constraint ),
                                          decltype ( op_set ) >
              selector ( *__score, sel_constraint, op_set );
            
            return __K2.learnStructure ( selector, modal, init_graph );
          }
          else {
            StructuralConstraintSetStatic<StructuralConstraintIndegree,
                                          StructuralConstraintDAG>
              sel_constraint;
            static_cast<StructuralConstraintIndegree&> ( sel_constraint ) =
              __constraint_Indegree;
          
            GraphChangesSelector4DiGraph< Score<>,
                                          decltype ( sel_constraint ),
                                          decltype ( op_set ) >
              selector ( *__score, sel_constraint, op_set );
            
            return __K2.learnStructure ( selector, modal, init_graph );
          }
        }

       // ========================================================================
       default:
        GUM_ERROR ( OperationNotAllowed,
                    "the learnDAG method has not been implemented for this "
                    "learning algorithm" );
      }
    }

      
    /// learn a Bayes Net from a file
    template <typename GUM_SCALAR>
    BayesNet<GUM_SCALAR> BNLearner::learnBN ( std::string filename ) {
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
      __createApriori ();
      __createScore ( fast_filter, modalities );

      // create the parameter estimator
      __createParamEstimator ( fast_filter, modalities );

      return DAG2BNLearner::createBN<ParamEstimator<>,GUM_SCALAR>
        ( *__param_estimator, __learnDAG ( fast_filter, modalities ),
          database.variableNames (), modalities );
    }


  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

