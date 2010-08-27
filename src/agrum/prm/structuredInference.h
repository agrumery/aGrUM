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

    /// Pointer over th GSpan instance used by this class.
    GSpan* __gspan;

    HashTable<const Sequence<Instance*>*, Set<Potential<prm_float>*>*> __elim_map;

    Set<Potential<prm_float>*> __trash;

    void __reducePattern(const gspan::Pattern* p);

    void __buildPatternGraph(UndiGraph& graph,
                             const Sequence<Instance*>& match,
                             NodeSet& outputs, NodeSet& inners,
                             Property<unsigned int>::onNodes& mod,
                             Bijection<NodeId, const DiscreteVariable*>& vars,
                             Bijection<NodeId, std::string>& node2attr,
                             Set<Potential<prm_float>*>& set);

    void __buildObsSet(const GSpan::MatchedInstances& matches,
                       const Sequence<Instance*>& match, NodeSet& obs,
                       Bijection<NodeId, std::string>& node2attr);

    void __eliminateNode(const DiscreteVariable* var,
                         Set<Potential<prm_float>*>& pool);

    Set<Potential<prm_float>*>*
    __eliminateObservedNodes(const Bijection<NodeId, const DiscreteVariable*>& vars,
                             const Set<Potential<prm_float>*>& pool,
                             const Sequence<Instance*>& source,
                             const Sequence<Instance*>& match,
                             const std::vector<NodeId>& elim_order,
                             Size start, Size end);

    Set<Potential<prm_float>*>*
    __translatePotSet(Set<Potential<prm_float>*>& set,
                      const Sequence<Instance*>& source,
                      const Sequence<Instance*>& match);

};

} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/structuredInference.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_STRUCTURED_INFERENCE_H */
// ============================================================================
