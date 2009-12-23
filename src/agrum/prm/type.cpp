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
 * @brief Source implementation of type.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/type.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/type.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

  Type::Type(const DiscreteVariable& var):
    PRMObject(var.name()), __var(var.copyFactory()),
    __super(0), __label_map(0)
  {
    GUM_CONSTRUCTOR( Type );
  }

  Type::Type(const Type& super_type, const std::vector<Idx>& label_map,
             const DiscreteVariable& var):
    PRMObject(var.name()), __var(var.copyFactory()),
    __super(&super_type), __label_map(new std::vector<Idx>(label_map))
  {
    GUM_CONSTRUCTOR( Type );
    bool invalid = false;
    if (__label_map->size() == __var->domainSize()) {
      for (size_t i = 0; i < __label_map->size(); ++i) {
        if (__label_map->at(i) >= (**__super).domainSize()) {
          invalid = true;
          break;
        }
      }
    } else {
      invalid = true;
    }
    if (invalid) {
      delete __label_map;
      GUM_ERROR(OperationNotAllowed, "Invalid label map.");
    }
  }

  Type::Type(const Type& from):
    PRMObject(from), __var(from.__var->copyFactory()),
    __super(from.__super), __label_map(0)
  {
    GUM_CONS_CPY( Type );
    if (not __super) {
      __label_map = new std::vector<Idx>(from.label_map());
    }
  }

  Type::~Type()
  {
    GUM_DESTRUCTOR( Type );
    delete __var;
    if (not __label_map) {
      delete __label_map;
    }
  }

  MultiDimImplementation<prm_float>*
  Type::cast_CPT() const {
    if (__super) {
      MultiDimArray<prm_float>* cpt = new MultiDimArray<prm_float>();
      cpt->add(*__var);
      cpt->add(*(__super->__var));
      Instantiation inst(*cpt);
      for (Idx i = 0; i < __var->domainSize(); ++i) {
        for (Idx j = 0; j < __super->__var->domainSize(); ++j) {
          inst.chgVal(*__var, i);
          inst.chgVal(*(__super->__var), j);
          cpt->set(inst, (__label_map->at(i) == j)?1:0);
        }
      }
      return cpt;
    } else {
      GUM_ERROR(OperationNotAllowed, "No super type for this type.");
    }
  }

  bool
  Type::isSubTypeOf(const Type& super) const {
    if (__super) {
      const Type* tmp = __super;
      while ( (super != (*tmp)) and (tmp) ) {
        tmp = tmp->__super;
      }
      return tmp;
    } else {
      return ((*this) == super);
    }
  }

} /* namespace prm */
} /* namespace gum */
// ============================================================================
