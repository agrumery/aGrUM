#include <agrum/CN/MCSampling.h>
#include <agrum/core/exceptions.h>

namespace gum {

  template< typename GUM_SCALAR, class BNInferenceEngine >
  MCSampling< GUM_SCALAR, BNInferenceEngine >::MCSampling ( const CredalNet< GUM_SCALAR > & credalNet ) : InferenceEngine< GUM_SCALAR >::InferenceEngine ( credalNet ) {
    GUM_CONSTRUCTOR ( MCSampling );
    stopN = false;
    __repetitiveInd = false;
    __timeLimit = 5 * 60;
    __iterStop = 1000;
    __storeVertices = false;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  MCSampling< GUM_SCALAR, BNInferenceEngine >::~MCSampling() {
    GUM_DESTRUCTOR ( MCSampling );
  }

  // TEST single thread dans testSuite
  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::makeInference_v3() {
    if ( __repetitiveInd ) {
      try {
        this->_repetitiveInit();
      } catch ( InvalidArgument & err ) {
        GUM_SHOWERROR ( err );
        __repetitiveInd = false;
      }
    }

    __mcInitApproximationScheme();
    // NO MEM LEAK TILL HERE : before multi threading
    __mcThreadDataCopy();

    #pragma omp parallel for

    for ( Size iter = 0; iter < this->burnIn(); iter++ ) {
      __threadInference();
      __threadUpdate();
    } // end of : parallel burnIn

    this->updateApproximationScheme( this->burnIn() );

    this->_updateOldMarginals(); // fusion threads + update old margi

    GUM_SCALAR eps = 1; // to validate testSuite ?
    this->continueApproximationScheme ( eps, false, false );

    // less overheads with high periodSize
    do {
      eps = 0;
      #pragma omp parallel for

      for ( Size iter = 0; iter < this->periodSize(); iter++ ) {
        __threadInference();
        __threadUpdate();
      } // end of : parallel periodSize

      this->updateApproximationScheme ( this->periodSize() );

      this->_updateMarginals(); // fusion threads + update margi

      eps = this->_computeEpsilon(); // also updates oldMargi

    } while ( this->continueApproximationScheme ( eps, false, false ) );

    this->_expFusion();

    if ( __storeVertices )
      this->_verticesFusion();

    if ( ! this->_modal.empty() )
      this->_dynamicExpectations(); // work with any network

    std::cout << this->messageApproximationScheme() << std::endl;

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__threadUpdate() {
    unsigned int tId = omp_get_thread_num();

    if ( _l_inferenceEngine[tId]->evidenceMarginal() > 0 ) {
      const gum::DAG &tDag = this->_workingSet[tId]->dag();

      for ( gum::DAG::NodeIterator it = tDag.beginNodes(); it != tDag.endNodes(); ++it ) {
        const gum::Potential< GUM_SCALAR > & potential ( _l_inferenceEngine[tId]->marginal ( *it ) );
        gum::Instantiation ins ( potential );
        std::vector< GUM_SCALAR > vertex;

        for ( ins.setFirst(); !ins.end(); ++ins )
          vertex.push_back ( potential[ins] );

        this->_updateThread ( *it, vertex, __storeVertices );
      } // end of : for all nodes
    } // end of : if ( p(e) > 0 )

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__threadInference() {
    int tId = omp_get_thread_num();
    __verticesSampling();
    _l_inferenceEngine[tId]->eraseAllEvidence();
    __insertEvidence ( *_l_inferenceEngine[tId] );
    _l_inferenceEngine[tId]->makeInference();

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
    unsigned int num_threads;
    #pragma omp parallel
    {
      unsigned int this_thread = omp_get_thread_num();

      // implicit wait clause (don't put nowait)
      #pragma omp single
      {
        num_threads = omp_get_num_threads();

        this->_initThreadsData ( num_threads, __storeVertices ); // in infEng
        _l_inferenceEngine.resize ( num_threads, NULL ); // in MCSampling
      } // end of : single region

      // we could put those below in a function in InferenceEngine, but let's keep this parallel region instead of breaking it and making another one to do the same stuff in 2 places since :
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
      //}
    }
  }

  // TEST single thread dans testSuite
  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::makeInference_v2() {

    ///////////////////////////////// typedef /////////////////////////////
    typedef typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes credalSet;
    typedef typename gum::Property< std::vector< GUM_SCALAR > >::onNodes margi;
    typedef typename gum::Property< GUM_SCALAR >::onNodes expe;

    typedef gum::BayesNet< GUM_SCALAR > bnet;
    typedef std::vector< margi > margis;
    typedef std::vector< expe > expes;
    typedef std::vector< credalSet > credalSets;

    typedef typename std::vector< std::map< std::string, std::vector< GUM_SCALAR > > > modals;
    ///////////////////////////////////////////////////////////////////////

    if ( __repetitiveInd ) {
      try {
        this->_repetitiveInit();
      } catch ( InvalidArgument & err ) {
        GUM_SHOWERROR ( err );
        __repetitiveInd = false;
      }
    }

    unsigned int num_threads;
    bool all_stop = false;

    GUM_TRACE ( "====== MCSampling PARALLEL REGION BEGINS ======" );

    #pragma omp parallel
    {
      int this_thread = omp_get_thread_num();

      // implicit wait clause (don't put nowait)
      #pragma omp single
      {
        num_threads = omp_get_num_threads();

        this->_initThreadsData ( num_threads, __storeVertices );
      } // end of : single region

      bnet * thread_bn = new bnet();
      #pragma omp critical(BNInit)
      {
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

      BNInferenceEngine inference_engine ( * ( this->_workingSet[this_thread] ) );
      const gum::DAG &thread_dag = this->_workingSet[this_thread]->dag();


      // otherwise you get STUCK !!!
      unsigned int reste = __iterStop % num_threads;

      if ( reste != 0 )
        __iterStop += ( __iterStop < num_threads ) ? reste : -reste;

      // + num_threads if upper value


      this->setMaxTime ( __timeLimit );
      this->setEpsilon ( std::numeric_limits< GUM_SCALAR >::min() );
      /**
       * VERIFIER d/dt(e(t+1)-e(t))
       */
      this->setMinEpsilonRate ( std::numeric_limits< GUM_SCALAR >::min() );
      this->setBurnIn ( __iterStop );
      this->setPeriodSize ( __iterStop );

      this->initApproximationScheme();

      //////////////////// burn in ///////////////////////

      do {
        __verticesSampling();

        inference_engine.eraseAllEvidence();
        __insertEvidence ( inference_engine );
        inference_engine.makeInference();

        // this is TOO slow (5% CPU occupied instead of 92%+ from times to times)
        /*
        #pragma omp critical(updateApproximationScheme)
        {
          this->updateApproximationScheme();
        }
        */

        // !!! if updates of approximation scheme are WRONG, get rid of nowait clause (updates may be slower then)!!!
        #pragma omp single //nowait
        {
          this->updateApproximationScheme ( num_threads );
        }

        if ( inference_engine.evidenceMarginal() > 0 ) {
          for ( gum::DAG::NodeIterator it = thread_dag.beginNodes(); it != thread_dag.endNodes(); ++it ) {
            const gum::Potential< GUM_SCALAR > & potential ( inference_engine.marginal ( *it ) );
            gum::Instantiation ins ( potential );
            std::vector< GUM_SCALAR > vertex;

            for ( ins.setFirst(); !ins.end(); ++ins )
              vertex.push_back ( potential[ins] );

            this->_updateThread ( *it, vertex, __storeVertices );
          } // end of : for all nodes
        } // end of : if ( p(e) > 0 )

        // information fusion is useless before end of burn in

      } while ( ! this->startOfPeriod() );

      /////////////////////////////////////////////////////////////////////////////////////////////////////// mettre taille burn in

      ///////////// thread information fusion /////////////
      //////////////// update _oldMarginal ////////////////

      int dSize;
      #pragma omp for

      for ( int i = 0; i < this->_workingSet[this_thread]->size(); i++ ) {
        dSize = this->_l_marginalMin[this_thread][i].size();

        for ( int j = 0; j < dSize; j++ ) {
          for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
            if ( this->_l_marginalMin[thread_id][i][j] < this->_oldMarginalMin[i][j] )
              this->_oldMarginalMin[i][j] = this->_l_marginalMin[thread_id][i][j];

            if ( this->_l_marginalMax[thread_id][i][j] > this->_oldMarginalMax[i][j] )
              this->_oldMarginalMax[i][j] = this->_l_marginalMax[thread_id][i][j];
          } // end of : all threads

          // update them
          for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
            //if ( this->_l_marginalMin[thread_id][i][j] > this->_oldMarginalMin[i][j] )
            this->_l_marginalMin[thread_id][i][j] = this->_oldMarginalMin[i][j];
            //if ( this->_l_marginalMax[thread_id][i][j] < this->_oldMarginalMax[i][j] )
            this->_l_marginalMax[thread_id][i][j] = this->_oldMarginalMax[i][j];
          } // end of : all threads
        } // end of : all modalities
      } // end of : all variables

      #pragma omp single
      {
        // if forgotten TestSuite listener don't pass the test
        this->continueApproximationScheme ( 1, false, false );
      }
      GUM_SCALAR epsilon_max = 0;

      ////////////////////// do while /////////////////////
      do {
        __verticesSampling();

        inference_engine.eraseAllEvidence();
        __insertEvidence ( inference_engine );
        inference_engine.makeInference();

        // implicit barrier needed ?
        #pragma omp single //nowait
        {
          this->updateApproximationScheme ( num_threads );
        }

        if ( inference_engine.evidenceMarginal() > 0 ) {
          for ( gum::DAG::NodeIterator it = thread_dag.beginNodes(); it != thread_dag.endNodes(); ++it ) {
            const gum::Potential< GUM_SCALAR > & potential ( inference_engine.marginal ( *it ) );
            gum::Instantiation ins ( potential );
            std::vector< GUM_SCALAR > vertex;

            for ( ins.setFirst(); !ins.end(); ++ins )
              vertex.push_back ( potential[ins] );

            this->_updateThread ( *it, vertex, __storeVertices );
          } // end of : for all nodes
        } // end of : if ( p(e) > 0 )

        // iif this->startOfPeriod() == true, fusion & update marginals only
        if ( this->startOfPeriod() ) {
          ///////////// thread information fusion /////////////
          ///////////////// update _marginal  /////////////////

          // ?? is this usefull ??
          //#pragma omp barrier

          int dSize;
          #pragma omp for

          for ( int i = 0; i < this->_workingSet[this_thread]->size(); i++ ) {
            dSize = this->_l_marginalMin[this_thread][i].size();

            for ( int j = 0; j < dSize; j++ ) {
              for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
                if ( this->_l_marginalMin[thread_id][i][j] < this->_marginalMin[i][j] )
                  this->_marginalMin[i][j] = this->_l_marginalMin[thread_id][i][j];

                if ( this->_l_marginalMax[thread_id][i][j] > this->_marginalMax[i][j] )
                  this->_marginalMax[i][j] = this->_l_marginalMax[thread_id][i][j];
              } // end of : all threads

              // update them
              for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
                //if ( this->_l_marginalMin[thread_id][i][j] > this->_marginalMin[i][j] )
                this->_l_marginalMin[thread_id][i][j] = this->_marginalMin[i][j];
                //if ( this->_l_marginalMax[thread_id][i][j] < this->_marginalMax[i][j] )
                this->_l_marginalMax[thread_id][i][j] = this->_marginalMax[i][j];
              } // end of : all threads
            } // end of : all modalities
          } // end of : all variables

          //////////////// update epsilon_max  ////////////////
          //////////////// update _oldMarginal ////////////////
          /*#pragma omp single
          {
            std::cout << "start of period" << std::endl;
            std::cout << this->nbrIterations() << std::endl;
            epsilon_max = 0;
          }*/
          #pragma omp single
          {
            epsilon_max = 0;
          }

          GUM_SCALAR thread_epsilon = 0;
          GUM_SCALAR delta;

          #pragma omp for

          for ( int i = 0; i < this->_workingSet[this_thread]->size(); i++ ) {
            dSize = this->_l_marginalMin[this_thread][i].size();

            for ( int j = 0; j < dSize; j++ ) {
              // on min
              delta = this->_marginalMin[i][j] - this->_oldMarginalMin[i][j];
              delta = ( delta < 0 ) ? ( - delta ) : delta;
              thread_epsilon = ( thread_epsilon < delta ) ? delta : thread_epsilon;

              // on max
              delta = this->_marginalMax[i][j] - this->_oldMarginalMax[i][j];
              delta = ( delta < 0 ) ? ( - delta ) : delta;
              thread_epsilon = ( thread_epsilon < delta ) ? delta : thread_epsilon;

              this->_oldMarginalMin[i][j] = this->_marginalMin[i][j];
              this->_oldMarginalMax[i][j] = this->_marginalMax[i][j];
            }
          } // end of : all variables

          #pragma omp critical(epsilon_max)
          {
            #pragma omp flush(epsilon_max)

            if ( epsilon_max < thread_epsilon )
              epsilon_max = thread_epsilon;
          }

          # pragma omp single
          {
            #pragma omp flush(epsilon_max)
            all_stop = ! this->continueApproximationScheme ( epsilon_max, false, false );
            //std::cout << "epsilon : " << epsilon_max << std::endl;
          }

          #pragma omp flush(all_stop)

        } // end of : if( startOfPeriod() )

        #pragma omp flush(all_stop)

      } while ( ! all_stop );

      // end of : while ( continueApproximationScheme ) (if single threaded)

      if ( ! this->_l_modal[this_thread].empty() ) {
        // expectation fusion ( need to do vertex fusion too )
        #pragma omp for
        for ( int i = 0; i < this->_workingSet[this_thread]->size(); i++ ) {
          std::string var_name, time_step;
          size_t delim;

          var_name = this->_workingSet[this_thread]->variable ( i ).name();
          delim = var_name.find_first_of ( "_" );
          time_step = var_name.substr ( delim + 1, var_name.size() );
          var_name = var_name.substr ( 0, delim );

          if ( this->_l_modal[this_thread].find ( var_name ) == this->_l_modal[this_thread].end() )
            continue;

          for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
            if ( this->_l_expectationMax[thread_id][i] > this->_expectationMax[i] )
              this->_expectationMax[i] = this->_l_expectationMax[thread_id][i];

            if ( this->_l_expectationMin[thread_id][i] < this->_expectationMin[i] )
              this->_expectationMin[i] = this->_l_expectationMin[thread_id][i];
          } // end of : each thread
        } // end of : each variable
      } // end of : if modals not empty

    } // end of : parallel region

    GUM_TRACE ( "====== MCSampling PARALLEL REGION ENDS ======" );

    if ( ! this->_modal.empty() )
      this->_dynamicExpectations(); // work with any network

    std::cout << this->messageApproximationScheme() << std::endl;

  } // end of : makeInference() v2

  /*
    // this will be called thousands of times, so inline
    template< typename GUM_SCALAR, class BNInferenceEngine >
    inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__updateThread( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex ) {
      int this_thread = omp_get_thread_num();
      // save E(X)
      if( ! this->_l_modal[this_thread].empty() ) {
        std::string var_name, time_step;
        size_t delim;

        var_name = this->_workingSet[this_thread]->variable ( id ).name();
        delim = var_name.find_first_of ( "_" );
        time_step = var_name.substr ( delim + 1, var_name.size() );
        var_name = var_name.substr ( 0, delim );

        if( this->_l_modal[this_thread].find(var_name) != this->_l_modal[this_thread].end() ) {
          GUM_SCALAR exp = 0;
          for ( Size mod = 0; mod < vertex.size(); mod++ )
            exp += vertex[mod] * this->_l_modal[this_thread][var_name][mod];

          if( exp > this->_l_expectationMax[this_thread][id] )
            this->_l_expectationMax[this_thread][id] = exp;
          if( exp < this->_l_expectationMin[this_thread][id] )
            this->_l_expectationMin[this_thread][id] = exp;
        }
      } // end of : if modal (map) not empty

      bool newOne = false;
      bool added = false;
      // for burn in, we need to keep checking on local marginals and not global ones (faster inference)
      for( Size mod = 0; mod < vertex.size(); mod++ ) {
        if(vertex[mod] <= this->_l_marginalMin[this_thread][id][mod]) {
          this->_l_marginalMin[this_thread][id][mod] = vertex[mod];
          newOne = true;
        }
        if(vertex[mod] >= this->_l_marginalMax[this_thread][id][mod]) {
          this->_l_marginalMax[this_thread][id][mod] = vertex[mod];
          newOne = true;
        }
        // store point to compute credal set vertices.
        // check for redundancy at each step or at the end ?
        if( __storeVertices && ! added && newOne ) {
          this->_l_marginalSets[this_thread][id].push_back(vertex);
          added = true;
        }
      }
    }
  */
  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::makeInference() {
    // shared stuff
    int64_t inf_cpt = 0;
    int num_threads;
    bool no_change = true;
    bool all_stop = false;
    int64_t no_change_iters = 0;
    int64_t timeElapsed;

    GUM_SCALAR epsilon_max = 0;

    // each thread keeps its own information before fusion
    // threads do not like sharing iterators and such, so copy all that is needed (more memory, faster read/write)
    // outside of parallel region if access is needed after all threads complete
    std::vector< typename gum::Property< std::vector< GUM_SCALAR> >::onNodes > l_marginalMin;
    std::vector< typename gum::Property< std::vector< GUM_SCALAR> >::onNodes > l_marginalMax;
    std::vector< typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes > l_marginalSets;
    std::vector< typename gum::Property< GUM_SCALAR >::onNodes > l_expectationMin;
    std::vector< typename gum::Property< GUM_SCALAR >::onNodes > l_expectationMax;
    std::vector< std::map< std::string, typename std::vector< GUM_SCALAR > > > l_modal;

    // to compute epsilon, we keep old marginals for K iterations, K being the stopping criterion
    // to do : we should use burnin, for now we set all previous probabilitiy values to uniform (that's bad)
    typename gum::Property< std::vector< GUM_SCALAR > >::onNodes previous_marginals;

    for ( int i = 0; i < this->_credalNet->current_bn().size(); i++ ) {
      int dSize = this->_credalNet->current_bn().variable ( i ).domainSize();
      previous_marginals.insert ( i, std::vector< GUM_SCALAR > ( dSize, 1./dSize ) );
    }

    //if(__repetitiveInd)
    try {
      this->_repetitiveInit();
    } catch ( InvalidArgument &err ) {
      GUM_SHOWERROR ( err )
      std::cout << "\nvoid MCSampling< GUM_SCALAR, BNInferenceEngine >::makeInference() : Will use strong independence" << std::endl;
      __repetitiveInd = false;
    }

    /*for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = this->_t0.begin(); it != this->_t0.end(); ++it ) {
      __varOrder.push_back ( it.key() );
    }

    for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = this->_t1.begin(); it != this->_t1.end(); ++it ) {
      __varOrder.push_back ( it.key() );
    }

    //__varInst.resize(__varOrder.size(),0);
    __VERT = 1;

    //int entries = 0;
    for ( Size pos = 0; pos < __varOrder.size(); pos++ ) {
      //int entry_size = this->_credalNet->current_bn().cpt(__varOrder[pos]).domainSize()/ this->_credalNet->current_bn().variable(__varOrder[pos]).domainSize();
      //entries += entry_size;
      //__VERT *= pow(this->_credalNet->current_bn().variable(__varOrder[pos]).domainSize(),entry_size);
      __VERT *= 2;

      __varInst.push_back ( std::vector<int> ( 1, 0 ) );
    }*/

    //__varInst = std::vector< std::vector<int> >(entries);

    //std::cout << "Will go through " << __VERT << " DBNs" << std::endl;

    #pragma omp parallel
    {
      int this_thread = omp_get_thread_num();
      num_threads = omp_get_num_threads();

      gum::BayesNet< GUM_SCALAR > * thread_bn = new gum::BayesNet< GUM_SCALAR >();
      // seg fault without critical (maybe iterators used for copy ?)
      #pragma omp critical
      {
        *thread_bn = this->_credalNet->current_bn();
      }

      #pragma omp single
      {
        this->_workingSet.clear();
        this->_workingSet.resize ( num_threads );
        this->_workingSetE.clear();
        this->_workingSetE.resize ( num_threads );
        l_marginalMin.resize ( num_threads );
        l_marginalMax.resize ( num_threads );
        l_marginalSets.resize ( num_threads );
        l_expectationMin.resize ( num_threads );
        l_expectationMax.resize ( num_threads );
        l_modal.resize ( num_threads );
      }

      this->_workingSet[this_thread] = thread_bn;

      //#pragma omp critical
      //{
      l_modal[this_thread] = this->_modal;
      l_marginalMin[this_thread] = this->_marginalMin;
      l_marginalMax[this_thread] = this->_marginalMax;
      l_marginalSets[this_thread] = this->_marginalSets;
      l_expectationMin[this_thread] = this->_expectationMin;
      l_expectationMax[this_thread] = this->_expectationMax;
      //}

      gum::List< const gum::Potential< GUM_SCALAR > * > * evi_list = new gum::List< const gum::Potential< GUM_SCALAR > * >();
      this->_workingSetE[this_thread] = evi_list;

      const gum::DAG &thread_dag = this->_workingSet[this_thread]->dag();

// above copies seems to work
      /*
            for ( gum::DAG::NodeIterator id = thread_dag.beginNodes(); id != thread_dag.endNodes(); ++id ) {
              int dSize = this->_workingSet[this_thread]->variable ( *id ).domainSize();
              l_marginalMin[this_thread].insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
              l_marginalMax[this_thread].insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
              l_marginalSets[this_thread].insert ( *id, std::vector< std::vector< GUM_SCALAR> >() );

              // since only monitored variables in _modal will be alble to compute expectations, it is useless to initialize those for all variables
              // _modal variables will always be checked further, so it is not necessary to check it here, but doing so will use less memory
              std::string var_name, time_step;
              size_t delim;

              var_name = this->_credalNet->current_bn().variable ( *id ).name();
              delim = var_name.find_first_of ( "_" );
              time_step = var_name.substr ( delim + 1, var_name.size() );
              var_name = var_name.substr ( 0, delim );

              if(l_modal[this_thread].empty() || l_modal[this_thread].find(var_name) == l_modal[this_thread].end())
                continue;

              l_expectationMin[this_thread].insert ( *id, 100 );
              l_expectationMax[this_thread].insert ( *id, 0 );
            }
      */

      BNInferenceEngine inference_engine ( * ( this->_workingSet[this_thread] ) );

      Timer tps_inf;
      tps_inf.reset();
      //Chrono tps_inf;
      //tps_inf.start();
      /*#pragma omp single
      {
        this->initApproximationScheme();
        this->setMaxTime(__timeLimit);
        //this->setPeriodSize(__iterStop);
        this->setEpsilon(std::numeric_limits< GUM_SCALAR >::min());
        std::cout << "min eps : " << this->epsilon() << std::endl;
        std::cout << "def eps rate : " << this->minEpsilonRate() << std::endl;
        //this->setMinEpsilonRate();
      }*/

      while ( /*inf_cpt < __VERT &&*/ /*tps_inf.getElapsedTime()*/ /*tps_inf.step() < __timeLimit &&*/ ! all_stop ) {
        #pragma omp atomic
        inf_cpt++;

        /*#pragma omp single
        {
          #pragma omp flush(inf_cpt)
          int i = tps_inf.step();//tps_inf.getElapsedTimeInSec();
          std::cout << "inference : " << inf_cpt << " time elapsed : " << i  << " time left : " << ( __timeLimit - i ) << " seconds       \r";
          std::cout.flush();

          //std::cout << "\ncurrent time : " << this->currentTime() << std::endl;

          if ( inf_cpt >= __VERT )
            stopN = true;
        }*/
        //#pragma omp flush(stopN)

        __verticesSampling();

        inference_engine.eraseAllEvidence();
        __insertEvidence ( inference_engine );

        inference_engine.makeInference();

        // each thread update its marginals
        /////// ATTENTION ///////
        // evidenceMarginal() is only available with LazyPropagation
        if ( inference_engine.evidenceMarginal() > 0 ) {
          for ( gum::DAG::NodeIterator it = thread_dag.beginNodes(); it != thread_dag.endNodes(); ++it ) {
            const gum::Potential< GUM_SCALAR > & potential ( inference_engine.marginal ( *it ) );
            gum::Instantiation ins ( potential );
            std::vector< GUM_SCALAR > vertex;

            for ( ins.setFirst(); !ins.end(); ++ins ) {
              vertex.push_back ( potential[ins] );
            } // end of : node cpt

            // save DBN E(X)
            // no need to fusion expectations yet, since stopping rule is set for marginals (intervals)
            if ( ! l_modal[this_thread].empty() ) {
              std::string var_name, time_step;
              size_t delim;

              var_name = this->_workingSet[this_thread]->variable ( *it ).name();
              delim = var_name.find_first_of ( "_" );
              time_step = var_name.substr ( delim + 1, var_name.size() );
              var_name = var_name.substr ( 0, delim );

              if ( l_modal[this_thread].find ( var_name ) != l_modal[this_thread].end() ) {
                GUM_SCALAR exp = 0;

                for ( Size mod = 0; mod < vertex.size(); mod++ )
                  exp += vertex[mod] * l_modal[this_thread][var_name][mod];

                if ( exp > l_expectationMax[this_thread][*it] )
                  l_expectationMax[this_thread][*it] = exp;

                if ( exp < l_expectationMin[this_thread][*it] )
                  l_expectationMin[this_thread][*it] = exp;
              }
            } // end of : if modal (map) not empty

            bool newOne = false;
            bool added = false;

            // we can check either global or local marginals since both are updated at each step
            for ( Size mod = 0; mod < vertex.size(); mod++ ) {
              if ( vertex[mod] <= this->_marginalMin[*it][mod] ) {
                l_marginalMin[this_thread][*it][mod] = vertex[mod];
                newOne = true;
              } else if ( vertex[mod] == this->_marginalMin[*it][mod] ) {
                newOne = true;
              }

              if ( vertex[mod] >= this->_marginalMax[*it][mod] ) {
                l_marginalMax[this_thread][*it][mod] = vertex[mod];
                newOne = true;
              } else if ( vertex[mod] == this->_marginalMax[*it][mod] ) {
                newOne = true;
              }

              // store point to compute credal set vertices.
              // check for redundancy at each step or at the end ?
              /*if( !added && newOne ) {
                l_marginalSets[this_thread][*it].push_back(vertex);
                added = true;
              }*/
            }
          } // end of : for all nodes
        } // end of : if p(e) > 0

        // global marginal update
        #pragma omp for

        for ( int i = 0; i < this->_workingSet[this_thread]->size(); i++ ) {
          int dSize = l_marginalMin[this_thread][i].size();

          for ( int j = 0; j < dSize; j++ ) {
            for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
              if ( l_marginalMin[thread_id][i][j] < this->_marginalMin[i][j] ) {
                no_change = false;
                this->_marginalMin[i][j] = l_marginalMin[thread_id][i][j];
              }

              if ( l_marginalMax[thread_id][i][j] > this->_marginalMax[i][j] ) {
                no_change = false;
                this->_marginalMax[i][j] = l_marginalMax[thread_id][i][j];
              }
            } // end of : all threads

            // update them
            for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
              if ( l_marginalMin[thread_id][i][j] > this->_marginalMin[i][j] )
                l_marginalMin[thread_id][i][j] = this->_marginalMin[i][j];

              if ( l_marginalMax[thread_id][i][j] < this->_marginalMax[i][j] )
                l_marginalMax[thread_id][i][j] = this->_marginalMax[i][j];
            } // end of : all threads
          } // end of : all modalities
        } // end of : all variables

        /*
                #pragma omp flush(inf_cpt)
                // compute epsilon if needed
                if(inf_cpt % __iterStop == 0) {
                  #pragma omp single
                  {
                    std::cout << "\nupdate epsilon" << std::endl;
                    epsilon_max = 0;
                  }

                  GUM_SCALAR thread_epsilon = 0;
                  GUM_SCALAR delta;
                  int dSize;
                  #pragma omp for
                  for(int i = 0; i < this->_workingSet[this_thread]->size(); i++) {
                    dSize = l_marginalMin[this_thread][i].size();
                    for(int j = 0; j < dSize; j++) {
                      delta = this->_marginalMin[i][j] - previous_marginals[i][j];
                      delta = (delta < 0) ? (-delta) : delta;
                      thread_epsilon = (thread_epsilon < delta) ? delta : thread_epsilon;
                      previous_marginals[i][j] = this->_marginalMin[i][j];
                    }
                  }
                  #pragma omp critical
                  {
                    #pragma omp flush(epsilon_max)
                    if(epsilon_max < thread_epsilon)
                      epsilon_max = thread_epsilon;
                  }
                  #pragma omp barrier
                  // give it to approxscheme if inf_cpt % critere arret = 0 ?
                  #pragma omp single
                  {
                    #pragma omp flush(epsilon_max)
                    this->updateApproximationScheme(__iterStop);
                    std::cout << epsilon_max << std::endl;
                    if( ! this->continueApproximationScheme(epsilon_max, false, true) ) {
                      std::cout << "\n\nApprox scheme stop\n" << std::endl;
                      std::cout << "epsilon : " << epsilon_max << std::endl;
                      std::cout << "\n" << this->messageApproximationScheme() << std::endl;
                      all_stop = true;
                    }
                  }
                  #pragma omp barrier
                }*/

        /*#pragma omp single
        {
          if(this->currentTime() > __timeLimit) {
            if( ! this->continueApproximationScheme(epsilon_max, false, false) ) {
              std::cout << "\n\nApprox scheme stop\n" << std::endl;
              std::cout << "epsilon : " << epsilon_max << std::endl;
              std::cout << "\n" << this->messageApproximationScheme() << std::endl;
              all_stop = true;
            }
          }
        }*/

        // vertex update
        /*
        #pragma omp for
        for(int i = 0; i < this->_workingSet[this_thread]->size(); i++) {
          for(int thread_id = 0; thread_id < num_threads; thread_id++) {
            bool opti = false;
            int dSize = l_marginalMin[this_thread][i].size();
            for(int j = 0; j < dSize; j++) {
              if(fabs(l_marginalMin[thread_id][i][j] - this->_marginalMin[i][j]) < 1e-4
                  || fabs(l_marginalMax[thread_id][i][j] - this->_marginalMax[i][j]) < 1e-4) {
                opti = true;
                //break; // comment this if uncomment others
              }
              l_marginalMin[thread_id][i][j] = 1.;
              l_marginalMax[thread_id][i][j] = 0.;
            }

            if(opti) {
              for(int vertex = 0; vertex < l_marginalSets[thread_id][i].size(); vertex++) {
                __marginalSets[i].push_back(l_marginalSets[thread_id][i][vertex]);
              }
            }
            l_marginalSets[thread_id][i].clear();
          } // end of : all threads
          */
        // mark all vertices which are at optimum
        /*std::vector< bool > opt(__marginalSets[i].size(), false);
        for(int vertex = 0; vertex < __marginalSets[i].size(); vertex++) {
          for(int j = 0; j < __marginalSets[i][vertex].size(); j++) {
            if(fabs(__marginalSets[i][vertex][j] - this->_marginalMin[i][j]) < 1e-4
                || fabs(__marginalSets[i][vertex][j] - this->_marginalMax[i][j]) < 1e-4){
              opt[vertex] = true;
              break;
            }
          }
        }
        */

        /*        for(int i = 0; i < this->_workingSet[this_thread]->size(); i++) {

                  std::vector< bool > opt(l_marginalSets[this_thread][i].size(), false);
                  for(int vertex = 0; vertex < l_marginalSets[this_thread][i].size(); vertex++) {
                    for(int j = 0; j < l_marginalSets[this_thread][i][vertex].size(); j++) {
                      if(fabs(l_marginalSets[this_thread][i][vertex][j] - this->_marginalMin[i][j]) < 1e-6
                          || fabs(l_marginalSets[this_thread][i][vertex][j] - this->_marginalMax[i][j]) < 1e-6){
                        opt[vertex] = true;
                        break;
                      }
                    }
                  }

                  // elim redundant vertices from remaining set (vertex at true)
                  for(int vertex = 0; vertex < opt.size(); vertex++) {
                    if(opt[vertex]) {
                      // mark all vertices that are equal to this vertex at false
                      for(int other = vertex + 1; other < opt.size(); other++) {
                        if(!opt[other])
                          continue; // only compare with other optimum

                        bool isEqual = true;

                        for(int j = 0; j < l_marginalSets[this_thread][i][vertex].size(); j++) {
                          if(fabs(l_marginalSets[this_thread][i][vertex][j] - l_marginalSets[this_thread][i][other][j]) > 1e-2) {

                        //for(int j = 0; j < __marginalSets[i][vertex].size(); j++) {
                          //if(fabs(__marginalSets[i][vertex][j] - __marginalSets[i][other][j]) > 1e-4) {
                            isEqual = false;
                            break;
                          }
                        } // end of : all modalities
                        if(isEqual)
                          opt[other] = false;
                      } // end of : comparison with others
                    } // end of : if vertex is at optimum
                  } // end of : all comparisons

                  std::vector< std::vector< GUM_SCALAR > > cSet;
                  for(int vertex = 0; vertex < opt.size(); vertex++)
                    if(opt[vertex])
                      cSet.push_back(l_marginalSets[this_thread][i][vertex]);
                      //cSet.push_back(__marginalSets[i][vertex]);
                  //__marginalSets[i] = cSet;
                  l_marginalSets[this_thread][i] = cSet;
                } // end of : all variables
        */

        //#pragma omp barrier

        #pragma omp single
        {
          #pragma omp flush(no_change_iters)
          #pragma omp flush(no_change)

          if ( no_change )
            no_change_iters++;
          else
            no_change_iters = 0;

          no_change = true;

          if ( no_change_iters >= __iterStop ) {
            all_stop = true;
            std::cout << " no change for " << __iterStop << " iterations " << std::endl;
          }

          if ( tps_inf.step() > __timeLimit ) {
            all_stop = true;
            std::cout << " time limit reached " << std::endl;
          }
        }

        #pragma omp flush(inf_cpt)
        #pragma omp flush(all_stop)

      } // end of : while time spent < __timeLimit && ! all_stop

      if ( ! l_modal[this_thread].empty() ) {
        // expectation fusion
        #pragma omp for
        for ( int i = 0; i < this->_workingSet[this_thread]->size(); i++ ) {
          std::string var_name, time_step;
          size_t delim;

          var_name = this->_workingSet[this_thread]->variable ( i ).name();
          delim = var_name.find_first_of ( "_" );
          time_step = var_name.substr ( delim + 1, var_name.size() );
          var_name = var_name.substr ( 0, delim );

          if ( l_modal[this_thread].find ( var_name ) == l_modal[this_thread].end() )          continue;

          for ( int thread_id = 0; thread_id < num_threads; thread_id++ ) {
            if ( l_expectationMax[thread_id][i] > this->_expectationMax[i] )
              this->_expectationMax[i] = l_expectationMax[thread_id][i];

            if ( l_expectationMin[thread_id][i] < this->_expectationMin[i] )
              this->_expectationMin[i] = l_expectationMin[thread_id][i];
          } // end of : each thread
        } // end of : each variable
      }

      #pragma omp critical
      {
        timeElapsed = tps_inf.step();//tps_inf.getElapsedTime();
      }
      //tps_inf.stop();
      tps_inf.pause();


    } // end of : parallel region

    if ( ! this->_modal.empty() )
      this->_dynamicExpectations(); // work with any network

    //std::cout << std::endl;
    /*
        #pragma omp for
        for(int i = 0; i < this->_workingSet[0]->size(); i++) {
          for(int thread_id = 0; thread_id < num_threads; thread_id++) {
            bool opti = false;
            int dSize = l_marginalMin[0][i].size();
            for(int j = 0; j < dSize; j++) {
              if(fabs(l_marginalMin[thread_id][i][j] - this->_marginalMin[i][j]) < 1e-6
                  || fabs(l_marginalMax[thread_id][i][j] - this->_marginalMax[i][j]) < 1e-6) {
                opti = true;
                break;
              }
            }

            if(opti) {
              for(int vertex = 0; vertex < l_marginalSets[thread_id][i].size(); vertex++) {
                __marginalSets[i].push_back(l_marginalSets[thread_id][i][vertex]);
              }
            }
            l_marginalSets[thread_id][i].clear();
          } // end of : all threads

          // mark all vertices which are at optimum
          std::vector< bool > opt(__marginalSets[i].size(), false);
          for(int vertex = 0; vertex < __marginalSets[i].size(); vertex++) {
            for(int j = 0; j < __marginalSets[i][vertex].size(); j++) {
              if(fabs(__marginalSets[i][vertex][j] - this->_marginalMin[i][j]) < 1e-4
                  || fabs(__marginalSets[i][vertex][j] - this->_marginalMax[i][j]) < 1e-4){
                opt[vertex] = true;
                break;
              }
            }
          }

          // elim redundant vertices from remaining set (vertex at true)
          for(int vertex = 0; vertex < opt.size(); vertex++) {
            if(opt[vertex]) {
              // mark all vertices that are equal to this vertex at false
              for(int other = vertex + 1; other < opt.size(); other++) {
                if(!opt[other])
                  continue; // only compare with other optimum

                bool isEqual = true;
                for(int j = 0; j < __marginalSets[i][vertex].size(); j++) {
                  if(fabs(__marginalSets[i][vertex][j] - __marginalSets[i][other][j]) > 1e-2) {
                    isEqual = false;
                    break;
                  }
                } // end of : all modalities
                if(isEqual)
                  opt[other] = false;
              } // end of : comparison with others
            } // end of : if vertex is at optimum
          } // end of : all comparisons

          std::vector< std::vector< GUM_SCALAR > > cSet;
          for(int vertex = 0; vertex < opt.size(); vertex++)
            if(opt[vertex])
              cSet.push_back(__marginalSets[i][vertex]);
          __marginalSets[i] = cSet;
        } // end of : all variables
    */


    /* if(all_stop && timeElapsed < __timeLimit)
       __stopType = false;
     else
       __stopType = true;*/

    /* for(int i = 0; i < this->_workingSet.size(); i++)
       delete this->_workingSet[i];*/

    std::cout << std::endl;
    //std::cout << this->toString();

    //std::cout << toString();

    //std::cout << "elapsed time : " << timeElapsed << " seconds" << std::endl;
    //std::cout << "stopping rule met : ";
    //if(!__stopType)
    //std::cout << "no change for " << __iterStop * num_threads << " inferences" << std::endl;
    //else
    //std::cout << "time limit" << std::endl;

    std::cout << std::endl << "# inferences : " << inf_cpt << std::endl;
    //std::cout << "# trajectoires : " << __trajectories.size() << std::endl;

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::eraseAllEvidence() {
    //this->eraseAllEvidence();
    // delete pointers
    for ( Size bn = 0; bn < this->_workingSet.size(); bn++ ) {
      if ( __storeVertices )
        this->_l_marginalSets[bn].clear();

      if ( this->_workingSet[bn] != NULL )
        delete this->_workingSet[bn];

      if ( this->_workingSetE[bn] != NULL ) {
        for ( typename gum::List< const gum::Potential< GUM_SCALAR > * >::iterator it = this->_workingSetE[bn]->begin(); it != this->_workingSetE[bn]->end(); ++it )
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
  }

  /*
  template< typename GUM_SCALAR, class BNInferenceEngine >
  std::string MCSampling< GUM_SCALAR, BNInferenceEngine >::toString() const {
  std::stringstream output;
  output << "\n";

  for ( typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes::const_iterator it = __marginalSets.begin(); it != __marginalSets.end(); ++it ) {
    output << "P(" << this->_credalNet->current_bn().variable ( it.key() ).name() << "|e) = [ ";

    for ( int vertex = 0; vertex < __marginalSets[it.key()].size(); vertex++ ) {
      if ( vertex > 0 )
        output << ", ";

      output << __marginalSets[it.key()][vertex];
    }

    output << " ]";

    if ( ! this->_query.empty() )
      if ( this->_query.exists ( it.key() ) )
        output << " QUERY";

    output << "\n";
    output << "\n";
  }

  return output.str();
  }
  */

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
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__verticesSampling() {
    int this_thread = omp_get_thread_num();
    gum::BayesNet< GUM_SCALAR > * working_bn = this->_workingSet[this_thread];

    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *cpt = &this->_credalNet->credalNet_cpt();

    int choosen_vertex;
    Size dSize;

    /*if ( __repetitiveInd || !stopN ) {
      if ( !stopN ) {
        std::vector< std::vector < int > > inst;
        #pragma omp critical
        {
          inst = __varInst;

          // for each local (conditional) credal set
          for(int credalSet = 0; credalSet < (*cpt)[__varOrder[pos]].size(); credalSet++) {
            __varInst[pos][credalSet]++;
            if(__varInst[pos][credalSet] >= working_bn->variable(__varOrder[pos]).domainSize())
              __varInst[pos][credalSet] = 0;
            else
              break;
          }

          __varInst[pos]++;
          if(__varInst[pos] >= working_bn->variable(__varOrder[pos]).domainSize()) {
            __varInst[pos] = 0;
          }
          else
            break;
        }
      }
    }

        typename gum::Property< std::vector< gum::NodeId > >::onNodes t0;
        typename gum::Property< std::vector< gum::NodeId > >::onNodes t1;
        t0 = this->_t0;
        t1 = this->_t1;

        for ( Size var = 0; var < __varOrder.size(); var++ ) {
          for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = t0.begin(); it != t0.end(); ++it ) {
            if ( it.key() == __varOrder[var] ) {
              dSize = working_bn->variable ( it.key() ).domainSize();
              gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
              std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

              for ( Size pconf = 0; pconf < ( *cpt ) [it.key()].size(); pconf++ ) {
                for ( Size mod = 0; mod < dSize; mod++ ) {
                  if ( inst[var][0] == 0 ) {
                    var_cpt[pconf *  + mod] = ( *cpt ) [it.key()][pconf][0][mod];
                  } else if ( inst[var][0] == 1 ) {
                    int CSsize = ( *cpt ) [it.key()][pconf].size();
                    var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][CSsize - 1][mod];

                  }

                  //var_cpt[pconf * dSize + mod] = (*cpt)[it.key()][pconf][inst[var][pconf]][mod];
                }
              } // end of : pconf

              potential->fillWith ( var_cpt );

              for ( Size pos = 0; pos < it->size(); pos++ ) {
                gum::Potential< GUM_SCALAR > * potential2 ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
                potential2->fillWith ( var_cpt );
              }
            } // end of : correct node
          } // end of : t_0 cluster

          for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = t1.begin(); it != t1.end(); ++it ) {
            if ( it.key() == __varOrder[var] ) {
              dSize = working_bn->variable ( it.key() ).domainSize();
              gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
              std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

              for ( Size pconf = 0; pconf < ( *cpt ) [it.key()].size(); pconf++ ) {
                for ( Size mod = 0; mod < dSize; mod++ ) {
                  if ( inst[var][0] == 0 ) {
                    var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][0][mod];
                  } else if ( inst[var][0] == 1 ) {
                    int CSsize = ( *cpt ) [it.key()][pconf].size();
                    var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][CSsize - 1][mod];
                  }

                  //var_cpt[pconf * dSize + mod] = (*cpt)[it.key()][pconf][inst[var][pconf]][mod];

                }
              } // end of : pconf

              potential->fillWith ( var_cpt );

              for ( Size pos = 0; pos < it->size(); pos++ ) {
                gum::Potential< GUM_SCALAR > * potential2 ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
                potential2->fillWith ( var_cpt );
              }
            }

          }
        }

        return;
      }
    */


    if ( __repetitiveInd ) {
      typename gum::Property< std::vector< gum::NodeId > >::onNodes t0;
      typename gum::Property< std::vector< gum::NodeId > >::onNodes t1;
      t0 = this->_t0;
      t1 = this->_t1;

      for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = t0.begin(); it != t0.end(); ++it ) {
        dSize = working_bn->variable ( it.key() ).domainSize();
        gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
        std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

        ///////////////////////////////////
//choosen_vertex = rand() % (*cpt)[it.key()][0].size();
/////////////////////////////
        for ( Size pconf = 0; pconf < ( *cpt ) [it.key()].size(); pconf++ ) {
          choosen_vertex = rand() % ( *cpt ) [it.key()][pconf].size();

          for ( Size mod = 0; mod < dSize; mod++ )
            var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][choosen_vertex][mod];
        } // end of : pconf

        potential->fillWith ( var_cpt );

        for ( Size pos = 0; pos < it->size(); pos++ ) {
          gum::Potential< GUM_SCALAR > * potential2 ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
          potential2->fillWith ( var_cpt );
        }
      }

      for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = t1.begin(); it != t1.end(); ++it ) {
        dSize = working_bn->variable ( it.key() ).domainSize();
        gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
        std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );
//choosen_vertex = rand() % (*cpt)[it.key()][0].size();

        for ( Size pconf = 0; pconf < ( *cpt ) [it.key()].size(); pconf++ ) {
          choosen_vertex = rand() % ( *cpt ) [it.key()][pconf].size();

          for ( Size mod = 0; mod < dSize; mod++ )
            var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][choosen_vertex][mod];
        } // end of : pconf

        potential->fillWith ( var_cpt );

        for ( Size pos = 0; pos < it->size(); pos++ ) {
          gum::Potential< GUM_SCALAR > * potential2 ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
          potential2->fillWith ( var_cpt );
        }
      }
    } else {
      for ( gum::DAG::NodeIterator id = working_bn->beginNodes(); id != working_bn->endNodes(); ++id ) {
        dSize = working_bn->variable ( *id ).domainSize();
        gum::Potential< GUM_SCALAR > * potential ( const_cast< gum::Potential< GUM_SCALAR > * > ( &working_bn->cpt ( *id ) ) );
        std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );
        //choosen_vertex = rand() % (*cpt)[*id][0].size();

        for ( Size pconf = 0; pconf < ( *cpt ) [*id].size(); pconf++ ) {
          choosen_vertex = rand() % ( *cpt ) [*id][pconf].size();

          for ( Size mod = 0; mod < dSize; mod++ )
            var_cpt[pconf * dSize + mod] = ( *cpt ) [*id][pconf][choosen_vertex][mod];
        } // end of : pconf

        potential->fillWith ( var_cpt );
      }
    }

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  inline void MCSampling< GUM_SCALAR, BNInferenceEngine >::__insertEvidence ( BNInferenceEngine &inference_engine ) { /*const*/
    if ( this->_evidence.size() == 0 )
      return;

    int this_thread = omp_get_thread_num();
    gum::BayesNet<GUM_SCALAR> * working_bn = this->_workingSet[this_thread];

    gum::List< const gum::Potential< GUM_SCALAR > * > * evi_list = this->_workingSetE[this_thread];

    if ( evi_list->size() > 0 ) {
      inference_engine.insertEvidence ( *evi_list );
      return;
    }


    //gum::List< const gum::Potential< GUM_SCALAR > * > evi_list;

    typename gum::Property< std::vector< GUM_SCALAR > >::onNodes thread_evidence = this->_evidence;

    for ( typename gum::Property< std::vector< GUM_SCALAR > >::onNodes::const_iterator it = thread_evidence.begin(); it != thread_evidence.end(); ++it ) {
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

    //this->_workingSetE[this_thread] = evi_list;

    if ( evi_list->size() > 0 )
      inference_engine.insertEvidence ( *evi_list );

  }

} // namespace cn
