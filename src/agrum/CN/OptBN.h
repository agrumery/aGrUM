#ifndef ___OPTBN__H___
#define ___OPTBN__H___

#include <tr1/functional>
#include "EmptyClass.h"

namespace gum {

  template < typename GUM_SCALAR >
  class OptBN {
    typedef std::vector< bool > dBN;

    typedef typename std::vector< unsigned int > varKey;
    typedef typename std::map< size_t, dBN > hashNet;
    typedef typename std::map< varKey, std::list< size_t > > varHashs;
    typedef typename std::map< size_t, std::list< varKey > > hashVars;

    // not used yet, keep expectations for each optimal net
    typedef typename std::map< size_t, typename gum::Property< GUM_SCALAR >::onNodes > hashExp;
    // not used yet, keep marginals for each optimal net
    typedef typename std::map< size_t, typename gum::Property< std::vector< GUM_SCALAR > >::onNodes > hashMargi;

    private :

    protected :
      // last version :
      hashNet _myHashNet;
      varHashs _myVarHashs;
      hashVars _myHashVars;

      // all samples have the same size
      // instead of creating them one by one, we will copy this one each time
      std::vector< std::vector< std::vector < bool > > > _sampleDef;
      // the sampled net during inference
      dBN _currentSample;
      size_t _currentHash;
      std::hash< std::vector< bool > > _vectHash;

    public :
      // initialize __sampleDef from credalNet
      OptBN ( const CredalNet<GUM_SCALAR> & cn );
      // used for fusion, no need of sampleDef anymore
      OptBN ();
      ~OptBN ( );

      // fusion insert ONLY ( do not remove stuff from maps )
      bool insert ( const std::vector< bool > & bn, const std::vector< unsigned int > & key );
      // insert do everything ( removing duplicates at insertion, etc )
      bool insert ( const std::vector< unsigned int > & key, const bool isBetter );

      // set the sample and its hash
      void setCurrentSample ( const std::vector< std::vector< std::vector < bool > > > & sample );
      // get the sample ( new form )
      const dBN & getCurrentSample ();

      const std::vector< std::vector< std::vector < bool > > > & getSampleDef ();

      const std::vector< dBN* > getBNOptsFromKey ( const std::vector< unsigned int > & key );

      // with delimiters
      std::vector< std::vector< std::vector< std::vector < bool > > > > getFullBNOptsFromKey ( const std::vector< unsigned int > & key );
      
      // how many dBNs stored 
      unsigned int getEntrySize() const;

  }; // end of OptBN

}

#include <agrum/CN/OptBN.tcc>

#endif
