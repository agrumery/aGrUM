/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Macros for error messages.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <sstream>

#ifndef GUM_PRM_O3PRM_ERRORS_H
#define GUM_PRM_O3PRM_ERRORS_H

#define O3PRM_TYPE_NOT_FOUND( val, errors )                              \
  {                                                                      \
    auto pos = val.position();                                           \
    auto msg = std::stringstream();                                      \
    msg << "Type error : "                                               \
        << "Unknown type " << val.label();                               \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_TYPE_AMBIGUOUS( val, matches, errors )                     \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Type error : "                                               \
        << "Ambiguous name " << val.label()                              \
        << ", found more than one elligible types: ";                    \
    for ( auto i = 0; i < matches.size() - 1; ++i ) {                    \
      msg << matches[i] << ", ";                                         \
    }                                                                    \
    msg << matches.back();                                               \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_TYPE_RESERVED( val, errors )                               \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Type error : "                                               \
        << "Type name " << val.label() << " is reserved";                \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_TYPE_DUPPLICATE( val, errors )                             \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Type error : "                                               \
        << "Type " << val.label() << " exists already";                  \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_TYPE_CYCLIC_INHERITANCE( sub_type, super_type, errors )    \
  {                                                                      \
    const auto& pos = sub_type.position();                               \
    auto msg = std::stringstream();                                      \
    msg << "Type error : "                                               \
        << "Cyclic inheritance between type " << sub_type.label()        \
        << " and type " << super_type.label();                           \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_TYPE_UNKNOWN_LABEL( type, l, errors )                      \
  {                                                                      \
    const auto& pos = l.position();                                      \
    auto msg = std::stringstream();                                      \
    msg << "Type error : "                                               \
        << "Unknown label " << l.label() << " in " << type.label();      \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_TYPE_INVALID_RANGE( val, errors )                          \
  {                                                                      \
    const auto& pos = val->name().position();                            \
    auto msg = std::stringstream();                                      \
    msg << "Type error : "                                               \
        << "Invalid range " << val->start().value() << " -> "            \
        << val->end().value();                                           \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_CLASS_NOT_FOUND( val, errors )                             \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Class error : "                                              \
        << "Unknown class " << val.label();                              \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_CLASS_AMBIGUOUS( val, matches, errors )                    \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Class error : "                                              \
        << "Name " << val.label() << " is ambiguous: ";                  \
    for ( auto i = 0; i < matches.size() - 1; ++i ) {                    \
      msg << matches[i].first << ", ";                                   \
    }                                                                    \
    msg << matches.back().first;                                         \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_INTERFACE_NOT_FOUND( val, errors )                         \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Interface error : "                                          \
        << "Interface " << val.label() << " not found";                  \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_INTERFACE_AMBIGUOUS( val, matches, errors )                \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Inteface error : "                                           \
        << "Name " << val.label() << " is ambiguous: ";                  \
    for ( auto i = 0; i < matches.size() - 1; ++i ) {                    \
      msg << matches[i] << ", ";                                         \
    }                                                                    \
    msg << matches.back();                                               \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_REFERENCE_NOT_FOUND( val, errors )                         \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Reference Slot error : "                                     \
        << "Reference Slot type " << val.label() << " not found";        \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#define O3PRM_REFERENCE_AMBIGUOUS( val, matches, errors )                \
  {                                                                      \
    const auto& pos = val.position();                                    \
    auto msg = std::stringstream();                                      \
    msg << "Reference Slot error : "                                     \
        << "Name " << val.label() << " is ambiguous: ";                  \
    for ( auto i = 0; i < matches.size() - 1; ++i ) {                    \
      msg << matches[i]->name() << ", ";                                 \
    }                                                                    \
    msg << matches.back()->name();                                       \
    errors->addError( msg.str(), pos.file(), pos.line(), pos.column() ); \
  }

#endif  // GUM_PRM_O3PRM_ERRORS_H

