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

    /// Private structure to reprensent data about a pattern.
    struct PData {
      /// The pattern for which this represents data about it
      const gspan::Pattern& pattern;
      /// A reference over the usable matches of pattern
      const GSpan::MatchedInstances& matches;
      /// A yet to be triangulated undigraph
      UndiGraph graph;
      /// The undigraph node's modalities
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

    /// Pointer over th GSpan instance used by this class.
    GSpan* __gspan;

    HashTable<const Sequence<Instance*>*, Set<Potential<prm_float>*>*> __elim_map;

    Set<Potential<prm_float>*> __trash;

    HashTable< const gspan::Pattern*, std::vector<Chain>* > __q;

    void __reducePattern(const gspan::Pattern* p);

    void __buildPatternGraph(PData& data, Set<Potential<prm_float>*>& pool);

    void __buildObsSet(PData& data);

    void __eliminateNode(const DiscreteVariable* var,
                         Set<Potential<prm_float>*>& pool);

    Set<Potential<prm_float>*>*
    __eliminateObservedNodes(StructuredInference::PData& data,
                             const Set<Potential<prm_float>*>& pool,
                             const Sequence<Instance*>& match,
                             const std::vector<NodeId>& elim_order);

    Set<Potential<prm_float>*>*
    __translatePotSet(Set<Potential<prm_float>*>& set,
                      const Sequence<Instance*>& source,
                      const Sequence<Instance*>& match);

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
