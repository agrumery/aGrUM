#include "LocalSearch.h"

namespace gum {
  
  template< typename GUM_SCALAR, class BNInferenceEngine >
  LocalSearch< GUM_SCALAR, BNInferenceEngine >::LocalSearch(const CredalNet< GUM_SCALAR > & credalNet) : InferenceEngine< GUM_SCALAR >::InferenceEngine(credalNet) {
    GUM_CONSTRUCTOR(LocalSearch);
    __repetitiveInd = false;
    __maxVertices = 1;
    __passN = 1;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  LocalSearch< GUM_SCALAR, BNInferenceEngine >::~LocalSearch() {
    GUM_DESTRUCTOR(LocalSearch);
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::setRepetitiveInd(const bool repetitive) {
    __repetitiveInd = repetitive;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::setMaxVertices(const int number) {
    __maxVertices = number;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::setPassN(const int number) {
    __passN = number;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::setAll(const bool repetitive, const int maxVertices, const int passN) {
    __repetitiveInd = repetitive;
    __maxVertices = maxVertices;
    __passN = passN;
  }


  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::eraseAllEvidence() {
    InferenceEngine< GUM_SCALAR >::InferenceEngine::eraseAllEvidence();
    __localOptMin.clear();
    __localOptMax.clear();
    __toDoAffec.clear();
    __nextToDoAffec.clear();
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::makeInference() {
    if(__repetitiveInd)
      try {
        this->_repetitiveInit();
      }
      catch(Errors & err) {
        std::cout << err.what() << std::endl;
        std::cout << "\nvoid LocalSearch< GUM_SCALAR, BNInferenceEngine >::makeInference() : Will use strong independence instead" << std::endl;
        __repetitiveInd = false;
      }

    __affec myAffec_init;
    const gum::DAG & dag = this->_credalNet->current_bn().dag();
    for(gum::DAG::NodeIterator iter = dag.beginNodes(); iter != dag.endNodes(); ++iter) {
      if(__repetitiveInd && ! (this->_t0.exists(*iter)) && !(this->_t1.exists(*iter)))
            continue;

      const gum::Potential<GUM_SCALAR> * potential(&this->_credalNet->current_bn().cpt(*iter));
      gum::Instantiation ins(*potential);
      int pconfs = ins.domainSize() / this->_credalNet->current_bn().variable(*iter).domainSize();
      std::vector< int > choosen_vertices(pconfs, 0);
      myAffec_init.insert(*iter, choosen_vertices);
    }

    std::vector< gum::NodeId > to_process;
    std::vector< gum::NodeId > processed;

    const gum::Sequence< gum::NodeId > order = this->_credalNet->current_bn().topologicalOrder();
    for(gum::Sequence<gum::NodeId>::iterator id = order.rbegin(); id != order.rend(); --id) {
      if(__repetitiveInd && (this->_t0.exists(*id) || this->_t1.exists(*id)) && this->_credalNet->getNodeType(*id) != this->_credalNet->PRECISE)
        to_process.push_back(*id);
      else if(! __repetitiveInd && this->_credalNet->getNodeType(*id) != this->_credalNet->PRECISE)
        to_process.push_back(*id);

      if((! this->_query.empty()) && (! this->_query.exists(*id)) )
        continue;

      int dSize = this->_credalNet->current_bn().variable(*id).domainSize();
      __localOptMin.insert(*id, typename std::vector< std::vector< __affec > >(dSize, typename std::vector< __affec >(1, myAffec_init)));
    	__localOptMax.insert(*id, typename std::vector< std::vector< __affec > >(dSize, typename std::vector< __affec >(1, myAffec_init)));
    }

    // threads marginals
    std::vector< typename gum::Property< std::vector< GUM_SCALAR > >::onNodes > l_marginalMin;
    std::vector< typename gum::Property< std::vector< GUM_SCALAR > >::onNodes > l_marginalMax;
    // threads optimal affecs
    std::vector< typename gum::Property< std::vector< std::vector< __affec > > >::onNodes > l_localOptMin;
    std::vector< typename gum::Property< std::vector< std::vector< __affec > > >::onNodes > l_localOptMax;

    #pragma omp parallel
    {
      int this_thread = omp_get_thread_num();
      int num_threads = omp_get_num_threads();

      gum::BayesNet< GUM_SCALAR > * thread_bn = new gum::BayesNet< GUM_SCALAR >();
      #pragma omp critical
      {
        *thread_bn = this->_credalNet->current_bn();
      }

      #pragma omp single
      {
        __workingSet.resize(num_threads, NULL);
        __engineSet.resize(num_threads, NULL);
        l_marginalMin.resize(num_threads);
        l_marginalMax.resize(num_threads);
        l_localOptMin.resize(num_threads);
        l_localOptMax.resize(num_threads);
      }

      __workingSet[this_thread] = thread_bn;
      __engineSet[this_thread] = new BNInferenceEngine(*(__workingSet[this_thread]));

      const gum::DAG& thread_dag = __workingSet[this_thread]->dag();
      for ( gum::DAG::NodeIterator id = thread_dag.beginNodes(); id != thread_dag.endNodes(); ++id )
      {
        int dSize = __workingSet[this_thread]->variable(*id).domainSize();
        l_marginalMin[this_thread].insert(*id, std::vector< GUM_SCALAR >(dSize, 1));
        l_marginalMax[this_thread].insert(*id, std::vector< GUM_SCALAR >(dSize, 0));

        if((! this->_query.empty()) && (! this->_query.exists(*id)) )
          continue;

        l_localOptMin[this_thread].insert(*id, typename std::vector< std::vector< __affec > >(dSize, typename std::vector< __affec >(1, myAffec_init)));
        l_localOptMax[this_thread].insert(*id, typename std::vector< std::vector< __affec > >(dSize, typename std::vector< __affec >(1, myAffec_init)));
      }
		
		  #pragma omp barrier

      #pragma omp master
      {
        __affec myAffec;
        for ( gum::DAG::NodeIterator id = dag.beginNodes(); id != dag.endNodes(); ++id )
        {
          if(__repetitiveInd && ! (this->_t0.exists(*id)) && !(this->_t1.exists(*id)))
            continue;

          const gum::Potential<GUM_SCALAR> * potential(&this->_credalNet->current_bn().cpt(*id));
          gum::Instantiation ins(*potential);
          int pconfs = ins.domainSize() / this->_credalNet->current_bn().variable(*id).domainSize();
          myAffec.insert(*id, std::vector< int >(pconfs, 0));

        }
        __toDoAffec.push_back(myAffec);
      }

    } // end of : parallel region

    int64_t inf_cpt = 0;
    int num_threads;
    int64_t timeElapsed;

    int passN = 1;

    Timer tps_inf;
    tps_inf.reset();

    //Chrono tps_inf;
    //tps_inf.start();

    while(! to_process.empty()) {
      __current = to_process.back();

      const gum::Potential<GUM_SCALAR> * potential(&this->_credalNet->current_bn().cpt(__current));
      gum::Instantiation ins(*potential);
      int pconfs = ins.domainSize() / this->_credalNet->current_bn().variable(__current).domainSize();

      __pconf = 0;

      std::cout << "\nProcessing variable : " << this->_credalNet->current_bn().variable(__current) << std::endl;

      while(__pconf < pconfs) {
        std::cout << "credal set : " << (__pconf+1) << " / " << pconfs << "\r";
        std::cout.flush();
        #pragma omp parallel for
        for(int saved_affec = 0; saved_affec < __toDoAffec.size(); saved_affec++) {
          for(int vertex = 0; vertex < this->_credalNet->credalNet_cpt()[__current][__pconf].size(); vertex++) {
            if(passN == 1 && vertex == 0 && inf_cpt > 0 )
              continue;
            
            __affec myTable;
            // threads do not like shared vectors and iterators !
            // use this copy now and further
            __affec thread_copy_toDoAffec = __toDoAffec[saved_affec];

            myTable.insert(__current, thread_copy_toDoAffec[__current]);
            myTable[__current][__pconf] = vertex;

            //double tdelta = tps_inf.getElapsedTime();
            
            __applyAffec(thread_copy_toDoAffec, false);
            __applyAffec(myTable, true, __pconf);

            //tdelta = tps_inf.getElapsedTime() - tdelta;
            //std::cout << "tps applyAffec " << tdelta << "\n";

            int this_thread = omp_get_thread_num();
            num_threads = omp_get_num_threads();

            __engineSet[this_thread]->eraseAllEvidence();
            __insertEvidence(*(__engineSet[this_thread]));
            __engineSet[this_thread]->makeInference();
            
            #pragma omp atomic
            inf_cpt++;

            const gum::DAG& thread_dag = __workingSet[this_thread]->dag();

            //tdelta = tps_inf.getElapsedTime();

            // each thread update its marginals
            /////// ATTENTION ///////
            // evidenceMarginal() is only available with LazyPropagation
            GUM_SCALAR pe = __engineSet[this_thread]->evidenceMarginal();
            if(pe > 0) {
              typename gum::Property< std::vector< bool > >::onNodes query = this->_query;
              for(gum::DAG::NodeIterator id = thread_dag.beginNodes(); id != thread_dag.endNodes(); ++id) {
                const gum::Potential< GUM_SCALAR > & potential(__engineSet[this_thread]->marginal(*id));
                gum::Instantiation ins(potential);
                int pos = 0;
                for(ins.setFirst(); !ins.end(); ++ins, pos++) {
                  bool isQuery = true;

                  if(! query.empty())
                    if(! query.exists(*id))
                      isQuery = false;
                    else if(! query[*id][pos])
                      isQuery = false;

                  // min
                  if(potential[ins] < l_marginalMin[this_thread][*id][pos]) {
                    l_marginalMin[this_thread][*id][pos] = potential[ins];

                    if(isQuery) {
                      thread_copy_toDoAffec.set(__current, myTable[__current]);
                      l_localOptMin[this_thread][*id][pos].clear();
                      l_localOptMin[this_thread][*id][pos] = std::vector< __affec >(1, thread_copy_toDoAffec);
                    }
                  }
                  else if(isQuery && potential[ins] == l_marginalMin[this_thread][*id][pos] && l_localOptMin[this_thread][*id][pos].size() < __maxVertices) {
                    thread_copy_toDoAffec.set(__current, myTable[__current]);
                    l_localOptMin[this_thread][*id][pos].push_back(thread_copy_toDoAffec);
                  }

                  // max
                  if(potential[ins] > l_marginalMax[this_thread][*id][pos]) {
                    l_marginalMax[this_thread][*id][pos] = potential[ins];
                    
                    if(isQuery) {
                      thread_copy_toDoAffec.set(__current, myTable[__current]);
                      l_localOptMax[this_thread][*id][pos].clear();
                      l_localOptMax[this_thread][*id][pos] = std::vector< __affec >(1, thread_copy_toDoAffec);
                    }
                  }
                  else if(isQuery && potential[ins] == l_marginalMax[this_thread][*id][pos] && l_localOptMax[this_thread][*id][pos].size() < __maxVertices) {
                    thread_copy_toDoAffec.set(__current, myTable[__current]);
                    l_localOptMax[this_thread][*id][pos].push_back(thread_copy_toDoAffec);
                  }

                }

              } // end of : for each node
            } // end of : if p(e) > 0
            
            //tdelta = tps_inf.getElapsedTime() - tdelta;
            //std::cout << "tps compare : " << tdelta << "\n";

          } // end of : for each vertex
        } // end of parallel region : for each saved_affec from __toDoAffec

        // information fusion
        #pragma omp parallel for
        for(int i = 0; i < __workingSet[0]->size(); i++) {
          typename gum::Property< std::vector< bool > >::onNodes query = this->_query;
          int dSize = l_marginalMin[0][i].size();
          for(int pos = 0; pos < dSize; pos++) {
            bool isQuery = true;

            if(! query.empty())
              if(! query.exists(i))
                isQuery = false;
              else if(! query[i][pos])
                isQuery = false;

            for(int thread_id = 0; thread_id < num_threads; thread_id++) {
              // min
              if(l_marginalMin[thread_id][i][pos] < this->_marginalMin[i][pos]) {
                this->_marginalMin[i][pos] = l_marginalMin[thread_id][i][pos];
                if(isQuery) {
                  __localOptMin[i][pos].clear();
                  __localOptMin[i][pos] = l_localOptMin[thread_id][i][pos];
                }
              }
              else if(l_marginalMin[thread_id][i][pos] == this->_marginalMin[i][pos]) {
                if(isQuery) {
                  int elem = 0;
                  while(elem < l_localOptMin[thread_id][i][pos].size() && __localOptMin[i][pos].size() < __maxVertices) {
                    __localOptMin[i][pos].push_back(l_localOptMin[thread_id][i][pos][elem]);
                    elem++;
                  }
                }
              }

              // max
              if(l_marginalMax[thread_id][i][pos] > this->_marginalMax[i][pos]) {
                this->_marginalMax[i][pos] = l_marginalMax[thread_id][i][pos];
                if(isQuery) {
                  __localOptMax[i][pos].clear();
                  __localOptMax[i][pos] = l_localOptMax[thread_id][i][pos];
                }
              }
              else if(l_marginalMax[thread_id][i][pos] == this->_marginalMax[i][pos]) {
                if(isQuery) {

                  int elem = 0;
                  while(elem < l_localOptMax[thread_id][i][pos].size() && __localOptMax[i][pos].size() < __maxVertices) {
                    __localOptMax[i][pos].push_back(l_localOptMax[thread_id][i][pos][elem]);
                    elem++;
                  }
                }
              }

            } // end of : for each thread_id

            // now update them all for this variable
            for(int thread_id = 0; thread_id < l_marginalMin.size(); thread_id++) {
              l_marginalMin[thread_id][i][pos] = this->_marginalMin[i][pos];
              l_marginalMax[thread_id][i][pos] = this->_marginalMax[i][pos];
            }

          }
        } // end of parallel region : for each node

        __nextToDoAffec.clear();

        for(int i = 0; i < __localOptMin.size(); i++) {
          if(!this->_query.empty() && ! this->_query.exists(i))
            continue;
          for(int j = 0; j < __localOptMin[i].size(); j++) {
            if(!this->_query.empty())
              if(! this->_query[i][j])
                continue;

            for(int k = 0; k < __localOptMin[i][j].size(); k++)
              __nextToDoAffec.push_back(__localOptMin[i][j][k]);
          }
        }

        __elimRedund(processed);

        std::vector< __affec > tmp = __toDoAffec;
        __nextToDoAffec.clear();

        for(int i = 0; i < __localOptMax.size(); i++) {
          if(!this->_query.empty() && ! this->_query.exists(i))
            continue;
          for(int j = 0; j < __localOptMax[i].size(); j++) {
            if(!this->_query.empty())
              if(! this->_query[i][j])
                continue;

              for(int k = 0; k < __localOptMax[i][j].size(); k++)
              __nextToDoAffec.push_back(__localOptMax[i][j][k]);
          }
        }

        __elimRedund(processed);

        __nextToDoAffec.clear();
        __nextToDoAffec.insert(__nextToDoAffec.end(), tmp.begin(), tmp.end());
        __nextToDoAffec.insert(__nextToDoAffec.end(), __toDoAffec.begin(), __toDoAffec.end());

        __elimRedund(processed);
        __nextToDoAffec.clear();

        __pconf++;
      } // end of : while conf_cpt < pconfs
  
      processed.push_back(__current);
      to_process.pop_back();

      if(passN > 1 && to_process.size() > 0) {
        for(std::vector< gum::NodeId >::iterator it = processed.begin(); it != processed.end(); ++it) {
          if(*it == to_process.back())
            processed.erase(it);
        }
      }
      else if(to_process.size() == 0 && passN < __passN) {
        passN++;
        std::cout << "\n\npass : " << passN << std::endl;
        to_process = processed;
        int pos_a, pos_b;
        gum::NodeId tmp;
        for(int i = 0; i < to_process.size() / 2; i ++) {
          pos_a = rand() % to_process.size();
          pos_b = rand() % to_process.size();
          tmp = to_process[pos_a];
          to_process[pos_a] = to_process[pos_b];
          to_process[pos_b] = tmp;
        }
        //processed.clear();
        for(std::vector< gum::NodeId >::iterator it = processed.begin(); it != processed.end(); ++it) {
          if(*it == to_process.back()) {
            processed.erase(it); // we can do it since we break
            break;
          }
        }
      }
            
    } // end of : while(! to_process.empty())

    std::cout << std::endl;

    timeElapsed = tps_inf.step();//tps_inf.getElapsedTime();
    //tps_inf.stop();
    tps_inf.pause();

    for(int i = 0; i < __workingSet.size(); i++) {
      delete __workingSet[i];
      delete __engineSet[i];
    }

    std::cout << this->toString();
    
    std::cout << "elapsed time : " << timeElapsed << " seconds" << std::endl;
        
    std::cout << std::endl << "# inferences : " << inf_cpt << std::endl;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  bool LocalSearch< GUM_SCALAR, BNInferenceEngine >::__affec_eq(const __affec & first, const __affec & second, const std::vector< gum::NodeId > & processed) const {
    for(int i = 0; i < processed.size(); i++)
      for(int j = 0; j < first[processed[i]].size(); j++)
        if(first[processed[i]][j] != second[processed[i]][j])
          return false;

    for(int j = 0; j < __pconf; j++)
      if(first[__current][j] != second[__current][j])
          return false;

    return true;
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::__elimRedund(const std::vector< gum::NodeId > & processed) {
    __toDoAffec.clear();

    std::vector< bool > eqls(__nextToDoAffec.size(), true);
    for(int i = 0; i < __nextToDoAffec.size(); i++) {
      if(!eqls[i])
        continue;

      #pragma omp parallel for
      for(int j = i + 1; j < __nextToDoAffec.size(); j++)
        if(eqls[j] && __affec_eq(__nextToDoAffec[i], __nextToDoAffec[j], processed))
          eqls[j] = false;
      
      __toDoAffec.push_back(__nextToDoAffec[i]);
    }
  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::__applyAffec(const __affec & affec, bool current, int pconf_pos) {
    int this_thread = omp_get_thread_num();
    gum::BayesNet< GUM_SCALAR > * working_bn = __workingSet[this_thread];

    const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes * cpt = &this->_credalNet->credalNet_cpt();

    for(__affec::const_iterator id = affec.begin(); id != affec.end(); ++id) {
      int dSize = working_bn->variable(id.key()).domainSize();
      gum::Potential< GUM_SCALAR > * potential(const_cast< gum::Potential< GUM_SCALAR > * > (&working_bn->cpt(id.key())));
      
      if(!current) {
        std::vector< GUM_SCALAR > new_cpt(potential->domainSize());
        for(int pconf = 0; pconf < (*cpt)[id.key()].size(); pconf++)
          for(int mod = 0; mod < dSize; mod++)
            new_cpt[pconf * dSize + mod] = (*cpt)[id.key()][pconf][(*id)[pconf]][mod];
        potential->fillWith(new_cpt);

        if(__repetitiveInd) {
          bool is_break = false;
          typename gum::Property< std::vector< gum::NodeId > >::onNodes t0;
          typename gum::Property< std::vector< gum::NodeId > >::onNodes t1;
          t0 = this->_t0;
          t1 = this->_t1;

          if(t0.exists(id.key())) {
            for(int pos = 0; pos < t0[id.key()].size(); pos++) {
              gum::Potential< GUM_SCALAR > * potential2(const_cast< gum::Potential< GUM_SCALAR > * > (&working_bn->cpt(t0[id.key()][pos])));
              potential2->fillWith(new_cpt);
            }
          }
          else if(t1.exists(id.key())) {
            for(int pos = 0; pos < t1[id.key()].size(); pos++) {
              gum::Potential< GUM_SCALAR > * potential2(const_cast< gum::Potential< GUM_SCALAR > * > (&working_bn->cpt(t1[id.key()][pos])));
              potential2->fillWith(new_cpt);
            }
          }
        }
      }
      else {
        // we loaded the saved cpts previously, now we modify the only vertex
        gum::Instantiation inst(*potential);
        inst.setFirst();
        for(int pos = 0; pos < pconf_pos; pos++)
          for(int mod = 0; mod < dSize; mod++)
            inst.inc();
        for(int mod = 0; mod < dSize; mod++) {
          potential->set(inst, (*cpt)[id.key()][pconf_pos][(*id)[pconf_pos]][mod]);
          inst.inc();
        }

        if(__repetitiveInd) {
          bool is_break = false;
          typename gum::Property< std::vector< gum::NodeId > >::onNodes t0;
          typename gum::Property< std::vector< gum::NodeId > >::onNodes t1;
          t0 = this->_t0;
          t1 = this->_t1;

          if(t0.exists(id.key())) {
            for(int pos = 0; pos < t0[id.key()].size(); pos++) {
              gum::Potential< GUM_SCALAR > * potential2(const_cast< gum::Potential< GUM_SCALAR > * > (&working_bn->cpt(t0[id.key()][pos])));
              gum::Instantiation ins(*potential2);
              ins.setFirst();
              for(int pos = 0; pos < pconf_pos; pos++)
                for(int mod = 0; mod < dSize; mod++)
                  ins.inc();
              for(int mod = 0; mod < dSize; mod++) {
                potential2->set(ins, (*cpt)[id.key()][pconf_pos][(*id)[pconf_pos]][mod]);
                ins.inc();
              }
            }
          }
          else if(t1.exists(id.key())) {
            for(int pos = 0; pos < t1[id.key()].size(); pos++) {
              gum::Potential< GUM_SCALAR > * potential2(const_cast< gum::Potential< GUM_SCALAR > * > (&working_bn->cpt(t1[id.key()][pos])));
              gum::Instantiation ins(*potential2);
              ins.setFirst();
              for(int pos = 0; pos < pconf_pos; pos++)
                for(int mod = 0; mod < dSize; mod++)
                  ins.inc();
              for(int mod = 0; mod < dSize; mod++) {
                potential2->set(ins, (*cpt)[id.key()][pconf_pos][(*id)[pconf_pos]][mod]);
                ins.inc();
              }
            }
          }

        } // end of : if __repetitiveInd
      } // end of : else (cpts previously loaded)
    }  // end of : for all nodes in affec

  }

  template< typename GUM_SCALAR, class BNInferenceEngine >
  void LocalSearch< GUM_SCALAR, BNInferenceEngine >::__insertEvidence(BNInferenceEngine & inference_engine) const {
    if(this->_evidence.size() == 0)
		  return;

    int this_thread = omp_get_thread_num();
	  gum::BayesNet<GUM_SCALAR> * working_bn = __workingSet[this_thread];
    gum::List< const gum::Potential< GUM_SCALAR > * > evi_list;

    typename gum::Property< std::vector< GUM_SCALAR > >::onNodes thread_evidence = this->_evidence;

    for(typename gum::Property< std::vector< GUM_SCALAR > >::onNodes::const_iterator it = thread_evidence.begin(); it != thread_evidence.end(); ++it) {
      gum::Potential< GUM_SCALAR > *p = new gum::Potential< GUM_SCALAR >;
		  (*p) << working_bn->variable(it.key());	
		  try {
        p->fillWith(*it);
      }
      catch(gum::Exception & err) {
        std::cout << "Error while inserting evidence : " << std::endl;
        GUM_SHOWERROR(err);
        throw(err);
      }
		  evi_list.insert(p);
    }

    if(evi_list.size() > 0)
		  inference_engine.insertEvidence(evi_list);
  }


} // namespace cn

