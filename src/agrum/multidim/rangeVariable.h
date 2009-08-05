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
/**
 * @file
 * @brief Header of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
// ============================================================================
#ifndef GUM_RANGE_VARIABLE_H
#define GUM_RANGE_VARIABLE_H
// ============================================================================
#include <string>
#include <sstream>
#include <ostream>
#include <agrum/core/utils.h>
#include <agrum/core/hashFunc.h>
#include <agrum/multidim/discreteVariable.h>


namespace gum {


  // ==============================================================================
  //                           GUM_DISCRETE_VARIABLE
  // ==============================================================================
  /**
   * @class RangeVariable
   * @brief Defines a discrete random variable over an integer interval.
   *
   * It is technically possible to create RangeVariable with minVal > maxVal
   * (or modify in that way an already created RangeVariable). The result is
   * an empty variable (i.e. empty() returns true).
   * If maxVal - minVal < 0, then domainsize() = 0.
   */
  // ==============================================================================

  class RangeVariable : public DiscreteVariable {
    // ============================================================================
  public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{
    // ========================================================================
    /// Default constructor
    // ========================================================================
    RangeVariable( const std::string& aName, const std::string& aDesc,
                   Idx minVal=0, Idx maxVal=1 );

    // ========================================================================
    /** Copy Constructor.
     *
     * If aDRV haves any listener, it will not be copied.
     *
     * @param aDRV the variable we copy
     */
    // ========================================================================
    RangeVariable( const RangeVariable& aDRV );

    // ========================================================================
    /// destructor
    // ========================================================================
    virtual ~RangeVariable();

    // ========================================================================
    /// Copy Factory.
    /// @return Returns a pointer on a new copy of this.
    // ========================================================================
    virtual DiscreteVariable* copyFactory() const;

    /// @}


    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{
    // ========================================================================
    /// returns the size of the random discrete variable domain
    // ========================================================================
    virtual Size domainSize() const;

    // ========================================================================
    ///
    /**
     * Get the indice-th label. 
     * @param indice the index of the label we wish to return
     * @throw OutOfBound
     */
    // ========================================================================
    virtual const std::string label( const Idx indice ) const;

    // ========================================================================
    /**
     * Returns the lower bound.
     */
    // ========================================================================
    Idx min() const;

    // ========================================================================
    /**
     * Set a new value for the lower bound.
     */
    // ========================================================================
    void setMin( Idx minVal );

    // ========================================================================
    /**
     * Returns the upper bound.
     */
    // ========================================================================
    Idx max() const;

    // ========================================================================
    /**
     * Set a new value of the upper bound.
     */
    // ========================================================================
    void setMax( Idx maxVal );

    // ========================================================================
    /**
     * Returns true if the param belongs to the variable's interval.
     */
    // ========================================================================
    bool belongs( Idx indice ) const;
    
    /**
     * @return the modality from the label
     * @throw NotFound
     */
    virtual Idx operator[](const std::string&) const;

    /// @}


    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{
    // ========================================================================
    /**
     * Copy operator
     * @param aRV to be copied
     * @return a ref to *this */
    // ========================================================================
    RangeVariable& operator=( const RangeVariable& aRV );
    
    /// @}

    virtual const std::string toString() const;
    
  private:
    // ============================================================================
    /// @name Private Members.
    // ============================================================================
    /// @{
    // ========================================================================
    /// The lower bound.
    Idx __min;

    /// The upper bound.
    Idx __max;
    
    /// @}
    
  };

  // ============================================================================
  /// For friendly displaying the content of the variable.
  // ============================================================================
  std::ostream& operator<<( std::ostream&, const RangeVariable&);

  
} /* namespace gum */



#ifndef GUM_NO_INLINE
#include <agrum/multidim/rangeVariable.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_RANGE_VARIABLE_H */
