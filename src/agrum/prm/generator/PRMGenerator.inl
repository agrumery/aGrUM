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
 * @author Lionel TORTI
 */

namespace gum {
  namespace prm {

    INLINE
    PRMGenerator::PRMGenerator() {
      GUM_CONSTRUCTOR( PRMGenerator );
    }

    INLINE
    PRMGenerator::PRMGenerator( const PRMGenerator& source ):
      _name_gen( source._name_gen ) {
      GUM_CONS_CPY( PRMGenerator );
    }

    INLINE
    PRMGenerator::~PRMGenerator() {
      GUM_DESTRUCTOR( PRMGenerator );
    }

    INLINE
    void
    PRMGenerator::setNameGenerator( const NameGenerator& name_gen ) {
      _name_gen = name_gen;
    }

    INLINE
    const NameGenerator&
    PRMGenerator::getNameGenerator() const {
      return _name_gen;
    }

    INLINE
    NameGenerator::NameGenerator():
      __counters( 3, 1 ) {
      GUM_CONSTRUCTOR( NameGenerator );
    }

    INLINE
    NameGenerator::NameGenerator( const NameGenerator& source ):
      __counters( source.__counters ) {
      GUM_CONS_CPY( NameGenerator );
    }

    INLINE
    NameGenerator::~NameGenerator() {
      GUM_DESTRUCTOR( NameGenerator );
    }

    INLINE
    NameGenerator&
    NameGenerator::operator=( const NameGenerator& source ) {
      __counters = source.__counters;
      return *this;
    }

    INLINE
    std::string
    NameGenerator::nextName( PRMObject::ObjectType type ) {
      std::stringstream s;

      switch ( type ) {
        case PRMObject::all:
        case PRMObject::prm_class:    { s << "class_" << ++( __counters[0] ); break; }

        case PRMObject::prm_interface: { s << "iface_" << ++( __counters[0] ); break; }

        case PRMObject::prm_system:   { s << "sys_"   << ++( __counters[0] ); break; }

        case PRMObject::prm_type:     { s << "type_"  << ++( __counters[0] ); break; }

        case PRMObject::prm_class_elt: { s << "elt_"   << ++( __counters[1] ); break; }

        case PRMObject::prm_instance: { s << "inst_"  << ++( __counters[2] ); break; }

        default: { GUM_ERROR( FatalError, "unknown PRMObject type" ); }
      }

      return s.str();
    }

  } /* namespace prm */
} /* namespace gum */

