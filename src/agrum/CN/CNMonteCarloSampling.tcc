#include "CNMonteCarloSampling.h"
#include <agrum/core/exceptions.h>

namespace gum {
  namespace credal {

    template< typename GUM_SCALAR, class BNInferenceEngine >
    CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::CNMonteCarloSampling ( const CredalNet< GUM_SCALAR >& credalNet ) : MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::MultipleInferenceEngine ( credalNet ) {
      infEs::_repetitiveInd = false;
      //infEs::_iterStop = 1000;
      infEs::_storeVertices = false;
      infEs::_storeBNOpt = false;

      this->setMaxTime ( 60 );
      this->enableMaxTime();

      ///this->setBurnIn ( 1000 );
      this->setPeriodSize ( 1000 );

      GUM_CONSTRUCTOR ( CNMonteCarloSampling );
    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::~CNMonteCarloSampling() {
      GUM_DESTRUCTOR ( CNMonteCarloSampling );
    }


    template< typename GUM_SCALAR, class BNInferenceEngine >
    void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::makeInference() {

      if ( infEs::_repetitiveInd ) {
        try {
          this->_repetitiveInit();
        } catch ( InvalidArgument& err ) {
          GUM_SHOWERROR ( err );
          infEs::_repetitiveInd = false;
        }
      }

      // debug
      ///notOptDelete = 0;

      __mcInitApproximationScheme();


      __mcThreadDataCopy();

      // don't put it after burnIn, it could stop with timeout : we want at least one burnIn and one periodSize
      GUM_SCALAR eps = 1.; // to validate testSuite ?
      ///this->continueApproximationScheme ( eps, false/*, false*/ );

      ///auto bsize = this->burnIn();
      auto psize = this->periodSize();

      auto remaining = this->remainingBurnIn();

      /// this->burnIn() should be 0 therefor the first do ... while should be skipped
      if ( remaining != 0 ) {
        /// instead of doing the whole burnIn in one pass, we do it period by period
        /// so we can test the timer ( done by continueApproximationScheme ) and exit during burnIn
        /// no error is thrown if we exit during burnIn ( although one should )
        do {
          eps = 0;

          auto iters = ( remaining < psize ) ? remaining : psize;

          #pragma omp parallel for

          for ( decltype ( iters ) iter = 0; iter < iters; iter++ ) {
            __threadInference();
            __threadUpdate();
          } // end of : parallel periodSize

          this->updateApproximationScheme ( iters );

          ///this->_updateMarginals(); // fusion threads + update margi

          ///eps = this->_computeEpsilon(); // also updates oldMargi

          remaining = this->remainingBurnIn();

        } while ( ( remaining > 0 )  && this->continueApproximationScheme ( eps ) );
      }

      if ( this->continueApproximationScheme ( eps ) ) {

        do {
          eps = 0;

          // less overheads with high periodSize
          #pragma omp parallel for

          for ( decltype ( psize ) iter = 0; iter < psize; iter++ ) {
            __threadInference();
            __threadUpdate();
          } // end of : parallel periodSize

          this->updateApproximationScheme ( psize );

          this->_updateMarginals(); // fusion threads + update margi

          eps = this->_computeEpsilon(); // also updates oldMargi

        } while ( this->continueApproximationScheme ( eps ) );

      }

      if ( ! this->_modal.empty() )
        this->_expFusion();

      if ( infEs::_storeBNOpt )
        this->_optFusion();

      if ( infEs::_storeVertices )
        this->_verticesFusion();

      if ( ! this->_modal.empty() )
        this->_dynamicExpectations(); // work with any network

      ///GUM_TRACE ( this->messageApproximationScheme() );

    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    inline void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::__threadUpdate() {
      int tId = getThreadNumber();
      //bool keepSample = false;

      if ( this->_l_inferenceEngine[tId]->evidenceMarginal() > 0 ) {
        const DAG& tDag = this->_workingSet[tId]->dag();

        //for ( auto it = tDag.beginNodes(), theEnd = tDag.endNodes(); it != theEnd; ++it ) {
        for(const auto it : tDag.nodes()) {
          const Potential< GUM_SCALAR >& potential ( this->_l_inferenceEngine[tId]->marginal ( it ) );
          Instantiation ins ( potential );
          std::vector< GUM_SCALAR > vertex;

          for ( ins.setFirst(); !ins.end(); ++ins )
            vertex.push_back ( potential[ins] );

          // true for redundancy elimination of node it credal set
          // but since global marginals are only updated at the end of each period of approximationScheme, it is "useless" ( and expensive ) to check now
          this->_updateThread ( it, vertex, false );

          //if ( this->_updateThread ( it, vertex, false ) )
          //keepSample = true;

        } // end of : for all nodes
      } // end of : if ( p(e) > 0 )

      /*
          if ( infEs::_storeBNOpt && ! keepSample ) {
            //delete this->_l_optimalNet[tId]->getCurrentSample();
            //std::cout << " net deleted, no opt found " << std::endl;
            #pragma omp critical(deleteCpt)
            {
              notOptDelete++;
            }
          }
      */
      // do NOT reset threads marginals ( otherwise we will store vertices and dBN that are NOT optimal )
      /*const DAG &tDag = this->_workingSet[tId]->dag();
      unsigned int dSize;
      for ( DAG::NodeIterator it = tDag.beginNodes(); it != tDag.endNodes(); ++it ) {
        dSize = this->_l_marginalMin[tId][*it].size();
        for ( unsigned int i = 0; i < dSize; i++ ) {
          this->_l_marginalMin[tId][*it][i] = 1;
          this->_l_marginalMax[tId][*it][i] = 0;
        }
      }*/

    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    inline void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::__threadInference() {
      int tId = getThreadNumber();
      __verticesSampling();
      this->_l_inferenceEngine[tId]->eraseAllEvidence();
      __insertEvidence();
      this->_l_inferenceEngine[tId]->makeInference();

    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::__mcInitApproximationScheme() {
      //this->setEpsilon ( std::numeric_limits< GUM_SCALAR >::min() );
      /**
       * VERIFIER d/dt(e(t+1)-e(t))
       */
      //this->setMinEpsilonRate ( std::numeric_limits< GUM_SCALAR >::min() );
      //this->setBurnIn ( infEs::_iterStop );
      //this->setPeriodSize ( infEs::_iterStop );

      ///this->setMaxIter ( this->burnIn() + this->periodSize() );
      this->setEpsilon ( 0. );
      this->enableEpsilon();  // to be sure

      this->disableMinEpsilonRate();
      this->disableMaxIter();

      this->initApproximationScheme();
    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::__mcThreadDataCopy() {
      int num_threads;
      #pragma omp parallel
      {
        int this_thread = getThreadNumber();

        // implicit wait clause (don't put nowait)
        #pragma omp single
        {
          // should we ask for max threads instead ( no differences here in practice )
          num_threads = getNumberOfRunningThreads();

          this->_initThreadsData ( num_threads, infEs::_storeVertices, infEs::_storeBNOpt );
          this->_l_inferenceEngine.resize ( num_threads, nullptr );

          //if ( infEs::_storeBNOpt )
          //this->_l_sampledNet.resize ( num_threads );
        } // end of : single region

        // we could put those below in a function in InferenceEngine, but let's keep this parallel region instead of breaking it and making another one to do the same stuff in 2 places since :
        // !!! BNInferenceEngine still needs to be initialized here anyway !!!

        BayesNet< GUM_SCALAR >* thread_bn = new BayesNet< GUM_SCALAR >();
        #pragma omp critical(Init)
        {
          //IBayesNet< GUM_SCALAR > * thread_bn = new IBayesNet< GUM_SCALAR >();//(this->_credalNet->current_bn());
          *thread_bn = this->_credalNet->current_bn();
        }
        this->_workingSet[this_thread] = thread_bn;

        this->_l_marginalMin[this_thread] = this->_marginalMin;
        this->_l_marginalMax[this_thread] = this->_marginalMax;
        this->_l_expectationMin[this_thread] = this->_expectationMin;
        this->_l_expectationMax[this_thread] = this->_expectationMax;
        this->_l_modal[this_thread] = this->_modal;

        infEs::_l_clusters[this_thread].resize ( 2 );
        infEs::_l_clusters[this_thread][0] = infEs::_t0;
        infEs::_l_clusters[this_thread][1] = infEs::_t1;

        if ( infEs::_storeVertices )
          this->_l_marginalSets[this_thread] = this->_marginalSets;

        List< const Potential< GUM_SCALAR > * >* evi_list = new List< const Potential< GUM_SCALAR > * >();
        this->_workingSetE[this_thread] = evi_list;

        BNInferenceEngine* inference_engine = new BNInferenceEngine ( * ( this->_workingSet[this_thread] ) );
        this->_l_inferenceEngine[this_thread] = inference_engine;

        if ( infEs::_storeBNOpt ) {
          VarMod2BNsMap<GUM_SCALAR>* threadOpt = new VarMod2BNsMap<GUM_SCALAR> ( *this->_credalNet );
          this->_l_optimalNet[this_thread] = threadOpt;
        }
      }
    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    inline void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::__binaryRep ( std::vector< bool >& toFill,  const unsigned int value ) const {
      unsigned int n = value;
      auto tfsize = toFill.size();

      // get bits of choosen_vertex
      for ( decltype ( tfsize ) i = 0; i < tfsize; i++ ) {
        toFill[i] = n & 1;
        n /= 2;
      }
    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    inline void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::__verticesSampling() {
      int this_thread = getThreadNumber();
      IBayesNet< GUM_SCALAR >* working_bn = this->_workingSet[this_thread];

      const typename Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes* cpt = &this->_credalNet->credalNet_currentCpt();

      typedef std::vector< std::vector< std::vector< bool > > > dBN;

      dBN sample;

      if ( infEs::_storeBNOpt )
        sample = dBN ( this->_l_optimalNet[this_thread]->getSampleDef() );

      if ( infEs::_repetitiveInd ) {
        const typename Property< std::vector< NodeId > >::onNodes& t0 = infEs::_l_clusters[this_thread][0];
        const typename Property< std::vector< NodeId > >::onNodes& t1 = infEs::_l_clusters[this_thread][1];

        // use cbegin() when available
        for ( auto it = t0.begin(), theEnd = t0.end(); it != theEnd; ++it ) {
          auto dSize = working_bn->variable ( it.key() ).domainSize();
          Potential< GUM_SCALAR >* potential ( const_cast< Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
          std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

          auto pconfs = ( *cpt ) [it.key()].size();

          for ( decltype ( pconfs ) pconf = 0; pconf < pconfs; pconf++ ) {
            auto choosen_vertex = rand() % ( *cpt ) [it.key()][pconf].size();

            if ( infEs::_storeBNOpt )
              __binaryRep ( ( sample ) [it.key()][pconf], choosen_vertex );

            for ( decltype ( dSize ) mod = 0; mod < dSize; mod++ )
              var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][choosen_vertex][mod];
          } // end of : pconf

          potential->fillWith ( var_cpt );

          auto t0esize = it->size();

          for ( decltype ( t0esize ) pos = 0; pos < t0esize; pos++ ) {
            if ( infEs::_storeBNOpt )
              ( sample ) [ ( *it ) [pos]] = ( sample ) [it.key()];

            Potential< GUM_SCALAR >* potential2 ( const_cast< Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
            potential2->fillWith ( var_cpt );
          }
        }

        // use cbegin()
        for ( auto it = t1.begin(), theEnd = t1.end(); it != theEnd; ++it ) {
          auto dSize = working_bn->variable ( it.key() ).domainSize();
          Potential< GUM_SCALAR >* potential ( const_cast< Potential< GUM_SCALAR > * > ( &working_bn->cpt ( it.key() ) ) );
          std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

          for ( Size pconf = 0; pconf < ( *cpt ) [it.key()].size(); pconf++ ) {
            auto choosen_vertex = rand() % ( *cpt ) [it.key()][pconf].size();

            if ( infEs::_storeBNOpt )
              __binaryRep ( ( sample ) [it.key()][pconf], choosen_vertex );

            for ( decltype ( dSize ) mod = 0; mod < dSize; mod++ )
              var_cpt[pconf * dSize + mod] = ( *cpt ) [it.key()][pconf][choosen_vertex][mod];
          } // end of : pconf

          potential->fillWith ( var_cpt );

          auto t1esize = it->size();

          for ( decltype ( t1esize ) pos = 0; pos < t1esize; pos++ ) {
            if ( infEs::_storeBNOpt )
              ( sample ) [ ( *it ) [pos]] = ( sample ) [it.key()];

            Potential< GUM_SCALAR >* potential2 ( const_cast< Potential< GUM_SCALAR > * > ( &working_bn->cpt ( ( *it ) [pos] ) ) );
            potential2->fillWith ( var_cpt );
          }
        }

        if ( infEs::_storeBNOpt ) {
          this->_l_optimalNet[this_thread]->setCurrentSample ( sample );
        }
      } else {
        for ( auto id = working_bn->beginNodes(), theEnd = working_bn->endNodes(); id != theEnd; ++id ) {
          auto dSize = working_bn->variable ( *id ).domainSize();
          Potential< GUM_SCALAR >* potential ( const_cast< Potential< GUM_SCALAR > * > ( &working_bn->cpt ( *id ) ) );
          std::vector< GUM_SCALAR > var_cpt ( potential->domainSize() );

          auto pConfs = ( *cpt ) [*id].size();

          for ( decltype ( pConfs ) pconf = 0; pconf < pConfs; pconf++ ) {
            auto nVertices = ( *cpt ) [*id][pconf].size();
            auto choosen_vertex = rand() % nVertices;

            if ( infEs::_storeBNOpt )
              __binaryRep ( ( sample ) [*id][pconf], choosen_vertex );

            for ( decltype ( dSize ) mod = 0; mod < dSize; mod++ )
              var_cpt[pconf * dSize + mod] = ( *cpt ) [*id][pconf][choosen_vertex][mod];
          } // end of : pconf

          potential->fillWith ( var_cpt );
        }

        if ( infEs::_storeBNOpt )
          this->_l_optimalNet[this_thread]->setCurrentSample ( sample );
      }

    }

    template< typename GUM_SCALAR, class BNInferenceEngine >
    inline void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::__insertEvidence( ) {
      if ( this->_evidence.size() == 0 )
        return;

      int this_thread = getThreadNumber();

      BNInferenceEngine* inference_engine = this->_l_inferenceEngine[ this_thread ];

      IBayesNet<GUM_SCALAR>* working_bn = this->_workingSet[this_thread];

      List< const Potential< GUM_SCALAR > * >* evi_list = this->_workingSetE[this_thread];

      if ( evi_list->size() > 0 ) {
        inference_engine->insertEvidence ( *evi_list );
        return;
      }

      typename Property< std::vector< GUM_SCALAR > >::onNodes thread_evidence = this->_evidence;

      // use cbegin()
      for ( auto it = thread_evidence.begin(), theEnd = thread_evidence.end(); it != theEnd; ++it ) {
        Potential< GUM_SCALAR >* p = new Potential< GUM_SCALAR >;
        ( *p ) << working_bn->variable ( it.key() );

        try {
          p->fillWith ( *it );
        } catch ( Exception& err ) {
          std::cout << "Error while inserting evidence : " << std::endl;
          GUM_SHOWERROR ( err );
          throw ( err );
        }

        evi_list->insert ( p );
      }

      if ( evi_list->size() > 0 )
        inference_engine->insertEvidence ( *evi_list );

    }

  } // namespace cn
}
