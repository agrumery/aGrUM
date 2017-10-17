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
 * @brief This file contains general methods for approximate inference.
 *
 * @author Pierre-Henri WUILLEMIN
 */

#ifndef GUM_APPROXIMATE_INFERENCE_H
#define GUM_APPROXIMATE_INFERENCE_H

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/inference/tools/marginalTargetedInference.h>
#include <agrum/core/approximations/approximationScheme.h>

namespace gum {

    /**
     * @class ApproximateInference approximateInference.h
     * <agrum/BN/inference/approximateInference.h>
     * @brief A generic class for making approximate inference in bayesian networks
     * adapted
     *
     * @ingroup bn_approximation
     *
     * The goal of this class is to define the general scheme used by all
     * approximate inference algorithms, which are implemented as derived classes of ApproximateInference.
     * This class inherits from MarginalTargetedInference for the handling of
     * marginal targets and from ApproximationScheme.
     */
  template < typename GUM_SCALAR >
  class ApproximateInference : public MarginalTargetedInference< GUM_SCALAR >,
                               public ApproximationScheme {
    public:
    ApproximateInference(const IBayesNet< GUM_SCALAR >* bn)
        : MarginalTargetedInference< GUM_SCALAR >(bn){};
  };
}

#endif //GUM_APPROXIMATE_INFERENCE_H
