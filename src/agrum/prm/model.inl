/**************************************************************************
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
 * @brief Inline implementation of Model.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
PRMObject::ObjectType
Model::obj_type() const { return PRMObject::prm_model; }

INLINE
Size
Model::size() const { return __nameMap.size(); }

INLINE
bool
Model::isInstantiated(const Class& c) const {
  return __instanceMap.exists(const_cast<Class*>(&c));
}

INLINE
bool
Model::isInstance(const std::string& name) const { return __nameMap.exists(name); }

INLINE
bool
Model::isArray(const std::string& name) const { return __arrayMap.exists(name); }

INLINE
Instance&
Model::get(const std::string& name) { return *(__nameMap[name]); }

INLINE
const Instance&
Model::get(const std::string& name) const { return *(__nameMap[name]); }

INLINE
const Set<Instance*>&
Model::get(const Class& type) const { return *(__instanceMap[&const_cast<Class&>(type)]); }

INLINE
void
Model::add(Instance* i)
{
  if (not __nameMap.exists(i->name())) {
    __nameMap.insert(i->name(), i);
    __add(i);
  } else {
    GUM_ERROR(DuplicateElement, i->name());
  }
}

INLINE
Class&
Model::getArrayType(const std::string& name) { return *(__arrayMap[name].first); }

INLINE
const Sequence<Instance*>&
Model::getArray(const std::string& name) const { return *(__arrayMap[name].second); }

INLINE
const Class&
Model::getArrayType(const std::string& name) const { return *(__arrayMap[name].first); }

INLINE
void
Model::add(const std::string& array, Instance* i)
{
  if ( (__nameMap.exists(i->name())) and (i != __nameMap[i->name()]) ) {
    GUM_ERROR(DuplicateElement, i->name());
  }
  try {
    if (i->type().isSubTypeOf(*(__arrayMap[array].first))) {
      __arrayMap[array].second->insert(i);
      add(i);
    } else {
      GUM_ERROR(OperationNotAllowed, "incompatible type for this array.");
    }
  } catch (NotFound&) {
    __arrayMap.insert(array, Model::model_pair(&(i->type()), new Sequence<Instance*>()));
    __arrayMap[array].second->insert(i);
  }
}

INLINE
void
Model::addArray(const std::string& array, Class& type)
{
  if (not __arrayMap.exists(array)) {
    __arrayMap.insert(array, Model::model_pair(&type, new Sequence<Instance*>()));
  } else {
    GUM_ERROR(DuplicateElement, array);
  }
}

INLINE
void
Model::__add(Instance* inst)
{
  try {
    __instanceMap[&(inst->type())]->insert(inst);
  } catch (NotFound&) {
    __instanceMap.insert(&(inst->type()), new Set<Instance*>());
    __instanceMap[&(inst->type())]->insert(inst);
  }
}

INLINE
Model::iterator
Model::begin()
{
  return ModelIterator(*this);
}

INLINE
Model::const_iterator
Model::begin() const
{
  return ModelConstIterator(*this);
}

// ============================================================================
//                            MODEL ITERATOR
// ============================================================================

INLINE
ModelIterator::ModelIterator(Model& model):
  __iter(model.__nameMap.begin()), __model(&model)
{
  GUM_CONSTRUCTOR( ModelIterator );
}

INLINE
ModelIterator::ModelIterator(const ModelIterator& from):
  __iter(from.__iter), __model(from.__model)
{
  GUM_CONS_CPY( ModelIterator );
}

INLINE
ModelIterator::~ModelIterator()
{
  GUM_DESTRUCTOR( ModelIterator );
}

INLINE
ModelIterator&
ModelIterator::operator=(const ModelIterator& from)
{
  __iter = from.__iter;
  return *this;
}

INLINE
ModelIterator&
ModelIterator::operator++()
{
  ++__iter;
  return *this;
}

INLINE
bool
ModelIterator::operator!=(const ModelIterator& from) const
{
  return __iter != from.__iter;
}

INLINE
bool
ModelIterator::operator==(const ModelIterator& from) const
{
  return __iter == from.__iter;
}

INLINE
Instance&
ModelIterator::operator*()
{
  return **__iter;
}

INLINE
const Instance&
ModelIterator::operator*() const
{
  return **__iter;
}

INLINE
Instance*
ModelIterator::operator->()
{
  return *__iter;
}

INLINE
const Instance*
ModelIterator::operator->() const
{
  return *__iter;
}

INLINE
void
ModelIterator::clear()
{
  __iter.clear();
}

INLINE
bool
ModelIterator::isEnd() const
{
  return (__iter == __model->__nameMap.end());
}

// ============================================================================
//                            MODEL CONST ITERATOR
// ============================================================================

INLINE
ModelConstIterator::ModelConstIterator(const Model& model):
  __iter(model.__nameMap.begin()), __model(&model)
{
  GUM_CONSTRUCTOR( ModelConstIterator );
}

INLINE
ModelConstIterator::ModelConstIterator(const ModelConstIterator& from):
  __iter(from.__iter), __model(from.__model)
{
  GUM_CONS_CPY( ModelConstIterator );
}

INLINE
ModelConstIterator::~ModelConstIterator()
{
  GUM_DESTRUCTOR( ModelConstIterator );
}

INLINE
ModelConstIterator&
ModelConstIterator::operator=(const ModelConstIterator& from)
{
  __iter = from.__iter;
  return *this;
}

INLINE
ModelConstIterator&
ModelConstIterator::operator++()
{
  ++__iter;
  return *this;
}

INLINE
bool
ModelConstIterator::operator!=(const ModelConstIterator& from) const
{
  return __iter != from.__iter;
}

INLINE
bool
ModelConstIterator::operator==(const ModelConstIterator& from) const
{
  return __iter == from.__iter;
}

INLINE
const Instance&
ModelConstIterator::operator*() const
{
  return **__iter;
}

INLINE
const Instance*
ModelConstIterator::operator->() const
{
  return *__iter;
}

INLINE
void
ModelConstIterator::clear()
{
  __iter.clear();
}

INLINE
bool
ModelConstIterator::isEnd() const
{
  return (__iter == __model->__nameMap.end());
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
