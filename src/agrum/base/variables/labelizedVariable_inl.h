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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Base class for labelized discrete random variables
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <ostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/core/hashTable.h>
#include <agrum/base/variables/discreteVariable.h>

// to ease IDE parsers
#include <agrum/base/variables/labelizedVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // erase all the labels

  INLINE void LabelizedVariable::eraseLabels() { _labels_.clear(); }

  // copies the content of aLDRV

  INLINE void LabelizedVariable::copy_(const LabelizedVariable& aLDRV) {
    DiscreteVariable::copy_(aLDRV);
    _labels_.clear();
    _labels_ = aLDRV._labels_;
  }

  // add a label with a new index (we assume that we will NEVER remove a label)
  INLINE LabelizedVariable& LabelizedVariable::addLabel(const std::string& aLabel) {
    _labels_.insert(aLabel);

    return *this;
  }

  INLINE void LabelizedVariable::changeLabel(Idx pos, const std::string& aLabel) const {
    if (_labels_[pos] == aLabel) return;

    if (isLabel(aLabel)) GUM_ERROR(DuplicateElement, "Label '" << aLabel << "' already exists")

    _labels_.setAtPos(pos, aLabel);
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
    _labels_.clear();
    for (Idx i = 0; i < labels.size(); ++i)
      _labels_.insert(labels[i]);
  }

  INLINE Idx LabelizedVariable::posLabel(const std::string& label) const {
    return _labels_.pos(label);
  }

  // Copy constructor

  INLINE
  LabelizedVariable::LabelizedVariable(const LabelizedVariable& aLDRV) :
      DiscreteVariable(aLDRV), _labels_(aLDRV._labels_) {   // for debugging purposes
    GUM_CONSTRUCTOR(LabelizedVariable);
  }

  // destructor

  INLINE LabelizedVariable::~LabelizedVariable() {
    eraseLabels();
    GUM_DESTRUCTOR(LabelizedVariable);
  }

  INLINE
  LabelizedVariable* LabelizedVariable::clone() const { return new LabelizedVariable(*this); }

  // copy operator
  INLINE LabelizedVariable& LabelizedVariable::operator=(const LabelizedVariable& aLDRV) {
    // avoid self assignment
    if (&aLDRV != this) { copy_(aLDRV); }

    return *this;
  }

  // indicates whether the variable already has the label passed in argument
  INLINE bool LabelizedVariable::isLabel(const std::string& aLabel) const {
    return _labels_.exists(aLabel);
  }

  // returns the ith label
  INLINE std::string LabelizedVariable::label(Idx i) const { return _labels_.atPos(i); }

  // get a numerical representation of the indice-th value.
  INLINE double LabelizedVariable::numerical(Idx indice) const { return double(indice); }

  ///  returns the closest index of the value
  INLINE Idx LabelizedVariable::closestIndex(double val) const {
      GUM_ERROR(InvalidArgument, "closestIndex has no meaning for LabelizedVariable")}

  INLINE Idx LabelizedVariable::index(const std::string& aLabel) const {
    try {
      return _labels_.pos(aLabel);
    } catch (...) {
      GUM_ERROR(OutOfBounds, "label '" << aLabel << "' is unknown in " << this->toString())
    }
  }

  // returns the size of the random discrete variable domain
  INLINE Size LabelizedVariable::domainSize() const { return _labels_.size(); }

  INLINE VarType LabelizedVariable::varType() const { return VarType::LABELIZED; }

  INLINE std::string LabelizedVariable::toFast() const {
    std::stringstream s;
    s << name() << domain();
    return s.str();
  }

  INLINE bool LabelizedVariable::_checkSameDomain_(const gum::Variable& aRV) const {
    // we can assume that aRV is a LabelizedVariable
    // we ask for a strict equality, label by label
    const auto& cv = static_cast< const LabelizedVariable& >(aRV);
    if (domainSize() != cv.domainSize()) return false;
    for (Idx i = 0; i < domainSize(); ++i)
      if (label(i) != cv.label(i)) return false;
    return true;
  }

} /* namespace gum */

#endif /* DOXYGEN SHOULD SKIP THIS */
