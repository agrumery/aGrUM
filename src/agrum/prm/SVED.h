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
 * @brief Headers of SVED (Structured Value Elimination with d-seperation).
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <vector>
// ============================================================================
#include <agrum/core/set.h>
// ============================================================================
#include <agrum/graphs/defaultTriangulation.h>
#include <agrum/graphs/partialOrderedTriangulation.h>
// ============================================================================
#include <agrum/BN/inference/variableElimination.h>
// ============================================================================
#include <agrum/multidim/multiDimBucket.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/multiDimArray.h>
// ============================================================================
#include <agrum/prm/PRMInference.h>
#include <agrum/prm/structuredBayesBall.h>
#include <agrum/prm/classBayesNet.h>
#include <agrum/prm/instanceBayesNet.h>
#include <agrum/prm/CDG.h>
// ============================================================================
#ifndef GUM_SVED_H
#define GUM_SVED_H
namespace gum {
namespace prm {

/**
 * @class SVED SVED.h <agrum/prm/SVED.h>
 * @brief This class is an implementation of the Structured Value Elimination
 *        algorithm on PRM.
 *
 */
class SVED: public PRMInference{
  public:
  // ========================================================================
  /// @name Constructors & destructor.
  // ========================================================================
    /// @{

    /// Default Constructor.
    SVED(const PRM& prm, const System& model);

    /// Default Constructor.
    SVED(const PRM& prm, const std::string& model);

    /// Destructor.
    ~SVED();

    /// @}
  // ========================================================================
  /// @name Getters & setters.
  // ========================================================================
    /// @{

    /// Returns the name of the current inference algorithm
    virtual std::string name() const;

    /// @}
  protected:
  // ========================================================================
  /// @name Query methods.
  // ========================================================================
    /// @{

    /// Code alias.
    typedef PRMInference::Chain Chain;

    /// See PRMInference::_evidenceAdded().
    virtual void _evidenceAdded(const Chain& chain);

    /// See PRMInference::_evidenceRemoved().
    virtual void _evidenceRemoved(const Chain& chain);

    /// See PRMInference::_marginal().
    virtual void _marginal(const Chain& chain, Potential<prm_float>& m);

    /// See PRMInference::_joint().
    virtual void _joint(const std::vector< Chain >& queries, Potential<prm_float>& j);

    /// @}
  private:
    /// Code alias
    typedef Set< Potential<prm_float>* > BucketSet;
    /// Code alias
    typedef Set< Potential<prm_float>* >::iterator BucketSetIterator;

    /// Code alias
    typedef Set< MultiDimArray<prm_float>* >::iterator ArraySetIterator;

    HashTable<const Class*, std::vector<NodeId>*> __elim_orders;

    /// The Set<NodeId> returned by StructuredBayesBall is unique for each
    /// family of instances with the same requisite set (thus the same lifted
    /// potentials).
    HashTable<const Set<NodeId>*, BucketSet*> __lifted_pools;

    Sequence<const ClassElementContainer*>* __class_elim_order;

    StructuredBayesBall __bb;

    /// First pair  -> requisite Attributes
    /// Second pair -> requisite SlotChains
    HashTable< const Set<NodeId>*, std::pair<Set<NodeId>*, Set<NodeId>*> > __req_set;

    // ============================================================================
    /// @name Inference sub methods.
    // ============================================================================
    /// @{

    void __eliminateNodes(const Instance* query, NodeId id,
                          BucketSet& pool, BucketSet& trash);

    void __eliminateNodesDownward(const Instance* from, const Instance* i,
                                  BucketSet& pool, BucketSet& trash,
                                  List<const Instance*>& elim_list,
                                  Set<const Instance*>& ignore);

    void __eliminateNodesUpward(const Instance* i,
                                BucketSet& pool, BucketSet& trash,
                                List<const Instance*>& elim_list,
                                Set<const Instance*>& ignore);

    void __eliminateNodesWithEvidence(const Instance* i,
                                     BucketSet& pool, BucketSet& trash);

    void __insertLiftedNodes(const Instance* i, BucketSet& pool,
                             BucketSet& trash);

    /// Returns true if second can be eliminated before first.
    bool __checkElimOrder(const Instance* first, const Instance* second);

    void __initElimOrder();

    void __insertEvidence(const Instance* i, BucketSet& pool);

    std::vector<NodeId>& __getElimOrder(const Class& c);

    Potential<prm_float>* __getAggPotential(const Instance* i,
                                        const Aggregate* agg);

    void __initLiftedNodes(const Instance* i, BucketSet& trash);

    void __initReqSets(const Instance* i);

    Set<NodeId>& __getAttrSet(const Instance* i);

    Set<NodeId>& __getSCSet(const Instance* i);

    void __reduceElimList(const Instance* i, List<const Instance*>& elim_list,
                          List<const Instance*>& reduced_list,
                          Set<const Instance*>& ignore,
                          BucketSet& pool, BucketSet& trash);
    /// @}
};

} /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/SVED.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_SVED_H */
// ============================================================================
