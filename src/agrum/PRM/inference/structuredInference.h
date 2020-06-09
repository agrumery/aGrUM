
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of StructuredInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_STRUCTURED_INFERENCE_H
#define GUM_STRUCTURED_INFERENCE_H

#include <string>

#include <agrum/tools/core/timer.h>

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/tools/multidim/implementations/multiDimSparse.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/tools/multidim/utils/operators/projections4MultiDim.h>

#include <agrum/tools/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/inference/PRMInference.h>
#include <agrum/PRM/inference/gspan.h>

namespace gum {
  namespace prm {

    /**
     * @class StructuredInference structuredInference.h
     *<agrum/PRM/structuredInference.h>
     *
     * This PRM<GUM_SCALAR> inference algorithm exploits the GSpan<GUM_SCALAR>
     *algorithm to discover new
     * patters and exploit them in a structured way.
     */
    template < typename GUM_SCALAR >
    class StructuredInference: public PRMInference< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      StructuredInference(const PRM< GUM_SCALAR >&             prm,
                          const PRMSystem< GUM_SCALAR >&       system,
                          gspan::SearchStrategy< GUM_SCALAR >* strategy = 0);

      /// Copy constructor.
      StructuredInference(const StructuredInference& source);

      /// Destructor.
      virtual ~StructuredInference();

      /// Copy operator.
      StructuredInference& operator=(const StructuredInference& source);

      /// @}
      // ========================================================================
      /// @name Getters and setters.
      // ========================================================================
      /// @{

      /// Tells this algorithm to use pattern mining or not.
      void setPatternMining(bool b);

      virtual std::string name() const;

      /// Returns the instance of gspan used to search patterns.
      GSpan< GUM_SCALAR >& gspan();

      /// Returns the instance of gspan used to search patterns.
      const GSpan< GUM_SCALAR >& gspan() const;

      /// Search for patterns without doing any computations.
      void searchPatterns();

      /// @}
      protected:
      // ========================================================================
      /// @name Protected members.
      // ========================================================================
      /// @{

      /// See PRMInference::evidenceAdded_().
      virtual void
         evidenceAdded_(const typename PRMInference< GUM_SCALAR >::Chain& chain);

      /// See PRMInference::evidenceRemoved_().
      virtual void
         evidenceRemoved_(const typename PRMInference< GUM_SCALAR >::Chain& chain);

      /// See PRMInference::posterior_().
      virtual void
         posterior_(const typename PRMInference< GUM_SCALAR >::Chain& chain,
                    Potential< GUM_SCALAR >&                          m);

      /// See PRMInference::joint_().
      virtual void joint_(
         const std::vector< typename PRMInference< GUM_SCALAR >::Chain >& queries,
         Potential< GUM_SCALAR >&                                         j);

      /// @}
      private:
      /// Private structure to represent data about a reduced graph.
      struct RGData {
        /// The reduced graph.
        UndiGraph reducedGraph;
        /// Mapping between NodeId and modalities.
        NodeProperty< Size > mods;
        /// Mapping between DiscreteVariable and NodeId.
        Bijection< const DiscreteVariable*, NodeId > var2node;
        /// The pool of potentials matching the reduced graph
        Set< Potential< GUM_SCALAR >* > pool;
        /// Partial order used for triangulation, first is outputs nodes, second
        /// query nodes.
        List< NodeSet > partial_order;
        /// Default constructor
        RGData();
        /// Destructor.
        ~RGData();
        /// Returns the set of outputs nodes (which will be eliminated).
        inline NodeSet& outputs() { return partial_order[0]; }
        /// Returns the set of query nodes (which will not be eliminated).
        inline NodeSet& queries() { return partial_order[1]; }
      };

      /// Private structure to represent data about a pattern.
      struct PData {
        /// The pattern for which this represents data about it
        const gspan::Pattern& pattern;
        /// A reference over the usable matches of pattern
        typename GSpan< GUM_SCALAR >::MatchedInstances& matches;
        /// A yet to be triangulated undigraph
        UndiGraph graph;
        /// The pattern's variables modalities
        NodeProperty< Size > mod;
        /// A bijection to easily keep track  between graph and attributes, its
        /// of
        /// the
        /// form instance_name DOT attr_name
        Bijection< NodeId, std::string > node2attr;
        /// To ease translating potentials from one match to another
        NodeProperty< std::pair< Idx, std::string > > map;
        /// Bijection between graph's nodes and their corresponding
        /// DiscreteVariable,
        /// for
        /// inference purpose
        Bijection< NodeId, const DiscreteVariable* > vars;
        /// To handle barren nodes
        NodeProperty< Potential< GUM_SCALAR >* > pots;
        /// Set of barren nodes
        Set< NodeId > barren;
        /// Default constructor.
        PData(const gspan::Pattern&                           p,
              typename GSpan< GUM_SCALAR >::MatchedInstances& m);
        /// Copy constructor.
        PData(const PData& source);
        /// Destructor
        ~PData();
        /// Returns the set of inner nodes
        inline NodeSet& inners() { return partial_order__[0]; }
        /// Returns the set of inner and observed nodes given all the matches of
        /// pattern
        inline NodeSet& obs() { return partial_order__[1]; }
        /// Returns the set of outputs nodes given all the matches of pattern
        inline NodeSet& outputs() { return partial_order__[2]; }
        /// Returns the set of queried nodes given all the matches of pattern
        inline NodeSet& queries() { return partial_order__[3]; }
        // We use the first match for computations
        // inline const Sequence<PRMInstance<GUM_SCALAR>*>& match() const {
        // return
        // **(matches.begin());}
        // Remove any empty set in partial_order
        const List< NodeSet >* partial_order();

        private:
        /// We'll use a PartialOrderedTriangulation with three sets: output,
        /// nodes
        /// and obs
        /// with children outside the pattern and the other nodes
        List< NodeSet > partial_order__;
        /// A copy of partial_order__ without empty sets
        List< NodeSet >* real_order__;
      };

      /// Private structure to represent data about a Class<GUM_SCALAR>.
      struct CData {
        /// The class about what this data is about.
        const PRMClass< GUM_SCALAR >& c;
        /// The class moral graph. NodeId matches those in c.
        UndiGraph moral_graph;
        /// The class variables modalities.
        NodeProperty< Size > mods;
        /// The partial order used of variable elimination.
        List< NodeSet > partial_order;
        /// The Set of Instances reduces at class level.
        Set< const PRMInstance< GUM_SCALAR >* > instances;
        /// The potential pool obtained by C elimination of inner nodes.
        Set< Potential< GUM_SCALAR >* > pool;
        /// Default constructor.
        CData(const PRMClass< GUM_SCALAR >& c);
        /// Destructor.
        ~CData();
        /// Returns the set of inner nodes.
        inline NodeSet& inners() { return inners__; }
        /// Returns the set of aggregators and their parents.
        inline NodeSet& aggregators() { return aggregators__; }
        /// Returns the set of outputs nodes.
        inline NodeSet& outputs() { return outputs__; }
        /// The elimination order for nodes of this class
        inline std::vector< NodeId >& elim_order() { return elim_order__; }

        private:
        std::vector< NodeId >           elim_order__;
        NodeSet                         inners__;
        NodeSet                         aggregators__;
        NodeSet                         outputs__;
        Set< Potential< GUM_SCALAR >* > trash__;
      };

      /// Pointer over th GSpan<GUM_SCALAR> instance used by this class.
      GSpan< GUM_SCALAR >* gspan__;

      /// Mapping between a Pattern's match and its potential pool after inner
      /// variables
      /// were eliminated.
      HashTable< const Sequence< PRMInstance< GUM_SCALAR >* >*,
                 Set< Potential< GUM_SCALAR >* >* >
         elim_map__;

      /// Mapping between a Class<GUM_SCALAR> and data about instances reduced
      /// using
      /// only Class<GUM_SCALAR> level
      /// information.
      HashTable< const PRMClass< GUM_SCALAR >*, CData* > cdata_map__;

      /// Keeping track of create potentials to delete them after inference.
      Set< Potential< GUM_SCALAR >* > trash__;

      HashTable< const PRMClass< GUM_SCALAR >*, std::vector< NodeId >* > outputs__;

      /// This keeps track of reduced instances.
      Set< const PRMInstance< GUM_SCALAR >* > reducedInstances__;

      /// The query
      typename PRMInference< GUM_SCALAR >::Chain query__;

      /// The pattern data of the pattern which one of its matches contains
      /// the query
      PData* pdata__;

      /// Flag which tells to use pattern mining or not
      bool mining__;

      /// Flag with an explicit name
      bool                          found_query__;
      std::pair< Idx, std::string > query_data__;

      /// This calls reducePattern__() over each pattern and then build the
      /// reduced
      /// graph
      /// which is used for inference.
      /// The reduce graph is a triangulated instance graph.
      void buildReduceGraph__(RGData& data);

      /// Add the nodes in the reduced graph.
      // MSVC void addNodesInReducedGraph__( RGData& data );

      /// Add edges in the reduced graph.
      void addEdgesInReducedGraph__(RGData& data);

      void removeNode__(typename StructuredInference::PData& data,
                        NodeId                               id,
                        Set< Potential< GUM_SCALAR >* >&     pool);

      /// Add the reduced potentials of instances not in any used patterns.
      void reduceAloneInstances__(RGData& data);

      /// Proceed with the elimination of all inner variables (observed or not)
      /// of
      /// all
      /// usable matches of Pattern p.
      /// Inner variables which are part of the query are not eliminated.
      void reducePattern__(const gspan::Pattern* p);

      /// Build the DAG corresponding to Pattern data.pattern, initialize pool
      /// with
      /// all the Potentials of all variables in data.pattern. The first match
      /// of
      /// data.pattern (aka data.match) is used.
      void
         buildPatternGraph__(PData&                                        data,
                             Set< Potential< GUM_SCALAR >* >&              pool,
                             const Sequence< PRMInstance< GUM_SCALAR >* >& match);

      void insertNodeInElimLists__(
         typename StructuredInference::PData&          data,
         const Sequence< PRMInstance< GUM_SCALAR >* >& match,
         PRMInstance< GUM_SCALAR >*                    inst,
         PRMAttribute< GUM_SCALAR >*                   attr,
         NodeId                                        id,
         std::pair< Idx, std::string >&                v);

      bool allInstanceNoRefAttr__(typename StructuredInference::PData& data,
                                  std::pair< Idx, std::string >        attr);

      void removeBarrenNodes__(typename StructuredInference::PData& data,
                               Set< Potential< GUM_SCALAR >* >&     pool);

      /// Add in data.queries() any queried variable in one of data.pattern
      /// matches.
      // MVSC void buildQuerySet__( PData& data );

      /// Proceeds with the elimination of observed variables in math and then
      /// call translatePotSet__().
      Set< Potential< GUM_SCALAR >* >* eliminateObservedNodes__(
         typename StructuredInference::PData&          data,
         const Set< Potential< GUM_SCALAR >* >&        pool,
         const Sequence< PRMInstance< GUM_SCALAR >* >& match,
         const std::vector< NodeId >&                  elim_order);

      Set< Potential< GUM_SCALAR >* >* eliminateObservedNodesInSource__(
         typename StructuredInference::PData&          data,
         const Set< Potential< GUM_SCALAR >* >&        pool,
         const Sequence< PRMInstance< GUM_SCALAR >* >& match,
         const std::vector< NodeId >&                  elim_order);

      /// Translate a given Potential Set into one w.r.t. variables in match.
      Set< Potential< GUM_SCALAR >* >*
         translatePotSet__(typename StructuredInference::PData&          data,
                           const Set< Potential< GUM_SCALAR >* >&        pool,
                           const Sequence< PRMInstance< GUM_SCALAR >* >& match);

      /// Unreduce the match containing the query.
      // MVSC void unreduceMatchWithQuery__();

      // MVSC std::vector<NodeId>* getClassOutputs__( const
      // PRMClass<GUM_SCALAR>* c );
      /// Used to create strings
      std::string dot__;
      std::string str__(const PRMInstance< GUM_SCALAR >*  i,
                        const PRMAttribute< GUM_SCALAR >* a) const;
      std::string str__(const PRMInstance< GUM_SCALAR >*  i,
                        const PRMAttribute< GUM_SCALAR >& a) const;
      std::string str__(const PRMInstance< GUM_SCALAR >*  i,
                        const PRMSlotChain< GUM_SCALAR >& a) const;

      public:
      // For bench/debug purpose.
      Timer       timer;
      Timer       plopTimer;
      double      triang_time;
      double      mining_time;
      double      pattern_time;
      double      inner_time;
      double      obs_time;
      double      full_time;
      std::string info() const;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class StructuredInference< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/structuredInference_tpl.h>

#endif /* GUM_STRUCTURED_INFERENCE_H */
