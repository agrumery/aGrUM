#ifndef ___CN_MC_SAMPLING__H__
#define ___CN_MC_SAMPLING__H__

#include <limits>
#include <agrum/CN/CNInferenceEngine.h>

namespace gum {

  template< typename GUM_SCALAR, class BNInferenceEngine >
  class MCSampling : public CNInferenceEngine< GUM_SCALAR, BNInferenceEngine > {
    private:
      bool __repetitiveInd;
      int __timeLimit;
      int __iterStop;

      bool __stopType; // false if done within __timeLimit
      bool __storeVertices;
      bool __storeBNOpt;

      //typename std::vector< BNInferenceEngine * > _l_inferenceEngine;

      std::vector< gum::NodeId > __varOrder;
      std::vector< std::vector< int > > __varInst;
      int __VERT;
      bool stopN;// = false;

      inline void __verticesSampling();
      // test another sampling method

      inline void __insertEvidence ( BNInferenceEngine &inference_engine ) /*const*/;
      void __mcInitApproximationScheme();
      void __mcThreadDataCopy();
      inline void __threadInference();
      inline void __threadUpdate(); 

      inline void __binaryRep ( std::vector< bool > & toFill,  const unsigned int value ) const;


    protected:
      
    public:
      MCSampling ( const CredalNet< GUM_SCALAR > & credalNet );
      virtual ~MCSampling();

      void makeInference();

      void eraseAllEvidence();

      bool getStopType() const;

      void setRepetitiveInd ( const bool repetitive );
      void setTimeLimit ( const int &time_limit );
      void setIterStop ( const int &no_change_time_limit );
      void setAll ( const bool repetitive, const int &time_limit, const int &no_change_time_limit );

      // this will call funcs in InferenceEngine and will be stored there (since more algorithms can do the same thing, but not the same way)
      void storeVertices ( const bool value );
      void storeBNOpt ( const bool value );

      //PH std::string toString() const;
      

      //// debug /////
      
      unsigned int notOptDelete; 
      
      ////////////////

  };

} // namespace cn

#include <agrum/CN/MCSampling.tcc>

#endif
