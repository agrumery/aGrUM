/**************************************************************************
*   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
/**
 * @file
 * @brief Inline implementation of GroundedInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */

namespace gum {
  namespace prm {

    template<typename GUM_SCALAR>
    GroundedInference<GUM_SCALAR>::~GroundedInference() {
      GUM_DESTRUCTOR ( GroundedInference );

      if ( __inf != 0 ) {
        delete __inf;
      }

      if ( not __obs.empty() ) {
        for ( List<const Potential<prm_float>* >::iterator iter = __obs.begin(); iter != __obs.end(); ++iter ) {
          // We used const ptrs only because of BayesNetInference::addEvidence() requires it
          delete const_cast<Potential<prm_float>*> ( *iter );
        }
      }
    }

    template<typename GUM_SCALAR>
    void
    GroundedInference<GUM_SCALAR>::_evidenceAdded ( const Chain& chain ) {
      Potential<prm_float>* bn_obs = new Potential<prm_float>();
      // Retrieving the BN's variable
      std::stringstream var_name;
      var_name << chain.first->name() << "." << chain.second->safeName();
      bn_obs->add ( __inf->bn().variableFromName ( var_name.str() ) );
      // Retrievin the PRM's evidence and copying it in bn_obs
      const Potential<prm_float>* prm_obs = evidence ( chain.first ) [chain.second->id()];
      Instantiation i ( *bn_obs ), j ( *prm_obs );

      for ( i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc() ) {
        bn_obs->set ( i, prm_obs->get ( j ) );
      }

      __obs.insert ( bn_obs );
    }

    template<typename GUM_SCALAR>
    void
    GroundedInference<GUM_SCALAR>::_evidenceRemoved ( const Chain& chain ) {
      std::stringstream var_name;
      var_name << chain.first->name() << "." << chain.second->safeName();
      const DiscreteVariable& var = __inf->bn().variableFromName ( var_name.str() );

      for ( List<const Potential<prm_float>* >::iterator iter = __obs.begin(); iter != __obs.end(); ++iter ) {
        if ( ( **iter ).contains ( var ) ) {
          __inf->eraseEvidence ( *iter );
          const Potential<prm_float>* e = *iter;
          __obs.erase ( iter );
          delete e;
          break;
        }
      }
    }


    template<typename GUM_SCALAR> INLINE
    GroundedInference<GUM_SCALAR>::GroundedInference ( const PRM& prm, const System& system ) :
      PRMInference ( prm, system ), __inf ( 0 ) {
      GUM_CONSTRUCTOR ( GroundedInference );
    }

    template<typename GUM_SCALAR> INLINE
    GroundedInference<GUM_SCALAR>::GroundedInference ( const GroundedInference& source ) :
      PRMInference ( source ), __inf ( 0 ) {
      GUM_CONS_CPY ( GroundedInference );
      GUM_ERROR ( FatalError, "illegal to copy constructor" );
    }

    template<typename GUM_SCALAR> INLINE
    GroundedInference&
    GroundedInference<GUM_SCALAR>::operator= ( const GroundedInference& source ) {
      GUM_ERROR ( FatalError, "illegal call to copy operator" );
    }

    template<typename GUM_SCALAR> INLINE
    BayesNetInference<prm_float>&
    GroundedInference<GUM_SCALAR>::getBNInference() {
      if ( __inf != 0 ) {
        return *__inf;
      } else {
        GUM_ERROR ( NotFound, "the inference engine is not yet defined" );
      }
    }

    template<typename GUM_SCALAR> INLINE
    void
    GroundedInference<GUM_SCALAR>::setBNInference ( BayesNetInference<prm_float>* bn_inf ) {
      if ( __inf != 0 ) {
        delete __inf;
      }

      __inf = bn_inf;
    }

    template<typename GUM_SCALAR> INLINE
    void
    GroundedInference<GUM_SCALAR>::_marginal ( const Chain& chain, Potential<prm_float>& m ) {
      if ( __inf == 0 ) {
        GUM_ERROR ( OperationNotAllowed, "no inference engine defined" );
      }

      std::stringstream sBuff;

      if ( not __obs.empty() ) {
        __inf->insertEvidence ( __obs );
      }

      sBuff << chain.first->name() << "." << chain.second->safeName();
      m = __inf->marginal ( __inf->bn().idFromName ( sBuff.str() ) );
    }

    template<typename GUM_SCALAR> INLINE
    void
    GroundedInference<GUM_SCALAR>::_joint ( const std::vector< Chain >& queries, Potential<prm_float>& j ) {
      GUM_ERROR ( FatalError, "not yet implemented" );
    }

    template<typename GUM_SCALAR> INLINE
    std::string
    GroundedInference<GUM_SCALAR>::name() const {
      return "grounded inference";
    }

  } /* namespace prm */
} /* namespace gum */

