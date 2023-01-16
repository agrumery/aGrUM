/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Interface-like class for disturbing Conditional Probability Tables.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele-Paolo MAESANO
 */

#ifndef GUM_I_CPT_DISTURBER_H
#define GUM_I_CPT_DISTURBER_H

#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>

namespace gum {
  /** @class ICPTDisturber
   * @headerfile ICPTDisturber.h <agrum/BN/generator/ICPTDisturber.h>
   * @brief Abstract class for Modifying Conditional Probability Tables.
   * @ingroup bn_generator
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing CPT generation policies for the BayesNetGenerator class.
   */
  template < typename GUM_SCALAR >
  class ICPTDisturber {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    ICPTDisturber();

    /**
     * Destructor.
     */
    virtual ~ICPTDisturber();
    /// @}

    // ############################################################################
    /// @name CPT disturbing methods
    // ############################################################################
    /**
     * Generates a CPT using GUM_SCALAR.
     * @param node The variable on the CPT owner.
     * @param bayesNet the Bayesian network.
     * @param src copy of the CPT before reduction.
     * @param marg of the inference before reduction on the node varIdi.
     */
    virtual void disturbReducCPT(NodeId                   node,
                                 BayesNet< GUM_SCALAR >&  bayesNet,
                                 Potential< GUM_SCALAR >& src,
                                 Potential< GUM_SCALAR >& marg)
       = 0;

    /**
     * Disturb a CPT using GUM_SCALAR when inserting a new parent.
     * @param node A reference on the CPT owner.
     * @param bayesNet the Bayesian network.
     * @param src copy of the CPT before augmentation.
     * @param variation degree of variation from the initial probability.
     */
    virtual void disturbAugmCPT(NodeId                   node,
                                BayesNet< GUM_SCALAR >&  bayesNet,
                                Potential< GUM_SCALAR >& src,
                                GUM_SCALAR               variation)
       = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ICPTDisturber< double >;
#endif


} /* namespace gum */

#include <agrum/BN/generator/ICPTDisturber_tpl.h>
#endif   // GUM_I_CPT_DISTURBER_H
