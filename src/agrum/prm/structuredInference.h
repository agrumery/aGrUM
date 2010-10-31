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
 * @author Lionel TORTI
 */
// ============================================================================
#include <string>
// ============================================================================
#include <agrum/BN/inference/variableElimination.h>
// ============================================================================
#include <agrum/multidim/potential.h>
// ============================================================================
#include <agrum/graphs/partialOrderedTriangulation.h>
// ============================================================================
#include <agrum/prm/PRM.h>
#include <agrum/prm/PRMInference.h>
#include <agrum/prm/gspan.h>
// ============================================================================
#ifndef GUM_STRUCTURED_INFERENCE_H
#define GUM_STRUCTURED_INFERENCE_H
// ============================================================================
namespace gum {
namespace prm {

/**
 * @class StructuredInference structuredInference.h <agrum/prm/structuredInference.h>
 *
 * This PRM inference algorithm exploits the GSpan algorithm to discover new
 * patters and exploit them in a structured way.
 */
class StructuredInference: public PRMInference {
  public:
  // ========================================================================
  /// @name Constructor & destructor.
  // ========================================================================
    /// @{

    /// Default constructor.
    StructuredInference(const PRM& prm, const System& system);

    /// Copy constructor.
    StructuredInference(const StructuredInference& source);

    /// Destructor.
    virtual ~StructuredInference();

    /// Copy operator.
    StructuredInference& operator=(const StructuredInference& source);

    /// @}

  protected:
  // ========================================================================
  /// @name Protected members.
  // ========================================================================
    /// @{

    /// See PRMInference::_evidenceAdded().
    virtual void _evidenceAdded(const Chain& chain);

    /// See PRMInference::_evidenceRemoved().
    virtual void _evidenceRemoved(const Chain& chain);

    /// See PRMInference::_marginal().
    virtual void _marginal(const Chain& chain, Potential<prm_float>& m);

    /// See PRMInference::_joint().
    virtual void _joint(const std::vector< Chain >& queries, Potential<prm_float>& j);

  private:

    /// Private structure to represent data about a reduced graph.
    struct RGData {
      /// The reduced graph.
      UndiGraph reducedGraph;
      /// Mapping between NodeId and modalities.
      Property<unsigned int>::onNodes mods;
      /// Mapping between DiscreteVariable and NodeId.
      Bijection<const DiscreteVariable*, NodeId> var2node;
      /// The pool of potentials matching the reduced graph
      Set<Potential<prm_float>*> pool;
      /// Partial order used for triangulation, first is outputs nodes, second
      /// query nodes.
      List< NodeSet > partial_order;
      /// Default constructor
      RGData();
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
      const GSpan::MatchedInstances& matches;
      /// A yet to be triangulated undigraph
      UndiGraph graph;
      /// The pattern's variables modalities
      Property<unsigned int>::onNodes mod;
      /// We'll use a PartialOrderedTriangulation with three sets: output, nodes and obs
      /// with children outside the pattern and the other nodes
      List<NodeSet> partial_order;
      /// A bijection to easily keep track  between graph and attributes, its of the
      /// form instance_name DOT attr_name
      Bijection<NodeId, std::string> node2attr;
      /// Bijection between graph's nodes and their corresponding DiscreteVariable, for
      /// inference purpose
      Bijection<NodeId, const DiscreteVariable*> vars;
      /// Default constructor.
      PData(const gspan::Pattern& p, const GSpan::MatchedInstances& m);
      /// Copy constructor.
      PData(const PData& source);
      /// Returns the set of inner nodes
      inline NodeSet& inners() { return partial_order[0]; }
      /// Returns the set of inner and observed nodes given all the matches of pattern
      inline NodeSet& obs() { return partial_order[1]; }
      /// Returns the set of outputs nodes given all the matches of pattern
      inline NodeSet& outputs() { return partial_order[3]; }
      /// Returns the set of queried nodes given all the matches of pattern
      inline NodeSet& queries() { return partial_order[4]; }
      // We use the first match for computations
      inline const Sequence<Instance*>& match() const { return **(matches.begin());}
    };

    /// Private structure to represent data about a Class.
    struct CData {
      /// The class about what this data is about.
      const Class& c;
      /// The class moral graph. NodeId matches those in c.
      UndiGraph moral_graph;
      /// The class variables modalities.
      Property<unsigned int>::onNodes mods;
      /// The partial order used of variable elimination.
      List<NodeSet> partial_order;
      /// The Set of Instances reduces at class level.
      Set<const Instance*> instances;
      /// The potential pool obtained by C elimination of inner nodes.
      Set<Potential<prm_float>*> pool;
      /// Default constructor.
      CData(const Class& c);
      /// Returns the set of inner nodes.
      inline NodeSet& inners() { return partial_order[0]; }
      /// Returns the set of aggregators and their parents.
      inline NodeSet& aggregators() { return partial_order[1]; }
      /// Returns the set of outputs nodes.
      inline NodeSet& outputs() { return partial_order[2]; }
      /// The elimination order for nodes of this class
      inline std::vector<NodeId>& elim_order() {
        if (not __elim_order) {
          PartialOrderedTriangulation t(&(moral_graph), &(mods), &(partial_order));
          __elim_order = new std::vector<NodeId>(t.eliminationOrder());
        }
        return *__elim_order;
      }
      private:
      std::vector<NodeId>* __elim_order;
    };

    /// Pointer over th GSpan instance used by this class.
    GSpan* __gspan;

    /// Mapping between a Pattern's match and its potential pool after inner variables
    /// were eliminated.
    HashTable<const Sequence<Instance*>*, Set<Potential<prm_float>*>*> __elim_map;

    /// Mapping between a Class and data about instances reduced using only Class level
    /// information.
    HashTable<const Class*, CData*> __cdata_map;

    /// Keeping track of create potentials to delete them after inference.
    Set<Potential<prm_float>*> __trash;

    HashTable< const Class*, std::vector<NodeId>* > __outputs;

    /// This keeps track of reduced instances.
    Set<const Instance*> __reducedInstances;

    /// The query
    PRMInference::Chain __query;

    /// The pattern data of the pattern which one of its matches contains the query
    PData* __pdata;

    /// This calls __reducePattern() over each pattern and then build the reduced graph
    /// which is used for inference.
    /// The reduce graph is a triangulated instance graph.
    void __buildReduceGraph(RGData& data);

    /// Add the nodes in the reduced graph.
    void __addNodesInReducedGraph(RGData& data);

    /// Add edges in the reduced graph.
    void __addEdgesInReducedGraph(RGData& data);

    /// Add the reduced potentials of instances not in any used patterns.
    void __reduceAloneInstances(RGData& data);

    /// Proceed with the elimination of all inner variables (observed or not) of all
    /// usable matches of Pattern p.
    /// Inner variables which are part of the query are not eliminated.
    void __reducePattern(const gspan::Pattern* p);

    /// Build the DAG corresponding to Pattern data.pattern, initialize pool with
    /// all the Potentials of all variables in data.pattern. The first match of
    /// data.pattern (aka data.match) is used.
    void __buildPatternGraph(PData& data, Set<Potential<prm_float>*>& pool);

    /// Add in data.obs() all observed variable (output or not).
    void __buildObsSet(PData& data);

    /// Add in data.queries() any queried variable in one of data.pattern matches.
    void __buildQuerySet(PData& data);

    /// Proceeds with the elimination of var in pool.
    void __eliminateNode(const DiscreteVariable* var,
                         Set<Potential<prm_float>*>& pool);

    /// Proceeds with the elimination of observed variables in math and then
    /// call __translatePotSet().
    Set<Potential<prm_float>*>*
    __eliminateObservedNodes(StructuredInference::PData& data,
                             const Set<Potential<prm_float>*>& pool,
                             const Sequence<Instance*>& match,
                             const std::vector<NodeId>& elim_order);

    /// Translate a given Potential Set into one w.r.t. variables in match.
    Set<Potential<prm_float>*>*
    __translatePotSet(Set<Potential<prm_float>*>& set,
                      const Sequence<Instance*>& source,
                      const Sequence<Instance*>& match);

    /// Unreduce the match containing the query.
    void __unreduceMatchWithQuery();

    std::vector<NodeId>* __getClassOutputs(const Class* c);
    /// Used to create strings
    std::string __dot;
    std::string __str(const Instance* i, const Attribute* a) const;
    std::string __str(const Instance* i, const Attribute& a) const;
    std::string __str(const Instance* i, const SlotChain& a) const;

};

} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/structuredInference.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_STRUCTURED_INFERENCE_H */
// ============================================================================
