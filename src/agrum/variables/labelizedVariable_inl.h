
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


/** @file
 * @brief Base class for labelized discrete random variables
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#include <ostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/core/hashTable.h>
#include <agrum/variables/discreteVariable.h>

// to ease IDE parsers
#include <agrum/variables/labelizedVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // erase all the labels

  INLINE void LabelizedVariable::eraseLabels() { __labels.clear(); }

  // copies the content of aLDRV

  INLINE void LabelizedVariable::_copy(const LabelizedVariable& aLDRV) {
    DiscreteVariable::_copy(aLDRV);
    __labels.clear();
    __labels = aLDRV.__labels;
  }

  // add a label with a new index (we assume that we will NEVER remove a label)
  INLINE LabelizedVariable&
         LabelizedVariable::addLabel(const std::string& aLabel) {
    __labels.insert(aLabel);

    return *this;
  }

  INLINE void LabelizedVariable::changeLabel(Idx                pos,
                                             const std::string& aLabel) const {
    if (__labels[pos] == aLabel) return;

    if (isLabel(aLabel))
      GUM_ERROR(DuplicateElement, "Label '" << aLabel << "' already exists");

    __labels.setAtPos(pos, aLabel);
  }

  // Default constructor

  INLINE LabelizedVariable::LabelizedVariable(const std::string& aName,
                                              const std::string& aDesc,
                                              const Size         nbrLabel) :
      DiscreteVariable(aName, aDesc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(LabelizedVariable);

    for (Idx i = 0; i < nbrLabel; ++i) {
      std::ostringstream oss;
      oss << i;
      addLabel(oss.str());
    }
  }
  INLINE
  LabelizedVariable::LabelizedVariable(const std::string&                aName,
                                       const std::string&                aDesc,
                                       const std::vector< std::string >& labels) :
      DiscreteVariable(aName, aDesc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(LabelizedVariable);
    __labels.clear();
    for (Idx i = 0; i < labels.size(); ++i)
      __labels.insert(labels[i]);
  }

  INLINE Idx LabelizedVariable::posLabel(const std::string& label) const {
    return __labels.pos(label);
  }

  // Copy constructor

  INLINE
  LabelizedVariable::LabelizedVariable(const LabelizedVariable& aLDRV) :
      DiscreteVariable(aLDRV), __labels(aLDRV.__labels) {
    // for debugging purposes
    GUM_CONSTRUCTOR(LabelizedVariable);
  }

  // destructor

  INLINE LabelizedVariable::~LabelizedVariable() {
    eraseLabels();
    GUM_DESTRUCTOR(LabelizedVariable);
  }

  INLINE
  LabelizedVariable* LabelizedVariable::clone() const {
    return new LabelizedVariable(*this);
  }

  // copy operator
  INLINE LabelizedVariable& LabelizedVariable::
                            operator=(const LabelizedVariable& aLDRV) {
    // avoid self assignment
    if (&aLDRV != this) { _copy(aLDRV); }

    return *this;
  }

  // indicates whether the variable already has the label passed in argument
  INLINE bool LabelizedVariable::isLabel(const std::string& aLabel) const {
    return __labels.exists(aLabel);
  }

  // returns the ith label
  INLINE std::string LabelizedVariable::label(Idx i) const {
    return __labels.atPos(i);
  }

  // get a numerical representation of the indice-th value.
  INLINE double LabelizedVariable::numerical(Idx indice) const {
    return double(indice);
  }


  INLINE Idx LabelizedVariable::index(const std::string& aLabel) const {
    try {
      return __labels.pos(aLabel);
    } catch (...) {
      GUM_ERROR(OutOfBounds,
                "label '" << aLabel << "' is unknown in " << this->toString());
    }
  }

  // returns the size of the random discrete variable domain
  INLINE Size LabelizedVariable::domainSize() const { return __labels.size(); }

  INLINE VarType LabelizedVariable::varType() const { return VarType::Labelized; }

} /* namespace gum */

#endif /* DOXYGEN SHOULD SKIP THIS */
