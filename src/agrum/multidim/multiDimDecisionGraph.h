
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of gum::MultiDimImplementation.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_H
#define GUM_MULTI_DIM_DECISION_GRAPH_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecisionGraphManager.h>
// ============================================================================

namespace gum {

  friend class MultiDimDecisionGraphManager;
  /**
   * @class MultiDimDecisionGraph multiDimDecisionGraph.h <agrum/multidim/multiDimDecisionGraph.h>
   * @brief Class implementingting a decision graph
   * For description of general methods, please refer to multidimdecisiondiagrambase
   * For specific implementation and use of multidimdecisiondiagram this is the right place :
   * The idea is that approximation policy is given as a template class to this class ( please
   * refer to agrum/core/approximationPolicy.h for more details on such classes ). MultiDimDecisionDiagram inherits from
   * this class.
   * A default classe exists : Exact Policy whixh commit no approximation on value given. As a default
   * you just have to declare MultiDimDecisionDiagram<GUM_SCALAR> and play with.
   * If you want to use a different approximation pattern, just MultiDimDecisionDiagram<GUM_SCALAR, CLASS_DESCRIBING_NEW_PATTERN>.
   * Therefor, if do operation on MultiDimDecisionDiagram the return type will likely be MultiDimDecisionDiagramBase
   * @ingroup multidim_group
   *
   */
  template<typename GUM_SCALAR>
  class MultiDimDecisionGraph : public MultiDimImplementation< GUM_SCALAR > {

  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimDecisionGraph();

      // ============================================================================
      /// Copy constructor.
      // ============================================================================
      MultiDimDecisionGraph( const MultiDimDecisionGraph<GUM_SCALAR>& from );

      // ============================================================================
      /// Copy Operator.
      // ============================================================================
      MultiDimDecisionGraph<GUM_SCALAR>& operator=(const MultiDimDecisionGraph<GUM_SCALAR>& from);

      // ============================================================================
      /// Destructor.
      // ============================================================================
      virtual ~MultiDimImplementation();

    /// @}

    // ============================================================================
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
    // ============================================================================
    virtual MultiDimContainer<GUM_SCALAR>* newFactory() const = 0;


    // ############################################################################
    /// @name Accessors / Modifiers herited from gum::MultiDimImplementation
    // ############################################################################
    /// @{

      // ============================================================================
      /**
       * Changes the value pointed by i.
       *
       * @warning If i variables set is disjoint with this gum::MultiDimContainer
       * then 0 is assumed for dimensions (i.e. variables) not prensent in the
       * instantiation.
       */
      // ============================================================================
      virtual void set ( const Instantiation &i, const GUM_SCALAR &value ) const;

      // ============================================================================
      /// see gum::MultiDimImplementation
      // ============================================================================
      virtual const std::string& name() const;

      // ============================================================================
      /**
       * An [] operator using a Instantiation as argument.
       *
       * @warning If i variables set is disjoint with this gum::MultiDimContainer
       * then 0 is assumed for dimensions (i.e. variables) not prensent in the
       * instantiation.
       *
       * @param i An Instantiation.
       * @return Returns the adressed (GUM_SCALAR) value.
       */
      // ============================================================================
      GUM_SCALAR operator[] ( const Instantiation &i ) const;

      // ============================================================================
      /**
       * Fill the table with d.
       * @param d The value used to fill this gum::MultiDimContainer.
       */
      // ============================================================================
      virtual void fill ( const GUM_SCALAR &d ) const = 0;

      // ==============================================================================
      /**
       * @brief Automatically fills this gum::MultiDimContainer with the values in v.
       *
       * The order used to fill this gum::MultiDimContainer is the same as with an
       * instantiation over it.
       * @code
       Size cpt = 0;
       Instantiation i( *this );
       for (i.setFirst(); !i.end(); ++i, ++cpt) {
         set(i, v[cpt]);
       }
       @endcode
       *
       * @param v Vector of values.
       * @throw SizeError If v size's does not matches this gum::MultiDimContainer
       *                  domain size.
       */
      // ==============================================================================
      virtual void fillWith ( const std::vector<GUM_SCALAR>& v ) const;

    /// @}


    // ############################################################################
    /// @name Implementation of MultiDimInterface
    // ############################################################################
    /// @{

      // ============================================================================
      /// See gum::MultiDimInterface::erase(const DiscreteVariable& v)
      // ============================================================================
      virtual void erase( const DiscreteVariable& v );

      // ============================================================================
      /**
       * Returns the real number of parameters used for this table. This function is
       * used by the compressionRatio() method.
       *
       * @see compressionRatio()
       */
      // ============================================================================
      virtual Size realSize() const;

    /// @}

public :

    // ############################################################################
    /// @name Slave management and extension due to slave management
    // ############################################################################
    /// @{
      // ============================================================================
      /**
       * Listen to change in a given Instantiation.
       * @param i the Instantiation
       * @param var The changed dim.
       * @param oldval The old value.
       * @param newval The changed value.
       */
      // ============================================================================
      virtual void changeNotification( Instantiation& i,
                                   const DiscreteVariable* const var,
                                   const Idx& oldval,const Idx& newval ) =0;

      // ============================================================================
      /**
       * Listen to setFirst in a given Instantiation.
       * @param i The Instantiation.
       */
      // ============================================================================
      virtual void setFirstNotification( Instantiation& i )=0;

      // ============================================================================
      /**
       * Listen to setLast in a given Instantiation.
       * @param i The Instantiation.
       */
      // ============================================================================
      virtual void setLastNotification( Instantiation& i ) =0;

      // ============================================================================
      /**
       * Listen to increment in a given Instantiation.
       * @param i The Instantiation
       */
      // ============================================================================
      virtual void setIncNotification( Instantiation& i ) =0;

      // ============================================================================
      /**
       * Listen to increment in each recorded Instantiation.
       * @param i The Instantiation
       */
      // ============================================================================
      virtual void setDecNotification( Instantiation& i ) =0;

      // ============================================================================
      /// Listen to an assignment of a value in a Instantiation.
      // ===========================================================================
      virtual void setChangeNotification( Instantiation& i ) =0;

      // ============================================================================
      /// String representation of internal data about i in this.
      // ============================================================================
      virtual const std::string toString( const Instantiation *i ) const = 0;

    /// @}


    // ############################################################################
    /// @name Copy methods.
    // ############################################################################
    /// @{
      // ============================================================================
      /**
       * @brief Basic copy src a gum::MultiDimContainer.
       * This method is virtual because it should be optimized in certain
       * gum::MultiDimContainer.
       *
       * @todo specific versions for decorator and for MultiDimArray
       *
       * @param src The gum::MultiDimContainer src which values are copied.
       * @param p_i Give the order to iterate in this gum::MultiDimContainer during
       *            the copy (natural order if null).
       * @throw OperationNotAllowed Raised if src does not have the same domain size
       *                            than this gum::MultiDimContainer.
       */
      // ============================================================================
      virtual void copyFrom ( const MultiDimContainer<GUM_SCALAR>& src,
                              Instantiation *p_i = ( Instantiation * ) 0 ) const;

      // ============================================================================
      /**
       * Removes all variables in this gum::MultiDimContainer and copy the content
       * of src, variables included.
       */
      // ============================================================================
      virtual void copy ( const MultiDimContainer<GUM_SCALAR>& src );

    /// @}

    // ############################################################################
    /// @name Various methods.
    // ############################################################################
    /// @{

      // ============================================================================
      /// Display the internal representation of i.
      // ============================================================================
      virtual const std::string toString ( const Instantiation *i ) const = 0;

    /// @}

  protected:

      // ============================================================================
      /// @brief Replace variable x by y.
      /// Technically this should be call by any subclass overloading this method
      /// to proceed with the changes in this class containers.
      // ============================================================================
      virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y ) =0;

  // ============================================================================
  /**
   * @brief Return a data, given a Instantiation.
   *
   * Note that get allows to change a value in the container.
   * The method is still tagged as const.
   *
   * @warning If i variables set is disjoint with this gum::MultiDimContainer
   * then 0 is assumed for dimensions (i.e. variables) not prensent in the
   * instantiation.
   *
   * @param i The instantiation used to find the data.
   */
  // ============================================================================
  virtual GUM_SCALAR &_get ( const Instantiation &i ) const = 0;

  // ==============================================================================
    /**
     * @brief Adds a new var to the sequence of vars.
     *
     * This function is not VIRTUAL (call of add which IS virtual).
     * @warning Note that the variable passed in argument is not duplicated, that is,
     *          only a pointer toward the variable is kept by the MultiDimInterface
     *
     * @code mat << var1 << var2 @endcode will insert @code var1 @endcode first.
     *
     * @param c the multidim container
     * @param v the new var
     * @return a reference to *this
     *
     * @throw DuplicateElement is thrown if the variable already belongs to the
     *                         sequence of variables.
     * @throw OperationNotAllowed if *this is non mutable
     *
     * @ingroup multidim_group
     */
  // ==============================================================================
    MultiDimInterface& operator<< ( MultiDimInterface& c, const DiscreteVariable& v );

  // ==============================================================================
    /**
     * @brief Removes a var from the variables of the MutliDimAdressing.
     *
     * @throw OperationNotAllowed if this object is non mutable.
     * @throw NotFound is v does not belong to this
     *
     * @ingroup multidim_group
     */
  // ==============================================================================
    MultiDimInterface& operator>> ( MultiDimInterface& c, const DiscreteVariable& v );

private:
};

#endif // GUM_MULTI_DIM_DECISION_GRAPH_H
