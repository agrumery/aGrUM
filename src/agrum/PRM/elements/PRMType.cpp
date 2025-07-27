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
 * @brief Source implementation of type.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#include <agrum/PRM/elements/PRMType.h>

#ifdef GUM_NO_INLINE
#  include <agrum/PRM/elements/PRMType_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  namespace prm {


    void PRMType::_updateName_() {
      const void*       address = static_cast< const void* >(this);
      std::stringstream ss;
      ss << this->name() << "-" << address;
      this->_var_->setName(ss.str());
    }

    PRMType::PRMType(const DiscreteVariable& var) :
        PRMObject(var.name()), _var_(var.clone()), _superType_(0), _label_map_(0) {
      GUM_CONSTRUCTOR(PRMType);
      this->_updateName_();
    }

    PRMType::PRMType(PRMType&                  super_type,
                     const std::vector< Idx >& label_map,
                     const DiscreteVariable&   var) :
        PRMObject(var.name()), _var_(var.clone()), _superType_(&super_type),
        _label_map_(new std::vector< Idx >(label_map)) {
      GUM_CONSTRUCTOR(PRMType);
      this->_updateName_();

      if (!_isValid_()) {
        delete _label_map_;
        _label_map_ = 0;
        GUM_ERROR(OperationNotAllowed, "Invalid label map.")
      }
    }

    PRMType::PRMType(const PRMType& from) :
        PRMObject(from), _var_(from._var_->clone()), _superType_(from._superType_), _label_map_(0) {
      GUM_CONS_CPY(PRMType);
      this->_updateName_();

      if (_superType_) { _label_map_ = new std::vector< Idx >(from.label_map()); }
    }

    PRMType::PRMType(PRMType&& from) : PRMObject(from) {
      GUM_CONS_MOV(PRMType);
      GUM_ERROR(FatalError, "Move constructor must not be used")
    }

    PRMType::~PRMType() {
      GUM_DESTRUCTOR(PRMType);
      delete _var_;
      if (_label_map_) { delete _label_map_; }
    }

    PRMType& PRMType::operator=(const PRMType& from){
        GUM_ERROR(FatalError, "Copy operator must not be used")}


    PRMType& PRMType::operator=(PRMType&& from) {
      GUM_ERROR(FatalError, "Move operator must not be used")
    }

    bool PRMType::isSubTypeOf(const PRMType& super) const {
      if ((*this) == super) {
        return true;
      } else if (_superType_) {
        return _superType_->isSubTypeOf(super);
      } else {
        return false;
      }
    }

    bool PRMType::_isValid_() const {
      if (!_superType_) { return _var_->domainSize() > 1; }

      if (_label_map_->size() == _var_->domainSize()) {
        for (size_t i = 0; i < _label_map_->size(); ++i) {
          if (_label_map_->at(i) >= (**_superType_).domainSize()) { return false; }
        }

        return true;
      }

      return false;
    }


  } /* namespace prm */
} /* namespace gum */
