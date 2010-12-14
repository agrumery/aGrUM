/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/** @file
 * @brief A class to combine efficiently several ScheduleMultiDims
 *
 * MultiDimCombinationBasic is a class designed to combine efficiently
 * several ScheduleMultiDims, that is, to compute expressions like
 * T1 op T2 op T3 op .... op Tn, where the Ti's are either the ScheduleMultiDims
 * or just MultiDimImplementations that will be later wrapped into
 * ScheduleMultiDims and op is an operator or a function taking in argument two
 * such objects and producing a new (combined) Ti object. By default, the
 * combination operation "op" is assumed to be COMMUTATIVE and ASSOCIATIVE.
 *
 * To be quite generic, the MultiDimCombinationBasic takes in argument the
 * function that produces the result of the combination of two multidimensional
 * objects. The following code gives an example of the usage of
 * MultiDimCombinationBasics:
 * @code
 * // a function used to combine two MultiDimImplementation<float>'s:
 * MultiDimImplementation<float>* add ( const MultiDimImplementation<float>& t1,
 *                                      const MultiDimImplementation<float>& t2 ) {
 *   return new MultiDimImplementation<float> (t1 + t2);
 * }
 *
 * // another function used to combine two MultiDimImplementation<float>'s:
 * MultiDimImplementation<float>*
 * mult ( const MultiDimImplementation<float>& t1,
 *        const MultiDimImplementation<float>& t2 ) {
 *   return new MultiDimImplementation<float> (t1 * t2);
 * }
 *   
 *
 * MultiDimImplementation<float> t1, t2, t3;
 * Set<const MultiDimImplementation<float>*> set;
 * set << &t1 << &t2 << &t3;
 * ScheduleCombinationBasic<float> Comb ( add );
 * ScheduleMultiDim<float> combined_table = Comb.combine ( set );
 *
 * // do the same thing only with ScheduleMultiDims
 * MultiDimImplementation<float> tt4, tt5;
 * ScheduleMultiDim<float> t4 ( &tt4 ), t5 ( &tt5 );
 * Set<const ScheduleMultiDim<float>*> set;
 * set << &t4 << &t5;
 * ScheduleCombinationBasic<float> Comb ( add );
 * ScheduleMultiDim<float> combined_table2 = Comb.combine ( set );
 *
 * // change the operator to apply
 * Comb.setCombinator ( mult );
 * ScheduleMultiDim<float> combined_table3 = Comb.combine ( set );
 *
 * @endcode
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SCHEDULE_COMBINATION_BASIC_H
#define GUM_SCHEDULE_COMBINATION_BASIC_H


#include <agrum/core/sequence.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/graphicalModels/inference/scheduleCombination.h>


namespace gum {


  template<typename T_DATA>
  class ScheduleCombinationBasic : public ScheduleCombination<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleCombinationBasic ( MultiDimImplementation<T_DATA>* (*combine)
                                 ( const MultiDimImplementation<T_DATA>&,
                                   const MultiDimImplementation<T_DATA>& ) );

    /// copy constructor
    ScheduleCombinationBasic ( const ScheduleCombinationBasic<T_DATA>& );

    /// destructor
    virtual ~ScheduleCombinationBasic ();

    /// virtual constructor
    /** @return a new fresh ScheduleCombinator with the same combination
     * function. */
    virtual ScheduleCombinationBasic<T_DATA>* newFactory () const;

    /// @}

    
    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    // adds to a given schedule the operations necessary to perform a combination
    /** @returns the ScheduleMultiDim resulting from the combination
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements */
    virtual ScheduleMultiDim<T_DATA>
    combine ( const Set<const ScheduleMultiDim<T_DATA>*>& set,
              Schedule<T_DATA>& );    
    ScheduleMultiDim<T_DATA>
    combine ( const Set<const MultiDimImplementation<T_DATA>*>& set,
              Schedule<T_DATA>& );
    template <template<typename> class TABLE>
    ScheduleMultiDim<T_DATA>
    combine ( const Set<const TABLE<T_DATA>*>& set,
              Schedule<T_DATA>& );

    /// changes the function used for combining two TABLES
    virtual void
    setCombinator( MultiDimImplementation<T_DATA>*
                   (*combine) ( const MultiDimImplementation<T_DATA>&,
                                const MultiDimImplementation<T_DATA>& ) );

    /** @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination */
    virtual float nbOperations ( const Set<const ScheduleMultiDim<T_DATA>*>& set,
                                 const Schedule<T_DATA>& schedule );
    float nbOperations ( const Set<const MultiDimImplementation<T_DATA>*>& set,
                         const Schedule<T_DATA>& schedule );
    template <template<typename> class TABLE>
    float nbOperations ( const Set<const TABLE<T_DATA>*>& set,
                         const Schedule<T_DATA>& schedule );

    /// @}


  protected:
    /// the function used to combine two tables
    MultiDimImplementation<T_DATA>*
    (*_combine) ( const MultiDimImplementation<T_DATA>& t1,
                  const MultiDimImplementation<T_DATA>& t2 );
    
    
    /** @brief returns the domain size of the Cartesian product of the union of
     * all the variables in seq1 and seq2 */
    Size _combined_size ( const Sequence<const DiscreteVariable *>& seq1,
                          const Sequence<const DiscreteVariable *>& seq2 ) const;

 };


} /* namespace gum */


// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleCombinationBasic.tcc>



#endif /* GUM_SCHEDULE_COMBINATION_BASIC_H */
