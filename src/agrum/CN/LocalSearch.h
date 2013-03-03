#ifndef __CN_LOCAL_SEARCH__H__
#define __CN_LOCAL_SEARCH__H__

#include <omp.h>

#include <agrum/CN/InferenceEngine.h>

namespace gum {

  template< typename GUM_SCALAR, class BNInferenceEngine >
  class LocalSearch : public InferenceEngine< GUM_SCALAR > {
    private:
      bool __repetitiveInd;
      int __maxVertices;
      int __passN;

      std::vector< gum::BayesNet< GUM_SCALAR > * > __workingSet;
      std::vector< BNInferenceEngine * > __engineSet;

      typedef typename gum::Property< std::vector< int > >::onNodes __affec;

      typename gum::Property< std::vector< std::vector< __affec > > >::onNodes __localOptMin;
      typename gum::Property< std::vector< std::vector< __affec > > >::onNodes __localOptMax;

      typename std::vector< __affec > __toDoAffec;
      typename std::vector< __affec > __nextToDoAffec;
      gum::NodeId __current;
      int __pconf;

      void __applyAffec ( const __affec &affec, bool current, int pconf_pos = -1 );
      void __insertEvidence ( BNInferenceEngine &inference_engine ) const;
      void __elimRedund ( const std::vector< gum::NodeId > & processed );
      bool __affec_eq ( const __affec &first, const __affec &second, const std::vector< gum::NodeId > & processed ) const;

    protected:

    public:
      LocalSearch ( const CredalNet< GUM_SCALAR > & credalNet );
      ~LocalSearch();

      void makeInference();
      void eraseAllEvidence();

      void setRepetitiveInd ( const bool repetitive );
      void setMaxVertices ( const int number );
      void setPassN ( const int number );
      void setAll ( const bool repetitive, const int maxVertices, const int passN );

  };

} // namespace cn

#include <agrum/CN/LocalSearch.tcc>

#endif
