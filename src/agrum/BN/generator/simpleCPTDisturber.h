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


/**
 * @file
 * @brief Source implementation of SimpleCPTDisturber.
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele-Paolo MAESANO
 *
 */

#ifndef GUM_SIMPLE_CPT_DISTURBER_H
#define GUM_SIMPLE_CPT_DISTURBER_H

#include <cstdlib>

#include <agrum/BN/generator/ICPTDisturber.h>
#include <agrum/agrum.h>

namespace gum {

  /** @class SimpleCPTDisturber simpleCPTDisturber.h
   * <agrum/BN/generator/simpleCPTDisturber.h>
   * @brief Class for disturbing Conditional Probability Tables.
   * @ingroup bn_generator
   *
   * This class implements a CPTGenerator CPT generation algorithm.
   */
  template < typename GUM_SCALAR >
  class SimpleCPTDisturber: public ICPTDisturber< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    SimpleCPTDisturber();

    /**
     * Destructor.
     */
    ~SimpleCPTDisturber() override;
    /// @}

    // ############################################################################
    /// @name CPT disturbing methods
    // ############################################################################
    /// @{
    /**
     * Disturb a CPT using GUM_SCALAR when removing parent varIdi.
     * @param node The variable on the CPT owner.
     * @param bayesNet the Bayesian network.
     * @param src copy of the CPT before reduction.
     * @param marg of the inference before reduction on the node varIdi.
     */
    void disturbReducCPT(NodeId                   node,
                         BayesNet< GUM_SCALAR >&  bayesNet,
                         Potential< GUM_SCALAR >& src,
                         Potential< GUM_SCALAR >& marg) override;

    /**
     * Disturb a CPT using GUM_SCALAR when inserting a new parent new_parent.
     * @param node A reference on the CPT owner.
     * @param bayesNet the Bayesian network.
     * @param src copy of the CPT before augmentation.
     * @param variation degree of variation from the initial probability.
     */
    void disturbAugmCPT(NodeId                   node,
                        BayesNet< GUM_SCALAR >&  bayesNet,
                        Potential< GUM_SCALAR >& src,
                        GUM_SCALAR               variation) override;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class SimpleCPTDisturber< double >;
#endif

} /* namespace gum */

#include <agrum/BN/generator/simpleCPTDisturber_tpl.h>
#endif   // SIMPLECPTDISTURBER_H
