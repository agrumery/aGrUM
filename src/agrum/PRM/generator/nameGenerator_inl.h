/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Inline implementation of PRMGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/generator/nameGenerator.h>

namespace gum {
  namespace prm {
    INLINE
    NameGenerator::NameGenerator() : _counters_(3, 1) { GUM_CONSTRUCTOR(NameGenerator); }

    INLINE NameGenerator::NameGenerator(const NameGenerator& source) :
        _counters_(source._counters_) {
      GUM_CONS_CPY(NameGenerator);
    }

    INLINE NameGenerator::~NameGenerator() {
      GUM_DESTRUCTOR(NameGenerator);
      ;
    }

    INLINE
    NameGenerator& NameGenerator::operator=(const NameGenerator& source) {
      _counters_ = source._counters_;
      return *this;
    }

    INLINE
    std::string NameGenerator::nextName(PRMObject::prm_type type) {
      std::stringstream s;

      switch (type) {
        case PRMObject::prm_type::ALL :
        case PRMObject::prm_type::CLASS : {
          s << "class_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::PRM_INTERFACE : {
          s << "iface_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::SYSTEM : {
          s << "sys_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::TYPE : {
          s << "type_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::CLASS_ELT : {
          s << "elt_" << ++(_counters_[1]);
          break;
        }

        case PRMObject::prm_type::INSTANCE : {
          s << "inst_" << ++(_counters_[2]);
          break;
        }

        default : {
          GUM_ERROR(FatalError, "unknown PRMObject type")
        }
      }

      return s.str();
    }

  } /* namespace prm */
} /* namespace gum */
