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
// ============================================================================
#include <agrum/BN/generator/MCBayesNetGenerator.h>

// ============================================================================


template<typename GUM_SCALAR>
gum::Size getMaxModality ( gum::BayesNet<GUM_SCALAR> & bayesNet ) {
  gum::Size maxMod = 0;
  gum::DAG::NodeIterator node = bayesNet.beginNodes();

  while ( node != bayesNet.endNodes() ) {
    if ( maxMod < bayesNet.variable ( *node ).domainSize() )
      maxMod = bayesNet.variable ( *node ).domainSize();

    ++node;
  }

  return maxMod;
}

namespace gum {

// Default constructor.
// Use the DefaultCPTGenerator for generating the BNs CPT.
  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::MCBayesNetGenerator ( Size nbrNodes, Size maxArcs, Idx maxModality, Size iteration , Idx p, Idx q ) :
    AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator> ( nbrNodes, maxArcs , maxModality ), _bayesNettemp(), _hashMarginal() {
    if ( p + q > 100 ) GUM_ERROR ( OperationNotAllowed, "the sum of the probabilities p and q must be at most equal to 100" );

    _c = 0;
    _iteration = iteration;
    _p = p;
    _q = q;
    _disturbing = false;

    GUM_CONSTRUCTOR ( MCBayesNetGenerator );
  }

  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::MCBayesNetGenerator ( BayesNet<GUM_SCALAR> bayesNet, Size iteration , Idx p, Idx q ) :
    AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator> ( bayesNet.size(), ( Size ) ( bayesNet.nbrArcs() * 1.1 ) , getMaxModality ( bayesNet ) ), _bayesNettemp(), _hashMarginal() {

    _c = 0;
    _iteration = iteration;
    _p = p;
    _q = q;
    _disturbing = false;

    GUM_CONSTRUCTOR ( MCBayesNetGenerator );
  }
// Destructor.
  template<typename GUM_SCALAR, template<class> class ICPTGenerator , template <class> class ICPTDisturber>
  MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::~MCBayesNetGenerator() {
    GUM_DESTRUCTOR ( MCBayesNetGenerator );

  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::generateBN ( BayesNet<GUM_SCALAR> & bayesNet ) {


    Idx iteration = _iteration;

    //AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = bayesNet;
    __createTree ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_nbrNodes );
    __transformPoly ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_nbrNodes / 2 );
    _bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet ;
    __PMMx_poly();

    AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::fillCPT();
    _iteration = iteration;

    bayesNet = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet;

  }


// density represent de
  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::disturbBN ( BayesNet<GUM_SCALAR> & bayesNetinit, Size iteration ) { // insert option for the variation
    _disturbing = true;
    Size iter = _iteration;

    if ( iteration ) _iteration = iteration;

    AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = bayesNetinit;

    if ( __checkConditions() ) {
      LazyPropagation<GUM_SCALAR> inf ( bayesNetinit );
      inf.makeInference();

      for ( DAG::NodeIterator it = bayesNetinit.beginNodes(); it != bayesNetinit.endNodes(); ++it ) {
        Potential<GUM_SCALAR> * pottemp = new Potential<GUM_SCALAR>();
        pottemp->copy ( inf.marginal ( *it ) );
        _hashMarginal.insert ( *it, pottemp );

      }

      _bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet ;

      if ( __isPolytree() ) __PMMx_poly();
      else __PMMx_multi();

      bayesNetinit = ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet );

      while ( _hashMarginal.size() ) {
        delete ( * ( _hashMarginal.begin() ) );
        _hashMarginal.erase ( _hashMarginal.begin() );

      }

    } else {
      std::cout << AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.toDot()  << std::endl;
      GUM_ERROR ( OperationNotAllowed, "BN is not valid cause it does not respect constraint " );
    }

    _iteration = iter;
    _disturbing = false;

  }

  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__checkConditions() {

    return AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_maxArcs >= AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag().sizeArcs();

  }

//main algorithme for moving between state of the BayesNet according on the nature of the topology polytree or multi-connected

  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__PMMx_poly() {
    if ( !_iteration-- )
      return;

    srand ( time ( NULL ) + _c++ );
    Idx per = rand() % 100;

    if ( per < _p ) {
      __AorR();

      if ( __checkConditions() ) {_bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet; __PMMx_multi();}
      else {

        AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = _bayesNettemp ;
        __PMMx_poly();
      }
    } else {
      if ( per < _p + _q ) {
        __AR();

        if ( !__checkConditions() ) {

          AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = _bayesNettemp;
        } else _bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet;

        __PMMx_poly();
      } else {
        __jump_poly();

        if ( __checkConditions() ) {
          _bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet ;
          __PMMx_multi();

        } else {
          AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet =   _bayesNettemp;
          __PMMx_poly();

        }
      }
    }

  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__PMMx_multi() {

    if ( !_iteration-- )
      return;

    srand ( time ( NULL ) + _c++ );
    Idx per = rand() % 100;

    if ( per < _p + _q ) {
      __AorR();

      if ( __checkConditions() ) {
        if ( __isPolytree() ) {
          if ( per < _p )  {_bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet ; __PMMx_poly(); }
          else {AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = _bayesNettemp; __PMMx_multi();}
        } else {_bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet; __PMMx_multi();}
      } else {

        AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = _bayesNettemp;
        __PMMx_multi();
      }
    } else {
      __jump_multi();

      if ( __checkConditions() ) {
        _bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet;

        if ( __isPolytree() )
          __PMMx_poly();
        else
          __PMMx_multi();//TODO verification required

      } else {
        AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = _bayesNettemp ;
        __PMMx_multi();
      }
    }

  }



  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__AorR() {
    srand ( time ( NULL ) + _c++ );
    NodeId i, j;
    __chooseNodes ( i, j );
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( __dag.existsArc ( i, j ) ) {
      __eraseArc ( i,  j );

      return;
    } else __insertArc ( i,  j );

  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__AR() {

    srand ( time ( NULL ) + _c++ );
    NodeId i, j, head, tail;
    __chooseNodes ( i, j );
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( __dag.existsArc ( i, j ) || __dag.existsArc ( j, i ) ) {
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


      for ( NodeSetIterator it = __dag.parents ( j ).begin(); it != __dag.parents ( j ).end(); ++it ) {
        NodeSet excluded;
        excluded.insert ( j );

        if ( __connect ( *it, i, excluded ) ) {
          std::string nameit = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.variable ( *it ).name();
          AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.eraseArc ( *it, j ); //TODO reflect
          AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( head, tail ); return;
        }
      }

      for ( NodeSetIterator it = __dag.children ( j ).begin(); it != __dag.children ( j ).end(); ++it ) {
        NodeSet excluded;
        excluded.insert ( j );

        if ( __connect ( *it, i, excluded ) ) {
          std::string nameit = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.variable ( *it ).name();
          AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.eraseArc ( j, *it );
          AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( head, tail ); return;
        }
      }
    }
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__jump_poly() {
    srand ( time ( NULL ) + _c++ );
    NodeId i, j;
    __chooseNodes ( i, j );
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( !__dag.existsArc ( i, j ) )
      __insertArc ( i, j );
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__jump_multi() {
    srand ( time ( NULL ) + _c++ );
    NodeId i, j;
    __chooseNodes ( i, j );
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( __dag.existsArc ( i, j ) ) {
      __eraseArc ( i, j );

    }
  }




  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__insertArc ( NodeId i, NodeId j ) {
    if ( __directedPath ( j, i ) ) return;

    if ( _disturbing ) {
      Potential<GUM_SCALAR> potj;
      potj = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.cpt ( j );
      AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( i, j );

      this->disturbAugmCPT ( i, j, AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet, potj, ( GUM_SCALAR ) 0.5 );
    } else AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( i, j );
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__eraseArc ( NodeId i, NodeId j, bool mustbeconnex ) {
    if ( _disturbing ) {
      const BayesNet<GUM_SCALAR> bayesNet ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet );
      Potential<GUM_SCALAR> potj;
      potj.copy ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.cpt ( j ) );
      AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.eraseArc ( i, j );

      if ( __connect ( i, j ) || ! mustbeconnex ) {
        Potential<GUM_SCALAR> marg;
        marg = ( *_hashMarginal[i] );

        this->disturbReducCPT ( i, j, AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet, potj, marg );
      } else AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( i, j );
    } else {
      AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.eraseArc ( i, j );


      if ( !__connect ( i, j ) && mustbeconnex ) {

        AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( i, j );

      }

    }
  }





  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >  INLINE
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__chooseNodes ( NodeId &i, NodeId &j ) {
    srand ( time ( NULL ) + _c++ );
    i = rand() % AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.size();
    j = rand() % AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.size();

    while ( i == j )
      j = rand() % AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.size();
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber>
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__chooseCloseNodes ( NodeId &i, NodeId &j ) {
    srand ( time ( NULL ) + _c++ );
    Idx temp = rand() % AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.size();
    Size co = 0;

    if ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag().parents ( temp ).size() ) {
      j = temp;
      NodeSetIterator it = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag().parents ( j ).begin();
      co = rand() % AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag().parents ( j ).size();

      while ( co-- ) {
        ++it;
      }

      i = *it;
    } else if ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag().children ( temp ).size() ) {
      i = temp;
      NodeSetIterator it = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag().children ( i ).begin();
      co = rand() % AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag().children ( i ).size();

      while ( co-- ) {
        ++it;
      }

      j = *it;
    } else GUM_ERROR ( FatalError, "Sorry Misconstructed BN because of isolated node." )

    }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__createTree ( Size BNSize ) {
    Idx n = 0;
    srand ( time ( NULL ) + n );
    Idx nb_mod = 2 + rand() % ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_maxModality - 1 );
    std::stringstream strBuff;
    strBuff << "n_" << n++;
    NodeId root = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.addVariable ( LabelizedVariable ( strBuff.str(), "" , nb_mod ) );
    Size maxNodes = BNSize - 1;
    Size SubG = 0;

    while ( maxNodes ) {
      SubG = ( rand() % ( maxNodes ) ) + 1 ;
      maxNodes = maxNodes - SubG;
      NodeId rootS = __createPartTree ( SubG , n );
      AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( root, rootS );
    }
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  NodeId MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__createPartTree ( Size BNSize , Idx &n ) {
    srand ( time ( NULL ) + n );
    Idx nb_mod = 2 + rand() % ( AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_maxModality - 1 );
    std::stringstream strBuff;
    strBuff << "n_" << n++;
    NodeId root = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.addVariable ( LabelizedVariable ( strBuff.str(), "" , nb_mod ) );
    Size maxNodes = BNSize - 1;
    Size SubG = 0;

    while ( maxNodes ) {
      SubG = ( rand() % ( maxNodes ) ) + 1 ;
      maxNodes = maxNodes - SubG;
      NodeId rootS = __createPartTree ( SubG , n );
      AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( root, rootS );
    }

    return root;
  }


// Allow to invert maximum nbiter arc to use from polytree only
  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__transformPoly ( Idx nbiter ) {
    while ( nbiter-- ) {
      NodeId i, j;
      __chooseCloseNodes ( i, j );
      _bayesNettemp = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet;
      __eraseArc ( i,  j, false );
      AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.insertArc ( j, i );

      if ( !__checkConditions() )
        AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet = _bayesNettemp;
    }
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber > INLINE
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__isPolytree() {
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();
    return AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.size() - 1 == AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.nbrArcs();
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__connect ( const NodeId i, const NodeId j ) {
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( __dag.existsArc ( i, j ) || __dag.existsArc ( j, i ) )
      return true;
    else {

      NodeSet excluded;
      excluded.insert ( i );
      const NodeSet set_parent = __dag.parents ( i );

      for ( NodeSetIterator par = set_parent.begin(); par != set_parent.end(); ++par ) {
        if ( !excluded.exists ( *par ) && __connect ( *par, j, excluded ) )
          return true;
      }

      const NodeSet  set_children = __dag.children ( i );

      for ( NodeSetIterator par = set_children.begin(); par != set_children.end(); ++par ) {
        if ( !excluded.exists ( *par ) && __connect ( *par, j, excluded ) )
          return true;
      }

      return false;
    }

  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__connect ( const NodeId i, const NodeId j, NodeSet &excluded ) {
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( __dag.existsArc ( i, j ) || __dag.existsArc ( j, i ) )
      return true;
    else {
      excluded.insert ( i );
      const NodeSet set_parent = __dag.parents ( i );

      for ( NodeSetIterator par = set_parent.begin(); par != set_parent.end(); ++par ) {
        if ( !excluded.exists ( *par ) && __connect ( *par, j, excluded ) )
          return true;
      }

      const NodeSet  set_children = __dag.children ( i );

      for ( NodeSetIterator par = set_children.begin(); par != set_children.end(); ++par ) {
        if ( !excluded.exists ( *par ) && __connect ( *par, j, excluded ) )
          return true;
      }

      return false;
    }
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber >
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__directedPath ( const NodeId tail, const NodeId head ) {
    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( __dag.existsArc ( tail, head ) )
      return true;
    else {
      NodeSet excluded;
      excluded.insert ( tail );
      const NodeSet  set_children = __dag.children ( tail );

      for ( NodeSetIterator node = set_children.begin(); node != set_children.end(); ++node ) {
        if ( __directedPath ( *node, head, excluded ) )
          return true;
      }

      return false;
    }
  }

  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber>
  bool MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__directedPath ( const NodeId tail, const NodeId head, NodeSet &excluded ) {

    const DAG __dag = AbstractBayesNetGenerator<GUM_SCALAR, ICPTGenerator>::_bayesNet.dag();

    if ( __dag.existsArc ( tail, head ) )
      return true;
    else {
      const NodeSet  set_children = __dag.children ( tail );
      excluded.insert ( tail );

      for ( NodeSetIterator node = set_children.begin(); node != set_children.end(); ++node ) {
        if ( !excluded.exists ( *node ) && __directedPath ( *node, head, excluded ) )
          return true;
      }

      return false;
    }
  }



  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
  Size MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::iteration() const {
    return _iteration;
  }

  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
  Idx MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::p() const {
    return _p;
  }

  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
  Idx MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::q() const {
    return _q;
  }


  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::setIteration ( Size iteration ) {
    _iteration = iteration;
  }
  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::setP ( Idx p ) {
    _p = p ;

    if ( p + _q > 100 ) GUM_ERROR ( OperationNotAllowed, "the sum of the probabilities p and q must be at most equal to 100" );
  }
  template<typename GUM_SCALAR, template<class> class ICPTGenerator, template <class> class ICPTDisturber> INLINE
  void MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::setQ ( Idx q ) {
    _q = q;

    if ( _p + q > 100 ) GUM_ERROR ( OperationNotAllowed, "the sum of the probabilities p and q must be at most equal to 100" );
  }


} /* namespace gum */
// kate: indent-mode cstyle; indent-width 1; replace-tabs on; ;

