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
 * @brief the no a priori class: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_NO_APRIORI_H
#define GUM_LEARNING_A_PRIORI_NO_APRIORI_H

#include <vector>

#include <agrum/config.h>
#include <agrum/learning/aprioris/apriori.h>

namespace gum {

  namespace learning {

    /** @class AprioriNoApriori
     * @brief the no a priori class: corresponds to 0 weight-sample
     * @ingroup learning_group
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class AprioriNoApriori : public Apriori<IdSetAlloc, CountAlloc> {
      public:
      using type = AprioriNoAprioriType;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      AprioriNoApriori();

      /// virtual copy constructor
      virtual AprioriNoApriori<IdSetAlloc, CountAlloc> *copyFactory() const;

      /// destructor
      virtual ~AprioriNoApriori();

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

      /// sets the weight of the a priori
      virtual void setWeight(float weight) final;

      /// @}

      protected:
      /// copy constructor
      AprioriNoApriori(const AprioriNoApriori<IdSetAlloc, CountAlloc> &from);

      /// move constructor
      AprioriNoApriori(AprioriNoApriori<IdSetAlloc, CountAlloc> &&from);

      /// prevent copy operator
      AprioriNoApriori<IdSetAlloc, CountAlloc> &
      operator=(const AprioriNoApriori<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/aprioriNoApriori.tcc>

#endif /* GUM_LEARNING_A_PRIORI_NO_APRIORI_H */
