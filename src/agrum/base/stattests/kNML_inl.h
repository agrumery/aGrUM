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

#pragma once


/**
 * @file
 * @brief The class for the NML penalty used in MIIC
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE KNML::KNML(const DBRowGeneratorParser&                                 parser,
                      const Prior&                                                prior,
                      const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                      const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        IndependenceTest(parser, prior, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(KNML);
    }

    /// default constructor
    INLINE KNML::KNML(const DBRowGeneratorParser&             parser,
                      const Prior&                            prior,
                      const Bijection< NodeId, std::size_t >& nodeId2columns) :
        IndependenceTest(parser, prior, nodeId2columns) {
      GUM_CONSTRUCTOR(KNML);
    }

    /// copy constructor
    INLINE KNML::KNML(const KNML& from) :
        IndependenceTest(from), _param_complexity_(from._param_complexity_) {
      GUM_CONS_CPY(KNML);
    }

    /// move constructor
    INLINE KNML::KNML(KNML&& from) :
        IndependenceTest(std::move(from)), _param_complexity_(std::move(from._param_complexity_)) {
      GUM_CONS_MOV(KNML);
    }

    /// virtual copy constructor
    INLINE KNML* KNML::clone() const { return new KNML(*this); }

    /// destructor
    INLINE KNML::~KNML() { GUM_DESTRUCTOR(KNML); }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
