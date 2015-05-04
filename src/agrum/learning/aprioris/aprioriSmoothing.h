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
/** @file
 * @brief the smooth a priori: adds a weight w to all the countings
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_SMOOTHING_H
#define GUM_LEARNING_A_PRIORI_SMOOTHING_H

#include <vector>

#include <agrum/config.h>
#include <agrum/learning/aprioris/apriori.h>

namespace gum {

  namespace learning {

    /** @class AprioriSmoothing
     * @brief the smooth a priori: adds a weight w to all the countings
     * @ingroup learning_group
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class AprioriSmoothing : public Apriori<IdSetAlloc, CountAlloc> {
      public:
      using type = AprioriSmoothingType;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      AprioriSmoothing();

      /// virtual copy constructor
      virtual AprioriSmoothing<IdSetAlloc, CountAlloc> *copyFactory() const;

      /// destructor
      virtual ~AprioriSmoothing();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// include the apriori into a given set of counts
      virtual void compute() final;

      /// indicates whether an apriori is of a certain type
      virtual bool isOfType(const std::string &type) final;

      /// returns the type of the apriori
      virtual const std::string &getType() const noexcept final;

      /// @}

      protected:
      /// copy constructor
      AprioriSmoothing(const AprioriSmoothing<IdSetAlloc, CountAlloc> &from);

      /// move constructor
      AprioriSmoothing(AprioriSmoothing<IdSetAlloc, CountAlloc> &&from);

      /// prevent copy operator
      AprioriSmoothing<IdSetAlloc, CountAlloc> &
      operator=(const AprioriSmoothing<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/aprioriSmoothing.tcc>

#endif /* GUM_LEARNING_A_PRIORI_SMOOTHING_H */
