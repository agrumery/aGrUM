#ifndef __CN_LOOPY_PROPAGATION__H__
#define __CN_LOOPY_PROPAGATION__H__

namespace gum {

  template< typename GUM_SCALAR >
  class LoopyPropagation : public InferenceEngine< GUM_SCALAR > {
    private:
      typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onArcs __arcsL;
      typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onArcs __arcsP;
      typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes __nodesL;
      typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes __nodesP;

      gum::NodeSet __activeNodes;
      gum::NodeSet __nextActiveNodes;
      gum::NodeSet __notifiedNodesOnP;
      gum::NodeSet __notifiedNodesOnL;

      void __init();
      void __msgP ( const gum::NodeId &id, const typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes &msgsParents, typename std::vector< std::vector< GUM_SCALAR > > & msgsP );

      std::map< int, std::vector< std::vector< int > > > __orders;

    protected:

    public:
      LoopyPropagation ( const CredalNet< GUM_SCALAR > & credalNet );
      ~LoopyPropagation();

      void makeInference();

      //void eraseAllEvidence();

  };

} // namespace cn

#include "LoopyPropagation.tcc"

#endif
