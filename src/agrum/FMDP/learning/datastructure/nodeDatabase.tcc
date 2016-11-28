/***************************************************************************
 *  Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN   *
 *  {prenom.nom}_at_lip6.fr                                                *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.               *
 ***************************************************************************/
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
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeDatabase<AttributeSelection, isScalar>::NodeDatabase(
      const Set<const DiscreteVariable*>* attrSet, const DiscreteVariable* value )
      : __value( value ) {

    GUM_CONSTRUCTOR( NodeDatabase );

    for ( SetIteratorSafe<const DiscreteVariable*> varIter = attrSet->cbeginSafe();
          varIter != attrSet->cendSafe();
          ++varIter )
      __attrTable.insert( *varIter, new TestPolicy<ValueType>() );

    __nbObservation = 0;
  }


  // ###################################################################
  // Default desstructor
  // ###################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeDatabase<AttributeSelection, isScalar>::~NodeDatabase() {

    for ( auto varIter = __attrTable.beginSafe(); varIter != __attrTable.endSafe();
          ++varIter )
      delete varIter.val();

    GUM_DESTRUCTOR( NodeDatabase );
  }


  // ==========================================================================
  // Observation handling methods
  // ==========================================================================

  // ###################################################################
  /* Updates database with new observation
   *
   * Calls either @fn __addObservation( const Observation*, Int2Type<true>)
   * or @fn __addObservation( const Observation*, Int2Type<false>)
   * depending on if we're learning reward function or transition probability
   */
  // ###################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  void NodeDatabase<AttributeSelection, isScalar>::addObservation(
      const Observation* newObs ) {
    __nbObservation++;
    this->__addObservation( newObs, Int2Type<isScalar>() );
  }

  template <TESTNAME AttributeSelection, bool isScalar>
  void NodeDatabase<AttributeSelection, isScalar>::__addObservation(
      const Observation* newObs, Int2Type<true> ) {

    for ( auto varIter = __attrTable.cbeginSafe();
          varIter != __attrTable.cendSafe();
          ++varIter )
      varIter.val()->addObservation( newObs->rModality( varIter.key() ),
                                     newObs->reward() );

    if ( __valueCount.exists( newObs->reward() ) )
      __valueCount[newObs->reward()]++;
    else
      __valueCount.insert( newObs->reward(), 1 );
  }

  template <TESTNAME AttributeSelection, bool isScalar>
  void NodeDatabase<AttributeSelection, isScalar>::__addObservation(
      const Observation* newObs, Int2Type<false> ) {

    for ( auto varIter = __attrTable.cbeginSafe();
          varIter != __attrTable.cendSafe();
          ++varIter )
      varIter.val()->addObservation( newObs->modality( varIter.key() ),
                                     newObs->modality( __value ) );

    if ( __valueCount.exists( newObs->modality( __value ) ) )
      __valueCount[newObs->modality( __value )]++;
    else
      __valueCount.insert( newObs->modality( __value ), 1 );
  }


  // ==========================================================================
  // Aggregation Methods
  // ==========================================================================


  // ###################################################################
  // Merges given NodeDatabase informations into current nDB.
  // ###################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeDatabase<AttributeSelection, isScalar>&
  NodeDatabase<AttributeSelection, isScalar>::
  operator+=( const NodeDatabase<AttributeSelection, isScalar>& src ) {
    this->__nbObservation += src.nbObservation();

    for ( auto varIter = __attrTable.beginSafe(); varIter != __attrTable.endSafe();
          ++varIter )
      varIter.val()->add( *( src.testPolicy( varIter.key() ) ) );

    for ( auto valIter = src.cbeginValues(); valIter != src.cendValues();
          ++valIter )
      if ( __valueCount.exists( valIter.key() ) )
        __valueCount[valIter.key()] += valIter.val();
      else
        __valueCount.insert( valIter.key(), valIter.val() );

    return *this;
  }


  template <TESTNAME AttributeSelection, bool isScalar>
  std::string NodeDatabase<AttributeSelection, isScalar>::toString() const {
    std::stringstream ss;

    ss << "NbObservation : " << this->nbObservation() << std::endl;
    for ( auto varIter = __attrTable.beginSafe(); varIter != __attrTable.endSafe();
          ++varIter )
      ss << "\t\tVariable : " << varIter.key()->name()
         << " - Associated Test : " << __attrTable[varIter.key()]->toString()
         << std::endl;

    return ss.str();
  }
}  // End of namespace gum


// LEFT HERE ON PURPOSE
// NOT TO BE DELETED

/*template<TESTNAME AttributeSelection, bool isScalar>
double *NodeDatabase<AttributeSelection, isScalar>::effectif(){
  double* ret = static_cast<double*>(
SmallObjectAllocator::instance().allocate(sizeof(double)*__value->domainSize()));
  for(Idx modality = 0; modality < __value->domainSize(); ++modality)
    if( __valueCount.exists(modality) )
      ret[modality] = (double)__valueCount[modality];
    else
      ret[modality] = 0.0;
  return ret;
}*/

/*template<TESTNAME AttributeSelection, bool isScalar>
double NodeDatabase<AttributeSelection, isScalar>::reward(){
  double ret = 0.0;
  for(auto valuTer = __valueCount.cbeginSafe(); valuTer !=
__valueCount.cendSafe(); ++valuTer)
    ret += valuTer.key() * (double) valuTer.val();
  return ret / __nbObservation;
}*/
