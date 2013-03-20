namespace gum {
  template< typename GUM_SCALAR >
  OptBN< GUM_SCALAR >::OptBN () {
    GUM_CONSTRUCTOR ( OptBN );
  }

  template< typename GUM_SCALAR >
  OptBN< GUM_SCALAR >::OptBN ( const CredalNet<GUM_SCALAR> & cn ) {
    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *cpt = &cn.credalNet_cpt();
    /*size_t elems = 0;
    Size nNodes = cpt->size();
      
    for ( Size node = 0; node < nNodes; node++ ) {
      Size pConfs = ( *cpt )[node].size();
      for ( Size pconf = 0; pconf < pConfs; pconf++ ) {
        Size nVertices = ( *cpt )[node][pconf].size();
        int nBits, newCard;
        cn.superiorPow(nVertices, nBits, newCard);
        elems += nBits;
      }
    }
    __sampleDef.resize( elems );*/

    Size nNodes = cpt->size();
    __sampleDef.resize( nNodes );

    for ( Size node = 0; node < nNodes; node++ ) {
      Size pConfs = ( *cpt )[node].size();
      __sampleDef[node].resize( pConfs );
      for ( Size pconf = 0; pconf < pConfs; pconf++ ) {
        Size nVertices = ( *cpt )[node][pconf].size();
        int nBits, newCard;
        cn.superiorPow(nVertices, nBits, newCard);
        __sampleDef[node][pconf].resize(nBits);
      }
    }

    GUM_CONSTRUCTOR ( OptBN );
  }

  template< typename GUM_SCALAR >
  OptBN< GUM_SCALAR >::~OptBN () {
    GUM_DESTRUCTOR ( OptBN );
  }

  template< typename GUM_SCALAR >
  bool OptBN< GUM_SCALAR >::insert ( const std::vector< bool > & bn, const std::vector< unsigned int > & key ) {
    __currentHash = __vectHash(bn);
    std::list< size_t > & nets = myVarHashs[ key ];
    for ( std::list< size_t >::iterator it = nets.begin(); it != nets.end(); ++it ) {
      if ( *it == __currentHash )
        return false;
    }

    // add it
    myHashNet[__currentHash] = bn;
    // insert net hash in our key net list
    nets.push_back(__currentHash);
    // insert out key in the hash key list
    myHashVars[__currentHash].push_back(key);
    return true;
  }


  template< typename GUM_SCALAR >
  bool OptBN< GUM_SCALAR >::insert ( const std::vector< unsigned int > & key, const bool isBetter ) {
    typedef typename std::vector< unsigned int > varKey;
    typedef typename std::map< size_t, dBN > hashNet;
    typedef typename std::map< varKey, std::list< size_t > > varHashs;
    typedef typename std::map< size_t, std::list< varKey > > hashVars;
/*
    hashNet myHashNet;
    varHashs myVarHashs;
    hashVars myHashVars;
*/

    if ( isBetter ) {
      // get all nets of this key (maybe entry does not exists)
      std::list< size_t > & old_nets = myVarHashs[ key ];
      // for each one
      for ( std::list< size_t >::iterator it = old_nets.begin(); it != old_nets.end(); ++it ) {
        // get all keys associated to this net
        std::list< varKey > & netKeys = myHashVars[ *it ];
        // if we are the sole user, delete the net entry
        if ( netKeys.size() == 1 ) {
          myHashVars.erase ( *it );
        }
        // other keys use the net, delete our key from list
        else {
          for ( std::list< varKey >::iterator it2 = netKeys.begin(); it2 != netKeys.end(); ++it2 ) {
            if ( *it2 == key ) {
              netKeys.erase ( it2 );
              break;
            }
          }
        }  
      } // end of : for each old_net

      // clear all old_nets
      old_nets.clear();
      // insert new net with it's hash
      myHashNet[__currentHash] = __currentSample;
      // insert net hash in our key net list
      old_nets.push_back(__currentHash);
      // insert out key in the hash key list
      myHashVars[__currentHash].push_back(key);
      return true;

    } // end of isBetter
    // another opt net
    else {
      // check that we didn't add it for this key
      std::list< size_t > & nets = myVarHashs[ key ];
      for ( std::list< size_t >::iterator it = nets.begin(); it != nets.end(); ++it ) {
        if ( *it == __currentHash )
          return false;
      }

      // add it
      myHashNet[__currentHash] = __currentSample;
      // insert net hash in our key net list
      nets.push_back(__currentHash);
      // insert out key in the hash key list
      myHashVars[__currentHash].push_back(key);
      return true;
    } // end of ! isBetter
  }

  
  template< typename GUM_SCALAR >
  void OptBN< GUM_SCALAR >::setCurrentSample ( const std::vector< std::vector< std::vector < bool > > > & sample ) {
    __currentSample.clear();
    for ( unsigned int i = 0; i < sample.size(); i++ )
      for ( unsigned int j = 0; j < sample[j].size(); j++ )
        for ( unsigned int k = 0; k < sample[i][j].size(); k++ )
          __currentSample.push_back( sample[i][j][k] );

    //std::cout << sample << std::endl;
    //std::cout << __currentSample << std::endl;

    __currentHash = __vectHash( __currentSample );
  }
  
  template< typename GUM_SCALAR >
  const std::vector< bool > & OptBN< GUM_SCALAR >::getCurrentSample () {
    return __currentSample;
  }

  template< typename GUM_SCALAR >
  const std::vector< std::vector< std::vector < bool > > > & OptBN< GUM_SCALAR >::getSampleDef () {
    return __sampleDef;
  }

  template< typename GUM_SCALAR >
  const std::vector< std::vector< bool > * > OptBN< GUM_SCALAR >::getBNOptsFromKey ( const std::vector< unsigned int > & key ) {
    typedef std::vector< bool > dBN;

    std::list< size_t > & netsHash = myVarHashs.at(key);

    std::vector< dBN * > nets;
    nets.resize( netsHash.size() );

    std::list< size_t >::iterator it = netsHash.begin();

    for ( unsigned int i = 0; i < netsHash.size(); i++, ++it ) {
      nets[i] = & myHashNet.at( *it );
    }

    return nets;
  }

  template< typename GUM_SCALAR >
  const std::vector< std::vector< std::vector< std::vector < bool > > > > OptBN< GUM_SCALAR >::getFullBNOptsFromKey ( const std::vector< unsigned int > & key ) {
    typedef typename std::vector< unsigned int > varKey;
    typedef typename std::map< size_t, dBN > hashNet;
    typedef typename std::map< varKey, std::list< size_t > > varHashs;
    typedef typename std::map< size_t, std::list< varKey > > hashVars;
/*
    hashNet myHashNet;
    varHashs myVarHashs;
    hashVars myHashVars;
*/

    typedef std::vector< bool > dBN;

    std::list< size_t > & netsHash = myVarHashs.at(key);

    std::vector< std::vector< std::vector< std::vector < bool > > > > nets;
    nets.resize( netsHash.size(), __sampleDef );

    std::list< size_t >::iterator it = netsHash.begin();

    for ( unsigned int i = 0; i < netsHash.size(); i++, ++it ) {
      //std::vector< std::vector< std::vector < bool > > > net(__sampleDef);
      dBN::iterator it2 = myHashNet.at( *it ).begin();
      for ( unsigned int j = 0; j < __sampleDef.size(); j++ ) {
        for( unsigned int k = 0; k < __sampleDef[j].size(); k++ ) {
          for ( unsigned int l = 0; l < __sampleDef[j][k].size(); l++ ) {
            nets[i][j][k][l] = *it2;
            ++it2;
          }
        }
      }
    }

    return nets;
  }

  
  template< typename GUM_SCALAR >
  unsigned int OptBN< GUM_SCALAR >::getEntrySize() const {
    return myHashNet.size();
  }


}
