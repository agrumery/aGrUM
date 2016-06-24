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

#include <cmath>

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>

namespace gum {
  /**
   * @class LazyPropagation lazyPropagation.h
   * <agrum/BN/inference/lazyPropagation.h>
   * @brief Implementation of lazy propagation for inference in Bayesian
   * Networks
   * @ingroup bn_inference
   */
  template <typename GUM_SCALAR>

  class LazyPropagation : public BayesNetInference<GUM_SCALAR> {
    public:
    /** @brief type of algorithm for determining the relevant potentials for
     * combinations using some d-separation analysis
     *
     * When constructing messages from one clique to its neighbor, we can
     * exploit d-separation to determine that some potentials are irrelevant for
     * the message computation. So we can discard them and, thereby, speed-up
     * the computations.
     */
    enum FindRelevantPotentialsType {
      FIND_RELEVANT_ALL,            // do not perform d-separation analysis
      FIND_RELEVANT_D_SEPARATION,   // BayesBall requisite nodes -> potentials
      FIND_RELEVANT_D_SEPARATION2,  // BayesBall requisite potentials (directly)
      FIND_RELEVANT_D_SEPARATION3   // Koller & Friedman 2009 requisite
                                    // potentials
    };

    /** @brief type of algorithm to determine barren nodes
     *
     * When constructing messages from one clique to its neighbor, we can
     * determine that some nodes are barren, i.e., they are the only one
     * at the left hand side of a conditioning bar and they appear in only one
     * potential. In such case, in a classical BN inference, there is no need
     * to take them into account since their removal will necessarily create
     * a constant potential. So, we can discard their potential from the
     * computation. However, when computing p(evidence), we should not do that
     * because the constant is important and need be computed.
     */
    enum FindBarrenNodesType {
      FIND_NO_BARREN_NODES,  // do not try to find barren nodes
      FIND_BARREN_NODES      // use a simple algorithm to detect barren nodes
    };


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    LazyPropagation( const IBayesNet<GUM_SCALAR>& BN,
                     FindRelevantPotentialsType =
                     FindRelevantPotentialsType::FIND_RELEVANT_D_SEPARATION2,
                     FindBarrenNodesType = FIND_BARREN_NODES );

    /// destructor
    ~LazyPropagation();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// set a new triangulation algorithm
    void setTriangulation ( const Triangulation& new_triangulation );

    /// sets how we determine the relevant potentials to combine
    /** When a clique sends a message to a separator, it first constitute the
     * set of the potentials it contains and of the potentials contained in the
     * messages it received. If FindRelevantPotentialsType = FIND_RELEVANT_ALL,
     * all these potentials are combined and projected to produce the message
     * sent to the separator.
     * If FindRelevantPotentialsType = FIND_RELEVANT_D_SEPARATION, then only the
     * set of potentials d-connected to the variables of the separator are kept
     * for combination and projection. */
    void setFindRelevantPotentialsType( FindRelevantPotentialsType type );

    /// sets the operator for performing the projections
    void setProjectionFunction ( Potential<GUM_SCALAR>* (* proj)
                                 ( const Potential<GUM_SCALAR>&,
                                   const Set<const DiscreteVariable*>& ) );

    /// sets the operator for performing the combinations
    void setCombinationFunction ( Potential<GUM_SCALAR>* (* comb)
                                  ( const Potential<GUM_SCALAR>&,
                                    const Potential<GUM_SCALAR>& ) );

    
    =========================
    

    


    

    /// insert new evidence in the graph
    /** @warning if an evidence already exists w.r.t. a given node and a new
     * evidence w.r.t. this node is inserted, the old evidence is removed. */
    virtual void insertEvidence( const List<const Potential<GUM_SCALAR>*>& );

    /// remove all evidence from the graph
    virtual void eraseAllEvidence();

    /// remove a given evidence from the graph
    virtual void eraseEvidence( const Potential<GUM_SCALAR>* );

    /// performs the collect phase of Lazy Propagation
    void collect( NodeId id, bool force_collect = false );

    /// performs the diffusion phase of Lazy Propagation
    void diffusion( NodeId id, bool force_diffusion = false );

    /// perfoms a whole inference (with force_inference flag at false)
    virtual void makeInference();

    /// performs a whole inference (collect + diffusion)
    void makeInference( bool force_inference );

    /// clears the messages of previous inferences
    void clearInference();

    /// returns the probability P(e) of the evidence enterred into the BN
    GUM_SCALAR evidenceProbability();

    /// returns the joint a posteriori probability P(nodes|e)
    /** @warning right now, method joint cannot compute joint a posteriori
     * probabilities of every nodeset. In cases where it is not able to perform
     * properly this task, it will raise a OperationNotAllowed exception.
     * @warning : joint computes a new Potential<GUM_SCALAR> and returns
     * a pointer :
     * do not forget to free it !
     * @return a pointer to a dynamically allocated Potential<GUM_SCALAR>
     * @throw OperationNotAllowed
     */
    Potential<GUM_SCALAR>* joint( const NodeSet& nodes );

    const JunctionTree* junctionTree() const;
    /// @}

    /// @name Information Theory related functions
    /// @{

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    GUM_SCALAR H( NodeId X );

    /** Mutual information between X and Y
     * @see http://en.wikipedia.org/wiki/Mutual_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR I( NodeId X, NodeId Y );

    /** Variation of information between X and Y
     * @see http://en.wikipedia.org/wiki/Variation_of_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR VI( NodeId X, NodeId Y );
    /// @}

    protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param posterior the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillPosterior( Id id, Potential<GUM_SCALAR>& posterior );



  protected:
    /**
     * _prepareInferenceStructure is called when the bn, the targets and soft/hard
     * evidence are known. Note that the values of evidence are not necessarily
     * known and can be changed between _prepare and _makeInference.
     */
    virtual void _prepareInferenceStructure ();


    

    

  private:
    typedef Set<const Potential<GUM_SCALAR>*> __PotentialSet;
    typedef SetIteratorSafe<const Potential<GUM_SCALAR>*>
        __PotentialSetIterator;
    
    
    /// the type of relevant potential finding algorithm to be used
    FindRelevantPotentialsType __find_relevant_potential_type;
    
    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void ( LazyPropagation<GUM_SCALAR>::*__findRelevantPotentials )
    ( __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /// the type of barren nodes computation we wish
    FindBarrenNodesType __barren_nodes_type;

    /// the function to compute barren nodes during message passing
    void ( LazyPropagation<GUM_SCALAR>::*__findBarrenNodes ) ();
    
    /// the triangulation class creating the junction tree used for inference
    Triangulation* __triangulation;

    /// the junction tree associated to the bayes net
    JunctionTree* __JT { nullptr };

    /// a clique node used as a root in each connected component of __JT
    NodeSet __roots;

    /// for each node of the bayes net, associate an ID in the JT
    HashTable<NodeId, NodeId> __node_to_clique;

    /// the list of all potentials stored in the cliques
    NodeProperty<List<const Potential<GUM_SCALAR>*>> __clique_potentials;

    /// the list of all potentials stored in the separators after inferences
    ArcProperty<List<const Potential<GUM_SCALAR>*>> __separator_potentials;

    /// indicates whether a message (from one clique to another) has been computed
    /** Here, all the messages, computed or not, are put into the property, only
     * the Boolean makes the difference between messages computed and those that
     * were not computed */
    ArcProperty<bool> __messages_computed;

    /// the list of all the evidence stored in the cliques
    NodeProperty<List<const Potential<GUM_SCALAR>*>> __clique_evidence;

    /// the list of small potentials created to take into account hard evidence
    List<const Potential<GUM_SCALAR>*> __hard_ev_potentials;

    /// the set of nodes that received hard evidences during the last inference
    NodeSet __hard_evidence_nodes;



    




    


    /// a possible partial order for triangulations provided by the user
    std::vector<NodeId> __elim_order;

    
    /// the list of hard evidence potentials per random variable
    NodeProperty<const Potential<GUM_SCALAR>*> __bn_node2hard_potential;


    /// the set of barren potentials: can be discarded from computations
    /** Assigns a set (possibly empty) of barren potentials to each message
     * sent in the junction tree. A message is represented by an arc from
     * one clique (id) to another. */
    ArcProperty<__PotentialSet> __barren_potentials;

    /// indicates whether we shall recompute barren nodes
    bool __need_recompute_barren_potentials { true };




    /// creates the message sent by clique from_id to clique to_id

    void __produceMessage( NodeId from_id, NodeId to_id );

    /// actually perform the collect phase

    void __collect( NodeId id, NodeId from );

    /// actually perform the diffusion phase

    void __diffusion( NodeId id, NodeId from );

    /// indicates that we need inference in a given Junction tree connected
    /// component
    /** This function indicates to the class that part of the junction tree
     * needs
     * a new inference. As a result, it updates the messages sent on the
     * separators
     * on this part of the junction tree. */

    void __setRequiredInference( NodeId id, NodeId from );

    /// remove variables del_vars from the list of potentials pot_list
    /** The function actually updates pot_list and, when it returns, pot_list
     * contains the list of potentials resulting from the marginalization of the
     * posterior*/

    void __marginalizeOut( __PotentialSet& pot_list,
                           Set<const DiscreteVariable*>& del_vars,
                           Set<const DiscreteVariable*>& kept_vars );

    void __aPosterioriMarginal( NodeId id, Potential<GUM_SCALAR>& posterior );

    void __aPosterioriJoint( const NodeSet& ids,
                             Potential<GUM_SCALAR>& posterior );

    /// initialization function
    void __initialize( const IBayesNet<GUM_SCALAR>& BN,
                       StaticTriangulation& triangulation,
                       const NodeProperty<Size>& modalities );

    /// check wether an evidence is a hard one
    bool __isHardEvidence( const Potential<GUM_SCALAR>* pot );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void __findRelevantPotentialsWithdSeparation(
        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void __findRelevantPotentialsWithdSeparation2(
        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void __findRelevantPotentialsWithdSeparation3(
        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void
    __findRelevantPotentialsGetAll( __PotentialSet& pot_list,
                                    Set<const DiscreteVariable*>& kept_vars );


    /// remove barren variables from a set of potentials
    void __removeBarrenVariables( __PotentialSet& pot_list,
                                  Set<const DiscreteVariable*>& del_vars );

    /// compute barren nodes if necessary
    void __computeBarrenPotentials();

    /// avoid copy constructors
    LazyPropagation( const LazyPropagation<GUM_SCALAR>& );

    /// avoid copy operators
    LazyPropagation<GUM_SCALAR>&
    operator=( const LazyPropagation<GUM_SCALAR>& );
  };


  extern template class LazyPropagation<float>;
  extern template class LazyPropagation<double>;


} /* namespace gum */

#include <agrum/BN/inference/lazyPropagation.tcc>

#endif /* GUM_LAZY_PROPAGATION_H */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
