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
 * @brief Abstract class for generating Utility Tables.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_UT_GENERATOR_H
#define GUM_UT_GENERATOR_H

#include <agrum/multidim/utilityTable.h>

namespace gum {

  /** @class UTGenerator
   * @brief Abstract class for generating Utility Tables.
   * @ingroup id_group
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing UT generation policies for the InfluenceDiagramGenerator class.
   */
  class UTGenerator {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    UTGenerator();

    /**
     * Destructor.
     */
    virtual ~UTGenerator();
    /// @}

    // ############################################################################
    /// @name UT generation methods
    // ############################################################################
    /// @{
    /**
     * Generates a UT using floats.
     * @param varId The variable id of the UT owner.
     * @param ut A reference on the UT to fill.
     */
    virtual void generateUT(const Idx &varId, const UtilityTable<float> &ut) = 0;

    /**
     * Generates a UT using doubles.
     * @param varId The variable id of the UT owner.
     * @param ut A reference on the UT to fill.
     */
    virtual void generateUT(const Idx &varId, const UtilityTable<double> &ut) = 0;
    /// @}
  };

} /* namespace gum */

#endif /* GUM_UT_GENERATOR_H */
