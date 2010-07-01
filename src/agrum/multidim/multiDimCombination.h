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
 * @brief A generic class to combine efficiently several MultiDim tables
 *
 * MultiDimCombination is a generic class designed to combine efficiently several
 * multidimensional objects, that is, to compute expressions like
 * T1 op T2 op T3 op .... op Tn, where the Ti's are the multidimensional objects
 * and op is an operator or a function taking in argument two such objects and
 * producing a new (combined) Ti object. Note that the MultiDimCombination
 * determines itself in which order the objects should be combined. As such, the
 * combination operation to perform should thus be COMMUTATIVE and ASSOCIATIVE.
 *
 * By multidimensional objects, we mean of course MultiDimDecorators,
 * MultiDimImplementations, but also more complex objects such as, for instance,
 * pairs of MultiDimDecorators the first one of which being a utility function and
 * the second one being a table of instantiations (useful, e.g., for computing
 * MPE's) but this can also be a pair (Utility,Potential) for the inference in
 * an Influence Diagram. Actually, the important point for a multidimensional
 * object to be eligible to be combined by the MultiDimCombination is:
 * # that the object contains a method variablesSequence that returns a 
 *   sequence of Discrete variables that represent the dimensions of the
 *   multidimensional object
 * # that there exists a function taking in arguments two such multidimensional
 *   objects and producing a new object of the same type, which is the so-called
 *   combined result of these two objects.
 *
 * To be quite generic, the MultiDimCombination takes in argument the function
 * that produces the result of the combination of two multidimensional objects.
 * The following code gives an example of the usage of MultiDimCombinations:
 * @code
 * // a function used to combine two Potential<float>'s:
 * Potential<float>* addPotential ( const Potential<float>& t1,
 *                                  const Potential<float>& t2 ) {
 *   return new Potential<float> (t1 + t2);
 * }
 *
 * // another function used to combine two Potential<float>'s:
 * Potential<float>* multPotential ( const Potential<float>& t1,
 *                                   const Potential<float>& t2 ) {
 *   return new Potential<float> (t1 * t2);
 * }
 *   
 *
 * Potential<float> t1, t2, t3;
 * Set<const Potential<float>*> set;
 * set << &table1 << &table2 << &table3;
 * MultiDimCombination<float,Potential> Comb ( addPotential );
 * Potential<float>* combined_table = Comb.combine ( set );
 *
 * // change the operator to apply
 * Comb.setCombinator ( multPotential );
 * Potential<float>* combined_table2 = Comb.combine ( set );
 *
 * @endcode
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MULTI_DIM_COMBINATION_H
#define GUM_MULTI_DIM_COMBINATION_H


#include <agrum/core/set.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecorator.h>


namespace gum {


  template< typename T_DATA, template<typename> class TABLE >
  class MultiDimCombination {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param combine a function that takes two tables in input and produces a
     * new table which is the result of the combination of the two tables
     * passed in argument. */
    MultiDimCombination ( TABLE<T_DATA>* (*combine)
                          ( const TABLE<T_DATA>&,const TABLE<T_DATA>& ) );

    /// copy constructor
    MultiDimCombination ( const MultiDimCombination<T_DATA,TABLE>& );

    /// destructor
    virtual ~MultiDimCombination ();

    /// virtual constructor
    /** @return a new fresh MultiDimCombinator with the same combination
     * function. */
    virtual MultiDimCombination<T_DATA,TABLE>* newFactory () const;

    /// @}

    
    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// creates and returns the result of the combination of the tables within set 
    /** @return a new freshly created TABLE which is the result of the combination
     * of all the TABLES passed in argument
     * @throws InvalidArgumentsNumber exception is thrown if the set passed in
     * argument contains less than two elements */
    virtual TABLE<T_DATA>* combine ( const Set<const TABLE<T_DATA>*>& set );
    virtual void combine ( TABLE<T_DATA>& container ,const Set<const TABLE<T_DATA>*>& set );

    /// changes the function used for combining two TABLES
    void setCombinator( TABLE<T_DATA>*
                        (*combine) ( const TABLE<T_DATA>&,const TABLE<T_DATA>& ) );

    /// @}
      

  private:
    /// the function used to combine two tables
    TABLE<T_DATA>* (*__combine) ( const TABLE<T_DATA>& t1,
                                  const TABLE<T_DATA>& t2 );
    
    
    /** @brief returns the domain size of the Cartesian product of the union of
     * all the variables in seq1 and seq2 */
    Size __combined_size ( const Sequence<const DiscreteVariable *>& seq1,
                           const Sequence<const DiscreteVariable *>& seq2 ) const;

 };


} /* namespace gum */


// always include the template implementation
#include <agrum/multidim/multiDimCombination.tcc>


#endif /* GUM_MULTI_DIM_COMBINATION_H */

