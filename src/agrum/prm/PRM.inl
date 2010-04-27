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
 * @brief Inline implementation of PRM.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
bool
PRM::isType(const std::string name) const { return __typeMap.exists(name); }

INLINE
bool
PRM::isClass(const std::string name) const { return __classMap.exists(name); }

INLINE
bool
PRM::isInterface(const std::string name) const { return __interfaceMap.exists(name); }

INLINE
bool
PRM::isSystem(const std::string name) const { return __systemMap.exists(name); }

INLINE
Type&
PRM::getType(const std::string& name) { return *(__typeMap[name]); }

INLINE
const Type&
PRM::getType(const std::string& name) const { return *(__typeMap[name]); }

INLINE
const Set<Type*>&
PRM::types() const { return __types; }

INLINE
Class&
PRM::getClass(const std::string& name) { return *(__classMap[name]); }

INLINE
const Class&
PRM::getClass(const std::string& name) const { return *(__classMap[name]); }

INLINE
const Set<Class*>&
PRM::classes() const { return __classes; }

INLINE
Interface&
PRM::getInterface(const std::string& name) { return *__interfaceMap[name]; }

INLINE
const Interface&
PRM::getInterface(const std::string& name) const { return *__interfaceMap[name]; }

INLINE
const Set<Interface*>&
PRM::interfaces() const { return __interfaces; }

INLINE
System&
PRM::getSystem(const std::string& name) { return *(__systemMap[name]); }

INLINE
const System&
PRM::getSystem(const std::string& name) const { return *(__systemMap[name]); }

INLINE
const Set<System*>&
PRM::systems() const { return __systems; }

} /* namespace prm */
} /* namespace gum */
// ============================================================================
