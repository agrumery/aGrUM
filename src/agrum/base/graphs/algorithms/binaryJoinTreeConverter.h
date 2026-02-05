/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief The base interface for converting a join tree into a binary join tree
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_BINARY_JOIN_TREE_CONVERTER_H
#define GUM_BINARY_JOIN_TREE_CONVERTER_H

#include <agrum/base/graphs/cliqueGraph.h>

namespace gum {

  class BinaryJoinTreeConverter {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    BinaryJoinTreeConverter();

    /// destructor
    virtual ~BinaryJoinTreeConverter();

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns a binary join tree corresponding to clique graph JT
    /** This method creates and returns a new binary join tree compatible with
     * that passed in argument (JT) and optimized for inference. As such, this
     * requires knowing the join tree to be converted (of course), but also
     * which roots will be used by the collect/diffusion inference engine and
     * the domain size of the variables contained in the cliques of JT (to
     * optimize the combination of the tensors contained in the cliques.
     * @throws InvalidNode exception is thrown if some roots do not belong to
     * JT or if several roots belong to the same connected component.
     * @warning If you do not pass in argument a root for each connected
     * component,
     * then for those with unspecified roots, an arbitrary root will be computed
     * and used for the binarization. */
    virtual CliqueGraph convert(const CliqueGraph&          JT,
                                const NodeProperty< Size >& domain_sizes,
                                const NodeSet&              roots)
        = 0;

    /// returns all the roots considered for all the connected components
    virtual const NodeSet& roots() const = 0;

    /// @}

    private:
    /// forbid copy constructor
    BinaryJoinTreeConverter(const BinaryJoinTreeConverter&);

    /// forbid copy operator
    BinaryJoinTreeConverter& operator=(const BinaryJoinTreeConverter&);
  };

} /* namespace gum */

#endif /* GUM_BINARY_JOIN_TREE_CONVERTER_H */
