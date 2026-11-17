/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

/**
 * @file
 * @brief This file contains definition for a PRMexplorer (read-only access to
 *PRM)
 *
 * @author Pierre-Henri WUILLEMIN and Yangfan XIANG
 */
#include <Python.h>
#include <string>
#include <vector>

#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/PRM.h>

template < typename GUM_SCALAR >
class classElement;

template < typename GUM_SCALAR >
class Aggregate;

class PRMexplorer {
  public:
  /**
   * Create an explorer
   */
  PRMexplorer(void) {}

  ~PRMexplorer(void) {}

  /**
   * Fill an explorer from a filename and a classpath
   * @param filename
   * @param classpath
   * @param verbose to print warnings and errors
   */
  void load(std::string filename, std::string classpath = "", bool verbose = false) {
    _prm_ = nullptr;

    std::stringstream stream;

    _reader_.setClassPath(classpath);

    _reader_.readFile(filename);   // returns the number of errors and warnings

    _reader_.showElegantErrorsAndWarnings(stream);
    if (_reader_.errors() > 0) {   // warnings alone do not prevent the loading
      _reader_.showErrorCounts(stream);
      GUM_ERROR(gum::FatalError, stream.str())
    }
    if (verbose) { std::cout << stream.str() << std::endl; }

    _prm_ = _reader_.prm();
  }

  /**
   * @param name The name of a possible Type<GUM_SCALAR> in this PRM.
   * @return Returns true if name names a Type<GUM_SCALAR> in this PRM.
   */
  PyAgrumBool* isType(std::string name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return PyBool_FromLong(_prm_->isType(name));
  }

  /**
   * @param name The name of a possible Class<GUM_SCALAR> in this PRM.
   * @return Returns true if name names a Class<GUM_SCALAR> in this PRM.
   */
  PyAgrumBool* isClass(std::string name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return PyBool_FromLong(_prm_->isClass(name));
  }

  /**
   * @param name The name of a possible Interface<GUM_SCALAR> in this PRM.
   * @return Returns true if name names a Interface<GUM_SCALAR> in this PRM.
   */
  PyAgrumBool* isInterface(std::string name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return PyBool_FromLong(_prm_->isInterface(name));
  }

  /************CLASS**************/
  // Every method returning a Python object returns nullptr, with the Python error set, if it could
  // not be built (see PyAgrumHelper in helpers.h).

  /**
   * @return a list of class names
   */
  PyAgrumListOfStr* classes() {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return namesOf_(_prm_->classes());
  }

  /**
   * @return a list of Tuplet(typename, attribute_name, depenson)
   * @param class_name : the name of the class
   */
  PyAgrumList* classAttributes(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto&    c   = _prm_->getClass(class_name);
    gum::DAG gra = c.containerDag();

    PyAgrumHelper::PyOwnedRef q(PyList_New(0));
    if (q.get() == nullptr) return nullptr;
    for (auto a: c.attributes()) {
      // remove automatically created attributes (cast-descendant)
      if (&(c.get(a->name())) != a) continue;
      PyObject* depenson
          = PyAgrumHelper::PyListFrom(gra.parents(a->id()), [&c](gum::NodeId parentId) {
              return PyAgrumHelper::PyStr(c.get(parentId).name());
            });
      // "N" takes the reference, and Py_BuildValue fails if it is nullptr
      PyObject* uplet
          = Py_BuildValue("(ssN)", a->type().name().c_str(), a->name().c_str(), depenson);
      if (!PyAgrumHelper::appendStolen(q.get(), uplet)) return nullptr;
    }
    return q.release();
  }

  /**
   * @param class_name : the name of the class
   * @param att_name : the name of the attribute contained in this class
   */
  PyAgrumBool* isAttribute(std::string class_name, std::string att_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto& ob = _prm_->getClass(class_name).get(att_name);

    return PyBool_FromLong(gum::prm::PRMClassElement< double >::isAttribute(ob));
  }

  /**
   * @return a list of Tuplet(typename,reference_name, isArray)
   * @param class_name : the name of the class
   */
  PyAgrumList* classReferences(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return referencesOf_(_prm_->getClass(class_name).referenceSlots());
  }

  /**
   * @return a list of parameters from a class
   * @param class_name : the name of the class
   */
  PyAgrumListOfStr* classParameters(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return PyAgrumHelper::PyListFrom(_prm_->getClass(class_name).parameters(), [](const auto* c) {
      return PyAgrumHelper::PyStr(c->safeName());
    });
  }

  /**
   * @return a list of interface which the class implemented
   * @param class_name : the name of the class
   */
  PyAgrumListOfStr* classImplements(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto& c = _prm_->getClass(class_name);
    try {
      // raise NotFound if this class doesn't implement any interface
      return namesOf_(c.implements());
    } catch (gum::NotFound&) { return PyList_New(0); }
  }

  /**
   * @return a list of Tuplet(typename, name, agg_type, agg_label=None,
   * applies_label_list)
   * @param class_name : the name of the class
   */
  PyAgrumList* classAggregates(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto&    c   = _prm_->getClass(class_name);
    gum::DAG gra = c.containerDag();

    return PyAgrumHelper::PyListFrom(c.aggregates(), [&c, &gra](const auto* a) {
      PyObject* param = PyAgrumHelper::PyListFrom(gra.parents(a->id()), [&c](gum::NodeId parentId) {
        return PyAgrumHelper::PyStr(c.get(parentId).name());
      });
      const std::string label = a->hasLabel() ? a->labelValue() : "";
      // "z" gives None for a null pointer
      return Py_BuildValue("(ssszN)",
                           a->type().name().c_str(),
                           a->name().c_str(),
                           _aggTypeNames_[static_cast< int >(a->agg_type())],
                           a->hasLabel() ? label.c_str() : nullptr,
                           param);
    });
  }

  /**
   * @return a list of Tuplet(typename, slotname, isMultiple)
   * @param class_name : the name of the class
   */
  PyAgrumList* classSlotChains(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return PyAgrumHelper::PyListFrom(_prm_->getClass(class_name).slotChains(), [](const auto* c) {
      return Py_BuildValue("(ssO)",
                           c->type().name().c_str(),
                           c->name().c_str(),
                           c->isMultiple() ? Py_True : Py_False);
    });
  }

  /**
   * @param class_name : the name of the class
   */
  PyObject* classDag(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto&     c   = _prm_->getClass(class_name);
    gum::DAG  gra = c.containerDag();
    PyObject* d   = PyAgrumHelper::PyDictFrom(
        gra.nodes(),
        PyAgrumHelper::PyIntFromNodeId,
        [&c](gum::NodeId id) { return PyAgrumHelper::PyStr(c.get(id).name()); });
    return Py_BuildValue("(Ns)", d, gra.toDot().c_str());
  }

  /**
   * @return a list of 3-uplets (nameofsystem,dictinary of ids  their name &
   * type,list of arcs:[(tail,head),..] )
   */
  PyAgrumList* getalltheSystems() {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return PyAgrumHelper::PyListFrom(_prm_->systems(), [](const auto* a_system) {
      auto      graph = a_system->skeleton();
      PyObject* n     = PyAgrumHelper::PyDictFrom(graph.nodes(),
                                                  PyAgrumHelper::PyIntFromNodeId,
                                                  [a_system](gum::NodeId node) {
                                                return Py_BuildValue(
                                                    "(ss)",
                                                    a_system->get(node).name().c_str(),
                                                    a_system->get(node).type().name().c_str());
                                                  });
      PyObject* a     = PyAgrumHelper::PyListFrom(graph.arcs(), PyAgrumHelper::PyTupleFromArc);
      return Py_BuildValue("(sNN)", a_system->name().c_str(), n, a);
    });
  }

  /**
   * @return the name of the super class
   * @param class_name : the name of the class
   */
  PyAgrumStr* getSuperClass(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto& c = _prm_->getClass(class_name);
    try {
      // raise NotFound if this class haven't super
      return PyAgrumHelper::PyStr(c.super().name());
    } catch (gum::NotFound&) { return Py_NewRef(Py_None); }
  }

  /**
   * @return the list of subclass name
   * @param class_name : the name of the class
   */
  PyAgrumListOfStr* getDirectSubClass(std::string class_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return namesOf_(_prm_->getClass(class_name).extensions());
  }

  /**
   * @return the tensor of an attribute in a class
   * @param classname : the name of the class   *
   * @param attribute : the name of the attribute
   */
  const gum::Tensor< double >& cpf(std::string class_name, std::string attribute) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }
    return _prm_->getClass(class_name).get(attribute).cpf();
  }

  /************TYPE**************/

  /**
   * @return the list of type names
   */
  PyAgrumListOfStr* types() {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return namesOf_(_prm_->types());
  }

  /**
   * @return the name of super type
   * @param type_name : the name of the type
   */
  PyAgrumStr* getSuperType(std::string type_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto& c = _prm_->type(type_name);
    if (c.isSubType()) {
      return PyAgrumHelper::PyStr(c.superType().name());
    } else {
      return Py_NewRef(Py_None);
    }
  }

  /**
   * @return the list of subtype name
   * @param type_name : the name of the type
   */
  PyAgrumListOfStr* getDirectSubTypes(std::string type_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto&                     selected = _prm_->type(type_name);
    PyAgrumHelper::PyOwnedRef l(PyList_New(0));
    if (l.get() == nullptr) return nullptr;
    for (auto c: _prm_->types()) {
      if (c->isSubType() && c->superType().name() == selected.name()) {
        if (!PyAgrumHelper::appendStolen(l.get(), PyAgrumHelper::PyStr(c->name()))) return nullptr;
      }
    }
    return l.release();
  }

  /**
   * @return the list of labels name
   * @param type_name : the name of the type
   */
  PyAgrumListOfStr* getLabels(std::string type_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return PyAgrumHelper::PyListFrom(_prm_->type(type_name)->labels(), PyAgrumHelper::PyStr);
  }

  /**
   * @return the dictionaty which the value is the super type of the key
   * @param type_name : the name of the type
   */
  PyObject* getLabelMap(std::string type_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto& selected = _prm_->type(type_name);
    if (!selected.isSubType()) { return Py_NewRef(Py_None); }
    PyAgrumHelper::PyOwnedRef d(PyDict_New());
    if (d.get() == nullptr) return nullptr;
    auto  typeLabelVector         = selected.variable().labels();
    auto  superTypeLabelVector    = selected.superType().variable().labels();
    auto& labelMapTypeToSuperType = selected.label_map();
    for (std::size_t i = 0; i != labelMapTypeToSuperType.size(); i++) {
      if (!PyAgrumHelper::setItemStolen(
              d.get(),
              PyAgrumHelper::PyStr(typeLabelVector[i]),
              PyAgrumHelper::PyStr(superTypeLabelVector[labelMapTypeToSuperType[i]])))
        return nullptr;
    }
    return d.release();
  }

  /************INTERFACE**************/

  /**
   * @return the list of interface names
   */
  PyAgrumListOfStr* interfaces() {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return namesOf_(_prm_->interfaces());
  }

  /**
   * @return a list of attribute names from a interface
   * @param interface_name : the name of the interface
   * @param allAttributes : even automatically generated attributes
   */
  PyAgrumList* interAttributes(std::string interface_name, bool allAttributes = false) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto&                     inter = _prm_->getInterface(interface_name);
    PyAgrumHelper::PyOwnedRef q(PyList_New(0));
    if (q.get() == nullptr) return nullptr;
    for (auto c: inter.attributes()) {
      // remove automatically created attributes (cast-descendant)
      if (!allAttributes && &(inter.get(c->name())) != c) continue;
      PyObject* uplet = Py_BuildValue("(ss)", c->type().name().c_str(), c->name().c_str());
      if (!PyAgrumHelper::appendStolen(q.get(), uplet)) return nullptr;
    }
    return q.release();
  }

  /**
   * @return a list of attribute names from a interface
   * @param interface_name : the name of the interface
   * @param allAttributes : even automatically generated attributes
   */
  PyAgrumList* interReferences(std::string interface_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return referencesOf_(_prm_->getInterface(interface_name).referenceSlots());
  }

  /**
   * @return the name of super interface
   * @param interface_name : the name of the interface
   */
  PyAgrumStr* getSuperInterface(std::string interface_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto& c = _prm_->getInterface(interface_name);
    try {
      // raise NotFound if this interface haven't super
      return PyAgrumHelper::PyStr(c.super().name());
    } catch (gum::NotFound&) { return Py_NewRef(Py_None); }
  }

  /**
   * @return the list of subinterface name
   * @param interface_name : the name of the interface
   */
  PyAgrumListOfStr* getDirectSubInterfaces(std::string interface_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    auto&                     selected = _prm_->getInterface(interface_name);
    PyAgrumHelper::PyOwnedRef l(PyList_New(0));
    if (l.get() == nullptr) return nullptr;
    for (auto c: _prm_->interfaces()) {
      try {
        // raise NotFound if this interface haven't super
        if (c->super().name() != selected.name()) continue;
      } catch (gum::NotFound&) { continue; }
      if (!PyAgrumHelper::appendStolen(l.get(), PyAgrumHelper::PyStr(c->name()))) return nullptr;
    }
    return l.release();
  }

  /**
   * @return the list of class name how implement this interface
   * @param interface_name : the name of the interface
   */
  PyAgrumListOfStr* getImplementations(std::string interface_name) {
    if (_prm_ == nullptr) { GUM_ERROR(gum::FatalError, "No loaded prm.") }

    return namesOf_(_prm_->getInterface(interface_name).implementations());
  }


  private:
  // names of the values of gum::prm::PRMAggregate::AggregateType, in the order of the enum
  static constexpr const char* _aggTypeNames_[]
      = {"min", "max", "count", "exists", "forall", "or", "and", "amplitude", "median", "sum"};

  // list of the names of the elements (pointers) of range
  template < typename RANGE >
  static PyObject* namesOf_(const RANGE& range) {
    return PyAgrumHelper::PyListFrom(range,
                                     [](const auto* e) { return PyAgrumHelper::PyStr(e->name()); });
  }

  // list of (typename, reference_name, isArray) of reference slots
  template < typename RANGE >
  static PyObject* referencesOf_(const RANGE& slots) {
    return PyAgrumHelper::PyListFrom(slots, [](const auto* r) {
      return Py_BuildValue("(ssO)",
                           r->slotType().name().c_str(),
                           r->name().c_str(),
                           r->isArray() ? Py_True : Py_False);
    });
  }

  gum::prm::PRM< double >*               _prm_ = nullptr;   // reader.prm() once a prm is loaded
  gum::prm::o3prm::O3prmReader< double > _reader_;
};
