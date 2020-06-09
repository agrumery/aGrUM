
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/PRM/generator/nameGenerator.h>

namespace gum {
  namespace prm {
    INLINE
    NameGenerator::NameGenerator() : counters__(3, 1) {
      GUM_CONSTRUCTOR(NameGenerator);
    }

    INLINE
    NameGenerator::NameGenerator(const NameGenerator& source) :
        counters__(source.counters__) {
      GUM_CONS_CPY(NameGenerator);
    }

    INLINE
    NameGenerator::~NameGenerator() { GUM_DESTRUCTOR(NameGenerator); }

    INLINE
    NameGenerator& NameGenerator::operator=(const NameGenerator& source) {
      counters__ = source.counters__;
      return *this;
    }

    INLINE
    std::string NameGenerator::nextName(PRMObject::prm_type type) {
      std::stringstream s;

      switch (type) {
        case PRMObject::prm_type::ALL:
        case PRMObject::prm_type::CLASS: {
          s << "class_" << ++(counters__[0]);
          break;
        }

        case PRMObject::prm_type::PRM_INTERFACE: {
          s << "iface_" << ++(counters__[0]);
          break;
        }

        case PRMObject::prm_type::SYSTEM: {
          s << "sys_" << ++(counters__[0]);
          break;
        }

        case PRMObject::prm_type::TYPE: {
          s << "type_" << ++(counters__[0]);
          break;
        }

        case PRMObject::prm_type::CLASS_ELT: {
          s << "elt_" << ++(counters__[1]);
          break;
        }

        case PRMObject::prm_type::INSTANCE: {
          s << "inst_" << ++(counters__[2]);
          break;
        }

        default: {
          GUM_ERROR(FatalError, "unknown PRMObject type");
        }
      }

      return s.str();
    }

  } /* namespace prm */
} /* namespace gum */
