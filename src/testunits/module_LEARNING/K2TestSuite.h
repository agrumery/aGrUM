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
#include <agrum/learning/structuralConstraintDiGraph.h>
#include <agrum/learning/structuralConstraintDAG.h>
#include <agrum/learning/structuralConstraintIndegree.h>
#include <agrum/learning/structuralConstraint2TimeSlice.h>
#include <agrum/learning/structuralConstraintSet.h>

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

      template <typename SCORE, typename STRUCT_CONSTRAINT>
      gum::DAG learnDAG ( SCORE& score,
                          const std::vector<unsigned int>& order,
                          STRUCT_CONSTRAINT& constraint ) {
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
                if ( constraint.checkArcAddition ( order[j], index ) ) {
                  new_parents.back () = order[j];
                  score.addNodeSet ( index, new_parents );
                  ++nb_checked[i];
                }
              }
              if ( nb_checked[i] == 0 ) {
                cont_by_node[i] = false;
                --cont;
              }
            }
          }

          // check each node to see if there is an improvement
          unsigned int score_index = 0;
          for ( unsigned int i = 0; i < nb_nodes; ++i ) {
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
                for ( unsigned int j = 0, k = 0; j < i; ++j ) {
                  if ( constraint.checkArcAddition ( order[j], index ) ) {
                    if ( k == best ) {
                      parents[i].push_back ( order[j] );
                      dag.insertArc ( order[j], index );
                      const gum::learning::GraphChange
                        change ( gum::learning::GraphChangeType::ARC_ADDITION,
                                 order[j], index );
                      constraint.modifyGraph ( change );
                      break;
                    }
                    ++k;
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


      void probaVarReordering ( gum::Potential<float>& pot,
                                const gum::Potential<float>& other_pot ) {
        // check that the variables are identical
        gum::Set<const gum::DiscreteVariable*> diff_vars =
          pot.variablesSequence ().diffSet ( other_pot.variablesSequence () );
        if ( ! diff_vars.empty () ) {
          GUM_ERROR ( gum::CPTError,
                      "the potentials do not have the same variables" );
        }

        // perform the copy
        gum::Instantiation i ( other_pot );
        gum::Instantiation j ( pot );
        for ( i.setFirst (); ! i.end (); ++i ) {
          j.setVals ( i );
          pot.set ( j, other_pot[i] );
        }
      }
      

      template <typename PARAM_ESTIMATOR>
      gum::BayesNet<float> learnBN ( PARAM_ESTIMATOR& estimator,
                                     const gum::DAG& dag,
                                     const std::vector<std::string>& names,
                                     const std::vector<unsigned int>& modal ) {
        gum::BayesNet<float> bn;

        // create a bn with dummy parameters corresponding to the dag
        for ( auto iter_id = dag.beginSafe ();
              iter_id != dag.endSafe (); ++iter_id ) {
          bn.add ( gum::LabelizedVariable ( names[*iter_id], "",
                                            modal[*iter_id] ), *iter_id );
        }

        // add the arcs
        bn.beginTopologyTransformation ();
        for ( const auto& arc : dag.arcs() ) {
          bn.addArc ( arc.tail (), arc.head () );
        }
        bn.endTopologyTransformation ();

        // estimate the parameters
        const gum::VariableNodeMap& varmap = bn.variableNodeMap ();
        estimator.clear ();
        for ( auto iter_id = dag.beginSafe ();
              iter_id != dag.endSafe (); ++iter_id ) {
          // get the sequence of variables and make the targets be the last
          const gum::Potential<float>& pot = bn.cpt ( *iter_id );
          const gum::DiscreteVariable& var = varmap.get ( *iter_id );
          gum::Sequence<const gum::DiscreteVariable*> vars =
            pot.variablesSequence ();
          if ( vars.pos ( &var ) != vars.size () - 1 ) {
            vars.erase ( &var );
            vars.insert ( &var );
          }

          // setup the setimation
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
        for ( auto iter_id = dag.beginSafe ();
              iter_id != dag.endSafe (); ++iter_id ) {
          // get the variables of the CPT of id in the correct order
          gum::Potential<float>& pot =
            const_cast< gum::Potential<float>& > ( bn.cpt ( *iter_id ) );
          const gum::DiscreteVariable& var = varmap.get ( *iter_id );
          gum::Sequence<const gum::DiscreteVariable*> vars =
            pot.variablesSequence ();
          if ( vars.pos ( &var ) != vars.size () - 1 ) {
            vars.erase ( &var );
            vars.insert ( &var );
          }

          // create a potential with the appropriate size
          gum::Potential<float> ordered_pot;
          ordered_pot.beginMultipleChanges ();
          for ( const auto var : vars ) {
            ordered_pot.add ( *var );
          }
          ordered_pot.endMultipleChanges ();
          estimator.setParameters ( index, ordered_pot );

          // assign the potential to the BN
          probaVarReordering ( pot, ordered_pot );
          ++index;
        }
         
        
        return bn;
      }

    };


    void test_k2_asia () {
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

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }
      
      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }
    
    void test_k2_asia_constraint_DAG () {
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

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }
      
      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }

    
    void test_k2_asia_constraint_2TimeSlice () {
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

      gum::NodeProperty<bool> slices;
      for ( unsigned int i = 0; i < modalities.size (); ++i ) {
        if ( i % 2 ) {
          slices.insert ( i, false );
        }
        else {
          slices.insert ( i, true );
        }
      }
      
      gum::learning::StructuralConstraint2TimeSlice
        struct_constraint ( slices );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }
      
      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
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

      gum::learning::StructuralConstraintIndegree
        struct_constraint ( modalities.size (), 6 );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }

      gum::Timer timer;
      
      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );
      std::cout << "learning time = " << timer.step () << std::endl;
      
      std::cout << dag << " " << bn << std::endl;
    }

  };


} /* namespace gum_tests */
