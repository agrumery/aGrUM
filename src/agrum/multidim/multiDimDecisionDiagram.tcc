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
* @brief Template implementation of MultiDimDecisionDiagram.h class.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#include <agrum/multidim/multiDimDecisionDiagram.h>

namespace gum {

  /* ********************************************************************************************
   */
  /*                                                */
  /*                CONSTRUCTOR, DESTRUCTOR & CO                  */
  /*                                                */
  /* ********************************************************************************************
   */

  // Default constructor

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram<GUM_SCALAR,
                          IApproximationPolicy>::MultiDimDecisionDiagram()
      : MultiDimDecisionDiagramBase<GUM_SCALAR>(),
        IApproximationPolicy<GUM_SCALAR>() {

    GUM_CONSTRUCTOR(MultiDimDecisionDiagram);
  }

  // Constructor with approximation initialisation

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy>::MultiDimDecisionDiagram(
      const MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy> &source)
      : MultiDimDecisionDiagramBase<GUM_SCALAR>(),
        IApproximationPolicy<GUM_SCALAR>(source) {

    GUM_CONSTRUCTOR(MultiDimDecisionDiagram);
  }

  // Constructor with approximation initialisation

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy>::MultiDimDecisionDiagram(
      const IApproximationPolicy<GUM_SCALAR> &source)
      : MultiDimDecisionDiagramBase<GUM_SCALAR>(),
        IApproximationPolicy<GUM_SCALAR>(source) {

    GUM_CONSTRUCTOR(MultiDimDecisionDiagram);
  }

  // destructor

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram<GUM_SCALAR,
                          IApproximationPolicy>::~MultiDimDecisionDiagram() {

    GUM_DESTRUCTOR(MultiDimDecisionDiagram);
  }

  // This method creates a clone of this object, without its content
  // (including variable), you must use this method if you want to ensure
  // that the generated object has the same type than the object containing
  // the called newFactory()
  // For example :
  //   MultiDimArray<double> y;
  //   MultiDimContainer<double>* x = y.newFactory();
  // Then x is a MultiDimArray<double>*
  //
  // @warning you must desallocate by yourself the memory
  // @return an empty clone of this object with the same type

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimContainer<GUM_SCALAR> *
  MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy>::newFactory() const {

    return new MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy>(*this);
  }

  // Returns a factory that used same approximation pattern

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *
  MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy>::getFactory() const {

    MultiDimDecisionDiagramFactory<GUM_SCALAR, IApproximationPolicy> *ret =
        new MultiDimDecisionDiagramFactory<GUM_SCALAR, IApproximationPolicy>(*this);

    return ret;
  }

  // Returns a factory that used same approximation pattern

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *
  MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy>::getFactory(
      const ApproximationPolicy<GUM_SCALAR> &source) const {

    MultiDimDecisionDiagramFactory<GUM_SCALAR, IApproximationPolicy> *ret =
        new MultiDimDecisionDiagramFactory<GUM_SCALAR, IApproximationPolicy>(
            dynamic_cast<const IApproximationPolicy<GUM_SCALAR> &>(source));

    return ret;
  }
}
