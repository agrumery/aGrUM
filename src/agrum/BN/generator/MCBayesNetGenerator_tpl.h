/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN & Ariele-Paolo MAESANO   *
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
/** @file
 * @brief Source implementation of MCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 *
 */

#include <agrum/BN/generator/MCBayesNetGenerator.h>

namespace gum {

#ifdef _MSC_VER
#define IBNG IBayesNetGenerator
#else
#define IBNG IBayesNetGenerator<GUM_SCALAR, ICPTGenerator>
#endif

  template <typename GUM_SCALAR>
  gum::Size getMaxModality( gum::BayesNet<GUM_SCALAR>& bayesNet ) {
    gum::Size maxMod = 0;

    for ( auto node : bayesNet.nodes() )
      if ( maxMod < bayesNet.variable( node ).domainSize() )
        maxMod = bayesNet.variable( node ).domainSize();

    return maxMod;
  }

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  MCBayesNetGenerator<GUM_SCALAR,
                      ICPTGenerator,
                      ICPTDisturber>::MCBayesNetGenerator( Size nbrNodes,
                                                           Size maxArcs,
                                                           Idx  maxModality,
                                                           Size iteration,
                                                           Idx  p,
                                                           Idx  q )
      : IBNG( nbrNodes, maxArcs, maxModality )
      , _bayesNettemp()
      , _hashMarginal() {
    if ( p + q > 100 )
      GUM_ERROR(
          OperationNotAllowed,
          "the sum of the probabilities p and q must be at most equal to 100" );

    _iteration = iteration;
    _p = p;
    _q = q;
    _disturbing = false;

    GUM_CONSTRUCTOR( MCBayesNetGenerator );
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::
      MCBayesNetGenerator( BayesNet<GUM_SCALAR> bayesNet,
                           Size                 iteration,
                           Idx                  p,
                           Idx                  q )
      : MCBayesNetGenerator( bayesNet.size(),
                             ( Size )( bayesNet.sizeArcs() * 1.1 ),
                             getMaxModality( bayesNet ) ) {

    _iteration = iteration;
    _p = p;
    _q = q;
    _disturbing = false;
  }

  // Destructor.
  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  MCBayesNetGenerator<GUM_SCALAR,
                      ICPTGenerator,
                      ICPTDisturber>::~MCBayesNetGenerator() {
    GUM_DESTRUCTOR( MCBayesNetGenerator );
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::generateBN(
      BayesNet<GUM_SCALAR>& bayesNet ) {

    Idx iteration = _iteration;

    // IBNG::_bayesNet = bayesNet;
    __createTree( IBNG::_nbrNodes );
    __transformPoly( IBNG::_nbrNodes / 2 );
    _bayesNettemp = IBNG::_bayesNet;
    __PMMx_poly();

    IBNG::fillCPT();
    _iteration = iteration;

    bayesNet = IBNG::_bayesNet;
  }

  // density represent de
  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::disturbBN(
      BayesNet<GUM_SCALAR>& bayesNetinit,
      Size                  iteration ) {  // insert option for the variation
    _disturbing = true;
    Size iter = _iteration;

    if ( iteration ) _iteration = iteration;

    IBNG::_bayesNet = bayesNetinit;

    if ( __checkConditions() ) {
      LazyPropagation<GUM_SCALAR> inf( &bayesNetinit );
      inf.makeInference();

      for ( auto node : bayesNetinit.nodes() ) {
        Potential<GUM_SCALAR>* pottemp = new Potential<GUM_SCALAR>();
        pottemp->copy( inf.posterior( node ) );
        _hashMarginal.insert( node, pottemp );
      }

      _bayesNettemp = IBNG::_bayesNet;

      if ( __isPolytree() )
        __PMMx_poly();
      else
        __PMMx_multi();

      bayesNetinit = ( IBNG::_bayesNet );

      while ( _hashMarginal.size() ) {
        delete ( _hashMarginal.begin().val() );
        _hashMarginal.erase(
            _hashMarginal.beginSafe() );  // safe iterator needed here.
      }

    } else {
      std::cout << IBNG::_bayesNet.toDot() << std::endl;
      GUM_ERROR( OperationNotAllowed,
                 "BN is not valid cause it does not respect constraint " );
    }

    _iteration = iter;
    _disturbing = false;
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE bool MCBayesNetGenerator<GUM_SCALAR,
                                  ICPTGenerator,
                                  ICPTDisturber>::__checkConditions() {

    return IBNG::_maxArcs >= IBNG::_bayesNet.dag().sizeArcs();
  }

  // main algorithme for moving between state of the IBayesNet according on the
  // nature of the topology polytree or multi-connected

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__PMMx_poly() {
    if ( !_iteration-- ) return;

    Idx per = rand() % 100;

    if ( per < _p ) {
      __AorR();

      if ( __checkConditions() ) {
        _bayesNettemp = IBNG::_bayesNet;
        __PMMx_multi();
      } else {

        IBNG::_bayesNet = _bayesNettemp;
        __PMMx_poly();
      }
    } else {
      if ( per < _p + _q ) {
        __AR();

        if ( !__checkConditions() ) {

          IBNG::_bayesNet = _bayesNettemp;
        } else
          _bayesNettemp = IBNG::_bayesNet;

        __PMMx_poly();
      } else {
        __jump_poly();

        if ( __checkConditions() ) {
          _bayesNettemp = IBNG::_bayesNet;
          __PMMx_multi();

        } else {
          IBNG::_bayesNet = _bayesNettemp;
          __PMMx_poly();
        }
      }
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__PMMx_multi() {

    if ( !_iteration-- ) return;

    Idx per = rand() % 100;

    if ( per < _p + _q ) {
      __AorR();

      if ( __checkConditions() ) {
        if ( __isPolytree() ) {
          if ( per < _p ) {
            _bayesNettemp = IBNG::_bayesNet;
            __PMMx_poly();
          } else {
            IBNG::_bayesNet = _bayesNettemp;
            __PMMx_multi();
          }
        } else {
          _bayesNettemp = IBNG::_bayesNet;
          __PMMx_multi();
        }
      } else {

        IBNG::_bayesNet = _bayesNettemp;
        __PMMx_multi();
      }
    } else {
      __jump_multi();

      if ( __checkConditions() ) {
        _bayesNettemp = IBNG::_bayesNet;

        if ( __isPolytree() )
          __PMMx_poly();
        else
          __PMMx_multi();  // TODO verification required

      } else {
        IBNG::_bayesNet = _bayesNettemp;
        __PMMx_multi();
      }
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__AorR() {
    NodeId i, j;
    __chooseNodes( i, j );
    const DAG __dag = IBNG::_bayesNet.dag();

    if ( __dag.existsArc( i, j ) ) {
      __eraseArc( i, j );

      return;
    } else
      __insertArc( i, j );
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__AR() {
    NodeId i, j, head, tail;
    __chooseNodes( i, j );
    const DAG __dag = IBNG::_bayesNet.dag();

    if ( __dag.existsArc( i, j ) || __dag.existsArc( j, i ) ) {
      return;
    } else {
      Idx per = rand() % 100;

      if ( per < 50 ) {
        head = i;
        tail = j;
      } else {
        head = j;
        tail = i;
      }

      for ( auto node : __dag.parents( j ) ) {
        NodeSet excluded;
        excluded.insert( j );

        if ( __connect( node, i, excluded ) ) {
          std::string nameit = IBNG::_bayesNet.variable( node ).name();
          IBNG::_bayesNet.eraseArc( node, j );  // TODO reflect
          IBNG::_bayesNet.addArc( head, tail );
          return;
        }
      }

      for ( auto node : __dag.children( j ) ) {
        NodeSet excluded;
        excluded.insert( j );

        if ( __connect( node, i, excluded ) ) {
          std::string nameit = IBNG::_bayesNet.variable( node ).name();
          IBNG::_bayesNet.eraseArc( j, node );
          IBNG::_bayesNet.addArc( head, tail );
          return;
        }
      }
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__jump_poly() {
    NodeId i, j;
    __chooseNodes( i, j );
    const DAG __dag = IBNG::_bayesNet.dag();

    if ( !__dag.existsArc( i, j ) ) __insertArc( i, j );
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__jump_multi() {
    NodeId i, j;
    __chooseNodes( i, j );
    const DAG __dag = IBNG::_bayesNet.dag();

    if ( __dag.existsArc( i, j ) ) {
      __eraseArc( i, j );
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__insertArc(
      NodeId i, NodeId j ) {
    if ( __directedPath( j, i ) ) return;

    if ( _disturbing ) {
      Potential<GUM_SCALAR> potj;
      potj = IBNG::_bayesNet.cpt( j );
      IBNG::_bayesNet.addArc( i, j );

      this->disturbAugmCPT( i, j, IBNG::_bayesNet, potj, (GUM_SCALAR)0.5 );
    } else
      IBNG::_bayesNet.addArc( i, j );
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__eraseArc(
      NodeId i, NodeId j, bool mustbeconnex ) {
    if ( _disturbing ) {
      const BayesNet<GUM_SCALAR> bayesNet( IBNG::_bayesNet );
      Potential<GUM_SCALAR>      potj;
      potj.copy( IBNG::_bayesNet.cpt( j ) );
      IBNG::_bayesNet.eraseArc( i, j );

      if ( __connect( i, j ) || !mustbeconnex ) {
        Potential<GUM_SCALAR> marg;
        marg = ( *_hashMarginal[i] );

        this->disturbReducCPT( i, j, IBNG::_bayesNet, potj, marg );
      } else
        IBNG::_bayesNet.addArc( i, j );
    } else {
      IBNG::_bayesNet.eraseArc( i, j );

      if ( !__connect( i, j ) && mustbeconnex ) {

        IBNG::_bayesNet.addArc( i, j );
      }
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__chooseNodes(
      NodeId& i, NodeId& j ) {
    i = rand() % IBNG::_bayesNet.size();
    j = rand() % IBNG::_bayesNet.size();

    while ( i == j )
      j = rand() % IBNG::_bayesNet.size();
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR,
                           ICPTGenerator,
                           ICPTDisturber>::__chooseCloseNodes( NodeId& i,
                                                               NodeId& j ) {
    NodeId temp = rand() % IBNG::_bayesNet.size();
    Size   co = 0;

    if ( IBNG::_bayesNet.dag().parents( temp ).size() ) {
      j = temp;
      auto it = IBNG::_bayesNet.dag().parents( j ).begin();
      co = rand() % IBNG::_bayesNet.dag().parents( j ).size();

      while ( co-- ) {
        ++it;
      }

      i = *it;
    } else if ( IBNG::_bayesNet.dag().children( temp ).size() ) {
      i = temp;
      auto it = IBNG::_bayesNet.dag().children( i ).begin();
      co = rand() % IBNG::_bayesNet.dag().children( i ).size();

      while ( co-- ) {
        ++it;
      }

      j = *it;
    } else
      GUM_ERROR( FatalError, "Sorry Misconstructed BN because of isolated node." )
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__createTree(
      Size BNSize ) {
    Idx               n = 0;
    int               nb_mod = 2 + rand() % ( IBNG::_maxModality - 1 );
    std::stringstream strBuff;
    strBuff << "n_" << n++;
    NodeId root =
        IBNG::_bayesNet.add( LabelizedVariable( strBuff.str(), "", nb_mod ) );
    Size maxNodes = BNSize - 1;
    Size SubG = 0;

    while ( maxNodes ) {
      SubG = ( rand() % ( maxNodes ) ) + 1;
      maxNodes = maxNodes - SubG;
      NodeId rootS = __createPartTree( SubG, n );
      IBNG::_bayesNet.addArc( root, rootS );
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  NodeId
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__createPartTree(
      Size BNSize, Idx& n ) {
    int               nb_mod = 2 + rand() % ( IBNG::_maxModality - 1 );
    std::stringstream strBuff;
    strBuff << "n_" << n++;
    NodeId root =
        IBNG::_bayesNet.add( LabelizedVariable( strBuff.str(), "", nb_mod ) );
    Size maxNodes = BNSize - 1;
    Size SubG = 0;

    while ( maxNodes ) {
      SubG = ( rand() % ( maxNodes ) ) + 1;
      maxNodes = maxNodes - SubG;
      NodeId rootS = __createPartTree( SubG, n );
      IBNG::_bayesNet.addArc( root, rootS );
    }

    return root;
  }

  // Allow to invert maximum nbiter arc to use from polytree only
  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__transformPoly(
      Idx nbiter ) {
    while ( nbiter-- ) {
      NodeId i, j;
      __chooseCloseNodes( i, j );
      _bayesNettemp = IBNG::_bayesNet;
      __eraseArc( i, j, false );
      IBNG::_bayesNet.addArc( j, i );

      if ( !__checkConditions() ) IBNG::_bayesNet = _bayesNettemp;
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE bool
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__isPolytree() {
    const DAG __dag = IBNG::_bayesNet.dag();
    return IBNG::_bayesNet.size() - 1 == IBNG::_bayesNet.sizeArcs();
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__connect(
      const NodeId i, const NodeId j ) {
    const DAG __dag = IBNG::_bayesNet.dag();

    if ( __dag.existsArc( i, j ) || __dag.existsArc( j, i ) )
      return true;
    else {

      NodeSet excluded;
      excluded.insert( i );

      for ( auto par : __dag.parents( i ) ) {
        if ( !excluded.exists( par ) && __connect( par, j, excluded ) )
          return true;
      }

      for ( auto chi : __dag.children( i ) ) {
        if ( !excluded.exists( chi ) && __connect( chi, j, excluded ) )
          return true;
      }

      return false;
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__connect(
      const NodeId i, const NodeId j, NodeSet& excluded ) {
    const DAG __dag = IBNG::_bayesNet.dag();

    if ( __dag.existsArc( i, j ) || __dag.existsArc( j, i ) )
      return true;
    else {
      excluded.insert( i );

      for ( auto par : __dag.parents( i ) ) {
        if ( !excluded.exists( par ) && __connect( par, j, excluded ) )
          return true;
      }

      for ( auto chi : __dag.children( i ) ) {
        if ( !excluded.exists( chi ) && __connect( chi, j, excluded ) )
          return true;
      }

      return false;
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  bool
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__directedPath(
      const NodeId tail, const NodeId head ) {
    const DAG __dag = IBNG::_bayesNet.dag();

    if ( __dag.existsArc( tail, head ) )
      return true;
    else {
      NodeSet excluded;
      excluded.insert( tail );

      for ( auto node : __dag.children( tail ) ) {
        if ( __directedPath( node, head, excluded ) ) return true;
      }

      return false;
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  bool
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__directedPath(
      const NodeId tail, const NodeId head, NodeSet& excluded ) {

    const DAG __dag = IBNG::_bayesNet.dag();

    if ( __dag.existsArc( tail, head ) )
      return true;
    else {
      excluded.insert( tail );

      for ( auto node : __dag.children( tail ) ) {
        if ( !excluded.exists( node ) && __directedPath( node, head, excluded ) )
          return true;
      }

      return false;
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE Size
         MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::iteration()
      const {
    return _iteration;
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE Idx
         MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::p() const {
    return _p;
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE Idx
         MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::q() const {
    return _q;
  }

  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::setIteration(
      Size iteration ) {
    _iteration = iteration;
  }
  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::setP( Idx p ) {
    _p = p;

    if ( p + _q > 100 )
      GUM_ERROR(
          OperationNotAllowed,
          "the sum of the probabilities p and q must be at most equal to 100" );
  }
  template <typename GUM_SCALAR,
            template <typename> class ICPTGenerator,
            template <typename> class ICPTDisturber>
  INLINE void
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::setQ( Idx q ) {
    _q = q;

    if ( _p + q > 100 )
      GUM_ERROR(
          OperationNotAllowed,
          "the sum of the probabilities p and q must be at most equal to 100" );
  }

} /* namespace gum */

#undef IBNG
