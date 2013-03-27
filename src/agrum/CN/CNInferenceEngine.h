#ifndef __CN_INFERENCE_ENGINE__H__
#define __CN_INFERENCE_ENGINE__H__

/**
 * @file
 * @brief Abstract class representing CredalNet inference engines
 */

#include <agrum/BN/algorithms/approximationScheme.h>
#include <agrum/CN/OptBN.h>
#include "EmptyClass.h"

namespace gum {

/////////////////
  //DO NOT add brief or class block to ANY class template, otherwise doxygen fails to document all class templates. class block is put on forward template declaration, and briefs blocks are put for each template specialisation.

  //DO NOT add EmptyClass as default value in first template, otherwise doxygen will make many errors ( wrong inheritances, classes, ... )

  //any brief block on the template forward declaration will be appened at the end of the brief block of the template with 2 arguments since class name is the same.
///////////////

  /**
   * @class InferenceEngine InferenceEngine.h <agrum/CN/InferenceEngine.h>
   */
  template < typename GUM_SCALAR, class BNInferenceEngine = EmptyClass >
  class CNInferenceEngine;

  /**
   * @brief Class template representing a CredalNet inference engine. Used by credal network inference algorithms such as LoopyPropagation (inner multi-threading).
   *
   * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
   */
  template < typename GUM_SCALAR >
  class CNInferenceEngine < GUM_SCALAR/*, EmptyClass*/ > : public ApproximationScheme {
    private:

    protected:
      const CredalNet< GUM_SCALAR > * _credalNet;

      typedef typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes credalSet;
      typedef typename gum::Property< std::vector< GUM_SCALAR > >::onNodes margi;
      typedef typename gum::Property< GUM_SCALAR >::onNodes expe;
      
      margi _oldMarginalMin;
      margi _oldMarginalMax;

      margi _marginalMin;
      margi _marginalMax;

      credalSet _marginalSets;

      expe _expectationMin;
      expe _expectationMax;

      typename std::map< std::string, std::vector< GUM_SCALAR > > _dynamicExpMin;
      typename std::map< std::string, std::vector< GUM_SCALAR > > _dynamicExpMax;

      // variables modalities (the real ones, not discretized 0 1 2 3 ...)
      typename std::map< std::string, std::vector< GUM_SCALAR > > _modal;

      margi _evidence;
      typename gum::Property< std::vector< bool > >::onNodes _query;

      // sampling (dynamic network)
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t0;
      typename gum::Property< std::vector< gum::NodeId > >::onNodes _t1;

      int _timeSteps;

      void _repetitiveInit();
      void _dynamicExpectations();
      void _initExpectations();

      inline const GUM_SCALAR _computeEpsilon();

    public:
      CNInferenceEngine ();
      CNInferenceEngine ( const CredalNet< GUM_SCALAR > & credalNet );
      ~CNInferenceEngine();

      virtual void makeInference() = 0;
      virtual void eraseAllEvidence();

      // initialize modalities to compute expectations
      // from file
      void insertModalsFile( const std::string &path );
      // from map (copy)
      void insertModals( const std::map< std::string, std::vector< GUM_SCALAR > > &modals);

      void insertEvidenceFile ( const std::string & path );
      void insertEvidence ( const std::map< std::string, std::vector< GUM_SCALAR > > & eviMap );
      // ? how the hell am i supposed to use a property without the network ??
      // use map (string instead of nodeId) ?
      void insertEvidence ( const typename gum::Property< std::vector< GUM_SCALAR > >::onNodes &evidence );

      void insertQueryFile ( const std::string &path );
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

      const typename gum::Property< std::vector< gum::NodeId > >::onNodes & getT0Cluster() const;
      const typename gum::Property< std::vector< gum::NodeId > >::onNodes & getT1Cluster() const;

      OptBN<GUM_SCALAR> * getOptBN ();
  };

  /**
   * @brief Class template representing a CredalNet inference engine using one or more BayesNet inference engines such as LazyPropagation. Extends InferenceEngine< GUM_SCALAR >. Used for outer multi-threading such as MCSampling.
   *
   * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
   * @tparam BNInferenceEngine A BayesNet inference engine such as LazyPropagation.
   */
  template < typename GUM_SCALAR, class BNInferenceEngine >
  class CNInferenceEngine : public CNInferenceEngine < GUM_SCALAR > {
    protected :
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

      typename std::vector< bnet * > _workingSet;
      typename std::vector< gum::List< const gum::Potential< GUM_SCALAR > * > * > _workingSetE;
      
//////////////////////////////////////////////////////////////

      typename std::vector< BNInferenceEngine * > _l_inferenceEngine;

      ////// remember sampled dBNs //////
      std::vector< OptBN< GUM_SCALAR > * > _l_optimalNet;
      OptBN< GUM_SCALAR > _threadFusion; // we should use this OptBN if omp is disabled (avoid creating 2 objects when only one is necessary)
      // it should also avoid calling thread fusion operations

      void _initThreadsData( const unsigned int & num_threads, const bool __storeVertices, const bool __storeBNOpt ); // called once

      // inline stuff ( algorithms are easier to read with those )
      // true if sampled net store
      inline bool _updateThread( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex, const bool __storeVertices, const bool __storeBNOpt );

      inline void _updateMarginals();
      inline const GUM_SCALAR _computeEpsilon();
      void _updateOldMarginals(); // called once only

      void _optFusion(); // called once
      void _expFusion(); // called once
      void _verticesFusion(); // called ?? not done yet

    public : 
      CNInferenceEngine ( const CredalNet< GUM_SCALAR > & credalNet );
      ~CNInferenceEngine();

      OptBN< GUM_SCALAR > * getOptBN ();
  };

} // namespace gum

#include <agrum/CN/CNInferenceEngine.tcc>

#endif
