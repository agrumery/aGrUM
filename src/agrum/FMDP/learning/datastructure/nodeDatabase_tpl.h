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
 * @brief Template implémentations of the NodeDatabase class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/datastructure/nodeDatabase.h>

// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  // Default constructor
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeDatabase< AttributeSelection, isScalar >::NodeDatabase(const gum::VariableSet* attrSet,
                                                             const DiscreteVariable* value) :
      _value_(value) {
    GUM_CONSTRUCTOR(NodeDatabase);

    for (SetIteratorSafe< const DiscreteVariable* > varIter = attrSet->cbeginSafe();
         varIter != attrSet->cendSafe();
         ++varIter)
      _attrTable_.insert(*varIter, new TestPolicy< ValueType >());

    _nbObservation_ = 0;
  }

  // ###################################################################
  // Default destructor
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeDatabase< AttributeSelection, isScalar >::~NodeDatabase() {
    for (auto varIter = _attrTable_.beginSafe(); varIter != _attrTable_.endSafe(); ++varIter)
      delete varIter.val();

    GUM_DESTRUCTOR(NodeDatabase);
  }

  // ==========================================================================
  // Observation handling methods
  // ==========================================================================

  // ###################################################################
  /* Updates database with new observation
   *
   * Calls either @fn  _addObservation_( const Observation*, Int2Type<true>)
   * or @fn  _addObservation_( const Observation*, Int2Type<false>)
   * depending on if we're learning reward function or transition probability
   */
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void NodeDatabase< AttributeSelection, isScalar >::addObservation(const Observation* newObs) {
    _nbObservation_++;
    this->_addObservation_(newObs, Int2Type< isScalar >());
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  void NodeDatabase< AttributeSelection, isScalar >::_addObservation_(const Observation* newObs,
                                                                      Int2Type< true >) {
    for (auto varIter = _attrTable_.cbeginSafe(); varIter != _attrTable_.cendSafe(); ++varIter)
      varIter.val()->addObservation(newObs->rModality(varIter.key()), newObs->reward());

    if (_valueCount_.exists(newObs->reward())) _valueCount_[newObs->reward()]++;
    else _valueCount_.insert(newObs->reward(), 1);
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  void NodeDatabase< AttributeSelection, isScalar >::_addObservation_(const Observation* newObs,
                                                                      Int2Type< false >) {
    for (auto varIter = _attrTable_.cbeginSafe(); varIter != _attrTable_.cendSafe(); ++varIter)
      varIter.val()->addObservation(newObs->modality(varIter.key()), newObs->modality(_value_));

    if (_valueCount_.exists(newObs->modality(_value_))) _valueCount_[newObs->modality(_value_)]++;
    else _valueCount_.insert(newObs->modality(_value_), 1);
  }

  // ==========================================================================
  // Aggregation Methods
  // ==========================================================================


  // ###################################################################
  // Merges given NodeDatabase informations into current nDB.
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeDatabase< AttributeSelection, isScalar >&
      NodeDatabase< AttributeSelection, isScalar >::operator+=(
          const NodeDatabase< AttributeSelection, isScalar >& src) {
    this->_nbObservation_ += src.nbObservation();

    for (auto varIter = _attrTable_.beginSafe(); varIter != _attrTable_.endSafe(); ++varIter)
      varIter.val()->add(*(src.testPolicy(varIter.key())));

    for (auto valIter = src.cbeginValues(); valIter != src.cendValues(); ++valIter)
      if (_valueCount_.exists(valIter.key())) _valueCount_[valIter.key()] += valIter.val();
      else _valueCount_.insert(valIter.key(), valIter.val());

    return *this;
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  std::string NodeDatabase< AttributeSelection, isScalar >::toString() const {
    std::stringstream ss;

    ss << "NbObservation : " << this->nbObservation() << std::endl;
    for (auto varIter = _attrTable_.beginSafe(); varIter != _attrTable_.endSafe(); ++varIter)
      ss << "\t\tVariable : " << varIter.key()->name()
         << " - Associated Test : " << _attrTable_[varIter.key()]->toString() << std::endl;

    return ss.str();
  }
}   // End of namespace gum

// LEFT HERE ON PURPOSE
// NOT TO BE DELETED

/*template<TESTNAME AttributeSelection, bool isScalar>
double *NodeDatabase<AttributeSelection, isScalar>::effectif(){
  double* ret = static_cast<double*>(
SmallObjectAllocator::instance().allocate(sizeof(double)* _value_->domainSize()));
  for(Idx modality = 0; modality <  _value_->domainSize(); ++modality)
    if(  _valueCount_.exists(modality) )
      ret[modality] = (double) _valueCount_[modality];
    else
      ret[modality] = 0.0;
  return ret;
}*/

/*template<TESTNAME AttributeSelection, bool isScalar>
double NodeDatabase<AttributeSelection, isScalar>::reward(){
  double ret = 0.0;
  for(auto valuTer =  _valueCount_.cbeginSafe(); valuTer !=
 _valueCount_.cendSafe(); ++valuTer)
    ret += valuTer.key() * (double) valuTer.val();
  return ret /  _nbObservation_;
}*/
