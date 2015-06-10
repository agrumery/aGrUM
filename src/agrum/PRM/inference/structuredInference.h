/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of StructuredInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_STRUCTURED_INFERENCE_H
#define GUM_STRUCTURED_INFERENCE_H

#include <string>

#include <agrum/core/timer.h>

#include <agrum/BN/inference/variableElimination.h>

#include <agrum/multidim/operators/multiDimCombinationDefault.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/multidim/multiDimSparse.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/operators/projections4MultiDim.h>

#include <agrum/graphs/triangulations/partialOrderedTriangulation.h>

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
    template <typename GUM_SCALAR>
    class StructuredInference : public PRMInference<GUM_SCALAR> {
      public:
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      StructuredInference(const PRM<GUM_SCALAR> &prm,
                          const System<GUM_SCALAR> &system,
                          gspan::SearchStrategy<GUM_SCALAR> *strategy = 0);

      /// Copy constructor.
      StructuredInference(const StructuredInference &source);

      /// Destructor.
      virtual ~StructuredInference();

      /// Copy operator.
      StructuredInference &operator=(const StructuredInference &source);

      /// @}
      // ========================================================================
      /// @name Getters and setters.
      // ========================================================================
      /// @{

      /// Tells this algorithm to use pattern mining or not.
      void setPatternMining(bool b);

      virtual std::string name() const;

      /// Returns the instance of gspan used to search patterns.
      GSpan<GUM_SCALAR> &gspan();

      /// Returns the instance of gspan used to search patterns.
      const GSpan<GUM_SCALAR> &gspan() const;

      /// Search for patterns without doing any computations.
      void searchPatterns();

      /// @}
      protected:
      // ========================================================================
      /// @name Protected members.
      // ========================================================================
      /// @{

      /// See PRMInference::_evidenceAdded().
      virtual void
      _evidenceAdded(const typename PRMInference<GUM_SCALAR>::Chain &chain);

      /// See PRMInference::_evidenceRemoved().
      virtual void
      _evidenceRemoved(const typename PRMInference<GUM_SCALAR>::Chain &chain);

      /// See PRMInference::_marginal().
      virtual void _marginal(const typename PRMInference<GUM_SCALAR>::Chain &chain,
                             Potential<GUM_SCALAR> &m);

      /// See PRMInference::_joint().
      virtual void
      _joint(const std::vector<typename PRMInference<GUM_SCALAR>::Chain> &queries,
             Potential<GUM_SCALAR> &j);

      /// @}
      private:
      /// Private structure to represent data about a reduced graph.
      struct RGData {
        /// The reduced graph.
        UndiGraph reducedGraph;
        /// Mapping between NodeId and modalities.
        NodeProperty<Size> mods;
        /// Mapping between DiscreteVariable and NodeId.
        Bijection<const DiscreteVariable *, NodeId> var2node;
        /// The pool of potentials matching the reduced graph
        Set<Potential<GUM_SCALAR> *> pool;
        /// Partial order used for triangulation, first is outputs nodes, second
        /// query nodes.
        List<NodeSet> partial_order;
        /// Default constructor
        RGData();
        /// Destructor.
        ~RGData();
        /// Returns the set of outputs nodes (which will be eliminated).
        inline NodeSet &outputs() { return partial_order[0]; }
        /// Returns the set of query nodes (which will not be eliminated).
        inline NodeSet &queries() { return partial_order[1]; }
      };

      /// Private structure to represent data about a pattern.
      struct PData {
        /// The pattern for which this represents data about it
        const gspan::Pattern &pattern;
        /// A reference over the usable matches of pattern
        typename GSpan<GUM_SCALAR>::MatchedInstances &matches;
        /// A yet to be triangulated undigraph
        UndiGraph graph;
        /// The pattern's variables modalities
        NodeProperty<Size> mod;
        /// A bijection to easily keep track  between graph and attributes, its of
        /// the
        /// form instance_name DOT attr_name
        Bijection<NodeId, std::string> node2attr;
        /// To ease translating potentials from one match to another
        NodeProperty<std::pair<Idx, std::string>> map;
        /// Bijection between graph's nodes and their corresponding DiscreteVariable,
        /// for
        /// inference purpose
        Bijection<NodeId, const DiscreteVariable *> vars;
        /// To handle barren nodes
        NodeProperty<Potential<GUM_SCALAR> *> pots;
        /// Set of barren nodes
        Set<NodeId> barren;
        /// Default constructor.
        PData(const gspan::Pattern &p,
              typename GSpan<GUM_SCALAR>::MatchedInstances &m);
        /// Copy constructor.
        PData(const PData &source);
        /// Destructor
        ~PData();
        /// Returns the set of inner nodes
        inline NodeSet &inners() { return __partial_order[0]; }
        /// Returns the set of inner and observed nodes given all the matches of
        /// pattern
        inline NodeSet &obs() { return __partial_order[1]; }
        /// Returns the set of outputs nodes given all the matches of pattern
        inline NodeSet &outputs() { return __partial_order[2]; }
        /// Returns the set of queried nodes given all the matches of pattern
        inline NodeSet &queries() { return __partial_order[3]; }
        // We use the first match for computations
        // inline const Sequence<Instance<GUM_SCALAR>*>& match() const { return
        // **(matches.begin());}
        // Remove any empty set in partial_order
        const List<NodeSet> *partial_order();

        private:
        /// We'll use a PartialOrderedTriangulation with three sets: output, nodes
        /// and obs
        /// with children outside the pattern and the other nodes
        List<NodeSet> __partial_order;
        /// A copy of __partial_order without empty sets
        List<NodeSet> *__real_order;
      };

      /// Private structure to represent data about a Class<GUM_SCALAR>.
      struct CData {
        /// The class about what this data is about.
        const Class<GUM_SCALAR> &c;
        /// The class moral graph. NodeId matches those in c.
        UndiGraph moral_graph;
        /// The class variables modalities.
        NodeProperty<Size> mods;
        /// The partial order used of variable elimination.
        List<NodeSet> partial_order;
        /// The Set of Instances reduces at class level.
        Set<const Instance<GUM_SCALAR> *> instances;
        /// The potential pool obtained by C elimination of inner nodes.
        Set<Potential<GUM_SCALAR> *> pool;
        /// Default constructor.
        CData(const Class<GUM_SCALAR> &c);
        /// Destructor.
        ~CData();
        /// Returns the set of inner nodes.
        inline NodeSet &inners() { return __inners; }
        /// Returns the set of aggregators and their parents.
        inline NodeSet &aggregators() { return __aggregators; }
        /// Returns the set of outputs nodes.
        inline NodeSet &outputs() { return __outputs; }
        /// The elimination order for nodes of this class
        inline std::vector<NodeId> &elim_order() { return __elim_order; }

        private:
        std::vector<NodeId> __elim_order;
        NodeSet __inners;
        NodeSet __aggregators;
        NodeSet __outputs;
        Set<Potential<GUM_SCALAR> *> __trash;
      };

      /// Pointer over th GSpan<GUM_SCALAR> instance used by this class.
      GSpan<GUM_SCALAR> *__gspan;

      /// Mapping between a Pattern's match and its potential pool after inner
      /// variables
      /// were eliminated.
      HashTable<const Sequence<Instance<GUM_SCALAR> *> *,
                Set<Potential<GUM_SCALAR> *> *> __elim_map;

      /// Mapping between a Class<GUM_SCALAR> and data about instances reduced using
      /// only Class<GUM_SCALAR> level
      /// information.
      HashTable<const Class<GUM_SCALAR> *, CData *> __cdata_map;

      /// Keeping track of create potentials to delete them after inference.
      Set<Potential<GUM_SCALAR> *> __trash;

      HashTable<const Class<GUM_SCALAR> *, std::vector<NodeId> *> __outputs;

      /// This keeps track of reduced instances.
      Set<const Instance<GUM_SCALAR> *> __reducedInstances;

      /// The query
      typename PRMInference<GUM_SCALAR>::Chain __query;

      /// The pattern data of the pattern which one of its matches contains
      /// the query
      PData *__pdata;

      /// Flag which tells to use pattern mining or not
      bool __mining;

      /// Flag with an explicit name
      bool __found_query;
      std::pair<Idx, std::string> __query_data;

      /// This calls __reducePattern() over each pattern and then build the reduced
      /// graph
      /// which is used for inference.
      /// The reduce graph is a triangulated instance graph.
      void __buildReduceGraph(RGData &data);

      /// Add the nodes in the reduced graph.
      void __addNodesInReducedGraph(RGData &data);

      /// Add edges in the reduced graph.
      void __addEdgesInReducedGraph(RGData &data);

      void __removeNode(StructuredInference::PData &data, NodeId id,
                        Set<Potential<GUM_SCALAR> *> &pool);

      /// Add the reduced potentials of instances not in any used patterns.
      void __reduceAloneInstances(RGData &data);

      /// Proceed with the elimination of all inner variables (observed or not) of
      /// all
      /// usable matches of Pattern p.
      /// Inner variables which are part of the query are not eliminated.
      void __reducePattern(const gspan::Pattern *p);

      /// Build the DAG corresponding to Pattern data.pattern, initialize pool with
      /// all the Potentials of all variables in data.pattern. The first match of
      /// data.pattern (aka data.match) is used.
      void __buildPatternGraph(PData &data, Set<Potential<GUM_SCALAR> *> &pool,
                               const Sequence<Instance<GUM_SCALAR> *> &match);

      void __insertNodeInElimLists(StructuredInference::PData &data,
                                   const Sequence<Instance<GUM_SCALAR> *> &match,
                                   Instance<GUM_SCALAR> *inst,
                                   Attribute<GUM_SCALAR> *attr, NodeId id,
                                   std::pair<Idx, std::string> &v);

      bool __allInstanceNoRefAttr(StructuredInference::PData &data,
                                  std::pair<Idx, std::string> attr);

      void __removeBarrenNodes(StructuredInference::PData &data,
                               Set<Potential<GUM_SCALAR> *> &pool);

      /// Add in data.queries() any queried variable in one of data.pattern matches.
      void __buildQuerySet(PData &data);

      /// Proceeds with the elimination of observed variables in math and then
      /// call __translatePotSet().
      Set<Potential<GUM_SCALAR> *> *
      __eliminateObservedNodes(StructuredInference::PData &data,
                               const Set<Potential<GUM_SCALAR> *> &pool,
                               const Sequence<Instance<GUM_SCALAR> *> &match,
                               const std::vector<NodeId> &elim_order);

      Set<Potential<GUM_SCALAR> *> *
      __eliminateObservedNodesInSource(StructuredInference::PData &data,
                                       const Set<Potential<GUM_SCALAR> *> &pool,
                                       const Sequence<Instance<GUM_SCALAR> *> &match,
                                       const std::vector<NodeId> &elim_order);

      /// Translate a given Potential Set into one w.r.t. variables in match.
      Set<Potential<GUM_SCALAR> *> *
      __translatePotSet(StructuredInference::PData &data,
                        const Set<Potential<GUM_SCALAR> *> &pool,
                        const Sequence<Instance<GUM_SCALAR> *> &match);

      /// Unreduce the match containing the query.
      void __unreduceMatchWithQuery();

      std::vector<NodeId> *__getClassOutputs(const Class<GUM_SCALAR> *c);
      /// Used to create strings
      std::string __dot;
      std::string __str(const Instance<GUM_SCALAR> *i,
                        const Attribute<GUM_SCALAR> *a) const;
      std::string __str(const Instance<GUM_SCALAR> *i,
                        const Attribute<GUM_SCALAR> &a) const;
      std::string __str(const Instance<GUM_SCALAR> *i,
                        const SlotChain<GUM_SCALAR> &a) const;

      public:
      // For bench/debug purpose.
      Timer timer;
      Timer plopTimer;
      double triang_time;
      double mining_time;
      double pattern_time;
      double inner_time;
      double obs_time;
      double full_time;
      std::string info() const;
    };

    extern template class StructuredInference<double>;
  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/structuredInference.tcc>

#endif /* GUM_STRUCTURED_INFERENCE_H */
