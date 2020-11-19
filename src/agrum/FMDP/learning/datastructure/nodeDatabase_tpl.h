/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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
  NodeDatabase< AttributeSelection, isScalar >::NodeDatabase(
     const Set< const DiscreteVariable* >* attrSet,
     const DiscreteVariable*               value) :
      value__(value) {
    GUM_CONSTRUCTOR(NodeDatabase);

    for (SetIteratorSafe< const DiscreteVariable* > varIter =
            attrSet->cbeginSafe();
         varIter != attrSet->cendSafe();
         ++varIter)
      attrTable__.insert(*varIter, new TestPolicy< ValueType >());

    nbObservation__ = 0;
  }


  // ###################################################################
  // Default desstructor
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeDatabase< AttributeSelection, isScalar >::~NodeDatabase() {
    for (auto varIter = attrTable__.beginSafe(); varIter != attrTable__.endSafe();
         ++varIter)
      delete varIter.val();

    GUM_DESTRUCTOR(NodeDatabase);
  }


  // ==========================================================================
  // Observation handling methods
  // ==========================================================================

  // ###################################################################
  /* Updates database with new observation
   *
   * Calls either @fn addObservation__( const Observation*, Int2Type<true>)
   * or @fn addObservation__( const Observation*, Int2Type<false>)
   * depending on if we're learning reward function or transition probability
   */
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void NodeDatabase< AttributeSelection, isScalar >::addObservation(
     const Observation* newObs) {
    nbObservation__++;
    this->addObservation__(newObs, Int2Type< isScalar >());
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  void NodeDatabase< AttributeSelection, isScalar >::addObservation__(
     const Observation* newObs, Int2Type< true >) {
    for (auto varIter = attrTable__.cbeginSafe();
         varIter != attrTable__.cendSafe();
         ++varIter)
      varIter.val()->addObservation(newObs->rModality(varIter.key()),
                                    newObs->reward());

    if (valueCount__.exists(newObs->reward()))
      valueCount__[newObs->reward()]++;
    else
      valueCount__.insert(newObs->reward(), 1);
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  void NodeDatabase< AttributeSelection, isScalar >::addObservation__(
     const Observation* newObs, Int2Type< false >) {
    for (auto varIter = attrTable__.cbeginSafe();
         varIter != attrTable__.cendSafe();
         ++varIter)
      varIter.val()->addObservation(newObs->modality(varIter.key()),
                                    newObs->modality(value__));

    if (valueCount__.exists(newObs->modality(value__)))
      valueCount__[newObs->modality(value__)]++;
    else
      valueCount__.insert(newObs->modality(value__), 1);
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
    this->nbObservation__ += src.nbObservation();

    for (auto varIter = attrTable__.beginSafe(); varIter != attrTable__.endSafe();
         ++varIter)
      varIter.val()->add(*(src.testPolicy(varIter.key())));

    for (auto valIter = src.cbeginValues(); valIter != src.cendValues(); ++valIter)
      if (valueCount__.exists(valIter.key()))
        valueCount__[valIter.key()] += valIter.val();
      else
        valueCount__.insert(valIter.key(), valIter.val());

    return *this;
  }


  template < TESTNAME AttributeSelection, bool isScalar >
  std::string NodeDatabase< AttributeSelection, isScalar >::toString() const {
    std::stringstream ss;

    ss << "NbObservation : " << this->nbObservation() << std::endl;
    for (auto varIter = attrTable__.beginSafe(); varIter != attrTable__.endSafe();
         ++varIter)
      ss << "\t\tVariable : " << varIter.key()->name()
         << " - Associated Test : " << attrTable__[varIter.key()]->toString()
         << std::endl;

    return ss.str();
  }
}   // End of namespace gum


// LEFT HERE ON PURPOSE
// NOT TO BE DELETED

/*template<TESTNAME AttributeSelection, bool isScalar>
double *NodeDatabase<AttributeSelection, isScalar>::effectif(){
  double* ret = static_cast<double*>(
SmallObjectAllocator::instance().allocate(sizeof(double)*value__->domainSize()));
  for(Idx modality = 0; modality < value__->domainSize(); ++modality)
    if( valueCount__.exists(modality) )
      ret[modality] = (double)valueCount__[modality];
    else
      ret[modality] = 0.0;
  return ret;
}*/

/*template<TESTNAME AttributeSelection, bool isScalar>
double NodeDatabase<AttributeSelection, isScalar>::reward(){
  double ret = 0.0;
  for(auto valuTer = valueCount__.cbeginSafe(); valuTer !=
valueCount__.cendSafe(); ++valuTer)
    ret += valuTer.key() * (double) valuTer.val();
  return ret / nbObservation__;
}*/
