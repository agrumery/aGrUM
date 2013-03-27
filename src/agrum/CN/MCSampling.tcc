#include <agrum/CN/MCSampling.h>
#include <agrum/core/exceptions.h>

namespace gum {

  template< typename GUM_SCALAR, class BNInferenceEngine >
  MCSampling< GUM_SCALAR, BNInferenceEngine >::MCSampling ( const CredalNet< GUM_SCALAR > & credalNet ) : CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::CNInferenceEngine ( credalNet ) {
    GUM_CONSTRUCTOR ( MCSampling );
    stopN = false;
    __repetitiveInd = false;
    __timeLimit = 5 * 60;
    __iterStop = 1000;
    __storeVertices = false;
    __storeBNOpt = false;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  MCSampling< GUM_SCALAR, BNInferenceEngine >::~MCSampling() {
    GUM_DESTRUCTOR ( MCSampling );
  }

  // TEST single thread dans testSuite
  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::makeInference() {
    if ( __repetitiveInd ) {
      try {
        this->_repetitiveInit();
      } catch ( InvalidArgument & err ) {
        GUM_SHOWERROR ( err );
        __repetitiveInd = false;
      }
    }

    // debug
    notOptDelete = 0;

    __mcInitApproximationScheme();
    __mcThreadDataCopy();

    auto bsize = this->burnIn();
    #pragma omp parallel for
    for ( decltype(bsize) iter = 0; iter < bsize; iter++ ) {
      __threadInference();
/*     
      // to check random sampling
      #pragma omp critical
      {
        unsigned int tid = gum_threads::getThreadNumber();
        std::cout << "thread\t" << tid << "\tsample : "<< this->_l_optimalNet[tid]->getCurrentSample() << std::endl;
      }
      std::cout << std::endl;
*/
      __threadUpdate();
    } // end of : parallel burnIn

    this->updateApproximationScheme( bsize );

    this->_updateOldMarginals(); // fusion threads + update old margi

    GUM_SCALAR eps = 1; // to validate testSuite ?
    this->continueApproximationScheme ( eps, false, false );
    
    auto psize = this->periodSize();
    // less overheads with high periodSize
    do {
      eps = 0;
      #pragma omp parallel for
      for ( decltype(psize) iter = 0; iter < psize; iter++ ) {
        __threadInference();
        __threadUpdate();
      } // end of : parallel periodSize

      this->updateApproximationScheme ( psize );

      this->_updateMarginals(); // fusion threads + update margi

      eps = this->_computeEpsilon(); // also updates oldMargi

    } while ( this->continueApproximationScheme ( eps, false, false ) );
    
    if ( ! this->_modal.empty() )
      this->_expFusion();
    
    if( __storeBNOpt )
      this->_optFusion();

    if ( __storeVertices )
      this->_verticesFusion();

    if ( ! this->_modal.empty() )
      this->_dynamicExpectations(); // work with any network

    GUM_TRACE ( this->messageApproximationScheme() );

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__threadUpdate() {
    int tId = gum_threads::getThreadNumber();
    bool keepSample = false;

    if ( this->_l_inferenceEngine[tId]->evidenceMarginal() > 0 ) {
      const gum::DAG &tDag = this->_workingSet[tId]->dag();

      for ( auto it = tDag.beginNodes(), theEnd = tDag.endNodes(); it != theEnd; ++it ) {
        const gum::Potential< GUM_SCALAR > & potential ( this->_l_inferenceEngine[tId]->marginal ( *it ) );
        gum::Instantiation ins ( potential );
        std::vector< GUM_SCALAR > vertex;

        for ( ins.setFirst(); !ins.end(); ++ins )
          vertex.push_back ( potential[ins] );
        
        //this->_updateThread ( *it, vertex, __storeVertices, __storeBNOpt );
        if ( this->_updateThread ( *it, vertex, __storeVertices, __storeBNOpt ) )
          keepSample = true;

      } // end of : for all nodes
    } // end of : if ( p(e) > 0 )

    if ( __storeBNOpt && ! keepSample ) {
      //delete this->_l_optimalNet[tId]->getCurrentSample();
      //std::cout << " net deleted, no opt found " << std::endl;
      #pragma omp critical(deleteCpt)
      {
        notOptDelete++;
      }
    }

    // do NOT reset threads marginals ( otherwise we will store vertices and dBN that are NOT optimal )
    /*const gum::DAG &tDag = this->_workingSet[tId]->dag();
    unsigned int dSize;
    for ( gum::DAG::NodeIterator it = tDag.beginNodes(); it != tDag.endNodes(); ++it ) {
      dSize = this->_l_marginalMin[tId][*it].size();
      for ( unsigned int i = 0; i < dSize; i++ ) {
        this->_l_marginalMin[tId][*it][i] = 1;
        this->_l_marginalMax[tId][*it][i] = 0;
      }
    }*/

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__threadInference() {
    int tId = gum_threads::getThreadNumber();
    __verticesSampling();
    this->_l_inferenceEngine[tId]->eraseAllEvidence();
    __insertEvidence ( *this->_l_inferenceEngine[tId] );
    this->_l_inferenceEngine[tId]->makeInference();

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::storeBNOpt ( const bool value ) {
    __storeBNOpt = value;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::__mcInitApproximationScheme() {
    this->setMaxTime ( __timeLimit );
    this->setEpsilon ( std::numeric_limits< GUM_SCALAR >::min() );
    /**
     * VERIFIER d/dt(e(t+1)-e(t))
     */
    this->setMinEpsilonRate ( std::numeric_limits< GUM_SCALAR >::min() );
    this->setBurnIn ( __iterStop );
    this->setPeriodSize ( __iterStop );

    this->initApproximationScheme();
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::__mcThreadDataCopy() {
    int num_threads;
    #pragma omp parallel
    {
      int this_thread = gum_threads::getThreadNumber();

      // implicit wait clause (don't put nowait)
      #pragma omp single
      {
        // should we ask for max threads instead ( no differences here in practice )
        num_threads = gum_threads::getNumberOfRunningThreads();

        this->_initThreadsData ( num_threads, __storeVertices, __storeBNOpt );
        this->_l_inferenceEngine.resize ( num_threads, NULL ); 
        
        //if ( __storeBNOpt )
          //this->_l_sampledNet.resize ( num_threads );
      } // end of : single region

      // we could put those below in a function in CNInferenceEngine, but let's keep this parallel region instead of breaking it and making another one to do the same stuff in 2 places since :
      // !!! BNInferenceEngine still needs to be initialized here anyway !!!

      gum::BayesNet< GUM_SCALAR > * thread_bn = new gum::BayesNet< GUM_SCALAR >();
      #pragma omp critical(Init)
      {
        //gum::BayesNet< GUM_SCALAR > * thread_bn = new gum::BayesNet< GUM_SCALAR >();//(this->_credalNet->current_bn());
        *thread_bn = this->_credalNet->current_bn();
      }
      this->_workingSet[this_thread] = thread_bn;

      this->_l_marginalMin[this_thread] = this->_marginalMin;
      this->_l_marginalMax[this_thread] = this->_marginalMax;
      this->_l_expectationMin[this_thread] = this->_expectationMin;
      this->_l_expectationMax[this_thread] = this->_expectationMax;
      this->_l_modal[this_thread] = this->_modal;

      if ( __storeVertices )
        this->_l_marginalSets[this_thread] = this->_marginalSets;

      gum::List< const gum::Potential< GUM_SCALAR > * > * evi_list = new gum::List< const gum::Potential< GUM_SCALAR > * >();
      this->_workingSetE[this_thread] = evi_list;

      BNInferenceEngine * inference_engine = new BNInferenceEngine ( * ( this->_workingSet[this_thread] ) );
      this->_l_inferenceEngine[this_thread] = inference_engine;

      if ( __storeBNOpt ) {
        OptBN<GUM_SCALAR> * threadOpt = new OptBN<GUM_SCALAR>( *this->_credalNet );
        this->_l_optimalNet[this_thread] = threadOpt;
      }
    }
  }


  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::eraseAllEvidence() {
    //this->eraseAllEvidence();
    auto tsize = this->_workingSet.size();
    // delete pointers
    for ( decltype(tsize) bn = 0; bn < tsize; bn++ ) {
      if ( __storeVertices )
        this->_l_marginalSets[bn].clear();

      if ( this->_workingSet[bn] != NULL )
        delete this->_workingSet[bn];

      if ( __storeBNOpt ) {
        if ( this->_l_inferenceEngine[bn] != NULL )
          //std::cout << "delete thread " << bn << " opt nets" << std::endl;
          delete this->_l_optimalNet[bn];
      }

      if ( this->_workingSetE[bn] != NULL ) {
        for ( auto it = this->_workingSetE[bn]->begin(), theEnd = this->_workingSetE[bn]->end(); it != theEnd; ++it )
          delete *it;

        delete this->_workingSetE[bn];
      }

      if ( this->_l_inferenceEngine[bn] != NULL )
        delete this->_l_inferenceEngine[bn];
    }

    // clear all vectors and maps
    this->_workingSet.clear();
    this->_workingSetE.clear();
    this->_l_inferenceEngine.clear();
    this->_l_optimalNet.clear();

    this->_l_marginalMin.clear();
    this->_l_marginalMax.clear();
    this->_l_expectationMin.clear();
    this->_l_expectationMax.clear();
    this->_l_modal.clear();
    this->_l_marginalSets.clear();

    this->_oldMarginalMin.clear();
    this->_oldMarginalMax.clear();

    this->_evidence.clear();
    this->_query.clear();

    this->_marginalMin.clear();
    this->_marginalMax.clear();
    this->_expectationMin.clear();
    this->_expectationMax.clear();
    this->_marginalSets.clear();
    this->_dynamicExpMin.clear();
    this->_dynamicExpMax.clear();
    this->_modal.clear();
    this->_marginalSets.clear();

    this->_t0.clear();
    this->_t1.clear();

    for ( auto id = this->_credalNet->current_bn().beginNodes(), theEnd = this->_credalNet->current_bn().endNodes(); id != theEnd; ++id ) {
      auto dSize = this->_credalNet->current_bn().variable ( *id ).domainSize();
      this->_marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      this->_marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
      this->_marginalSets.insert ( *id, std::vector< std::vector< GUM_SCALAR > >() );
    }

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  bool MCSampling< GUM_SCALAR, BNInferenceEngine >::getStopType() const {
    return __stopType;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::setRepetitiveInd ( const bool repetitive ) {
    __repetitiveInd = repetitive;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::setTimeLimit ( const int &time_limit ) {
    __timeLimit = time_limit * 60;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::setIterStop ( const int &no_change_time_limit ) {
    __iterStop = no_change_time_limit;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::setAll ( const bool repetitive, const int &time_limit, const int &no_change_time_limit ) {
    __repetitiveInd = repetitive;
    __timeLimit = time_limit * 60;
    __iterStop = no_change_time_limit;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__binaryRep ( std::vector< bool > & toFill,  const unsigned int value ) const {
    unsigned int n = value;
    auto tfsize = toFill.size();
    // get bits of choosen_vertex
    for ( decltype(tfsize) i = 0; i < tfsize; i++)
    {
      toFill[i] = n & 1;
      n /= 2;
    }
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__verticesSampling() {
    int this_thread = gum_threads::getThreadNumber();
    gum::BayesNet< GUM_SCALAR > * working_bn = this->_workingSet[this_thread];

    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *cpt = &this->_credalNet->credalNet_cpt();

    typedef std::vector< std::vector< std::vector< bool > > > dBN;

    dBN sample;
    if ( __storeBNOpt )
      sample = dBN( this->_l_optimalNet[this_thread]->getSampleDef() );

    if ( __repetitiveInd ) {
      typename gum::Property< std::vector< gum::NodeId > >::onNodes t0;
      typename gum::Property< std::vector< gum::NodeId > >::onNodes t1;
      t0 = this->_t0;
      t1 = this->_t1;

      // use cbegin() when available
      for ( auto it = t0.begin(), theEnd = t0.end(); it != theEnd; ++it ) {
        auto dSize = working_bn->variable ( it.key() ).domainSize();
        gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
        std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

        auto pconfs = ( *cpt ) [it.key()].size();
        for ( decltype(pconfs) pconf = 0; pconf < pconfs; pconf++ ) {
          auto choosen_vertex = rand() % ( *cpt ) [it.key()][pconf].size();

          if ( __storeBNOpt )
            __binaryRep( ( sample )[it.key()][pconf], choosen_vertex );

          for ( decltype(dSize) mod = 0; mod < dSize; mod++ )
            var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][choosen_vertex][mod];
        } // end of : pconf

        potential->fillWith ( var_cpt );

        auto t0esize = it->size();
        for ( decltype(t0esize) pos = 0; pos < t0esize; pos++ ) {
          if ( __storeBNOpt )
            ( sample )[( *it )[pos]] = ( sample )[it.key()];
          
          gum::Potential< GUM_SCALAR > * potential2 ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
          potential2->fillWith ( var_cpt );
        }
      }

      // use cbegin()
      for ( auto it = t1.begin(), theEnd = t1.end(); it != theEnd; ++it ) {
        auto dSize = working_bn->variable ( it.key() ).domainSize();
        gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
        std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

        for ( Size pconf = 0; pconf < ( *cpt ) [it.key()].size(); pconf++ ) {
          auto choosen_vertex = rand() % ( *cpt ) [it.key()][pconf].size();

          if ( __storeBNOpt )
            __binaryRep( ( sample )[it.key()][pconf], choosen_vertex );

          for ( decltype(dSize) mod = 0; mod < dSize; mod++ )
            var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][choosen_vertex][mod];
        } // end of : pconf

        potential->fillWith ( var_cpt );

        auto t1esize = it->size();
        for ( decltype(t1esize) pos = 0; pos < t1esize; pos++ ) {
          if ( __storeBNOpt )
            ( sample )[( *it )[pos]] = ( sample )[it.key()];

          gum::Potential< GUM_SCALAR > * potential2 ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
          potential2->fillWith ( var_cpt );
        }
      }
      if ( __storeBNOpt ) {
        this->_l_optimalNet[this_thread]->setCurrentSample( sample );
      }
    } 
    else {
      for ( auto id = working_bn->beginNodes(), theEnd = working_bn->endNodes(); id != theEnd; ++id ) {
        auto dSize = working_bn->variable ( *id ).domainSize();
        gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( *id ) ) );
        std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );
        
        auto pConfs = ( *cpt ) [*id].size();
        for ( decltype(pConfs) pconf = 0; pconf < pConfs; pconf++ ) {
          Size nVertices = ( *cpt ) [*id][pconf].size();
          auto choosen_vertex = rand() % nVertices;

          if ( __storeBNOpt )
            __binaryRep( ( sample )[*id][pconf], choosen_vertex );

          for ( decltype(dSize) mod = 0; mod < dSize; mod++ )
            var_cpt[pconf * dSize + mod] = ( *cpt ) [*id][pconf][choosen_vertex][mod];
        } // end of : pconf

        potential->fillWith ( var_cpt );
      }
      if ( __storeBNOpt )
        this->_l_optimalNet[this_thread]->setCurrentSample( sample );
    }

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__insertEvidence ( BNInferenceEngine &inference_engine ) { /*const*/
    if ( this->_evidence.size() == 0 )
      return;

    int this_thread = gum_threads::getThreadNumber();
    gum::BayesNet<GUM_SCALAR> * working_bn = this->_workingSet[this_thread];

    gum::List< const gum::Potential< GUM_SCALAR > * > * evi_list = this->_workingSetE[this_thread];

    if ( evi_list->size() > 0 ) {
      inference_engine.insertEvidence ( *evi_list );
      return;
    }

    typename gum::Property< std::vector< GUM_SCALAR > >::onNodes thread_evidence = this->_evidence;

    // use cbegin()
    for ( auto it = thread_evidence.begin(), theEnd = thread_evidence.end(); it != theEnd; ++it ) {
      gum::Potential< GUM_SCALAR > *p = new gum::Potential< GUM_SCALAR >;
      ( *p ) << working_bn->variable ( it.key() );

      try {
        p->fillWith ( *it );
      } catch ( gum::Exception &err ) {
        std::cout << "Error while inserting evidence : " << std::endl;
        GUM_SHOWERROR ( err );
        throw ( err );
      }

      evi_list->insert ( p );
    }

    if ( evi_list->size() > 0 )
      inference_engine.insertEvidence ( *evi_list );

  }

} // namespace cn
