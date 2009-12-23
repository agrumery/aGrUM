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
Type&
PRM::getType(const std::string& name) { return *(__typeMap[name]); }

INLINE
const Type&
PRM::getType(const std::string& name) const { return *(__typeMap[name]); }

INLINE
Class&
PRM::getClass(const std::string& name) { return *(__classMap[name]); }

INLINE
const Class&
PRM::getClass(const std::string& name) const { return *(__classMap[name]); }

INLINE
const Sequence<Class*>&
PRM::classes() const { return __classes; }

INLINE
const Sequence<Type*>&
PRM::types() const { return __types; }

INLINE
Model&
PRM::getModel(const std::string& name) { return *(__modelMap[name]); }

INLINE
const Model&
PRM::getModel(const std::string& name) const { return *(__modelMap[name]); }

INLINE
const Sequence<Model*>&
PRM::models() const { return __models; }

} /* namespace prm */
} /* namespace gum */
// ============================================================================
