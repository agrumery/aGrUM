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
 * @brief Abstract class for generating Utility Tables.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef GUM_SIMPLE_UT_GENERATOR_H
#define GUM_SIMPLE_UT_GENERATOR_H

#include <cstdlib>

#include <agrum/ID/generator/UTGenerator.h>

namespace gum {

  /** @class SimpleUTGenerator
   * @brief Class for generating Utility Tables.
   * @ingroup id_group
   *
   * This class implements a CPTGenerator UT generation algorithm.
   */
  class SimpleUTGenerator: public UTGenerator {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    SimpleUTGenerator();

    /**
     * Destructor.
     */
    virtual ~SimpleUTGenerator();
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
    virtual void generateUT(const Idx& varId, const Potential< float >& ut);

    /**
     * Generates a UT using doubles.
     * @param varId The variable id of the UT owner.
     * @param ut A reference on the UT to fill.
     */
    virtual void generateUT(const Idx& varId, const Potential< double >& ut);
    /// @}
  };

} /* namespace gum */

#endif /* GUM_SIMPLE_UT_GENERATOR_H */
