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
 * @brief Abstract base class for all multi dimensionnal aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_MULTI_DIM_AGGREGATOR_H
#define GUM_MULTI_DIM_AGGREGATOR_H

#include <agrum/multidim/multiDimReadOnly.h>


namespace gum {

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                     GUM_MULTI_DIM_AGGREGATOR                     === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class MultiDimAggregator
  * @brief Abstract base class for all multi dimensionnal aggregator
  * @ingroup multidim_group
  *
  * The principle of a deterministic aggregator is that \f$P[i,J_1,\cdots,J_n]=1 \iff i==f(J_1,f(J_2,\cdots,f(J_n,NeutraElement)\cdots))\f$ and 0 otherwise.
  * f is a virtual function called the folder function (like folder_left or folder_right in OCaml).
  * NeutralElement is an Idx
  *
  * @warning
  *  - the way aggregators are implemented assumed that the FIRST variable in the multiDim is the aggregator variable.
  *  - the way aggregators are implemented does not check types and domain size (e.g domain(folder function)==domain(aggregator variable)). However,
  * \f$f(J_1,f(J_2,\cdots,f(J_n,NeutraElement)\cdots))\f$ is truncated in order to fit in domain(aggregator variable)
  */
  /* ============================================================================ */
  template<typename T_DATA>

  class MultiDimAggregator : public MultiDimReadOnly<T_DATA> {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimAggregator();
      MultiDimAggregator( const MultiDimAggregator<T_DATA>& from );

      // ============================================================================
      /// Destructor.
      // ============================================================================
      virtual ~MultiDimAggregator();

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{
      // ============================================================================
    public:
      virtual T_DATA get( const Instantiation &i ) const;

      virtual std::string aggregatorName( void ) const=0;
      const std::string toString( void ) const;

      virtual void changeNotification( gum::Instantiation&, const gum::DiscreteVariable*, const gum::Idx&, const gum::Idx& ) {};

      virtual void setFirstNotification( gum::Instantiation& ) {};

      virtual void setLastNotification( gum::Instantiation& ) {};

      virtual void setIncNotification( gum::Instantiation& ) {};

      virtual void setDecNotification( gum::Instantiation& ) {};

      virtual void setChangeNotification( gum::Instantiation& ) {};

      const std::string toString( const gum::Instantiation* i ) const {return i->toString();};

      /// @return the real number of parameters used for this table. This function is used for compute @see compressionRatio()
      virtual Size realSize() const {return 0;};


      /// @}
    protected:
      /// _neutralElt() is the result value for the first application of _folder
      virtual Idx _neutralElt( void ) const = 0;

      /// _folder is applied on value i1 for variable v. the actual result for precedent applications is i2.
      /// @return the new result for applications up to v.
      virtual Idx _folder( const DiscreteVariable& v,Idx i1,Idx i2,bool& stop_iteration ) const = 0;
  };

  // ==============================================================================
  /// For friendly displaying the content of the array.
  // ==============================================================================
  template<typename T_DATA>
  std::ostream& operator<< ( std::ostream& s,
                             const MultiDimAggregator<T_DATA>& ag );


} /* namespace gum */

#include <agrum/multidim/aggregators/multiDimAggregator.tcc>


#endif /* GUM_MULTI_DIM_AGGREGATOR_H */

