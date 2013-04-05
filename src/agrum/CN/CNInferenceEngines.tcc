///////////////////////// 2 argument template ////////////////////////
 
namespace gum {

  template < typename GUM_SCALAR, class BNInferenceEngine >
  CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::CNInferenceEngines ( const CredalNet< GUM_SCALAR > & credalNet ) : CNInferenceEngine< GUM_SCALAR >::CNInferenceEngine ( credalNet ) {
    std::cout << "CNInferenceEngines construct" << std::endl;
    GUM_CONSTRUCTOR ( CNInferenceEngines );
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::~CNInferenceEngines() {
    std::cout << "CNInferenceEngines destructor" << std::endl;
    GUM_DESTRUCTOR ( CNInferenceEngines );
  }


  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_initThreadsData( const unsigned int & num_threads, const bool __storeVertices, const bool __storeBNOpt ) {
    _workingSet.clear();
    _workingSet.resize( num_threads, NULL );
    _workingSetE.clear();
    _workingSetE.resize( num_threads, NULL );

    _l_marginalMin.clear();
    _l_marginalMin.resize( num_threads );
    _l_marginalMax.clear();
    _l_marginalMax.resize( num_threads );
    _l_expectationMin.clear();
    _l_expectationMin.resize( num_threads );
    _l_expectationMax.clear();
    _l_expectationMax.resize( num_threads );


    _l_clusters.clear();
    _l_clusters.resize( num_threads );

    if ( __storeVertices ) {
      _l_marginalSets.clear();
      _l_marginalSets.resize( num_threads );
    }

    if ( __storeBNOpt ) {
      for ( Size ptr = 0; ptr < this->_l_optimalNet.size(); ptr++ )
        if ( this->_l_optimalNet[ptr] != NULL )
          delete _l_optimalNet[ptr];

      _l_optimalNet.clear();
      _l_optimalNet.resize( num_threads );
    }

    _l_modal.clear();
    _l_modal.resize( num_threads );

    this->_oldMarginalMin.clear();
    this->_oldMarginalMin = this->_marginalMin;
    this->_oldMarginalMax.clear();
    this->_oldMarginalMax = this->_marginalMax;
  }
 
  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline bool CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_updateThread( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex/*, const bool __storeVertices, const bool __storeBNOpt*/ ) {
    int tId = gum_threads::getThreadNumber();
    // save E(X) if we don't save vertices
    if( ! infE::_storeVertices && ! _l_modal[tId].empty() ) {
      std::string var_name = _workingSet[tId]->variable ( id ).name();
      auto delim = var_name.find_first_of ( "_" );
      //std::string time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );

      if( _l_modal[tId].exists( var_name )/*_l_modal[tId].find(var_name) != _l_modal[tId].end()*/ ) {
        GUM_SCALAR exp = 0;
        auto vsize = vertex.size();
        for ( decltype(vsize) mod = 0; mod < vsize; mod++ )
          exp += vertex[mod] * _l_modal[tId][var_name][mod];

        if( exp > _l_expectationMax[tId][id] )
          _l_expectationMax[tId][id] = exp;
        if( exp < _l_expectationMin[tId][id] )
          _l_expectationMin[tId][id] = exp;
      }
    } // end of : if modal (map) not empty

    bool newOne = false;
    bool added = false;
    bool result = false;
    // for burn in, we need to keep checking on local marginals and not global ones (faster inference)
    // we also don't want to store dbn for observed variables since there will be a huge number of them (probably all of them).
    auto vsize = vertex.size();
    for( decltype(vsize) mod = 0; mod < vsize; mod++ ) {
      if(vertex[mod] < _l_marginalMin[tId][id][mod]) {
        _l_marginalMin[tId][id][mod] = vertex[mod];
        newOne = true;
        if ( infE::_storeBNOpt && ! infE::_evidence.exists( id ) ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 0;
          if ( _l_optimalNet[tId]->insert( key, true ) )
            result = true;
        }
      }
      if(vertex[mod] > _l_marginalMax[tId][id][mod]) {
        _l_marginalMax[tId][id][mod] = vertex[mod];
        newOne = true;
        if ( infE::_storeBNOpt && ! infE::_evidence.exists( id ) ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 1;
          if ( _l_optimalNet[tId]->insert( key, true ) )
            result = true;
        }
      }
      else if ( vertex[mod] == _l_marginalMin[tId][id][mod] 
              || vertex[mod] == _l_marginalMax[tId][id][mod] ) {
        newOne = true;
        if ( infE::_storeBNOpt && vertex[mod] == _l_marginalMin[tId][id][mod] && ! infE::_evidence.exists( id ) ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 0;
          if ( _l_optimalNet[tId]->insert( key, false ) )
            result = true;
        }
        if ( infE::_storeBNOpt && vertex[mod] == _l_marginalMax[tId][id][mod] && ! infE::_evidence.exists( id ) ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 1;
          if ( _l_optimalNet[tId]->insert(  key, false ) )
            result = true;
        }
      }
      // store point to compute credal set vertices.
      // check for redundancy at each step or at the end ?
      if( infE::_storeVertices && ! added && newOne ) {
        _l_marginalSets[tId][id].push_back(vertex);
        added = true;
      }
    }

    // if all variables didn't get better marginals, we will delete
    // this->_l_optimalNet[tId]->getCurrentSample() ( the caller will )
    if ( infE::_storeBNOpt && result )
      return true;

    return false;

  }
  
  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_updateMarginals() {
    #pragma omp parallel
    {
      int threadId = gum_threads::getThreadNumber();
      auto nsize = _workingSet[threadId]->size();
      #pragma omp for
      for ( decltype(nsize) i = 0; i < nsize; i++ ) {
        auto dSize = _l_marginalMin[threadId][i].size();
        for ( decltype(dSize) j = 0; j < dSize; j++ ) {
          auto tsize = _l_marginalMin.size();
          // go through all threads
          for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
            if ( _l_marginalMin[tId][i][j] < this->_marginalMin[i][j] )
              this->_marginalMin[i][j] = _l_marginalMin[tId][i][j];
            if ( _l_marginalMax[tId][i][j] > this->_marginalMax[i][j] )
              this->_marginalMax[i][j] = _l_marginalMax[tId][i][j];
          } // end of : all threads
          
          /*
          // update them
          for ( Size tId = 0; tId < this->_l_marginalMin.size(); tId++ ) {
            //if ( this->_l_marginalMin[tId][i][j] > this->_marginalMin[i][j] )
              this->_l_marginalMin[tId][i][j] = this->_marginalMin[i][j];
            //if ( this->_l_marginalMax[tId][i][j] < this->_oldMarginalMax[i][j] )
              this->_l_marginalMax[tId][i][j] = this->_marginalMax[i][j];
          } // end of : all threads
          */
          
        } // end of : all modalities
      } // end of : all variables
    } // end of : parallel region

  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline const GUM_SCALAR CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_computeEpsilon() {
    GUM_SCALAR eps = 0;
    #pragma omp parallel
    {
      GUM_SCALAR tEps = 0;
      GUM_SCALAR delta;

      int tId = gum_threads::getThreadNumber();
      auto nsize = _workingSet[tId]->size();

      #pragma omp for
      for ( decltype(nsize) i = 0; i < nsize; i++ ) {
        auto dSize = _l_marginalMin[tId][i].size();
        for ( decltype(dSize) j = 0; j < dSize; j++ ) {
          // on min
          delta = this->_marginalMin[i][j] - this->_oldMarginalMin[i][j];
          delta = ( delta < 0 ) ? ( - delta ) : delta;
          tEps = ( tEps < delta ) ? delta : tEps;

          // on max
          delta = this->_marginalMax[i][j] - this->_oldMarginalMax[i][j];
          delta = ( delta < 0 ) ? ( - delta ) : delta;
          tEps = ( tEps < delta ) ? delta : tEps;

          this->_oldMarginalMin[i][j] = this->_marginalMin[i][j];
          this->_oldMarginalMax[i][j] = this->_marginalMax[i][j];
        }
      } // end of : all variables

      #pragma omp critical(epsilon_max)
      {
        #pragma omp flush(eps)
        eps = ( eps < tEps ) ? tEps : eps;
      }

    } // end of : parallel region
    return eps;
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_updateOldMarginals() {
    #pragma omp parallel
    {
      int threadId = gum_threads::getThreadNumber();
      auto nsize = _workingSet[threadId]->size();
      
      #pragma omp for
      for ( decltype(nsize) i = 0; i < nsize; i++ ) {
        auto dSize = _l_marginalMin[threadId][i].size();
        for ( decltype(dSize) j = 0; j < dSize; j++ ) {
          auto tsize = _l_marginalMin.size();
          // go through all threads
          for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
            if ( _l_marginalMin[tId][i][j] < this->_oldMarginalMin[i][j] )
              this->_oldMarginalMin[i][j] = _l_marginalMin[tId][i][j];
            if ( _l_marginalMax[tId][i][j] > this->_oldMarginalMax[i][j] )
              this->_oldMarginalMax[i][j] = _l_marginalMax[tId][i][j];
          } // end of : all threads
          /*
          // update them
          for ( Size tId = 0; tId < this->_l_marginalMin.size(); tId++ ) {
            //if ( this->_l_marginalMin[tId][i][j] > this->_oldMarginalMin[i][j] )
              this->_l_marginalMin[tId][i][j] = this->_oldMarginalMin[i][j];
            //if ( this->_l_marginalMax[tId][i][j] < this->_oldMarginalMax[i][j] )
              this->_l_marginalMax[tId][i][j] = this->_oldMarginalMax[i][j];
          } // end of : all threads
          */
        } // end of : all modalities
      } // end of : all variables
    } // end of : parallel region

  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_verticesFusion() {
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_expFusion() {
    // don't create threads if there are no modalities to compute expectations
    if ( this->_modal.empty() )
      return;

    #pragma omp parallel
    {
      int threadId = gum_threads::getThreadNumber();

      if ( ! this->_l_modal[threadId].empty() ) {
        auto nsize = _workingSet[threadId]->size();

        #pragma omp for
        for ( decltype(nsize) i = 0; i < nsize; i++ ) {
          std::string var_name = _workingSet[threadId]->variable ( i ).name();
          auto delim = var_name.find_first_of ( "_" );
          std::string time_step = var_name.substr ( delim + 1, var_name.size() );
          var_name = var_name.substr ( 0, delim );

          if ( ! _l_modal[ threadId ].exists( var_name )/*_l_modal[threadId].find(var_name) == _l_modal[threadId].end()*/ )
            continue;

          auto tsize = _l_expectationMax.size();
          for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
            if( _l_expectationMax[tId][i] > this->_expectationMax[i] )
              this->_expectationMax[i] = _l_expectationMax[tId][i];
            if( _l_expectationMin[tId][i] < this->_expectationMin[i] )
              this->_expectationMin[i] = _l_expectationMin[tId][i];
          } // end of : each thread
        } // end of : each variable
      } // end of : if modals not empty

    } // end of : parallel region
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::_optFusion() {
    typedef std::vector< bool > dBN;

    std::cout << "thread opt fusion : " << _l_marginalMin.size() << std::endl;

    ///*_threadFusion*/infE::_dbnOpt = OptBN<GUM_SCALAR>( *this->_credalNet );

    auto nsize = _workingSet[0]->size();

    // no parallel insert in hash-tables (OptBN)
    for ( decltype(nsize) i = 0; i < nsize; i++ ) {

      // we don't store anything for observed variables
      if ( infE::_evidence.exists( i ) )
        continue;

      std::cout << _workingSet[0]->variable(i).name() << std::endl;
      auto dSize = _l_marginalMin[0][i].size();
      for ( decltype(dSize) j = 0; j < dSize; j++ ) {
        std::cout << "\t mod : " << j << std::endl;
        // go through all threads
        std::vector< unsigned int > keymin(3);
        keymin[0] = i; keymin[1] = j; keymin[2] = 0;
        std::vector< unsigned int > keymax(keymin);
        keymax[2] = 1;

        auto tsize = _l_marginalMin.size();
        for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
          //if ( fabs( _l_marginalMin[tId][i][j] - this->_marginalMin[i][j] ) <= 1e-6 ) {
          if ( _l_marginalMin[tId][i][j] == this->_marginalMin[i][j] ) {
            std::cout << "\t\t min : ";
            const std::vector< dBN* > & tOpts = _l_optimalNet[tId]->getBNOptsFromKey( keymin );
            std::cout << " size : " << tOpts.size() << std::endl;
            auto osize = tOpts.size();
            for ( decltype(osize) bn = 0; bn < osize; bn++ ) {
              /*_threadFusion*/infE::_dbnOpt.insert ( *tOpts[bn], keymin );
            }
          }
          //if ( fabs( _l_marginalMax[tId][i][j] - this->_marginalMax[i][j] ) <= 1e-6 ) {
          if ( _l_marginalMax[tId][i][j] == this->_marginalMax[i][j] ) {
            std::cout << "\t\t max : ";
            const std::vector< dBN* > & tOpts = _l_optimalNet[tId]->getBNOptsFromKey( keymax );
            std::cout << " size : " << tOpts.size() << std::endl;
            auto osize = tOpts.size();
            for ( decltype(osize) bn = 0; bn < osize; bn++ ) {
              /*_threadFusion*/infE::_dbnOpt.insert ( *tOpts[bn], keymax );
            }
          }
        } // end of : all threads
      } // end of : all modalities
    } // end of : all variables

    std::cout << "fusion size : " << /*_threadFusion*/infE::_dbnOpt.getEntrySize() << std::endl;
  }


  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngines< GUM_SCALAR, BNInferenceEngine >::eraseAllEvidence() {
    infE::eraseAllEvidence();
    auto tsize = _workingSet.size();
    // delete pointers
    for ( decltype(tsize) bn = 0; bn < tsize; bn++ ) {
      if ( infE::_storeVertices )
        _l_marginalSets[bn].clear();

      if ( _workingSet[bn] != NULL )
        delete _workingSet[bn];

      if ( infE::_storeBNOpt )
        if ( _l_inferenceEngine[bn] != NULL )
          delete _l_optimalNet[bn];

      if ( this->_workingSetE[bn] != NULL ) {
        for ( auto it = _workingSetE[bn]->begin(), theEnd = _workingSetE[bn]->end(); it != theEnd; ++it )
          delete *it;

        delete _workingSetE[bn];
      }

      if ( _l_inferenceEngine[bn] != NULL )
        delete _l_inferenceEngine[bn];
    }

    // this is important, those will be resized with the correct number of threads.

    _workingSet.clear();
    _workingSetE.clear();
    _l_inferenceEngine.clear();
    _l_optimalNet.clear();

    _l_marginalMin.clear();
    _l_marginalMax.clear();
    _l_expectationMin.clear();
    _l_expectationMax.clear();
    _l_modal.clear();
    _l_marginalSets.clear();
    _l_evidence.clear();
    _l_clusters.clear();

  }


}
