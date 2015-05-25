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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_DIRICHLET_FROM_DATABASE_H
#define GUM_LEARNING_A_PRIORI_DIRICHLET_FROM_DATABASE_H

#include <vector>
#include <algorithm>
#include <utility>

#include <agrum/config.h>
#include <agrum/learning/scores_and_tests/counter.h>
#include <agrum/learning/aprioris/apriori.h>

namespace gum {

  namespace learning {

    /** @class AprioriDirichletFromDatabase
     * @brief A dirichlet priori: computes its N'_ijk from a database
     * @ingroup learning_group
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class AprioriDirichletFromDatabase : public Apriori<IdSetAlloc, CountAlloc>,
                                         private Counter<IdSetAlloc, CountAlloc> {
      public:
      using type = AprioriDirichletType;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      template <typename RowFilter>
      AprioriDirichletFromDatabase(const RowFilter &filter,
                                   const std::vector<unsigned int> &var_modalities);

      /// virtual copy constructor
      virtual AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> *
      copyFactory() const;

      /// destructor
      virtual ~AprioriDirichletFromDatabase();

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
      AprioriDirichletFromDatabase(
          const AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> &from);

      /// move constructor
      AprioriDirichletFromDatabase(
          AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> &&from);

      /// prevent copy operator
      AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> &operator=(
          const AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/aprioriDirichletFromDatabase.tcc>

#endif /* GUM_LEARNING_A_PRIORI_DIRICHLET_FROM_DATABASE_H */
