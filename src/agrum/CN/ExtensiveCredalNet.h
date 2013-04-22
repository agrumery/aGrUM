#ifndef __EXTENSIVECREDAL_NET__H__
#define __EXTENSIVECREDAL_NET__H__

#include <agrum/CN/CredalNet.h>
//#include <agrum/CN/SeparatedCredalNet.h>

namespace gum {

  template< typename GUM_SCALAR >
  class ExtensiveCredalNet : public CredalNet< GUM_SCALAR > {
      typedef typename Property< std::vector < std::vector < std::vector < GUM_SCALAR > > > >::onNodes vCPT;
      typedef typename Property< std::vector < Potential< GUM_SCALAR > > >::onNodes pCPT;

    private :
      // this CPT (vCPT) is the same type than CredalNet.h cpt field
      vCPT __credalNet_src_cptV; // to use with custom algorithms (without using gum potentials)

      // or
      pCPT __credalNet_src_cptP; // to use with BNEngines (sampling etc), faster fill

      vCPT *__credalNet_cptV;
      pCPT *__credalNet_cptP;

      void __initCPTs();

    protected :

    public :
      // from (extensive) file
      //ExtensiveCredalNet ( const std::string & path );
      // to get a DAG, we don't care about the cpts in this case
      // unless there is a way to contaminate a BN to an extensive CN ?
      ExtensiveCredalNet ( const BayesNet< GUM_SCALAR > & bn );

      // from separately specified credal net (beware of huge number of tables), may fail, should have some memory size anticipation !
      //ExtensiveCredalNet ( const SeparatedCredalNet< GUM_SCALAR > & cn );

      // copy net
      //ExtensiveCredalNet ( const ExtensiveCredalNet< GUM_SCALAR > & cn );
      ~ExtensiveCredalNet ();

      // no reader yet, manual fill
      void fillCPT ( const NodeId &id, const std::vector< Potential< GUM_SCALAR > > & cpt );
      void fillCPT ( const NodeId &id, const std::vector< std::vector< std::vector< GUM_SCALAR > > > & cpt );
      void setCPT ( const vCPT &cpt );
      void setCPT ( const pCPT &cpt );

      void extensiveDTS();
      //void bin();

      // from one specification to another
      // this is a DTS, it gives a precise network with vacuous decision nodes, separately specified (may work with GL2U !)
      //SeparatedCredalNet & separatedDTS () const;

      // will call DTS() and rework D nodes (huge number of tables, separated above use much less space to store the same information)
      //ExtensiveCredalNet & extensiveDTS () const;

      // get src cpts
      const vCPT &vSrcCpt () const;
      const pCPT &pSrcCpt () const;

      // get the net CPTs (either one, upToDate)
      const vCPT &vCpt() const;
      const pCPT &pCpt() const;


  };

}

#include <agrum/CN/ExtensiveCredalNet.tcc>

#endif
