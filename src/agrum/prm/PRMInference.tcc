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
 * @brief Inline implementation of PRMInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/prm/PRMInference.h>

namespace gum {
  namespace prm {


    template<typename GUM_SCALAR>
    void
    PRMInference::clearEvidence() {
      for ( EvidenceIterator iter = __evidences.begin(); iter != __evidences.end(); ++iter ) {
        for ( PRMInference::EMapIterator jter = ( *iter )->begin(); jter != ( *iter )->end(); ++jter ) {
          delete *jter;
        }

        delete *iter;
      }

      __evidences.clear();
    }

    template<typename GUM_SCALAR>
    PRMInference::PRMInference ( const PRMInference& source ) :
      _prm ( source._prm ), _sys ( source._sys ) {
      GUM_CONS_CPY ( PRMInference );

      for ( PRMInference::EvidenceConstIterator iter = source.__evidences.begin(); iter != source.__evidences.end(); ++iter ) {
        __evidences.insert ( iter.key(), new PRMInference::EMap() );

        for ( PRMInference::EMapIterator jter = ( *iter )->begin(); jter != ( *iter )->end(); ++jter ) {
          Potential<prm_float>* e = new Potential<prm_float>();
          e->add ( * ( ( **jter ).variablesSequence().front() ) );
          Instantiation i ( *e );

          for ( i.setFirst(); not i.end(); i.inc() ) {
            e->set ( i, ( **jter ).get ( i ) );
          }

          __evidences[iter.key()]->insert ( jter.key(), e );
        }
      }
    }

    template<typename GUM_SCALAR>
    PRMInference&
    PRMInference::operator= ( const PRMInference& source ) {
      clearEvidence();
      _prm = source._prm;
      _sys = source._sys;

      for ( PRMInference::EvidenceConstIterator iter = source.__evidences.begin(); iter != source.__evidences.end(); ++iter ) {
        __evidences.insert ( iter.key(), new PRMInference::EMap() );

        for ( PRMInference::EMapIterator jter = ( *iter )->begin(); jter != ( *iter )->end(); ++jter ) {
          Potential<prm_float>* e = new Potential<prm_float>();
          e->add ( * ( ( **jter ).variablesSequence().front() ) );
          Instantiation i ( *e );

          for ( i.setFirst(); not i.end(); i.inc() ) {
            e->set ( i, ( **jter ).get ( i ) );
          }

          __evidences[iter.key()]->insert ( jter.key(), e );
        }
      }

      return *this;
    }

    template<typename GUM_SCALAR>
    PRMInference::EMap&
    PRMInference::__EMap ( const Instance* i ) {
      if ( __evidences.exists ( i ) ) {
        return * ( __evidences[i] );
      } else {
        __evidences.insert ( i, new PRMInference::EMap() );
        return * ( __evidences[i] );
      }
    }

    template<typename GUM_SCALAR>
    void
    PRMInference::addEvidence ( const Chain& chain, const Potential<prm_float>& p ) {
      if ( chain.first->exists ( chain.second->id() ) ) {
        if ( ( p.nbrDim() != 1 ) or ( not p.contains ( chain.second->type().variable() ) ) )
          GUM_ERROR ( OperationNotAllowed, "illegal evidence for the given Attribute." );

        Potential<prm_float>* e = new Potential<prm_float>();
        e->add ( chain.second->type().variable() );
        Instantiation i ( *e );

        for ( i.setFirst(); not i.end(); i.inc() )
          e->set ( i, p.get ( i ) );

        PRMInference::EMap& emap = __EMap ( chain.first );

        if ( emap.exists ( chain.second->id() ) ) {
          delete emap[chain.second->id()];
          emap[chain.second->id()] = e;
        } else {
          emap.insert ( chain.second->id(), e );
        }

        _evidenceAdded ( chain );
      } else {
        GUM_ERROR ( NotFound, "the given Attribute does not belong to this Instance." );
      }
    }

    template<typename GUM_SCALAR> INLINE
    PRMInference::PRMInference ( const PRM& prm, const System& system ) :
      _prm ( &prm ), _sys ( &system ) {
      GUM_CONSTRUCTOR ( PRMInference );
    }

    template<typename GUM_SCALAR> INLINE
    PRMInference::~PRMInference() {
      GUM_DESTRUCTOR ( PRMInference );
      clearEvidence();
    }

    template<typename GUM_SCALAR> INLINE
    PRMInference::EMap&
    PRMInference::evidence ( const Instance& i ) {
      try {
        return * ( __evidences[&i] );
      } catch ( NotFound& ) {
        GUM_ERROR ( NotFound, "this instance has no evidence." );
      }
    }

    template<typename GUM_SCALAR> INLINE
    const PRMInference::EMap&
    PRMInference::evidence ( const Instance& i ) const {
      try {
        return * ( __evidences[&i] );
      } catch ( NotFound& ) {
        GUM_ERROR ( NotFound, "this instance has no evidence." );
      }
    }

    template<typename GUM_SCALAR> INLINE
    PRMInference::EMap&
    PRMInference::evidence ( const Instance* i ) {
      try {
        return * ( __evidences[i] );
      } catch ( NotFound& ) {
        GUM_ERROR ( NotFound, "this instance has no evidence." );
      }
    }

    template<typename GUM_SCALAR> INLINE
    const PRMInference::EMap&
    PRMInference::evidence ( const Instance* i ) const {
      try {
        return * ( __evidences[i] );
      } catch ( NotFound& ) {
        GUM_ERROR ( NotFound, "this instance has no evidence." );
      }
    }

    template<typename GUM_SCALAR> INLINE
    bool
    PRMInference::hasEvidence ( const Instance& i ) const {
      return __evidences.exists ( &i );
    }

    template<typename GUM_SCALAR> INLINE
    bool
    PRMInference::hasEvidence ( const Instance* i ) const {
      return __evidences.exists ( i );
    }

    template<typename GUM_SCALAR> INLINE
    bool
    PRMInference::hasEvidence ( const Chain& chain ) const {
      return ( hasEvidence ( chain.first ) ) ?evidence ( chain.first ).exists ( chain.second->id() ) :false;
    }

    template<typename GUM_SCALAR> INLINE
    bool
    PRMInference::hasEvidence() const {
      return __evidences.size();
    }

    template<typename GUM_SCALAR> INLINE
    void
    PRMInference::removeEvidence ( const Chain& chain ) {
      try {
        if ( __EMap ( chain.first ).exists ( chain.second->id() ) ) {
          _evidenceRemoved ( chain );
          delete __EMap ( chain.first ) [chain.second->id()];
          __EMap ( chain.first ).erase ( chain.second->id() );
        }
      } catch ( NotFound& ) {
        // Ok, we are only removing
        GUM_CHECKPOINT;
      }
    }

    template<typename GUM_SCALAR> INLINE
    void
    PRMInference::marginal ( const PRMInference::Chain& chain, Potential<prm_float>& m ) {
      if ( m.nbrDim() > 0 ) {
        GUM_ERROR ( OperationNotAllowed, "the given Potential is not empty." );
      }

      if ( hasEvidence ( chain ) ) {
        m.add ( chain.second->type().variable() );
        const Potential<prm_float>& e = * ( evidence ( chain.first ) [chain.second->id()] );
        Instantiation i ( m ), j ( e );

        for ( i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc() )
          m.set ( i, e.get ( j ) );
      } else {
        if ( chain.second != & ( chain.first->get ( chain.second->safeName() ) ) ) {
          PRMInference::Chain good_chain = std::make_pair ( chain.first, & ( chain.first->get ( chain.second->safeName() ) ) );
          m.add ( good_chain.second->type().variable() );
          _marginal ( good_chain, m );
        } else {
          m.add ( chain.second->type().variable() );
          _marginal ( chain, m );
        }
      }
    }

    template<typename GUM_SCALAR> INLINE
    void
    PRMInference::joint ( const std::vector< PRMInference::Chain >& chains, Potential<prm_float>& j ) {
      if ( j.nbrDim() > 0 ) {
        GUM_ERROR ( OperationNotAllowed, "the given Potential is not empty." );
      }

      for ( std::vector< PRMInference::Chain >::const_iterator chain = chains.begin(); chain != chains.end(); ++chain ) {
        j.add ( chain->second->type().variable() );
      }

      _joint ( chains, j );
    }

  } /* namespace prm */
} /* namespace gum */

