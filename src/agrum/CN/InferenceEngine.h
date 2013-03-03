#ifndef __CN_INFERENCE_ENGINE__H__
#define __CN_INFERENCE_ENGINE__H__

namespace gum {

  template< typename GUM_SCALAR >
  class InferenceEngine {
    private:
      
    protected:
      const CredalNet< GUM_SCALAR > * _credalNet;

      typename gum::Property< std::vector< GUM_SCALAR> >::onNodes _marginalMin;
      typename gum::Property< std::vector< GUM_SCALAR> >::onNodes _marginalMax;

      typename gum::Property< std::vector< GUM_SCALAR > >::onNodes _evidence;
      typename gum::Property< std::vector< bool > >::onNodes _query;

      // sampling
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t0;
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t1;
      void _repetitiveInit();

    public:
      InferenceEngine(const CredalNet< GUM_SCALAR > & credalNet);
      ~InferenceEngine();

      virtual void makeInference() = 0;
      virtual void eraseAllEvidence();

      void insertEvidence(const std::string & path);
      void insertEvidence(const typename gum::Property< std::vector< GUM_SCALAR > >::onNodes & evidence);

      void insertQuery(const std::string & path);
      void insertQuery(const typename gum::Property< std::vector< bool > >::onNodes & query);

      const std::vector< GUM_SCALAR > & marginalMin(const gum::NodeId id) const;
      const std::vector< GUM_SCALAR > & marginalMax(const gum::NodeId id) const;

      void saveMarginals(const std::string & path) const;

      std::string toString() const;
      
  }; // CNInferenceEngine

} // namespace cn

#include "InferenceEngine.tcc"

#endif
