#include <agrum/CN/ExtensiveCredalNet.h>

namespace gum {

  template< typename GUM_SCALAR >
  void ExtensiveCredalNet< GUM_SCALAR >::__initCPTs () {
    __credalNet_cptV = NULL;
    __credalNet_cptV = NULL;
  }

  template< typename GUM_SCALAR >
  void ExtensiveCredalNet< GUM_SCALAR >::extensiveDTS () {
    GUM_TRACE ( " ==== DTS ==== " );

    BayesNet< GUM_SCALAR > * src_bn = this->src_bn();
    DAG dag = src_bn->dag();
    const Sequence< NodeId > topoOrder = src_bn->topologicalOrder();

    for ( Sequence< NodeId >::const_iterator id = topoOrder.begin(); id != topoOrder.end(); ++id ) {
      unsigned int dSize = src_bn->variable( id ).domainSize();
      // nothing to do, copy cpts as such
      if ( dSize == 2 ) {
        
      }
      // add D node, rework cpts
      else {

        //NodeId dNodeId = src_bn->add
      }
    }



  }

  /*template< GUM_SCALAR >
  ExtensiveCredalNet< GUM_SCALAR >::ExtensiveCredalNet ( const std::string & path ) {
    GUM_CONSTRUCTOR ( ExtensiveCredalNet );
    __initCPTs ();
  }*/

  template< typename GUM_SCALAR >
  ExtensiveCredalNet< GUM_SCALAR >::ExtensiveCredalNet ( const BayesNet< GUM_SCALAR > & bn ) : CredalNet< GUM_SCALAR >::CredalNet ( bn ) {
    GUM_CONSTRUCTOR ( ExtensiveCredalNet );
    __initCPTs ();
  }

  /*template< GUM_SCALAR >
  ExtensiveCredalNet< GUM_SCALAR >::ExtensiveCredalNet ( const SeparatedCredalNet< GUM_SCALAR > & cn ) {
    GUM_CONSTRUCTOR ( ExtensiveCredalNet );
    __initCPTs ();
  }*/

  /*template< GUM_SCALAR >
  ExtensiveCredalNet< GUM_SCALAR >::ExtensiveCredalNet ( const ExtensiveCredalNet< GUM_SCALAR > & cn ) {
    GUM_CONSTRUCTOR ( ExtensiveCredalNet );
    __initCPTs ();
  }*/

  template< typename GUM_SCALAR >
  ExtensiveCredalNet< GUM_SCALAR >::~ExtensiveCredalNet () {
    GUM_DESTRUCTOR( ExtensiveCredalNet );
  }

  template< typename GUM_SCALAR >
  void ExtensiveCredalNet< GUM_SCALAR >::fillCPT ( const NodeId & id, const std::vector< Potential< GUM_SCALAR > > & cpt ) {
    __credalNet_src_cptP[id] = cpt;
  }
  
  template< typename GUM_SCALAR >
  void ExtensiveCredalNet< GUM_SCALAR >::fillCPT ( const NodeId & id, const std::vector< std::vector< std::vector< GUM_SCALAR > > > & cpt ) {
    __credalNet_src_cptV[id] = cpt;
  }

  template< typename GUM_SCALAR >
  void ExtensiveCredalNet< GUM_SCALAR >::setCPT ( const vCPT & cpt ) {
    __credalNet_src_cptV = cpt;
  }
  
  template< typename GUM_SCALAR >
  void ExtensiveCredalNet< GUM_SCALAR >::setCPT ( const pCPT & cpt ) {
    __credalNet_src_cptP = cpt;
  }

  template< typename GUM_SCALAR >
  const typename Property< std::vector < std::vector < std::vector < GUM_SCALAR > > > >::onNodes & ExtensiveCredalNet< GUM_SCALAR >::vCpt () const {
    if ( __credalNet_cptV == NULL )
      GUM_ERROR ( OperationNotAllowed, " there is no __credalNet_cptV yet ! maybe you wanted __credalNet_src_cptV with vSrcCPT ()" );
    return *__credalNet_cptV;
  }

  template< typename GUM_SCALAR >
  const typename Property< std::vector < Potential< GUM_SCALAR > > >::onNodes & ExtensiveCredalNet< GUM_SCALAR >::pCpt () const {
    if ( __credalNet_cptP == NULL )
      GUM_ERROR ( OperationNotAllowed, " there is no __credalNet_cptP yet ! maybe you wanted __credalNet_src_cptP with pSrcCPT" );

    return *__credalNet_cptP;
  }

  template< typename GUM_SCALAR >
  const typename Property< std::vector < std::vector < std::vector < GUM_SCALAR > > > >::onNodes & ExtensiveCredalNet< GUM_SCALAR >::vSrcCpt () const {
    return __credalNet_src_cptV;
  }

  template< typename GUM_SCALAR >
  const typename Property< std::vector < Potential< GUM_SCALAR > > >::onNodes & ExtensiveCredalNet< GUM_SCALAR >::pSrcCpt () const {
    return __credalNet_src_cptP;
  }

}
