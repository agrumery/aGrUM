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
 * @brief Headers of gum::MultiDimImplementation.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
// ============================================================================
#ifndef GUM_MULTI_DIM_IMPLEMENTATION_H
#define GUM_MULTI_DIM_IMPLEMENTATION_H
// ============================================================================
#include <vector>
// ============================================================================
#include <agrum/core/utils.h>
// ============================================================================
#include <agrum/multidim/multiDimContainer.h>
// ============================================================================
namespace gum {
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                     GUM_MULTI_DIM_IMPLEMENTATION                     === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class MultiDimImplementation
   * @brief Abstract base class for all multi dimensionnal implementations
   * @ingroup multidim_group
   *
   * The gum::MultiDimImplementation is an abstract class for all multidimensional
   * implementation of container of T_DATA. Its purpose is to implement
   * base algorithms with no regard to how the storage is done (tree, matrix...)
   *
   * It deals also with variables and implements pure virtual methods concerned
   * with dimensions.
   *
   * See operator<<(std::ostream&, const MultiDimImplementation<T_DATA>&) to print
   * a gum::MultiDimImplementation.
   */
  /* ============================================================================ */
  template<typename T_DATA>
  class MultiDimImplementation : public MultiDimContainer<T_DATA> {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimImplementation();

      // ============================================================================
      /// Copy constructor.
      // ============================================================================
      MultiDimImplementation( const MultiDimImplementation<T_DATA>& from );

      // ============================================================================
      /// Destructor.
      // ============================================================================
      virtual ~MultiDimImplementation();

      /// @}

      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{

      // ============================================================================
      using MultiDimContainer<T_DATA>::get;

      // ============================================================================
      // see gum::MultiDimInterface
      // ============================================================================
      virtual const Sequence<const DiscreteVariable *>&
      variablesSequence( void ) const  ;

      /// @}

      // ############################################################################
      /// @name Implementation of MultiDimInterface
      // ############################################################################
      /// @{

      // ============================================================================
      /// See gum::MultiDimInterface::add(const DiscreteVariable& v)
      // ============================================================================
      virtual void add( const DiscreteVariable& v );

      // ============================================================================
      /// See gum::MultiDimInterface::erase(const DiscreteVariable& v)
      // ============================================================================
      virtual void erase( const DiscreteVariable& v );

      // ============================================================================
      /// See gum::MultiDimInterface::nbrDim()
      // ============================================================================
      virtual Idx nbrDim() const ;

      // ============================================================================
      /// See gum::MultiDimInterface::domainSize()
      // ============================================================================
      virtual Size domainSize() const ;

      // ============================================================================
      /**
       * Returns the real number of parameters used for this table. This function is
       * used by the compressionRatio() method.
       *
       * @see compressionRatio()
       */
      // ============================================================================
      virtual Size realSize() const =0;

      // ============================================================================
      /**
       * The compression ratio of the table (depending on the type of implementation).
       *
       * This method uses domainSize() and realSize() to compute the ration,
       * both methods are virtual and should be surcharge if a subclass has a
       * special policies about memory management.
       *
       * @warning This compression ratio is not exactly the memory compression ratio.
       *          It is computed in terms of number of parameters.
       */
      // ============================================================================
      float compressionRate() const;

      // ============================================================================
      /// See gum::MultiDimInterface::variable(const idx i)
      // ============================================================================
      const DiscreteVariable& variable( const Idx i ) const ;

      // ============================================================================
      Idx pos( const DiscreteVariable& v ) const ;

      // ============================================================================
      /// See gum::MultiDimInterface::contains(const DiscreteVariable& v)
      // ============================================================================
      bool contains( const DiscreteVariable& v ) const ;

      // ============================================================================
      /// See gum::MultiDimInterface::empty()
      // ============================================================================
      bool empty() const ;

      /// @}


      // ############################################################################
      /// @name Slave management and extension due to slave management
      // ############################################################################
      /// @{
      // ============================================================================
      virtual MultiDimAdressable& getMasterRef( void );

      // ============================================================================
      virtual const MultiDimAdressable& getMasterRef( void ) const;

      // ============================================================================
      virtual bool registerSlave( Instantiation& i );

      // ============================================================================
      virtual bool unregisterSlave( Instantiation& i );

      /// @}


      // ############################################################################
      /// @name fast large modifications in structures
      // ############################################################################
      /// @{

      // ============================================================================
      /// See gum::MultiDimContainer::beginMultipleChanges().
      // ============================================================================
      virtual void beginMultipleChanges( void );

      // ============================================================================
      /// See gum::MultiDimContainer::endMultipleChanges().
      // ============================================================================
      virtual void endMultipleChanges( void );

    protected:
      // ============================================================================
      /// Synchronize content after MultipleChanges.
      // ============================================================================
      virtual void _commitMultipleChanges( void );

      // ============================================================================
      /// Get the actual change method of *this.
      // ============================================================================
      bool _isInMultipleChangeMethod() const;

      // ============================================================================
      /// Get the actual state of *this.
      // ============================================================================
      bool _isCommitNeeded() const;

      // ============================================================================
      /// Returns a constant reference over the list of slaved instantiations.
      // ============================================================================
      const List<Instantiation*>& _slaves() const;

      /// @}

    private:
      // ============================================================================
      // list of discrete variables (dimensions)
      // ============================================================================
      Sequence<const DiscreteVariable *> __vars;

      // ============================================================================
      // list of instantiations of the tuples (sequences) of variables
      // ============================================================================
      List<Instantiation*> __slaveInstantiations;

      // ============================================================================
      enum __ENUM_InternalChangeMethod { DIRECT_CHANGE, MULTIPLE_CHANGE};

      // ============================================================================
      enum __ENUM_InternalChangeState { NO_CHANGE, NOT_COMMITTED_CHANGE};

      // ============================================================================
      __ENUM_InternalChangeMethod __internalChangeMethod;

      // ============================================================================
      __ENUM_InternalChangeState __internalChangeState;

      // ============================================================================
      void __setNotCommitedChange();

      // ============================================================================
      Size __domainSize;


  };


  // ==============================================================================
  /// For friendly displaying the content of the array.
  // ==============================================================================
  template<typename T_DATA>
  std::ostream& operator<< ( std::ostream&,
                             const MultiDimImplementation<T_DATA>& );

} /* namespace gum */
// ============================================================================
#include <agrum/multidim/multiDimImplementation.tcc>
// ============================================================================
#endif /* GUM_MULTI_DIM_IMPLEMENTATION_H */
// ============================================================================
