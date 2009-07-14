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
 * @brief Base class for discrete random variable.
 *
 * This class is used as an interface.
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_DISCRETE_VARIABLE_H
#define GUM_DISCRETE_VARIABLE_H

#include <string>
#include <ostream>
#include <agrum/core/utils.h>
#include <agrum/core/hashFunc.h>
#include <agrum/multidim/variable.h>


namespace gum {


  class DiscreteVariable;
  class DiscreteVariableListener;
  class DiscreteVariableEvent;

  
  // ===============================================================================
  /// for friendly displaying the content of the variable
  // ===============================================================================
  std::ostream& operator<<( std::ostream&, const DiscreteVariable&);


  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                         GUM_DISCRETE_VARIABLE                        === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class DiscreteVariable
   * @brief Base class for discrete random variable.
   * @ingroup multidim_group
   *
   * This class is used as an interface. */
  /* ============================================================================= */
  class DiscreteVariable : public Variable {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// Default constructor
    // ============================================================================
    DiscreteVariable( const std::string& aName, const std::string& aDesc );

    // ============================================================================
    /// Copy constructor
    // ============================================================================
    /** Copy Constructor.
     *
     * If aDRV haves any listener, it will not be copied.
     *
     * @param aDRV the variable we copy
     */
    DiscreteVariable( const DiscreteVariable& aDRV );

    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~DiscreteVariable();

    // ============================================================================
    /// Copy Factory.
    /// @return Returns a pointer on a new copy of this.
    // ============================================================================
    virtual DiscreteVariable* copyFactory() const =0;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// Overload of Variable::setName
    // ============================================================================
    void setName( const std::string& theValue );

    // ============================================================================
    /// Overload of Variable::setDescription
    // ============================================================================
    void setDescription( const std::string& theValue );

    // ============================================================================
    /// @return true if the domainSize() < 2;
    // ============================================================================
    bool empty() const;

    // ============================================================================
    /// @return the number of modalities of the random discrete 
    // ============================================================================
    virtual Size domainSize() const =0;

    // ============================================================================
    /// get the indice-th label. This method is pure virtual.
    /** @param indice the index of the label we wish to return
     * @throw OutOfBound
     */
    // ============================================================================
    virtual const std::string label(const Idx indice ) const = 0;

    /**
     * Sets the listener of *this.
     * Can only be done once.
     *
     * @throws OperationNotAllowed Raised if the listener have already been defined.
     */
    void setDiscreteVariableListener( DiscreteVariableListener* l );

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// Copy operator
    /** @param aRV to be copied
     * @return a ref to *this */
    // ============================================================================
    DiscreteVariable& operator=( const DiscreteVariable& aRV );

    // ============================================================================
    /// equality operator
    // ============================================================================
    virtual bool operator== ( const DiscreteVariable& aRV ) const ;

    // ============================================================================
    /// inequality operator
    // ============================================================================
    virtual bool operator!= ( const DiscreteVariable& aRV ) const ;

    /// @}
    
    /// from the label to its index in var.
    ///  @warning This operation may have different complexity in different
    /// subclasses.
    /// @throws NotFound
    virtual Idx operator[](const std::string& label) const =0;

    /// string version of *this
    virtual const std::string toString() const;

    
  protected:
		/// (protected) Default constructor
    DiscreteVariable( ) {GUM_CONSTRUCTOR(DiscreteVariable);};
		
    // ############################################################################
    /// @name Notification methods
    // ############################################################################
    /// @{
    /**
     * Warns the listener that the variable's domain size changed.
     */
    void raiseDomainSizeChanged();
    
    /**
     * Warns the listener that the variable's name changed.
     */
    void raiseNameChanged();

    /**
     * Warns the listener that the variable's description changed.
     */
    void raiseDescriptionChanged();

    /// @}


  private:
    DiscreteVariableListener* listener;
  };



  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                GUM_DISCRETE_VARIABLE_LISTENER                        === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class DiscreteVariableListener
   * @brief This abstract class represent a listenener of DiscreteVariable.
   *
   * @see DiscreteVariable, DiscreteVariableEvent
   */
  class DiscreteVariableListener {
  public:
    /**
     * Default constructor.
     */
    DiscreteVariableListener();

    /**
     * Destructor
     */
    virtual ~DiscreteVariableListener();

    /**
     * Called when the variable's domain size changes.
     *
     * @param e The source event.
     */
    virtual void variableDomainChanged( DiscreteVariableEvent& e ) =0;

    /**
     * Called when the variable's name changes.
     *
     * @param e The source event.
     */
    virtual void variableNameChanged( DiscreteVariableEvent& e ) =0;

    /**
     * Called when the variable's description changes.
     *
     * @param e The source event.
     */
    virtual void variableDescriptionChanged( DiscreteVariableEvent& e ) =0;
  };



  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                   GUM_DISCRETE_VARIABLE_EVENT                        === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class DiscreteVariableEvent
   * @brief Represents an event initiated by a change in a DiscreteVariable.
   *
   * @see DiscreteVariable, DiscreteVariableListener
   */
  class DiscreteVariableEvent {
  public:
    /**
     * @param variable The source of this event.
     */
    DiscreteVariableEvent( DiscreteVariable* variable );

    /**
     * Destructor.
     */
    ~DiscreteVariableEvent();

    /**
     * @return Returns a pointer on the changed DiscreteVariable.
     */
    DiscreteVariable* getSource();


  private:
    /*
     * Reference on the source of this event.
     */
    DiscreteVariable* source;
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/multidim/discreteVariable.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DISCRETE_VARIABLE_H */
