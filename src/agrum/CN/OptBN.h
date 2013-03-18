#ifndef ___OPTBN__H___
#define ___OPTBN__H___

// graphe bi-partie avec hash-maps

/**
 * { [ nodeId, modality, min(0) | max(1) ] } <-- dBN
 * [ nodeId, modality, min(0) | max(1) ] --> { dBN }
 */

namespace gum {

  template< typename GUM_SCALAR >
  class OptBN {
    typedef std::vector< std::vector< std::vector< bool > > > dBN;

    // A -> { *B } et *B -> { A } maps
    // beware of deletes / inserts
    typedef typename std::map< std::vector< unsigned int > , std::vector< dBN* > > varKey;
    typedef typename std::map< dBN*, std::vector< std::vector< unsigned int > > > dBNKey;

    private :
      varKey varToBN;
      dBNKey dBNToVar;

      // all samples have the same size
      // instead of creating them one by one, we will copy this one each time
      dBN __sampleDef;
      // the sampled net during inference
      dBN * __currentSample;

    protected :

    public :
      // initialize __sampleDef from credalNet
      OptBN ( const CredalNet<GUM_SCALAR> & cn );
      // used for fusion, no need of sampleDef anymore
      OptBN ();
      ~OptBN ( );

      // insert do everything ( removing duplicates at insertion, etc )
      bool insert ( /*dBN * bn,*/ std::vector< unsigned int > & key, const bool isBetter );
      void insertCopyNoCheck ( dBN * bn, std::vector< unsigned int > & key/*, const bool isBetter*/ );

      void setCurrentSample ( dBN * sample );
      dBN * getCurrentSample ();
      const dBN & getSampleDef ();


      std::vector< dBN* > * getBNOptsFromKey ( std::vector< unsigned int > & key );
      /*std::vector< std::vector< unsigned int > > & getKeysFromBNOpts ( dBN & bn );*/

      // how many dBN stored 
      unsigned int getEntrySize() const;

  }; // end of OptBN

}

#include <agrum/CN/OptBN.tcc>

#endif
