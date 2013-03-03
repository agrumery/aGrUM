#include <algorithm>

#include <agrum/CN/LoopyPropagation.h>

namespace gum {

  template< typename GUM_SCALAR >
  LoopyPropagation< GUM_SCALAR >::LoopyPropagation ( const CredalNet< GUM_SCALAR > & credalNet ) : InferenceEngine< GUM_SCALAR >::InferenceEngine ( credalNet ) {
    GUM_CONSTRUCTOR ( LoopyPropagation );
  }

  template< typename GUM_SCALAR >
  LoopyPropagation< GUM_SCALAR >::~LoopyPropagation() {
    GUM_DESTRUCTOR ( LoopyPropagation );
  }

  template< typename GUM_SCALAR >
  void LoopyPropagation< GUM_SCALAR >::makeInference() {

    __init();

    gum::DAG dag = this->_credalNet->current_bn().dag();
  }

  template< typename GUM_SCALAR >
  void LoopyPropagation< GUM_SCALAR >::__init() {
    gum::DAG dag = this->_credalNet->current_bn().dag();

    const gum::Sequence< gum::NodeId > & topoNodes = this->_credalNet->current_bn().topologicalOrder();

    for ( gum::Sequence< gum::NodeId >::const_iterator id = topoNodes.begin(); id != topoNodes.end(); ++id ) {
      //__notifiedNodesOnP.insert(*id);
      //__notifiedNodesOnL.insert(*id);

      // create orders
      std::vector< int > order ( this->_credalNet->current_bn().variable ( *id ).domainSize() );

      for ( int mod = 0; mod < order.size(); mod++ )
        order[mod] = mod;

      std::vector< std::vector< int > > perms;

      do {
        perms.push_back ( order );
      } while ( std::next_permutation ( order.begin(), order.end() ) );

      __orders.insert ( std::pair< int, std::vector< std::vector < int > > > ( order.size(), perms ) );

      // get parents' messages
      const gum::Potential< GUM_SCALAR > * cpt = &this->_credalNet->current_bn().cpt ( *id );
      typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes msgsParents;

      for ( gum::Sequence< const gum::DiscreteVariable * >::const_iterator pv = cpt->begin(); pv != cpt->end(); ++pv ) {
        gum::NodeId pid = this->_credalNet->current_bn().nodeId ( **pv );

        if ( pid == *id )
          continue;

        msgsParents.insert ( pid, __nodesP[pid] );
      }

      std::vector< std::vector< GUM_SCALAR > > msgsP;
      __msgP ( *id, msgsParents, msgsP );


    }
  }

  template< typename GUM_SCALAR >
  void LoopyPropagation< GUM_SCALAR >::__msgP ( const gum::NodeId &id, const typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes &msgsParents, typename std::vector< std::vector< GUM_SCALAR > > & msgsP ) {

    int dSize = this->_credalNet->current_bn().variable ( id ).domainSize();
    std::vector< std::vector< int > > orders = __orders[dSize];


  }


} // namespace cn
