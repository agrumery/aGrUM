#ifndef ___CN_MC_SAMPLING__H__
#define ___CN_MC_SAMPLING__H__

#include <omp.h>
#include <limits>
#include <agrum/CN/InferenceEngine.h>

namespace gum {

  template< typename GUM_SCALAR, class BNInferenceEngine >
  class MCSampling : public InferenceEngine< GUM_SCALAR > {
    private:
      bool __repetitiveInd;
      int __timeLimit;
      int __iterStop;

      bool __stopType; // false if done within __timeLimit
      bool __storeVertices;

      //typename std::vector< gum::BayesNet< GUM_SCALAR > * > __workingSet;
      //typename std::vector< gum::List< const gum::Potential< GUM_SCALAR > * > * > __workingSetE;

      std::vector< gum::NodeId > __varOrder;
      std::vector< std::vector< int > > __varInst;
      int __VERT;
      bool stopN;// = false;

      void __verticesSampling();
      void __insertEvidence ( BNInferenceEngine &inference_engine ) /*const*/;
 
    protected:
      
    public:
      MCSampling ( const CredalNet< GUM_SCALAR > & credalNet );
      virtual ~MCSampling();

      void makeInference();

      void makeInference_v2();
      //void eraseAllEvidence();

      bool getStopType() const;

      void setRepetitiveInd ( const bool repetitive );
      void setTimeLimit ( const int &time_limit );
      void setIterStop ( const int &no_change_time_limit );
      void setAll ( const bool repetitive, const int &time_limit, const int &no_change_time_limit );

      std::string toString() const;
  };

} // namespace cn

#include <agrum/CN/MCSampling.tcc>

#endif
