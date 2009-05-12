/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief This file contains gibbs sampling class definitions Bayesian networks
 * inference classes.
 */
#ifndef GUM_GIBBS_H
#define GUM_GIBBS_H

#include <vector>
#include <agrum/BN/inference/BayesNetInference.h>


namespace gum {


  /**
   * @class Gibbs
   * @brief class for making Gibbs sampling inference in bayesian networks.
   * @ingroup bn_group
   *
   */
  template <typename T_DATA>

  class Gibbs : public BayesNetInference<T_DATA> {
  public:
    /**
     * Default constructor
     */
    Gibbs( const BayesNet<T_DATA>& BN );

    /**
     * Destructor.
     */
    virtual ~Gibbs();

    /**
     * Makes the inference: all marginals are computed.
     */
    virtual void makeInference() ;

    /**
     * Insert new evidence in the graph.
     * @warning if an evidence already w.r.t. a given node and a new
     * evidence w.r.t. this node is onserted, the old evidence is removed.
     */
    virtual void insertEvidence( const List<const Potential<T_DATA>*>& pot_list ) ;

    /**
     * Remove a given evidence from the graph.
     */
    virtual void eraseEvidence( const Potential<T_DATA>* e ) ;

    /**
     * Remove all evidence from the graph.
     */
    virtual void eraseAllEvidence() ;
    void setRequiredInference();
    bool isInferenceRequired();

    /// setter for Gibbs parameters
    ///@{

    /// stopping criterion on KL(t,t+1)
    void setEpsilon( double );

    /// stopping criterion on dKL(t,t+1)/dt
    void setMinEpsilonRate( double );

    /// stopping criterion on number of iteration
    void setMaxIter( Size );

    /// size of burn in on number of iteration
    void setBurnIn( Size );

    /// 0 for deterministic order, n for n vars randomly choosen
    void setNbrDrawnBySample( Size );

    /// how many samples between 2 stopping tests
    void setPeriodeSize( Size );

    /// verbosity
    void setVerbosity( bool );

    ///@}


  protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param marginal the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillMarginal( NodeId id ,Potential<T_DATA>& marginal );


  private:
    /// Threshold for convergence
    double __eps;

    /// Threshold for rate of epsilon
    double __min_rate_eps;

    /// parameters for gibbs sampler
    ///@{
    Size __nbr_drawn_by_sample;
    Size __burn_in;
    Size __max_iter;
    Size __periode_size;
    bool __verbosity;
    ///@}

    ///inference flag
    bool __inference_is_required;

    /// the actual values of sample
    Instantiation __current_sample;

    /// the actual number of sampling for each modality by node
    typename Property<Potential<T_DATA>*>::onNodes __sampling_nbr;

    /// a table of instantiation for direct access on _sampling_nbr
    Property<Instantiation*>::onNodes __sampling_idx;

    /// a table of potential for posterior computed in the markov blanket
    typename Property<Potential<T_DATA>*>::onNodes __sampling_posterior;

    /// a table of children for each node
    typename Property<std::vector<NodeId>*>::onNodes __node_children;

    /// a table of instantiation for each cpt
    Property<Instantiation*>::onNodes __cpt_idx;

    /// a list of all the evidence stored into the graph
    typename Property<const Potential<T_DATA>*>::onNodes __evidences;

    /// a list of hard evidence (not sampled)
    Property<Idx>::onNodes __hard_evidences;

    void __unsetRequiredInference();
    void __initStats();
    void __updateStats_without_err();
    double __updateStats_with_err( Size nbr );
    Idx __getValVar(NodeId id);
    void __chgValVar( NodeId id, Idx choice );

    // normalize and draw wrt __sampling_posterior[id]
    void __drawVar( NodeId id );
    void __generateSample( std::vector<NodeId>& nodes_array,Size nbr );
    void __MonteCarloSample();
    void __GibbsSample( NodeId id );
  };


} /* namespace gum */


#include <agrum/BN/inference/Gibbs.tcc>


#endif /* GUM_GIBBS_H */
