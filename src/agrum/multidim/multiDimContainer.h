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
 * Headers of the MultiDimContainer class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
// ============================================================================
#ifndef GUM_MULTIDIM_H
#define GUM_MULTIDIM_H
// ============================================================================
#include <iostream>
#include <vector>
// ============================================================================
#include <agrum/core/utils.h>
// ============================================================================
#include <agrum/multidim/multiDimAdressable.h>
// ============================================================================
namespace gum {
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                             GUM_MULTI_DIM                            === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class MultiDimContainer
   * @brief Abstract base class for all multi dimensionnal containers.
   * @ingroup multidim_group
   *
   * The gum::MultiDimContainer is an abstract base class for all multi
   * dimensionnal container of T_DATA. Its purpose is to deal with T_DATA access
   * with no regard to how the storage is done (tree, matrix...).
   *
   * MultiDimContainer is considered as const if its dimension does not change.
   * Contained values may change even in a const MultiDimContainer :
   * @code
   MultiDimArray<char> t;
   LabelizedVariable v( "v" ,"v" , 3);
   LabelizedVariable w( "w" ,"w" , 3);
   {
   t<<v;
   Instantiation I( t );I.setFirst();
   t[I]='a';
   }
   {
   MultiDimContainer<char>& notconst_t=t;
   notconst_t<<w;
   Instantiation I( notconst_t );I.setFirst();
   notconst_t[I]='b';
   }
   {
   const MultiDimContainer<char>& const_t=t;
   Instantiation I( const_t );I.setFirst();
   // const_t<<w; : ******************** DOES NOT COMPILE : YOU CANNOT DO THIS !!!!
   const_t[I]='c';
   }
   * @endcode
   */
  /* ============================================================================ */
  template<typename T_DATA>
  class MultiDimContainer : public MultiDimAdressable {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimContainer();

      // ============================================================================
      /// Copy constructor.
      // ============================================================================
      MultiDimContainer( const MultiDimContainer<T_DATA>& from );

      // ============================================================================
      /// Destructor.
      // ============================================================================
      virtual ~MultiDimContainer();

      /// @}

      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{
    public:
      // ============================================================================
      /**
       * Changes the value pointed by i.
       *
       * @warning If i variables set is disjoint with this gum::MultiDimContainer
       * then 0 is assumed for dimensions (i.e. variables) not prensent in the 
       * instantiation.
       */
      // ============================================================================
      virtual void set( const Instantiation& i,const T_DATA& value ) const;

      // ============================================================================
      /**
       * Returns the value pointed by i.
       *
       * @warning If i variables set is disjoint with this gum::MultiDimContainer
       * then 0 is assumed for dimensions (i.e. variables) not prensent in the 
       * instantiation.
       */
      // ============================================================================
      virtual T_DATA get( const Instantiation& i ) const ;

      // ============================================================================
      /**
       * An [] operator using a Instantiation as argument.
       *
       * @warning If i variables set is disjoint with this gum::MultiDimContainer
       * then 0 is assumed for dimensions (i.e. variables) not prensent in the 
       * instantiation.
       *
       * @param i An Instantiation.
       * @return Returns the adressed (T_DATA) value.
       */
      // ============================================================================
      virtual T_DATA operator[]( const Instantiation& i ) const;

      // ============================================================================
      /**
       * Fill the table with d.
       * @param d The value used to fill this gum::MultiDimContainer.
       */
      // ============================================================================
      virtual void fill( const T_DATA& d ) const =0;

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
      virtual void fillWith( const std::vector<T_DATA>& v ) const;

      /// @}


      // ############################################################################
      /// @name Copy methods.
      // ############################################################################
      /// @{
      /**
       * @brief Basic copy from a gum::MultiDimContainer.
       * This method is virtual because it should be optimized in certain
       * gum::MultiDimContainer.
       *
       * @todo specific versions for decorator and for MultiDimArray
       *
       * @param from The gum::MultiDimContainer from which values are copied.
       * @param p_i Give the order to iterate in this gum::MultiDimContainer during
       *            the copy (natural order if null).
       * @throw OperationNotAllowed Raised if from does not have the same domain size
       *                            than this gum::MultiDimContainer.
       */
      // ============================================================================
      virtual void copyFrom( const MultiDimContainer<T_DATA>& from,
                             Instantiation* p_i=( Instantiation * )0 ) const;

      // ============================================================================
      /**
       * Removes all variables in this gum::MultiDimContainer and copy the content
       * of from, variables included.
       */
      // ============================================================================
      virtual void copy( const MultiDimContainer<T_DATA>& from );

      /// @}

      // ############################################################################
      /// @name Various methods.
      // ############################################################################
      /// @{

      // ============================================================================
      /// Display the content of the array.
      // ============================================================================
      virtual const std::string toString() const;

      // ============================================================================
      /// Display the internal representation of i.
      // ============================================================================
      virtual const std::string toString( const Instantiation *i ) const=0;

      /// @}

      // ############################################################################
      /// @name Fast large modifications in structures.
      // ############################################################################
      /// @{
      // ============================================================================
      /**
       * @brief Call this method before doing important changes in this
       *        gum::MultiDimContainer.
       * @warning Remember to call endMultipleChanges() when you finish your changes.
       */
      // ============================================================================
      virtual void beginMultipleChanges( void ) = 0 ;

      // ============================================================================
      /**
       * @brief Call this method after doing important changes in this
       *        gum::MultiDimContainer.
       */
      // ============================================================================
      virtual void endMultipleChanges( void ) = 0 ;
      // ============================================================================

      /// @}

    protected:
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
      virtual T_DATA& _get( const Instantiation& i ) const =0;
  };
} /* namespace gum */
// ============================================================================
#include <agrum/multidim/multiDimContainer.tcc>
// ============================================================================
#endif /* GUM_MULTIDIM_H */
// ============================================================================
