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
 * @brief This file contains Weighted sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#ifndef GUM_WEIGHTED_INFERENCE_H
#define GUM_WEIGHTED_INFERENCE_H

#include <agrum/BN/inference/tools/approximateInference.h>

namespace gum {

    /**
        * @class WeightedInference weightedInference.h
        *<agrum/BN/inference/weightedInference.h>
        * @brief class for making Weighted sampling inference in bayesian networks.
        * @ingroup bn_approximation
        *
        * This class overrides pure function declared in the inherited class ApproximateInference.
        * It defines the way Weighted sampling draws a sample.
        *
        */

    template<typename GUM_SCALAR>
    class WeightedSampling : public ApproximateInference<GUM_SCALAR> {

    public:

        /**
         * Default constructor
         */
        WeightedSampling(const IBayesNet<GUM_SCALAR> *BN);

        /**
         * Destructor
         */
        virtual ~WeightedSampling();

    protected:

        /// draws a defined number of samples without updating the estimators
        virtual Instantiation _burnIn();

        /// draws a sample according to Weighted sampling
        /**
        * @param w the weight of sample being generated
        * @param prev the previous sample generated
        * @param bn the bayesian network containing the evidence
        * @param hardEvNodes hard evidence nodes
        * @param hardEv hard evidences values
        *
        * Generates a new sample in topological order. Each sample has a weight bias.
        * The sample weight is the product of each node's weight.
        *
        */
        virtual Instantiation
        _draw(float *w, Instantiation prev, const IBayesNet<GUM_SCALAR> &bn = BayesNet<GUM_SCALAR>(),
              const NodeSet &hardEvNodes = NodeSet(), const NodeProperty<Idx> &hardEv = NodeProperty<Idx>());

    };

    extern template
    class WeightedSampling<float>;

    extern template
    class WeightedSampling<double>;

}

#include <agrum/BN/inference/weightedSampling_tpl.h>

#endif
