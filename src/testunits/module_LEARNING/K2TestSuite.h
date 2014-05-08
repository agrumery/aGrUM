/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/DAG.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/databaseFromCSV.h>
#include <agrum/learning/scoreK2.h>
#include <agrum/learning/scoreBDeu.h>
#include <agrum/learning/paramEstimatorML.h>

namespace gum_tests {

  class K2TestSuite: public CxxTest::TestSuite {
  public:

    class CellTranslator : public gum::learning::DBCellTranslator<1,1> {
    public:
      void translate () { out (0) = in (0).getFloat (); }
      void initialize () {
        unsigned int nb = in(0).getFloat ();
        if ( ! __values.exists ( nb ) ) __values.insert ( nb );
      }
      void modalities ( std::vector<unsigned int>& modal ) const noexcept {
        modal.push_back ( __values.size () );
      }
      bool requiresInitialization () const noexcept { return true; }
      std::string translateBack ( unsigned int col,
                                  unsigned int translated_val ) {
        std::stringstream str;
        str << translated_val;
        return  str.str ();
      }
      void variableNames ( const std::vector<std::string>& db_var,
                           std::vector<std::string>& output_vars ) const {
        output_vars.push_back ( db_var[_input_cols[0]] );
      }

    private:
      gum::Set<unsigned int> __values;
      
    };

    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      unsigned int _computeRows () { return 1; }
    };


    class K2 {
    public:
      K2 () {}

      ~K2 () {}

      template <typename SCORE>
      gum::DAG learnDAG ( SCORE& score,
                          const std::vector<unsigned int>& order,
                          unsigned int nb_max_parents ) {
        // create a DAG with all the nodes but without any arc
        gum::DAG dag;
        unsigned int nb_nodes = order.size ();
        for ( unsigned int i = 0; i < nb_nodes; ++i ) {
          dag.insertNode ( i );
        }
        
        // initialization: assign a score to each node (without parents)
        score.clear ();
        for ( unsigned int i = 0; i < nb_nodes; ++i ) {
          score.addNodeSet ( i );
        }
        std::vector<float> scores ( nb_nodes );
        for ( unsigned int i = 0; i < nb_nodes; ++i ) {
          scores[i] = score.score ( i );
        }

        // now, add as many parents as needed
        unsigned int cont = nb_nodes;
        std::vector<bool> cont_by_node ( nb_nodes, true );
        std::vector< std::vector<unsigned int> > parents ( nb_nodes );
        std::vector<unsigned int> nb_checked ( nb_nodes );
        while ( cont ) {
          // compute the scores by adding one additional arc to all the nodes
          score.clear ();
          for ( unsigned int i = 0; i < nb_nodes; ++i ) {
            if ( cont_by_node[i] ) {
              unsigned int index = order[i];
              nb_checked[i] = 0;
              std::vector<unsigned int> new_parents = parents[i];
              new_parents.push_back ( 0 );
              for ( unsigned int j = 0; j < i; ++j ) {
                if ( ! dag.existsArc ( order[j], index ) ) {
                  new_parents.back () = order[j];
                  score.addNodeSet ( index, new_parents );
                  ++nb_checked[i];
                }
              }
            }
          }

          // check each node to see if there is an improvement
          unsigned int score_index = 0;
          for ( unsigned int i = 0; i < nb_nodes; ++i ) {
            if ( cont_by_node[i] ) {
              bool better = false;
              unsigned int best;
              for ( unsigned int j = 0; j < nb_checked[i]; ++j, ++score_index ) {
                float new_score = score.score ( score_index );
                if ( new_score > scores[i] ) {
                  better = true;
                  best = j;
                  scores[i] = new_score;
                }
              }
              if ( better ) {
                unsigned int index = order[i];
                for ( unsigned int k = 0; k < i; ++k ) {
                  if ( ! dag.existsArc ( order[k], index ) ) {
                    if ( k == best ) {
                      parents[i].push_back ( order[k] );
                      dag.insertArc ( order[k], index );
                      if ( dag.parents ( index ).size () == nb_max_parents ) {
                        --cont;
                        cont_by_node[i] = false;
                      }
                      break;
                    }
                  }
                }
              }
              else {
                --cont;
                cont_by_node[i] = false;
              }
            }
          }
        }

        /*
        unsigned int nbp = 0;
        for ( auto iter = dag.begin (); iter != dag.end (); ++iter ) {
          auto x = dag.parents ( *iter ).size ();
          if ( x > nbp ) nbp = x;
        }
        std::cout << "nb max parents = " << nbp << std::endl;
        */
        
        return dag;
      }

      template <typename PARAM_ESTIMATOR>
      gum::BayesNet<float> learnBN ( PARAM_ESTIMATOR& estimator,
                                     const gum::DAG& dag,
                                     const std::vector<std::string>& names,
                                     const std::vector<unsigned int>& modal ) {
        gum::BayesNet<float> bn;

        // create a bn with dummy parameters corresponding to the dag
        for ( const auto id : dag ) {
          bn.add ( gum::LabelizedVariable ( names[id], "", modal[id] ), id );
        }

        // add the arcs
        bn.beginTopologyTransformation ();
        for ( const auto& arc : dag.arcs() ) {
          bn.addArc ( arc.tail (), arc.head () );
        }
        bn.endTopologyTransformation ();

        // make the targets be the last dimension in the CPTs
        const gum::VariableNodeMap& varmap = bn.variableNodeMap ();
        for ( const auto id : dag ) {
          const gum::DiscreteVariable& var = varmap.get ( id );
          gum::Potential<float>& pot =
            const_cast<gum::Potential<float>&> ( bn.cpt ( id ) );
          const gum::Sequence<const gum::DiscreteVariable*>& vars =
            pot.variablesSequence ();
          if ( vars.pos ( &var ) != vars.size () - 1 ) {
            pot.beginMultipleChanges ();
            pot.erase ( var );
            pot.add ( var );
            pot.endMultipleChanges ();
          }
        }
        
        // estimate the parameters
        estimator.clear ();
        for ( const auto id : dag ) {
          const gum::Potential<float>& pot = bn.cpt ( id );
          const gum::Sequence<const gum::DiscreteVariable*>& vars =
            pot.variablesSequence ();
          unsigned int target = varmap.get ( *( vars[vars.size() -1] ) );
          if ( vars.size () > 1 ) {
            std::vector<unsigned int> cond_ids ( vars.size () - 1 );
            for ( unsigned int i = 0; i < cond_ids.size (); ++i ) {
              cond_ids[i] = varmap.get ( *( vars[i] ) );
            }
            estimator.addNodeSet ( target, cond_ids );
          }
          else {
            estimator.addNodeSet ( target ); 
          }
        }

        // assign the parameters to the potentials
        unsigned int index = 0;
        for ( const auto id : dag ) {
          gum::Potential<float>& pot =
            const_cast< gum::Potential<float>& > ( bn.cpt ( id ) );
          estimator.setParameters ( index, pot );
          ++index;
        }
         
        return bn;
      }

    };


    void xtest_k2_asia () {
      K2 k2;

      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto handler = database.handler ();
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( handler, translators,
                                                        generators );

      std::vector<unsigned int> modalities = filter.modalities ();

      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }
      
      gum::DAG dag = k2.learnDAG ( score, order, 100 );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }

    
    void test_k2 () {
      K2 k2;

      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "alarm.csv" ) );
      
      auto handler = database.handler ();
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 37 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( handler, translators,
                                                        generators );

      std::vector<unsigned int> modalities = filter.modalities ();

      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }

      gum::Timer timer;
      
      gum::DAG dag = k2.learnDAG ( score, order, 6 );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );
      std::cout << "learning time = " << timer.step () << std::endl;
      
      std::cout << dag << " " << bn << std::endl;
    }

  };


} /* namespace gum_tests */
