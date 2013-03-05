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
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  MCSampling< GUM_SCALAR, BNInferenceEngine >::~MCSampling() {
    GUM_DESTRUCTOR ( MCSampling );
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::makeInference() {
    int64_t inf_cpt = 0;
    int num_threads;
    bool no_change = true;
    bool all_stop = false;
    int64_t no_change_iters = 0;
    int64_t timeElapsed;

    // each thread keeps its own information before fusion
    std::vector< typename gum::Property< std::vector< GUM_SCALAR> >::onNodes > l_marginalMin;
    std::vector< typename gum::Property< std::vector< GUM_SCALAR> >::onNodes > l_marginalMax;
    std::vector< typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes > l_marginalSets;
    std::vector< typename gum::Property< GUM_SCALAR >::onNodes > l_expectationMin;
    std::vector< typename gum::Property< GUM_SCALAR >::onNodes > l_expectationMax;
    std::vector< std::map< std::string, typename std::vector< GUM_SCALAR > > > l_modal;

    
    //if(__repetitiveInd)
    try {
      this->_repetitiveInit();
    } catch ( InvalidArgument &err ) {
      GUM_SHOWERROR ( err )
      std::cout << "\nvoid MCSampling< GUM_SCALAR, BNInferenceEngine >::makeInference() : Will use strong independence" << std::endl;
      __repetitiveInd = false;
    }

    for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = this->_t0.begin(); it != this->_t0.end(); ++it ) {
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
    }

    //__varInst = std::vector< std::vector<int> >(entries);

    std::cout << "Will go through " << __VERT << " DBNs" << std::endl;

    #pragma omp parallel
    {
      int this_thread = omp_get_thread_num();
      num_threads = omp_get_num_threads();

      gum::BayesNet< GUM_SCALAR > * thread_bn = new gum::BayesNet< GUM_SCALAR >();
      #pragma omp critical
      {
        *thread_bn = this->_credalNet->current_bn();
      }

      #pragma omp single
      {
        __workingSet.clear();
        __workingSet.resize ( num_threads );
        __workingSetE.clear();
        __workingSetE.resize ( num_threads );
        l_marginalMin.resize ( num_threads );
        l_marginalMax.resize ( num_threads );
        l_marginalSets.resize ( num_threads );
        l_expectationMin.resize( num_threads );
        l_expectationMax.resize( num_threads );
        l_modal.resize( num_threads );
      }

      __workingSet[this_thread] = thread_bn;

      #pragma omp critical
      {
        l_modal[this_thread] = this->_modal;
      }

      gum::List< const gum::Potential< GUM_SCALAR > * > * evi_list = new gum::List< const gum::Potential< GUM_SCALAR > * >();
      __workingSetE[this_thread] = evi_list;


      const gum::DAG &thread_dag = __workingSet[this_thread]->dag();

      for ( gum::DAG::NodeIterator id = thread_dag.beginNodes(); id != thread_dag.endNodes(); ++id ) {
        int dSize = __workingSet[this_thread]->variable ( *id ).domainSize();
        l_marginalMin[this_thread].insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
        l_marginalMax[this_thread].insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
        l_marginalSets[this_thread].insert ( *id, std::vector< std::vector< GUM_SCALAR> >() );
        l_expectationMin[this_thread].insert ( *id, 100 );
        l_expectationMax[this_thread].insert ( *id, 0 );
      }

      BNInferenceEngine inference_engine ( * ( __workingSet[this_thread] ) );

      Timer tps_inf;
      tps_inf.reset();
      //Chrono tps_inf;
      //tps_inf.start();

      while ( /*inf_cpt < __VERT &&*/ /*tps_inf.getElapsedTime()*/tps_inf.step() < 0.1/*__timeLimit*/ && ! all_stop ) {
        #pragma omp atomic
        inf_cpt++;

        #pragma omp single
        {
          int i = tps_inf.step();//tps_inf.getElapsedTimeInSec();
          std::cout << "inference : " << inf_cpt << " time elapsed : " << i  << " time left : " << ( __timeLimit - i ) << " seconds       \r";
          std::cout.flush();

          if ( inf_cpt >= __VERT )
            stopN = true;
        }
        #pragma omp flush(stopN)

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
            if( ! l_modal[this_thread].empty() ) {
              std::string var_name, time_step;
              size_t delim;

              var_name = __workingSet[this_thread]->variable ( *it ).name();
              delim = var_name.find_first_of ( "_" );
              time_step = var_name.substr ( delim + 1, var_name.size() );
              var_name = var_name.substr ( 0, delim );

              if( l_modal[this_thread].find(var_name) != l_modal[this_thread].end() ) {
                GUM_SCALAR exp = 0;
                for ( Size mod = 0; mod < vertex.size(); mod++ )
                  exp += vertex[mod] * l_modal[this_thread][var_name][mod];

                if( exp > l_expectationMax[this_thread][*it] )
                  l_expectationMax[this_thread][*it] = exp;
                if( exp < l_expectationMin[this_thread][*it] )
                  l_expectationMin[this_thread][*it] = exp;
              }
            } // end of : if modal (map) not empty

            bool newOne = false;
            bool added = false;
            // we can check either global or local marginals since both are updated at each step
            for( Size mod = 0; mod < vertex.size(); mod++ ) {
              if(vertex[mod] <= this->_marginalMin[*it][mod]) {
                l_marginalMin[this_thread][*it][mod] = vertex[mod];
                newOne = true;
              }
              else if(vertex[mod] == this->_marginalMin[*it][mod]) {
                newOne = true;
              }
              if(vertex[mod] >= this->_marginalMax[*it][mod]) {
                l_marginalMax[this_thread][*it][mod] = vertex[mod];
                newOne = true;
              }
              else if(vertex[mod] == this->_marginalMax[*it][mod]) {
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
        for(int i = 0; i < __workingSet[this_thread]->size(); i++) {
          int dSize = l_marginalMin[this_thread][i].size();
          for(int j = 0; j < dSize; j++) {
            for(int thread_id = 0; thread_id < num_threads; thread_id++) {
              if(l_marginalMin[thread_id][i][j] < this->_marginalMin[i][j]) {
                no_change = false;
                this->_marginalMin[i][j] = l_marginalMin[thread_id][i][j];
              }
              if(l_marginalMax[thread_id][i][j] > this->_marginalMax[i][j]) {
                no_change = false;
                this->_marginalMax[i][j] = l_marginalMax[thread_id][i][j];
              }
            } // end of : all threads
            // update them
            for(int thread_id = 0; thread_id < num_threads; thread_id++) {
              if(l_marginalMin[thread_id][i][j] > this->_marginalMin[i][j])
                l_marginalMin[thread_id][i][j] = this->_marginalMin[i][j];
              if(l_marginalMax[thread_id][i][j] < this->_marginalMax[i][j])
                l_marginalMax[thread_id][i][j] = this->_marginalMax[i][j];
            } // end of : all threads
          } // end of : all modalities
        } // end of : all variables

        

        // vertex update
        /*
        #pragma omp for
        for(int i = 0; i < __workingSet[this_thread]->size(); i++) {
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

        /*        for(int i = 0; i < __workingSet[this_thread]->size(); i++) {

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

        #pragma omp barrier

        #pragma omp single
        {
          #pragma omp flush(no_change)

          if ( no_change )
            no_change_iters++;
          else
            no_change_iters = 0;

          no_change = true;

          if ( no_change_iters >= __iterStop /*|| inf_cpt >1*/ )
            all_stop = true;
        }
        #pragma omp flush(inf_cpt)
        #pragma omp flush(all_stop)

      } // end of : while time spent < __timeLimit && ! all_stop

      // expectation fusion
      #pragma omp for
      for(int i = 0; i < __workingSet[this_thread]->size(); i++) {
        for(int thread_id = 0; thread_id < num_threads; thread_id++) {
          if( l_expectationMax[thread_id][i] > this->_expectationMax[i] )
            this->_expectationMax[i] = l_expectationMax[thread_id][i];
          if( l_expectationMin[thread_id][i] < this->_expectationMin[i] )
            this->_expectationMin[i] = l_expectationMin[thread_id][i];
        } // end of : each thread
      } // end of : each variable

      #pragma omp critical
      {
        timeElapsed = tps_inf.step();//tps_inf.getElapsedTime();
      }
      //tps_inf.stop();
      tps_inf.pause();


    } // end of : parallel region

    //std::cout << std::endl;
    /*
        #pragma omp for
        for(int i = 0; i < __workingSet[0]->size(); i++) {
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

    /* for(int i = 0; i < __workingSet.size(); i++)
       delete __workingSet[i];*/

    std::cout << std::endl;
    //std::cout << this->toString();

    //std::cout << toString();

       std::cout << "elapsed time : " << timeElapsed << " seconds" << std::endl;
       std::cout << "stopping rule met : ";
       if(!__stopType)
         std::cout << "no change for " << __iterStop * num_threads << " inferences" << std::endl;
       else
         std::cout << "time limit" << std::endl;

    std::cout << std::endl << "# inferences : " << inf_cpt << std::endl;
    //std::cout << "# trajectoires : " << __trajectories.size() << std::endl;

  }

  /*
  template< typename GUM_SCALAR, class BNInferenceEngine >
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::eraseAllEvidence() {
    //this->eraseAllEvidence();
    __marginalSets.clear();
  }*/

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
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::__verticesSampling() {
    int this_thread = omp_get_thread_num();
    gum::BayesNet< GUM_SCALAR > * working_bn = __workingSet[this_thread];

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


  if( __repetitiveInd ) {
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
  void MCSampling< GUM_SCALAR, BNInferenceEngine >::__insertEvidence ( BNInferenceEngine &inference_engine ) { /*const*/
    if ( this->_evidence.size() == 0 )
      return;

    int this_thread = omp_get_thread_num();
    gum::BayesNet<GUM_SCALAR> * working_bn = __workingSet[this_thread];

    gum::List< const gum::Potential< GUM_SCALAR > * > * evi_list = __workingSetE[this_thread];

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

    __workingSetE[this_thread] = evi_list;

    if ( evi_list->size() > 0 )
      inference_engine.insertEvidence ( *evi_list );

  }

} // namespace cn
