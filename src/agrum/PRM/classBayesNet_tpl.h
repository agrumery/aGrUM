/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#pragma once


/**
 * @file
 * @brief Inline implementation of ClassBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/core/inline.h>
#include <agrum/PRM/classBayesNet.h>   // to ease IDE parser

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    void ClassBayesNet< GUM_SCALAR >::_init_(const PRMClass< GUM_SCALAR >& c) {
      for (const auto node: c.containerDag().nodes()) {
        try {
          // Adding the attribute
          if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(node))
              || PRMClassElement< GUM_SCALAR >::isAggregate(c.get(node))) {
            const PRMClassElement< GUM_SCALAR >& elt = c.get(node);
            this->dag_.addNodeWithId(elt.id());
            this->_varNodeMap_.insert(&(elt.type().variable()), &elt);
          }
        } catch (NotFound const&) {
          // Not an attribute
        }
      }

      for (const auto& arc: c.containerDag().arcs()) {
        try {
          this->dag_.addArc(arc.tail(), arc.head());
        } catch (InvalidNode const&) {
          // Not added means not an attribute
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >::ClassBayesNet(const PRMClass< GUM_SCALAR >& c) :
        IBayesNet< GUM_SCALAR >(), _class_(&c) {
      GUM_CONSTRUCTOR(ClassBayesNet);
      _init_(c);
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >::ClassBayesNet(const ClassBayesNet< GUM_SCALAR >& from) :
        IBayesNet< GUM_SCALAR >(from), _class_(from._class_) {
      GUM_CONS_CPY(ClassBayesNet);
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >::~ClassBayesNet() {
      GUM_DESTRUCTOR(ClassBayesNet);
    }

    template < typename GUM_SCALAR >
    INLINE ClassBayesNet< GUM_SCALAR >&
           ClassBayesNet< GUM_SCALAR >::operator=(const ClassBayesNet< GUM_SCALAR >& from) {
      if (this != &from) {
        IBayesNet< GUM_SCALAR >::operator=(from);

        _class_ = from._class_;
      }

      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE const Tensor< GUM_SCALAR >& ClassBayesNet< GUM_SCALAR >::cpt(NodeId varId) const {
      return _get_(varId).cpf();
    }

    template < typename GUM_SCALAR >
    INLINE const VariableNodeMap& ClassBayesNet< GUM_SCALAR >::variableNodeMap() const {
      GUM_ERROR(FatalError, "Sorry no VarMap in a ClassBayesNet.")
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable& ClassBayesNet< GUM_SCALAR >::variable(NodeId id) const {
      return _get_(id).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId ClassBayesNet< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
      return _varNodeMap_[&var]->id();
    }

    template < typename GUM_SCALAR >
    INLINE NodeId ClassBayesNet< GUM_SCALAR >::idFromName(const std::string& name) const {
      return _get_(name).id();
    }

    template < typename GUM_SCALAR >
    INLINE const DiscreteVariable&
        ClassBayesNet< GUM_SCALAR >::variableFromName(const std::string& name) const {
      return _get_(name).type().variable();
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 ClassBayesNet< GUM_SCALAR >::_get_(NodeId id) const {
      if (this->dag_.exists(id)) {
        return _class_->get(id);
      } else {
        GUM_ERROR(NotFound, "no element found with that id.")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 ClassBayesNet< GUM_SCALAR >::_get_(const std::string& name) const {
      try {
        return _class_->get(name);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "no element found with that id.") }
    }

    template < typename GUM_SCALAR >
    INLINE const NodeProperty< Size >& ClassBayesNet< GUM_SCALAR >::modalities() const {
      if (_modalities_.empty()) {
        for (const auto node: this->nodes()) {
          _modalities_.insert(node, (Size)variable(node).domainSize());
        }
      }

      return _modalities_;
    }

    template < typename GUM_SCALAR >
    INLINE std::string ClassBayesNet< GUM_SCALAR >::toDot() const {
      std::string       tab = "  ";
      std::stringstream output;
      output << "digraph \"";
      output << _class_->name() << "\" {" << std::endl;

      for (const auto node: this->nodes()) {
        if (this->children(node).size() > 0)
          for (const auto chi: this->children(node)) {
            output << tab << "\"" << variable(node).name() << "\" -> ";
            output << "\"" << variable(chi).name() << "\";" << std::endl;
          }
        else if (this->parents(node).size() == 0) {
          output << tab << "\"" << variable(node).name() << "\";" << std::endl;
        }
      }

      output << "}" << std::endl;
      return output.str();
    }

  } /* namespace prm */
} /* namespace gum */
