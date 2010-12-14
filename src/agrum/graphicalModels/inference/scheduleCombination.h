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
 * @brief A generic interface to combine efficiently several ScheduleMultiDims
 *
 * MultiDimCombination is a generic interface designed to combine efficiently
 * several ScheduleMultiDims, that is, to compute expressions like
 * T1 op T2 op T3 op .... op Tn, where the Ti's are either the ScheduleMultiDims
 * or just MultiDimImplementations that will be later wrapped into
 * ScheduleMultiDims and op is an operator or a function taking in argument two
 * such objects and producing a new (combined) Ti object. By default, the
 * combination operation "op" is assumed to be COMMUTATIVE and ASSOCIATIVE.
 *
 * To be quite generic, the MultiDimCombination takes in argument the function
 * that produces the result of the combination of two multidimensional objects.
 * The following code gives an example of the usage of MultiDimCombinations:
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

#ifndef GUM_SCHEDULE_COMBINATION_H
#define GUM_SCHEDULE_COMBINATION_H


#include <utility>
#include <agrum/core/set.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/graphicalModels/inference/scheduleMultiDim.h>
#include <agrum/graphicalModels/inference/schedule.h>


namespace gum {


  template<typename T_DATA>
  class ScheduleCombination {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ScheduleCombination ();

    /// copy constructor
    ScheduleCombination ( const ScheduleCombination<T_DATA>& );

    /// destructor
    virtual ~ScheduleCombination ();

    /// virtual constructor
    /** @return a new fresh ScheduleCombinator with the same combination
     * function. */
    virtual ScheduleCombination<T_DATA>* newFactory () const = 0;

    /// @}

    
    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    // adds operations to an already created schedule
    /**  @returns the ScheduleMultiDim resulting from the combination
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements */
    virtual ScheduleMultiDim<T_DATA>
    combine ( const Set<const ScheduleMultiDim<T_DATA>*>& set,
              Schedule<T_DATA>& schedule ) = 0;
    ScheduleMultiDim<T_DATA>
    combine ( const Set<const MultiDimImplementation<T_DATA>*>& set,
              Schedule<T_DATA>& schedule );
    template <template<typename> class TABLE>
    ScheduleMultiDim<T_DATA>
    combine ( const Set<const TABLE<T_DATA>*>& set,
              Schedule<T_DATA>& schedule );
   
    /// changes the function used for combining two TABLES
    virtual void
    setCombinator( MultiDimImplementation<T_DATA>*
                   (*combine) ( const MultiDimImplementation<T_DATA>&,
                                const MultiDimImplementation<T_DATA>& ) ) = 0;

    /** @brief returns a rough estimate of the number of operations that will be
     * performed to compute the combination */
    virtual float nbOperations ( const Set<const ScheduleMultiDim<T_DATA>*>& set,
                                 const Schedule<T_DATA>& schedule ) = 0;
    float nbOperations ( const Set<const MultiDimImplementation<T_DATA>*>& set,
                         const Schedule<T_DATA>& schedule );
    template <template<typename> class TABLE>
    float nbOperations ( const Set<const TABLE<T_DATA>*>& set,
                         const Schedule<T_DATA>& schedule );

    /// @}


  private:
    /// forbid copy operators
    ScheduleCombination<T_DATA>& operator= ( const ScheduleCombination<T_DATA>& );
    
 };


} /* namespace gum */


// always include the template implementation
#include <agrum/graphicalModels/inference/scheduleCombination.tcc>



#endif /* GUM_SCHEDULE_COMBINATION_H */
