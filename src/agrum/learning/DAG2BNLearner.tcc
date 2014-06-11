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
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#include <agrum/variables/labelizedVariable.h>


namespace gum {

  
  namespace learning {

    
    /// create a BN
    template <typename PARAM_ESTIMATOR>
    BayesNet<float>
    DAG2BNLearner::createBN ( PARAM_ESTIMATOR& estimator,
                              const DAG& dag,
                              const std::vector<std::string>& names,
                              const std::vector<unsigned int>& modal ) {
      BayesNet<float> bn;

      // create a bn with dummy parameters corresponding to the dag
      for ( const auto id : dag ) {
        bn.add ( LabelizedVariable ( names[id], "", modal[id] ), id );
      }

      // add the arcs
      bn.beginTopologyTransformation ();
      for ( const auto& arc : dag.arcs() ) {
        bn.addArc ( arc.tail (), arc.head () );
      }
      bn.endTopologyTransformation ();

      // estimate the parameters
      const VariableNodeMap& varmap = bn.variableNodeMap ();
      estimator.clear ();
      for ( const auto id : dag ) {
        // get the sequence of variables and make the targets be the last
        const Potential<float>& pot = bn.cpt ( id );
        const DiscreteVariable& var = varmap.get ( id );
        Sequence<const DiscreteVariable*> vars =
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
      for ( const auto id : dag ) {
        // get the variables of the CPT of id in the correct order
        Potential<float>& pot =
          const_cast< Potential<float>& > ( bn.cpt ( id ) );
        const DiscreteVariable& var = varmap.get ( id );
        Sequence<const DiscreteVariable*> vars =
          pot.variablesSequence ();
        if ( vars.pos ( &var ) != vars.size () - 1 ) {
          vars.erase ( &var );
          vars.insert ( &var );
        }

        // create a potential with the appropriate size
        Potential<float> ordered_pot;
        ordered_pot.beginMultipleChanges ();
        for ( const auto var : vars ) {
          ordered_pot.add ( *var );
        }
        ordered_pot.endMultipleChanges ();
        estimator.setParameters ( index, ordered_pot );

        // assign the potential to the BN
        __probaVarReordering ( pot, ordered_pot );
        ++index;
      }
        
      return bn;
    }
    

  } /* namespace learning */
  
  
} /* namespace gum */
