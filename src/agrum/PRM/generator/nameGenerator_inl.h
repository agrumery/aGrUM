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
        case PRMObject::prm_type::ALL:
        case PRMObject::prm_type::CLASS: {
          s << "class_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::PRM_INTERFACE: {
          s << "iface_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::SYSTEM: {
          s << "sys_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::TYPE: {
          s << "type_" << ++(_counters_[0]);
          break;
        }

        case PRMObject::prm_type::CLASS_ELT: {
          s << "elt_" << ++(_counters_[1]);
          break;
        }

        case PRMObject::prm_type::INSTANCE: {
          s << "inst_" << ++(_counters_[2]);
          break;
        }

        default: {
          GUM_ERROR(FatalError, "unknown PRMObject type")
        }
      }

      return s.str();
    }

  } /* namespace prm */
} /* namespace gum */
