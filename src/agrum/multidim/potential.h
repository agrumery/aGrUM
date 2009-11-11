/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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
 * @brief Class representing a potential. Using the decorator pattern, this
 * representation is independant from the implementation of the multidimension
 * matrix
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_POTENTIAL_H
#define GUM_POTENTIAL_H

#include <vector>
#include <agrum/core/utils.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/multiDimImplementation.h>


namespace gum {


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                          GUM_PROBA                                  === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class Potential
   * @brief Class representing a potential. Using the decorator pattern, this
   * representation is independant from the implementation of the multidimension
   * matrix 
   * @ingroup multidim_group */
  /* =========================================================================== */
  template<typename T_DATA>
  class Potential : public MultiDimDecorator<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /**
     * Default constructor: creates an empty null dimensional matrix
     * choose a MultiDimArray<> as decorated implementation.
     */
    // ============================================================================
    Potential( );
    
    // ============================================================================
    /**
     * Creates an empty null dimensional matrix.
     * @param aContent decorated implementation  
     */
    // ============================================================================
    explicit Potential( MultiDimImplementation<T_DATA> *aContent );

    // ============================================================================
    /** 
     * Copy constructor.
     * This copy should create a new decorator for the same content ... 
     * Too dangerous. 
     */
    // ============================================================================
    explicit Potential( const Potential<T_DATA>& from );

    // ============================================================================
    /** 
     * Copy constructor.
     *
     * The newly created matrix contains the same variables and the same values as
     * from, but no instantiation is associated to it.
     * @param aContent decorated implementation
     * @param from the multidimensional matrix we copy into this
     */
    // ============================================================================
    explicit Potential( MultiDimImplementation<T_DATA> *aContent, 
                        const MultiDimContainer<T_DATA>& from );

    // ============================================================================
    /**
     * Destructor.
     *
     * Note that, when the multidimensional array is removed from memory, its
     * variables are not removed as well. 
     */
    // ============================================================================
    ~Potential();

    /// @}

    /**
     * This method creates a clone of this object, withouth its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    virtual Potential<T_DATA>* newFactory() const;



    /// @name classic operations on probability in Shafer-Shenoy
    ///@{

    /// normalisation of this do nothing if sum is 0
    Potential<T_DATA>& normalize() const;

    /// marginalizing p over the vars on *this.
    /// @throw OperationNotAllowed if there is var in *this not in p.
    Potential<T_DATA>& marginalize( const Potential<T_DATA>& p ) const;

    /// Multiplication of args.
    /// @throw OperationNotAllowed if *this is not empty
    void multiplicate( const Potential<T_DATA>& p1,const Potential<T_DATA>& p2 ) ;

    /// Multiplication of this and arg (in this).
    Potential<T_DATA>& multiplicateBy( const Potential<T_DATA>& p1 ) ;

    ///@}

    Potential<T_DATA>& operator=( const Potential<T_DATA>& from );


  protected:
    /// perform the marginalization p over the vars on *this.
    void _marginalize( const Potential& p ) const;

    /// perform the multiplication of args.
    void _multiplicate( const Potential& p1,const Potential& p2 ) ;

    /// sum of all elements in this
    const T_DATA getSum() const ;
  };


} /* namespace gum */

  
#include <agrum/multidim/potential.tcc>


#endif /* GUM_POTENTIAL_H */

