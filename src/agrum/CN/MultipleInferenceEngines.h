#ifndef __MULTIPLE_INFERENCE_ENGINES__H__
#define __MULTIPLE_INFERENCE_ENGINES__H__

/**
 * @file
 * @brief Abstract class representing CredalNet inference engines
 */

/// @todo virtual for all functions that MAY be one day redefined in any derived class

#include <agrum/CN/InferenceEngine.h>

namespace gum {
namespace credal {

  /**
   * @class MultipleInferenceEngines MultipleInferenceEngines.h <agrum/CN/MultipleInferenceEngines.h>
   * @brief Class template representing a CredalNet inference engine using one or more BayesNet inference engines such as LazyPropagation. Extends InferenceEngine< GUM_SCALAR >. Used for outer multi-threading such as CNMonteCarloSampling.
   *
   * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
   * @tparam BNInferenceEngine A BayesNet inference engine such as LazyPropagation.
   */
  template < typename GUM_SCALAR, class BNInferenceEngine >
  class MultipleInferenceEngines : public InferenceEngine < GUM_SCALAR > {
    private :
      /** To easily access InferenceEngine< GUM_SCALAR > methods. */
      typedef InferenceEngine<GUM_SCALAR> infE;

      typedef typename gum::Property< std::vector< gum::NodeId > >::onNodes cluster;
      typedef typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes credalSet;
      typedef typename gum::Property< std::vector< GUM_SCALAR > >::onNodes margi;
      typedef typename gum::Property< GUM_SCALAR >::onNodes expe;

      typedef gum::BayesNet< GUM_SCALAR > bnet;
      typedef std::vector< margi > margis;
      typedef std::vector< expe > expes;
      typedef std::vector< credalSet > credalSets;
      typedef std::vector< std::vector< cluster > > clusters;

      //typedef typename std::vector< std::map< std::string, std::vector< GUM_SCALAR > > > modals;
      typedef typename std::vector< gum::HashTable< std::string, std::vector< GUM_SCALAR > > > modals;
			
			/**
			 * @brief Ask for redundancy elimination of a node credal set of a calling thread.
			 * 
			 * Called by _updateThread if vertices are stored.
			 * 
			 * @param id A constant reference to the node id whose credal set is to be checked for redundancy.
			 * @param vertex The vertex to add to the credal set.
			 * @param elimRedund \c true if redundancy elimination is to be performed, \c false otherwise and by default.
			 */
			inline void __updateThreadCredalSets( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex, const bool & elimRedund );

    protected :
      /** Threads lower marginals, one per thread. */
      margis _l_marginalMin;
      /** Threads upper marginals, one per thread. */
      margis _l_marginalMax;
      /** Threads lower expectations, one per thread. */
      expes _l_expectationMin;
      /** Threads upper expectations, one per thread. */
      expes _l_expectationMax;
      /** Threads modalities. */
      modals _l_modal;
      /** Threads vertices. */
      credalSets _l_marginalSets;
      /** Threads evidence. */
      margis _l_evidence;
      /** Threads clusters. */
      clusters _l_clusters;

      /** Threads BayesNet. */
      typename std::vector< bnet * > _workingSet;
      /** Threads evidence. */
      typename std::vector< gum::List< const gum::Potential< GUM_SCALAR > * > * > _workingSetE;
      
      /** Threads BNInferenceEngine. */
      typename std::vector< BNInferenceEngine * > _l_inferenceEngine;
      /** Threads optimal BayesNet. */
      std::vector< VarMod2BNsMap< GUM_SCALAR > * > _l_optimalNet;
      /** Fusion of threads optimal BayesNet. */
      //OptBN< GUM_SCALAR > _threadFusion; // we should use this OptBN if omp is disabled (avoid creating 2 objects when only one is necessary)
      // it should also avoid calling thread fusion operations

////////////////////////////////////////// 
      /// @name Protected initialization methods
//////////////////////////////////////////
      /// @{
     
      /**
       * @brief Initialize threads data.
			 * 
       * @param num_threads The number of threads.
       * @param __storeVertices \c True if vertices should be stored, \c False otherwise.
       * @param __storeBNOpt \c True if optimal BayesNet should be stored, \c false otherwise.
       */
      void _initThreadsData( const unsigned int & num_threads, const bool __storeVertices, const bool __storeBNOpt );

      /// @}

////////////////////////////////////////// 
      /// @name Protected algorithms methods
//////////////////////////////////////////
      /// @{

      /**
       * @brief Update thread information (marginals, expectations, BayesNet, vertices) for a given node id.
			 * 
       * @param id The id of the node to be updated.
       * @param vertex The vertex.
			 * @param elimRedund \c true if redundancy elimination is to be performed, \c false otherwise and by default.
       * @return \c True if the BayesNet is kept (for now), \c False otherwise.
       */
      inline bool _updateThread( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex, const bool & elimRedund = false );

      /**
       * @brief Fusion of threads marginals.
       */
      inline void _updateMarginals();
      
      /**
       * @brief Compute epsilon and update old marginals.
			 * 
       * @return Epsilon.
       */
      inline const GUM_SCALAR _computeEpsilon();
      
      /**
       * Update old marginals (from current marginals). Call this once to initialize old marginals (after burn-in for example) and then use _computeEpsilon which does the same job but compute epsilon too.
       */
      void _updateOldMarginals();

      /// @}

////////////////////////////////////////// 
      /// @name Proptected post-inference methods
//////////////////////////////////////////
      /// @{

      /** Fusion of threads optimal BayesNet. */
      void _optFusion();
      /** Fusion of threads expectations. */
      void _expFusion();
      /** @deprecated Fusion of threads vertices. */
      void _verticesFusion(); // called ?? not done yet

      /// @}

    public : 
//////////////////////////////////////////
      /// @name Constructors / Destructors
//////////////////////////////////////////
      /// @{

      /**
       * Constructor.
       * @param credalNet The CredalNet to be used.
       */
      MultipleInferenceEngines ( const CredalNet< GUM_SCALAR > & credalNet );
      /** Destructor. */
      virtual ~MultipleInferenceEngines();

      /// @}
      
////////////////////////////////////////// 
      /// @name Post-inference methods
//////////////////////////////////////////
      /// @{
      /**
       * Erase all inference related data to perform another one. You need to insert evidence again if needed but modalities are kept. You can insert new ones by using the appropriate method which will delete the old ones. 
       */
      virtual void eraseAllEvidence();
      /// @}

//////////////////////////////////////////
      /// @name Pure virtual methods
//////////////////////////////////////////
      /// @{
      /** To be redefined by each credal net algorithm. Starts the inference. */
      virtual void makeInference() = 0;
      /// @}

////////////////////////////////////////// 
      /// @name Getters and setters
//////////////////////////////////////////
      /// @{
      /**
       * Get optimum BayesNet.
       * @return A pointer to the optimal net object.
       */
      //OptBN< GUM_SCALAR > * getOptBN ();

      /// @}
  };

} // end of namespace
}

#include <agrum/CN/MultipleInferenceEngines.tcc>

#endif

