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
 * @brief Interface-like class for generating Conditional Probability Tables.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6) and
 *Ariele-Paolo MAESANO
 */
#ifndef GUM_I_CPT_GENERATOR_H
#define GUM_I_CPT_GENERATOR_H

#include <agrum/agrum.h>
#include <agrum/tools/multidim/potential.h>

namespace gum {
  /** @class ICPTGenerator
   * @headerfile ICPTGenerator.h <agrum/BN/generator/ICPTGenerator.h>
   * @brief Abstract class for generating Conditional Probability Tables.
   * @ingroup bn_generator
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing CPT generation policies for the BayesNetGenerator class.
   */
  template < typename GUM_SCALAR >
  class ICPTGenerator {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    ICPTGenerator();

    /**
     * Destructor.
     */
    virtual ~ICPTGenerator();
    /// @}

    // ############################################################################
    /// @name CPT generation methods
    // ############################################################################
    /// @{
    /**
     * Generates a CPT using floats.
     * @param varId The variable id of the CPT owner.
     * @param cpt A reference on the CPT to fill.
     */
    virtual void generateCPT(const Idx& varId, const Potential< GUM_SCALAR >& cpt) const = 0;
    /// @}
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ICPTGenerator< double >;
#endif

} /* namespace gum */

#include <agrum/BN/generator/ICPTGenerator_tpl.h>
#endif /* GUM_I_CPT_GENERATOR_H */
