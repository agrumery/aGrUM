#ifndef __CN_INFERENCE_ENGINE__H__
#define __CN_INFERENCE_ENGINE__H__

#include <agrum/BN/algorithms/approximationScheme.h>

namespace gum {

  template< typename GUM_SCALAR >
  class InferenceEngine : public ApproximationScheme {
    private:

    protected:
      const CredalNet< GUM_SCALAR > * _credalNet;

////////////////////////// from MCSampling /////////////////
      typedef typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes credalSet;
      typedef typename gum::Property< std::vector< GUM_SCALAR > >::onNodes margi;
      typedef typename gum::Property< GUM_SCALAR >::onNodes expe;

      typedef gum::BayesNet< GUM_SCALAR > bnet;
      typedef std::vector< margi > margis;
      typedef std::vector< expe > expes;
      typedef std::vector< credalSet > credalSets;

      typedef typename std::vector< std::map< std::string, std::vector< GUM_SCALAR > > > modals;
      
      margis _l_marginalMin;
      margis _l_marginalMax;
      expes _l_expectationMin;
      expes _l_expectationMax;
      modals _l_modal;
      credalSets _l_marginalSets;

      margi _oldMarginalMin;
      margi _oldMarginalMax;

      typename std::vector< bnet * > _workingSet;
      typename std::vector< gum::List< const gum::Potential< GUM_SCALAR > * > * > _workingSetE;

//////////////////////////////////////////////////////////////

      typename gum::Property< std::vector< GUM_SCALAR > >::onNodes _marginalMin;
      typename gum::Property< std::vector< GUM_SCALAR > >::onNodes _marginalMax;

      typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes _marginalSets;

      typename gum::Property< GUM_SCALAR >::onNodes _expectationMin;
      typename gum::Property< GUM_SCALAR >::onNodes _expectationMax;
      typename std::map< std::string, std::vector< GUM_SCALAR > > _dynamicExpMin;
      typename std::map< std::string, std::vector< GUM_SCALAR > > _dynamicExpMax;

      // variables modalities (the real ones, not discretized 0 1 2 3 ...)
      typename std::map< std::string, std::vector< GUM_SCALAR > > _modal;

      typename gum::Property< std::vector< GUM_SCALAR > >::onNodes _evidence;
      typename gum::Property< std::vector< bool > >::onNodes _query;

      // sampling (dynamic network)
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t0;
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t1;

      int _timeSteps;

      void _repetitiveInit();
      void _dynamicExpectations();
      void _initExpectations();

      void _initThreadsData( const unsigned int & num_threads, const bool __storeVertices );

      // inline stuff ( algorithms are easier to read with those )
      inline void _updateThread( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex, const bool __storeVertices );

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

      void insertEvidence ( const std::string & path );
      void insertEvidence ( const std::map< std::string, std::vector< GUM_SCALAR > > & eviMap );
      // ? how the hell am i supposed to use a property without the network ??
      // use map (string instead of nodeId) ?
      void insertEvidence ( const typename gum::Property< std::vector< GUM_SCALAR > >::onNodes &evidence );

      void insertQuery ( const std::string &path );
      // why property ??? check old sources
      void insertQuery ( const typename gum::Property< std::vector< bool > >::onNodes &query );

      const std::vector< GUM_SCALAR > & marginalMin ( const gum::NodeId id ) const;
      const std::vector< GUM_SCALAR > & marginalMax ( const gum::NodeId id ) const;
      const std::vector< GUM_SCALAR > & marginalMin( const std::string & varName ) const;
      const std::vector< GUM_SCALAR > & marginalMax( const std::string & varName ) const;

      const GUM_SCALAR & expectationMin ( const gum::NodeId id ) const;
      const GUM_SCALAR & expectationMax ( const gum::NodeId id ) const;
      const GUM_SCALAR & expectationMin ( const std::string & varName ) const;
      const GUM_SCALAR & expectationMax ( const std::string & varName ) const;

      const std::vector< GUM_SCALAR > & dynamicExpMin ( const std::string & varName ) const;
      const std::vector< GUM_SCALAR > & dynamicExpMax ( const std::string & varName ) const;

      const std::vector< std::vector< GUM_SCALAR > > & vertices ( const gum::NodeId id ) const;
      
      void saveMarginals ( const std::string &path ) const;
      void saveExpectations ( const std::string &path ) const;
      void saveVertices ( const std::string &path ) const;
      void dynamicExpectations(); // if someone forgets the protected call at the end of its own algorithm, the user can call it

      std::string toString() const;

  }; // CNInferenceEngine

} // namespace cn

#include <agrum/CN/InferenceEngine.tcc>

#endif
