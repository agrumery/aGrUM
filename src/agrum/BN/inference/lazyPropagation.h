/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
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
 * @brief Implementation of lazy propagation for inference in Bayesian Networks.
 */
#ifndef GUM_LAZY_PROPAGATION_H
#define GUM_LAZY_PROPAGATION_H

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/graphs/defaultTriangulation.h>

namespace gum {
  /**
   * @class LazyPropagation lazyPropagation.h <agrum/BN/inference/lazyPropagation.h>
   * @brief Implementation of lazy propagation for inference in Bayesian Networks
   * @ingroup bn_group
   */
  template<typename T_DATA>
  class LazyPropagation: public BayesNetInference<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// default constructor
    // ============================================================================
    LazyPropagation( const AbstractBayesNet<T_DATA>& BN );

    // ============================================================================
    /// constructor with a given elimination sequence
    // ============================================================================
    LazyPropagation( const AbstractBayesNet<T_DATA>& BN,
                     const std::vector<NodeId>& elim_order );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~LazyPropagation();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    // ============================================================================
    /// insert new evidence in the graph
    /** @warning if an evidence already exists w.r.t. a given node and a new
     * evidence w.r.t. this node is inserted, the old evidence is removed. */
    // ============================================================================
    virtual void insertEvidence( const List<const Potential<T_DATA>*>& );

    // ============================================================================
    /// remove all evidence from the graph
    // ============================================================================
    virtual void eraseAllEvidence();

    // ============================================================================
    /// remove a given evidence from the graph
    // ============================================================================
    virtual void eraseEvidence( const Potential<T_DATA>* );

    // ============================================================================
    /// performs the collect phase of Lazy Propagation
    // ============================================================================
    void collect( NodeId id, bool force_collect = false );

    // ============================================================================
    /// performs the diffusion phase of Lazy Propagation
    // ============================================================================
    void diffusion( NodeId id, bool force_diffusion = false );

    // ============================================================================
    /// perfoms a whole inference (with force_inference flag at false)
    // ============================================================================
    virtual void makeInference();

    // ============================================================================
    /// performs a whole inference (collect + diffusion)
    // ============================================================================
    void makeInference( bool force_inference );

    // ============================================================================
    /// returns the probability P(e) of the evidence enterred into the BN
    // ============================================================================
    T_DATA evidenceMarginal();

    // ============================================================================
    /// returns the joint a posteriori probability P(nodes|e)
    /** @warning right now, method joint cannot compute joint a posteriori
     * probabilities of every nodeset. In cases where it is not able to perform
     * properly this task, it will raise a OperationNotAllowed exception. */
    // ============================================================================
    Potential<T_DATA>* joint( const NodeSet& nodes );

    const JunctionTree *junctionTree() const ;
    /// @}


  protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param marginal the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillMarginal( Id id , Potential<T_DATA>& marginal );


  private:
    typedef Set<const Potential<T_DATA>*> __PotentialSet;
    typedef SetIterator<const Potential<T_DATA>*> __PotentialSetIterator;

    /// the triangulation class creating the junction tree used for inference
    DefaultTriangulation __triangulation;
    /// the junction tree associated to the bayes net
    JunctionTree *__JT;

    /// for each node of the bayes net, associate an ID in the JT
    HashTable< NodeId, NodeId> __node_to_clique;

    /// the list of all potentials stored in the cliques
    typename Property< List <const Potential<T_DATA>*> >::onNodes
    __clique_potentials;

    /// the list of all the evidence stored in the cliques
    typename Property< List <const Potential<T_DATA>*> >::onNodes
    __clique_evidence;

    /// the list of all potentials stored in the separators
    typename Property< __PotentialSet >::onArcs __sep_potentials;

    /// a list of all the evidence stored into the graph
    __PotentialSet __evidences;

    /// the list of all potentials created during a propagation phase
    __PotentialSet __created_potentials;

    /// an indicator of whether the collect phase passed through a given clique
    Property< bool >::onNodes __collected_cliques;

    /// an indicator of whether the diffusion phase passed through a given clique
    Property< bool >::onNodes __diffused_cliques;

    /// the id of the last clique from which we performed a collect
    NodeId __last_collect_clique;

    NodeSet __roots;

    // ============================================================================
    /// creates the message sent by clique from_id to clique to_id
    // ============================================================================
    void __produceMessage( NodeId from_id, NodeId to_id );

    // ============================================================================
    /// actually perform the collect phase
    // ============================================================================
    void __collect( NodeId id, NodeId from );

    // ============================================================================
    /// actually perform the diffusion phase
    // ============================================================================
    void __diffusion( NodeId id, NodeId from );

    // ============================================================================
    /// indicates that we need inference in a given Junction tree connected component
    /** This function indicates to the class that part of the junction tree needs
     * a new inference. As a result, it updates the messages sent on the separators
     * on this part of the junction tree. */
    // ============================================================================
    void __setRequiredInference( NodeId id, NodeId from );

    // ============================================================================
    /// remove variables del_vars from the list of potentials pot_list
    /** The function actually updates pot_list and, when it returns, pot_list
     * contains the list of potentials resulting from the marginalization */
    // ============================================================================
    void __marginalizeOut( __PotentialSet& pot_list,
                           Set<const DiscreteVariable*>& del_vars );

    // ============================================================================
    // ============================================================================
    void __aPosterioriMarginal( NodeId id, Potential<T_DATA>& marginal );

    // ============================================================================
    // ============================================================================
    void __aPosterioriJoint( const NodeSet& ids, Potential<T_DATA>& marginal );


    // ============================================================================
    /// initialization function
    // ============================================================================
    void __initialize ( const AbstractBayesNet<T_DATA>& BN,
                        StaticTriangulation& triangulation,
                        const HashTable<NodeId, unsigned int>& modalities );

    /// avoid copy constructors
    LazyPropagation( const LazyPropagation<T_DATA>& );

    /// avoid copy operators
    LazyPropagation<T_DATA>& operator= ( const LazyPropagation<T_DATA>& );
  };


} /* namespace gum */


#include <agrum/BN/inference/lazyPropagation.tcc>


#endif /* GUM_LAZY_PROPAGATION_H */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
