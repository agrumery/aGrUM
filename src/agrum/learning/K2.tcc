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

#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/paramUtils/DAG2BNLearner.h>


namespace gum {

  
  namespace learning {


    /// learns the structure of a Bayes net
    template <typename SCORE, typename STRUCTURAL_CONSTRAINT>
    DAG K2::learnStructure ( SCORE& score,
                             STRUCTURAL_CONSTRAINT& constraint,
                             const std::vector<unsigned int>& order,
                             DAG dag ) {
      // fill the DAG with all the missing nodes
      unsigned int nb_nodes = order.size ();
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        if ( ! dag.existsNode ( i ) ) {
          dag.insertNode ( i );
        }
      }
      constraint.setGraph ( dag );

      // save the set of parents of the node (vector parents element
      // are sorted w.r.t. order
      std::vector< std::vector<unsigned int> > parents ( nb_nodes );
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        unsigned int index = order[i];
        const NodeSet& dag_parents = dag.parents ( index );
        if ( dag_parents.size () ) {
          parents[i].resize ( dag_parents.size () );
          unsigned int j = 0;
          for ( const auto par : dag_parents ) {
            parents[i][j] = par;
            ++j;
          }
        }
      }
      
      // initialization: assign a score to each node given its parents in
      // the current graph
      score.clear ();
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        unsigned int index = order[i];
        score.addNodeSet ( index, parents[i] );
      }
      std::vector<float> scores ( nb_nodes );
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        scores[i] = score.score ( i );
      }

      // now, add as many parents as needed
      unsigned int cont = nb_nodes;
      std::vector<bool> cont_by_node ( nb_nodes, true );
      std::vector<unsigned int> nb_checked ( nb_nodes );
      std::vector<bool> constraints_satisfied ( ( nb_nodes + 1 ) * nb_nodes / 2 );
      while ( cont ) {
        // compute the scores by adding one additional arc to all the nodes
        score.clear ();
        for ( unsigned int i = 0, r = 0; i < nb_nodes; ++i ) {
          if ( cont_by_node[i] ) {
            unsigned int index = order[i];
            nb_checked[i] = 0;
            std::vector<unsigned int> new_parents = parents[i];
            new_parents.push_back ( 0 );
            for ( unsigned int j = 0; j < i; ++j, ++r ) {
              if ( constraint.checkArcAddition ( order[j], index ) ) {
                new_parents.back () = order[j];
                score.addNodeSet ( index, new_parents );
                ++nb_checked[i];
                constraints_satisfied[r] = true;
              }
              else {
                constraints_satisfied[r] = false;
              }
            }
            if ( nb_checked[i] == 0 ) {
              r -= i;
              cont_by_node[i] = false;
              --cont;
            }
          }
        }

        // check each node to see if there is an improvement
        unsigned int score_index = 0;
        for ( unsigned int i = 0, r = 0; i < nb_nodes; ++i ) {
          if ( cont_by_node[i] ) {
            bool better = false;
            unsigned int best;
            for ( unsigned int k = 0; k < nb_checked[i]; ++k, ++score_index ) {
              float new_score = score.score ( score_index );
              if ( new_score > scores[i] ) {
                better = true;
                best = k;
                scores[i] = new_score;
              }
            }
            if ( better ) {
              unsigned int index = order[i];
              for ( unsigned int j = 0, k = 0; j < i; ++j, ++r ) {
                if ( constraints_satisfied[r] ) {
                  if ( k == best ) {
                    parents[i].push_back ( order[j] );
                    dag.insertArc ( order[j], index );
                    constraint.modifyGraph ( ArcAddition ( order[j], index ) );
                    r += i - j;
                    break;
                  }
                  ++k;
                }
              }
            }
            else {
              r += i;
              --cont;
              cont_by_node[i] = false;
            }
          }
        }

      }
      
      return dag;
    }


    /// learns a BN
    template <typename GUM_SCALAR,
              typename SCORE,
              typename STRUCT_CONSTRAINT,
              typename PARAM_ESTIMATOR> INLINE
    BayesNet<GUM_SCALAR> K2::learnBN ( SCORE& score,
                                       STRUCT_CONSTRAINT& constraint,
                                       PARAM_ESTIMATOR& estimator,
                                       const std::vector<std::string>& names,
                                       const std::vector<unsigned int>& modal,
                                       const std::vector<unsigned int>& order,
                                       DAG initial_dag ) {
      return DAG2BNLearner::createBN<PARAM_ESTIMATOR,GUM_SCALAR>
        ( estimator,
          learnStructure ( score, constraint,  order, initial_dag ),
          names, modal );
    }


    /// basic learning of structure and parameters of a BN from a CSV
    template <typename GUM_SCALAR>
    BayesNet<GUM_SCALAR> K2::learnBNFromCSV
    ( std::string filename,
      std::vector<unsigned int> order ) {
      DatabaseFromCSV database ( filename );
      
      DBRowTranslatorSetDynamic<CellTranslatorUniversal> translators;
      translators.insertTranslator ( Col<0> (), database.nbVariables () );

      auto generators = make_generators ( RowGeneratorIdentity () );
      
      auto filter = make_DB_row_filter ( database, translators, generators );

      std::vector<unsigned int> modalities = filter.modalities ();

      ScoreK2<> score ( filter, modalities );

      ParamEstimatorMLwithUniformApriori<> estimator ( filter, modalities );

      StructuralConstraintDiGraph struct_constraint;

      // if the order is incorrect, fix it:
      // 1. get the list of the nodes and remove those that were to high
      unsigned int max_node = database.nbVariables ();
      HashTable<unsigned int, bool> nodes_in_order;
      {
        unsigned int i = 0;
        while ( i < order.size () ) {
          if ( nodes_in_order.exists ( order[i] ) || ( order[i] >= max_node ) ) {
            order[i] = *( order.rbegin () );
            order.pop_back ();
          }
          else {
            nodes_in_order.insert ( order[i], true );
            ++i;
          }
        }
      }

      // 2. add at the end all the nodes that were not in nodes_in_order
      for ( unsigned int i = 0; i < max_node; ++i ) {
        if ( ! nodes_in_order.exists ( i ) ) {
          order.push_back ( i );
        }
      }
      
      return learnBN<GUM_SCALAR> ( score, struct_constraint, estimator,
                                   database.variableNames (),
                                   modalities, order );
      
    }


  } /* namespace learning */
  
  
} /* namespace gum */
