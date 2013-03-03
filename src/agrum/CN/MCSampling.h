#ifndef __MC_SAMPLING__H__
#define __MC_SAMPLING__H__

#include "InferenceEngine.h"
//#include "Chrono.h"
#include <omp.h>

namespace gum {

  template< typename GUM_SCALAR, class BNInferenceEngine >
  class MCSampling : public InferenceEngine< GUM_SCALAR > {
    private:
      bool __repetitiveInd;
      int __timeLimit;
      int __iterStop;
     
      bool __stopType; // false if done within __timeLimit

      typename std::vector< gum::BayesNet< GUM_SCALAR > * > __workingSet;

      typename gum::Property< std::vector < std::vector< GUM_SCALAR> > >::onNodes __marginalSets;

      std::vector< gum::NodeId > __varOrder;
      std::vector< std::vector< int > > __varInst;
      int __VERT;
      bool stopN = false;
      typename std::vector< std::vector < std::vector< GUM_SCALAR > > > __trajectories;

      void __verticesSampling();
      void __insertEvidence(BNInferenceEngine & inference_engine) /*const*/;

      // added (memory leak)
      //typename gum::List< const gum::Potential< GUM_SCALAR > * > __evi_list;
      typename std::vector< gum::List< const gum::Potential< GUM_SCALAR > * > * > __workingSetE;


    protected:

    public:
      MCSampling(const CredalNet< GUM_SCALAR > & credalNet);
      ~MCSampling();

      void makeInference();
      void eraseAllEvidence();
      
      const std::vector< std::vector< GUM_SCALAR > > & vertices(const gum::NodeId id) const;
      void saveVertices(const std::string & path) const;
      void saveExpectations(const std::string & path) const;

      bool getStopType() const;

      void setRepetitiveInd(const bool repetitive);
      void setTimeLimit(const int & time_limit);
      void setIterStop(const int & no_change_time_limit);
      void setAll(const bool repetitive, const int & time_limit, const int & no_change_time_limit);

      std::string toString() const;

      void memoryCheck() const;
  };

} // namespace cn

#include "MCSampling.tcc"

#endif
