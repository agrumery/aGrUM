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
 * @brief Abstract class for generating Conditional Probability Tables.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SIMPLE_CPT_GENERATOR_H
#define GUM_SIMPLE_CPT_GENERATOR_H

#include <cstdlib>

#include <agrum/config.h>
#include <agrum/BN/generator/ICPTGenerator.h>

namespace gum {

  /** @class SimpleCPTGenerator
   * @brief Class for generating Conditional Probability Tables.
   * @ingroup bn_group
   *
   * This class implements a CPTGenerator CPT generation algorithm.
   */
  template <typename GUM_SCALAR>
  class SimpleCPTGenerator : public ICPTGenerator<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    SimpleCPTGenerator();

    /**
     * Destructor.
     */
    virtual ~SimpleCPTGenerator();
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
    virtual void generateCPT(const Idx &varId, const Potential<GUM_SCALAR> &cpt);
  };

  extern template class SimpleCPTGenerator<float>;
  extern template class SimpleCPTGenerator<double>;
} /* namespace gum */

#include <agrum/BN/generator/simpleCPTGenerator.tcc>
#endif /* GUM_SIMPLE_CPT_GENERATOR_H */
