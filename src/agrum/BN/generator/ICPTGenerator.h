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
/** @file
 * @brief Interface-like class for generating Conditional Probability Tables.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 */
#ifndef GUM_I_CPT_GENERATOR_H
#define GUM_I_CPT_GENERATOR_H

#include <agrum/config.h>
#include <agrum/multidim/potential.h>

namespace gum {
  /** @class ICPTGenerator *
   * @brief Abstract class for generating Conditional Probability Tables.
   * @ingroup bn_group
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing CPT generation policies for the BayesNetGenerator class.
   */
  template <typename GUM_SCALAR> class ICPTGenerator {
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
    virtual void generateCPT(const Idx &varId, const Potential<GUM_SCALAR> &cpt) = 0;
    /// @}
  };

  extern template class ICPTGenerator<float>;
  extern template class ICPTGenerator<double>;

} /* namespace gum */

#include <agrum/BN/generator/ICPTGenerator.tcc>
#endif /* GUM_I_CPT_GENERATOR_H */
