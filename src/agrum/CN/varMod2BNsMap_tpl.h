#include <agrum/CN/varMod2BNsMap.h>

namespace gum {
  namespace credal {

    template <typename GUM_SCALAR>
    VarMod2BNsMap<GUM_SCALAR>::VarMod2BNsMap() {
      cnet = nullptr;

      GUM_CONSTRUCTOR( VarMod2BNsMap );
    }

    template <typename GUM_SCALAR>
    VarMod2BNsMap<GUM_SCALAR>::VarMod2BNsMap( const CredalNet<GUM_SCALAR>& cn ) {
      setCNet( cn );

      GUM_CONSTRUCTOR( VarMod2BNsMap );
    }

    template <typename GUM_SCALAR>
    VarMod2BNsMap<GUM_SCALAR>::~VarMod2BNsMap() {
      GUM_DESTRUCTOR( VarMod2BNsMap );
    }

    template <typename GUM_SCALAR>
    void VarMod2BNsMap<GUM_SCALAR>::setCNet( const CredalNet<GUM_SCALAR>& cn ) {
      auto* cpt = &cn.credalNet_currentCpt();
      auto  nNodes = cpt->size();
      _sampleDef.resize( nNodes );

      for ( NodeId node = 0; node < nNodes; node++ ) {
        auto pConfs = ( *cpt )[node].size();
        _sampleDef[node].resize( pConfs );

        for ( Size pconf = 0; pconf < pConfs; pconf++ ) {
          Size          nVertices = Size( ( *cpt )[node][pconf].size() );
          unsigned long b, c;  // needed by superiorPow
          superiorPow( static_cast<unsigned long>( nVertices ), b, c );
          Size nBits = Size( b );
          Size newCard = Size( c );
          _sampleDef[node][pconf].resize( nBits );
        }
      }

      cnet = &cn;
    }

    template <typename GUM_SCALAR>
    bool VarMod2BNsMap<GUM_SCALAR>::insert( const std::vector<bool>& bn,
                                            const std::vector<Size>& key ) {
      _currentHash = Size( _vectHash( bn ) );
      std::list<Size>& nets =
          _myVarHashs.getWithDefault( key, std::list<Size>() );  //[ key ];

      for ( std::list<Size>::iterator it = nets.begin(); it != nets.end(); ++it ) {
        if ( *it == _currentHash ) return false;
      }

      // add it
      _myHashNet.set( _currentHash, bn );  //[_currentHash] = bn;
      // insert net hash in our key net list
      nets.push_back( _currentHash );
      // insert out key in the hash key list
      _myHashVars
          .getWithDefault(
              _currentHash,
              std::list<varKey>() ) /*[_currentHash]*/.push_back( key );
      return true;
    }

    template <typename GUM_SCALAR>
    bool VarMod2BNsMap<GUM_SCALAR>::insert( const std::vector<Size>& key,
                                            const bool               isBetter ) {
      if ( isBetter ) {
        // get all nets of this key (maybe entry does not exists)
        std::list<Size>& old_nets =
            _myVarHashs.getWithDefault( key, std::list<Size>() );  //[ key ];

        // for each one
        for ( std::list<Size>::iterator it = old_nets.begin();
              it != old_nets.end();
              ++it ) {
          // get all keys associated to this net
          std::list<varKey>& netKeys =
              _myHashVars.getWithDefault( *it, std::list<varKey>() );  //[ *it ];

          // if we are the sole user, delete the net entry
          if ( netKeys.size() == 1 ) {
            _myHashVars.erase( *it );
          }
          // other keys use the net, delete our key from list
          else {
            for ( std::list<varKey>::iterator it2 = netKeys.begin();
                  it2 != netKeys.end();
                  ++it2 ) {
              if ( *it2 == key ) {
                netKeys.erase( it2 );
                break;
              }
            }
          }
        }  // end of : for each old_net

        // clear all old_nets
        old_nets.clear();
        // insert new net with it's hash
        _myHashNet.set( _currentHash,
                        _currentSample );  //[_currentHash] = _currentSample;
        // insert net hash in our key net list
        old_nets.push_back( _currentHash );
        // insert out key in the hash key list
        _myHashVars
            .getWithDefault( _currentHash, std::list<varKey>() ) /*[_currentHash]*/
            .push_back( key );
        return true;

      }  // end of isBetter
      // another opt net
      else {
        // check that we didn't add it for this key
        std::list<Size>& nets =
            _myVarHashs.getWithDefault( key, std::list<Size>() );  //[ key ];

        for ( std::list<Size>::iterator it = nets.begin(); it != nets.end();
              ++it ) {
          if ( *it == _currentHash ) return false;
        }

        // add it
        _myHashNet.set( _currentHash, _currentSample );
        // insert net hash in our key net list
        nets.push_back( _currentHash );
        // insert out key in the hash key list
        _myHashVars.getWithDefault( _currentHash, std::list<varKey>() )
            .push_back( key );

        /*
              // add it
              _myHashNet[_currentHash] = _currentSample;
              // insert net hash in our key net list
              nets.push_back(_currentHash);
              // insert out key in the hash key list
              _myHashVars[_currentHash].push_back(key);
        */
        return true;
      }  // end of ! isBetter
    }

    template <typename GUM_SCALAR>
    void VarMod2BNsMap<GUM_SCALAR>::setCurrentSample(
        const std::vector<std::vector<std::vector<bool>>>& sample ) {
      _currentSample.clear();

      for ( Size i = 0; i < sample.size(); i++ )
        for ( Size j = 0; j < sample[j].size(); j++ )
          for ( Size k = 0; k < sample[i][j].size(); k++ )
            _currentSample.push_back( sample[i][j][k] );

      // std::cout << sample << std::endl;
      // std::cout << _currentSample << std::endl;

      _currentHash = Size( _vectHash( _currentSample ) );
    }

    template <typename GUM_SCALAR>
    const std::vector<bool>& VarMod2BNsMap<GUM_SCALAR>::getCurrentSample() {
      return _currentSample;
    }

    template <typename GUM_SCALAR>
    const std::vector<std::vector<std::vector<bool>>>&
    VarMod2BNsMap<GUM_SCALAR>::getSampleDef() {
      return _sampleDef;
    }

    template <typename GUM_SCALAR>
    const std::vector<std::vector<bool>*>
    VarMod2BNsMap<GUM_SCALAR>::getBNOptsFromKey( const std::vector<Size>& key ) {
      // return something even if key does not exist
      if ( !_myVarHashs.exists( key ) ) return std::vector<std::vector<bool>*>();

      std::list<Size>& netsHash = _myVarHashs[key];  //.at(key);

      std::vector<dBN*> nets;
      nets.resize( netsHash.size() );

      std::list<Size>::iterator it = netsHash.begin();

      for ( Size i = 0; i < netsHash.size(); i++, ++it ) {
        nets[i] = &_myHashNet /*.at(*/[*it];  //);
      }

      return nets;
    }

    template <typename GUM_SCALAR>
    std::vector<std::vector<std::vector<std::vector<bool>>>>
    VarMod2BNsMap<GUM_SCALAR>::getFullBNOptsFromKey(
        const std::vector<Size>& key ) {
      if ( cnet == nullptr )
        GUM_ERROR( OperationNotAllowed,
                   "No CredalNet associated to me ! Can't get FullBNOptsFromKey : "
                       << key );

      if ( !_myVarHashs.exists( key ) )
        return std::vector<std::vector<std::vector<std::vector<bool>>>>();

      std::list<Size>& netsHash = _myVarHashs[key];  //.at(key);

      std::vector<std::vector<std::vector<std::vector<bool>>>> nets;
      nets.resize( netsHash.size(), _sampleDef );

      std::list<Size>::iterator it = netsHash.begin();

      for ( Size i = 0; i < netsHash.size(); i++, ++it ) {
        // std::vector< std::vector< std::vector < bool > > > net(_sampleDef);
        dBN::iterator it2 = _myHashNet /*.at(*/[*it] /*)*/.begin();

        for ( Size j = 0; j < _sampleDef.size(); j++ ) {
          for ( Size k = 0; k < _sampleDef[j].size(); k++ ) {
            for ( Size l = 0; l < _sampleDef[j][k].size(); l++ ) {
              nets[i][j][k][l] = *it2;
              ++it2;
            }
          }
        }
      }

      return nets;
    }

    template <typename GUM_SCALAR>
    Size VarMod2BNsMap<GUM_SCALAR>::getEntrySize() const {
      return _myHashNet.size();
    }

  }  // end of credal namespace
}
