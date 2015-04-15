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
/**
 * @file
 * @brief Inline implementation of PRMGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/generator/nameGenerator.h>

namespace gum {
  namespace prm {
    INLINE
    NameGenerator::NameGenerator() : __counters(3, 1) {
      GUM_CONSTRUCTOR(NameGenerator);
    }

    INLINE
    NameGenerator::NameGenerator(const NameGenerator &source)
        : __counters(source.__counters) {
      GUM_CONS_CPY(NameGenerator);
    }

    INLINE
    NameGenerator::~NameGenerator() { GUM_DESTRUCTOR(NameGenerator); }

    INLINE
    NameGenerator &NameGenerator::operator=(const NameGenerator &source) {
      __counters = source.__counters;
      return *this;
    }

    INLINE
    std::string NameGenerator::nextName(PRMObject::PRMType type) {
      std::stringstream s;

      switch (type) {
        case PRMObject::PRMType::ALL:
        case PRMObject::PRMType::CLASS: {
          s << "class_" << ++(__counters[0]);
          break;
        }

        case PRMObject::PRMType::INTERFACE: {
          s << "iface_" << ++(__counters[0]);
          break;
        }

        case PRMObject::PRMType::SYSTEM: {
          s << "sys_" << ++(__counters[0]);
          break;
        }

        case PRMObject::PRMType::TYPE: {
          s << "type_" << ++(__counters[0]);
          break;
        }

        case PRMObject::PRMType::CLASS_ELT: {
          s << "elt_" << ++(__counters[1]);
          break;
        }

        case PRMObject::PRMType::INSTANCE: {
          s << "inst_" << ++(__counters[2]);
          break;
        }

        default: { GUM_ERROR(FatalError, "unknown PRMObject type"); }
      }

      return s.str();
    }

  } /* namespace prm */
} /* namespace gum */
