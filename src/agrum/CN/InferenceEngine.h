#ifndef __CN_INFERENCE_ENGINE__H__
#define __CN_INFERENCE_ENGINE__H__

namespace gum {

  template< typename GUM_SCALAR >
  class InferenceEngine {
    private:

    protected:
      const CredalNet< GUM_SCALAR > * _credalNet;

      typename gum::Property< std::vector< GUM_SCALAR > >::onNodes _marginalMin;
      typename gum::Property< std::vector< GUM_SCALAR > >::onNodes _marginalMax;

      typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes _marginalSets;

      typename gum::Property< GUM_SCALAR >::onNodes _expectationMin;
      typename gum::Property< GUM_SCALAR >::onNodes _expectationMax;

      // variables modalities (the real ones, not discretized 0 1 2 3 ...)
      typename std::map< std::string, std::vector< GUM_SCALAR > > _modal;

      typename gum::Property< std::vector< GUM_SCALAR > >::onNodes _evidence;
      typename gum::Property< std::vector< bool > >::onNodes _query;

      // sampling (dynamic network)
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t0;
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t1;

      int _timeSteps;

      void _repetitiveInit();

    public:
      InferenceEngine ( const CredalNet< GUM_SCALAR > & credalNet );
      ~InferenceEngine();

      virtual void makeInference() = 0;
      virtual void eraseAllEvidence();

      // initialize modalities to compute expectations
      // from file
      void insertModals( const std::string &path);
      // from map (copy)
      void insertModals( const std::map< std::string, std::vector< GUM_SCALAR > > &modals);

      void insertEvidence ( const std::string &path );
      // ? how the hell am i supposed to use a property without the network ??
      // use map (string instead of nodeId) ?
      void insertEvidence ( const typename gum::Property< std::vector< GUM_SCALAR > >::onNodes &evidence );

      void insertQuery ( const std::string &path );
      // why property ??? check old sources
      void insertQuery ( const typename gum::Property< std::vector< bool > >::onNodes &query );

      const std::vector< GUM_SCALAR > & marginalMin ( const gum::NodeId id ) const;
      const std::vector< GUM_SCALAR > & marginalMax ( const gum::NodeId id ) const;

      const GUM_SCALAR & expectationMin ( const gum::NodeId id ) const;
      const GUM_SCALAR & expectationMax ( const gum::NodeId id ) const;
      const std::vector< std::vector< GUM_SCALAR > > & vertices ( const gum::NodeId id ) const;
      
      void saveMarginals ( const std::string &path ) const;
      void saveExpectations ( const std::string &path ) const;
      void saveVertices ( const std::string &path ) const;

      std::string toString() const;

  }; // CNInferenceEngine

} // namespace cn

#include <agrum/CN/InferenceEngine.tcc>

#endif
