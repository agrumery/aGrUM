namespace gum {
  template< typename GUM_SCALAR >
  OptBN< GUM_SCALAR >::OptBN () {
    GUM_CONSTRUCTOR ( OptBN );
  }

  template< typename GUM_SCALAR >
  OptBN< GUM_SCALAR >::OptBN ( const CredalNet<GUM_SCALAR> & cn ) {
    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *cpt = &cn.credalNet_cpt();
    
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
    // delete all pointers ( beware of duplicates in maps )
    // it is clear we will use the BN --> { var - mod - min|max } map
    typedef typename std::map< dBN*, std::vector< std::vector< unsigned int > > > dBNKey;

    std::cout << "nb pointers : " << dBNToVar.size() << std::endl;
    unsigned int cpt = 0;
    for ( dBNKey::iterator it = dBNToVar.begin(); it != dBNToVar.end(); ++it ) {
      delete it->first;
      cpt++;
      std::cout << " #delete : " << cpt << "\r";
    }
    std::cout << std::endl;
    dBNToVar.clear();
    varToBN.clear(); // all pointers should have been deleted BEFORE

    GUM_DESTRUCTOR ( OptBN );
  }

  // use only for fusion, when you know that what you add does not require to delete older entries
  // realise a copy of all elements with new (delete needed)
  template< typename GUM_SCALAR >
  void OptBN< GUM_SCALAR >::insertCopyNoCheck ( std::vector< std::vector< std::vector< bool > > > * bn, std::vector< unsigned int > & key/*, const bool isBetter*/ ) {
    typedef std::vector< std::vector< std::vector< bool > > > dBN;

    typedef typename std::map< std::vector< unsigned int > , std::vector< dBN* > > varKey;
    typedef typename std::map< dBN*, std::vector< std::vector< unsigned int > > > dBNKey;

    std::vector< dBN* > & bnValue = varToBN[key];
    unsigned int bnSet = bnValue.size();
    // there is an identical net already registered, do nothing
    /*
    bool found = false;
    unsigned int bnSet = bnValue.size();
    unsigned int tSize;
    unsigned int tsampl;
    unsigned int rtsampl;
    #pragma omp parallel
    {
      #pragma omp single
      {
        tSize = gum_threads::getNumberOfRunningThreads();
        tsampl = bnSet / tSize;
        rtsampl = bnSet - tsampl * tSize;
      }
      #pragma omp barrier
      #pragma omp flush(tSize)
    }

    if ( bnSet >= tSize ) {
      #pragma omp parallel
      {
        unsigned int tnum = gum_threads::getThreadNumber();
        unsigned int first = tsampl * tnum;
        unsigned int last = first + tsampl - 1;
        if ( tnum == (tSize - 1) )
          last += rtsampl;

        while ( ! found && first <= last ) {
          if ( * ( bnValue[first] ) == * bn ) // don't compare adresses but values
            found = true;
          first++;
          #pragma omp flush(found)
        }
      }
      if ( found )
        return;
    }
    else
    {*/
      for ( Size iter = 0; iter < bnSet ; iter++ )
        if ( *( bnValue[iter] ) == *bn )
          return;
  /*  }
   
    bnSet = dBNToVar.size();
    #pragma omp parallel
    {
      #pragma omp single
      {
        tSize = gum_threads::getNumberOfRunningThreads();
        tsampl = bnSet / tSize;
        rtsampl = bnSet - tsampl * tSize;
      }
      #pragma omp barrier
      #pragma omp flush(tSize)
    }
    if ( bnSet >= tSize ) {
      found = false;
      #pragma omp parallel
      {
        unsigned int tnum = gum_threads::getThreadNumber();
        unsigned int first = tsampl * tnum;
        unsigned int last = first + tsampl - 1;
        if ( tnum == (tSize - 1) )
          last += rtsampl;

        while ( ! found && first <= last ) {
          dBNKey::iterator it = dBNToVar.begin();
          for ( unsigned int pos = 0; pos < first; pos++ )
            ++it;
          if ( * ( it->first ) == * bn ) {
            it->second.push_back( key );
            bnValue.push_back( it->first );
            found = true;
          }
          first++;
        #pragma omp flush(found)
        }
      }
    
      if ( found )
        return;
    }
    else {*/
      for ( dBNKey::iterator it = dBNToVar.begin(); it != dBNToVar.end(); ++it ) {
        // we found an identical net, do NOT use a new pointer
        if ( *( it->first ) == *bn ) {
          it->second.push_back(key);
          bnValue.push_back(it->first);
          return;
        }
      }
    //}

    // no identical bn was found, insert everything with new pointer
    dBN * newBN = new dBN(*bn);
    std::vector< std::vector< unsigned int > > & keyValue = dBNToVar[newBN];
    keyValue.push_back(key);
    bnValue.push_back(bn);
  }


  template< typename GUM_SCALAR >
  bool OptBN< GUM_SCALAR >::insert ( /*std::vector< std::vector< std::vector< bool > > > * bn,*/ std::vector< unsigned int > & key, const bool isBetter ) {
    // typedef
    ///////////////////////////////////////////////////////////////////
    typedef std::vector< std::vector< std::vector< bool > > > dBN;

    // *A -> { *B } et *B -> { *A } maps
    // beware of deletes / inserts
    typedef typename std::map< std::vector< unsigned int > , std::vector< dBN* > > varKey;
    typedef typename std::map< dBN*, std::vector< std::vector< unsigned int > > > dBNKey;
    ///////////////////////////////////////////////////////////////////
  std::vector< std::vector< std::vector< bool > > > * bn = this->__currentSample;

    // bool isMin = (key[2] == 0) ? true : false;
    // better min/max pValue :
    // replace old BN :
    // if not shared anymore : delete it
    // still shared : don't delete it, just remove the old map entry
    // do the same for vars ids according to previous result
    if ( isBetter ) {
      // optimum nets for this key
      std::vector< dBN* > & bnValue = varToBN[key];
      // before deleting them, check if other keys uses them, with the reverse map
      // keys using the same networks
      for ( unsigned int iter = 0; iter < bnValue.size(); iter++ ) {
        dBN * currentBN = bnValue[iter];
        std::vector< std::vector< unsigned int > > & keyValue = dBNToVar[currentBN];
        // we are the sole key using this BN, delete it and entry
        if ( keyValue.size() == 1 /*&& keyValue[0] == key*/ ) {
          // delete the net from its pointer
          dBNToVar.erase(currentBN);
          delete currentBN;
        }
        // others vars use the same BN, juste remove ourself from the keys
        else {
          for ( std::vector< std::vector< unsigned int > >::iterator it = keyValue.begin(); it != keyValue.end(); ++it ) {
            if ( *it == key ) {
              keyValue.erase(it); // invalidate iterators, break needed !
              break;
            }
          }
        } // end of else

      } // end of : for each old network

      // add the new net to varToBN entry
      //dBN * addbn = bn;
      bnValue.push_back(bn/*__currentSample*/);

      // and to dBNToVar
      std::vector< std::vector< unsigned int > > & keyValue = dBNToVar[/*__currentSample*/bn];
      keyValue.push_back(key);

      return true;

    } // end of : if better bn ( from better marginal )
    // push-back in one map and insert in the other ( it may already exist )
    else {
      std::vector< dBN* > & bnValue = varToBN[key];

      // there is an identical net already registered, do nothing
      for ( Size iter = 0; iter < bnValue.size(); iter++ )
        if ( *( bnValue[iter] ) == *bn ) // no adress comparison
          return false;

      // don't use bn as key !!!! it will look for adress instead of value
      // compare values with iterators
      /*for ( dBNKey::iterator it = dBNToVar.begin(); it != dBNToVar.end(); ++it ) {
        // we found an identical net, do NOT use the new pointer bn
        if ( *( it->first ) == *bn ) {
          
          it->second.push_back(key);
          bnValue.push_back(it->first);
          return false; // we didn't use the new pointer ! (argument bn)
        }
      }*/

      // no identical bn was found, insert everything with new pointer
      std::vector< std::vector< unsigned int > > & keyValue = dBNToVar[bn];
      keyValue.push_back(key);
      bnValue.push_back(bn);
      return true;

    } // end of : not better net, another opt

  }

  template< typename GUM_SCALAR >
  void OptBN< GUM_SCALAR >::setCurrentSample ( dBN * sample ) {
    __currentSample = sample;
  }
  
  template< typename GUM_SCALAR >
  std::vector< std::vector< std::vector< bool > > > * OptBN< GUM_SCALAR >::getCurrentSample () {
    return __currentSample;
  }

  template< typename GUM_SCALAR >
  const std::vector< std::vector< std::vector< bool > > > & OptBN< GUM_SCALAR >::getSampleDef () {
    return __sampleDef;
  }

  template< typename GUM_SCALAR >
  std::vector< std::vector< std::vector< std::vector< bool > > > * > * OptBN< GUM_SCALAR >::getBNOptsFromKey ( std::vector< unsigned int > & key ) {
    if ( key.size() > 3 )
      GUM_ERROR(NotFound, "wrong key format");
    return &varToBN[key];

  }
  
  /*template< typename GUM_SCALAR >
  std::vector< std::vector< unsigned int > > & OptBN< GUM_SCALAR >::getKeysFromBNOpts ( std::vector< std::vector< std::vector< bool > > > & bn ) {
    return dBNToVar[&bn];
  }*/

  template< typename GUM_SCALAR >
  unsigned int OptBN< GUM_SCALAR >::getEntrySize() const {
    return dBNToVar.size();
  }


}
