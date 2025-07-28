/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Source implementation of PRMObject.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#include <agrum/PRM/elements/PRMObject.h>

#ifdef GUM_NO_INLINE
#  include <agrum/PRM/elements/PRMObject_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {
  namespace prm {
    // ==========================================================================
    // Constructor.
    // @param type The type of this object.
    // ==========================================================================
    PRMObject::PRMObject(const std::string& name) : _name_(name) { GUM_CONSTRUCTOR(PRMObject); }

    // ==========================================================================
    // Copy constructor.
    // ==========================================================================
    PRMObject::PRMObject(const PRMObject& source) : _name_(source._name_) {
      GUM_CONS_CPY(PRMObject);
    }

    // ==========================================================================
    // Move constructor.
    // ==========================================================================
    PRMObject::PRMObject(PRMObject&& source) : _name_(std::move(source._name_)) {
      GUM_CONS_CPY(PRMObject);
    }

    // ==========================================================================
    // Destructor.
    // ==========================================================================
    PRMObject::~PRMObject() {
      GUM_DESTRUCTOR(PRMObject);
      ;
    }

    // Copy operator.
    PRMObject& PRMObject::operator=(const PRMObject& source) {
      _name_ = source._name_;
      return *this;
    }

    // Move operator.
    PRMObject& PRMObject::operator=(PRMObject&& source) {
      _name_ = std::move(source._name_);
      return *this;
    }

    // ==========================================================================
    // For printing PRMType easily.
    // ==========================================================================
    std::ostream& operator<<(std::ostream& out, PRMObject::prm_type obj_type) {
      return (out << PRMObject::enum2str(obj_type));
    }

    // ==========================================================================
  } /* namespace prm */
} /* namespace gum */
