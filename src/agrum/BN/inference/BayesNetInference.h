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
 * @brief This file contains abstract class definitions bayesian networks inference
 * classes.
 */
#ifndef GUM_INFERENCE_H
#define GUM_INFERENCE_H

#include <agrum/core/exceptions.h>
#include <agrum/BN/BayesNet.h>


namespace gum {


  /**
   * @class BayesNetInference
   * @brief Abstract class for making inference in bayesian networks.
   * @ingroup bn_group
   *
   */
  template <typename T_DATA>
  class BayesNetInference {
  public:
    /**
     * Default constructor
     */
    BayesNetInference( const BayesNet<T_DATA>& bn );

    /**
     * Destructor.
     */
    virtual ~BayesNetInference();

    /**
     * Makes the inference
     */
    virtual void makeInference() =0;

    /**
     * Returns the probability of the variable.
     * If makeInference wasn't yet called, then only the marginal
     * of the given variable will be computed.
     *
     * @param id The variable's id.
     * @return The probability of the variable.
     * @throw NotFound Raised if no variable matches id.
     * @throw OperationNotAllowed Raised if the inference has not and can
     * not be done.
     */
    virtual const Potential<T_DATA>& marginal( Id id );

    /**
     * Insert new evidence in the graph.
     * @warning if an evidence already w.r.t. a given node and a new
     * evidence w.r.t. this node is onserted, the old evidence is removed.
     */
    virtual void insertEvidence( const List<const Potential<T_DATA>*>& pot_list ) =0;

    /**
     * Remove a given evidence from the graph.
     */
    virtual void eraseEvidence( const Potential<T_DATA>* e ) =0;

    /**
     * Remove all evidence from the graph.
     */
    virtual void eraseAllEvidence() =0;

    const BayesNet<T_DATA>& bn();


  protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param marginal the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillMarginal( Id id ,Potential<T_DATA>& marginal )=0;

    /**
     * invalidate the set of marginals kept here
     */
    void _invalidateMarginals();

    typename Property<Potential<T_DATA>*>::onNodes _marginals;

    /// the Bayes net we wish to perform inference on
    const BayesNet<T_DATA>& __bayesNet;
  };


} /* namespace gum */


#include <agrum/BN/inference/BayesNetInference.tcc>


#endif /* GUM_INFERENCE_H */
