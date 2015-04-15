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
 * @brief the base class for all a prioris
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_H
#define GUM_LEARNING_A_PRIORI_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/core/exceptions.h>
#include <agrum/learning/aprioris/aprioriTypes.h>

namespace gum {

  namespace learning {

    /** @class Apriori
     * @brief the base class for all a prioris
     * @ingroup learning_group
     */
    template <typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float>>
    class Apriori {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      Apriori();

      /// virtual copy constructor
      virtual Apriori<IdSetAlloc, CountAlloc> *copyFactory() const = 0;

      /// destructor
      virtual ~Apriori();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets the weight of the a priori (kind of effective sample size)
      virtual void setWeight(float weight);

      /// returns the weight assigned to the apriori
      float weight() const noexcept;

      /// indicates whether an apriori is of a certain type
      virtual bool isOfType(const std::string &type) = 0;

      /// returns the type of the apriori
      virtual const std::string &getType() const noexcept = 0;

      /// sets the parameters for the apriori
      void setParameters(
          const std::vector<unsigned int> &modalities,
          std::vector<std::vector<float, CountAlloc>> &counts,
          const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                      unsigned int> *> &target_nodesets,
          const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                      unsigned int> *> &conditioning_nodesets);

      /// compute the apriori into a given set of counts
      virtual void compute() = 0;

      /// returns the apriori vector for a given (conditioned) target set
      const std::vector<float, CountAlloc> &getAllApriori(unsigned int index);

      /// returns the apriori vector for a conditioning set
      const std::vector<float, CountAlloc> &
      getConditioningApriori(unsigned int index);

      /// @}

      protected:
      /// the weight of the apriori
      float _weight{1.0f};

      /// the modalities of the nodes
      const std::vector<unsigned int> *_modalities{nullptr};

      /// the counts that were performed by the class on which we add an apriori
      /** Say that a score needs an apriori, then _unapriori_counts corresponds
       * to the countings performed by the score. Those are given to the apriori
       * so that the latter can add what is needed to _unapriori_counts to take
       * into account the so-called apriori. */
      std::vector<std::vector<float, CountAlloc>> *_unapriori_counts{nullptr};

      /// the set of target + cond nodes, and their counting indices
      /** The first element of the pairs correspond to a nodeset and the
       * second one to the corresponding index in _unapriori_counts. Some
       * pointers on the pair might be nullptr. In this case, this means that
       * the countings have already been processed and have been put into a cache.
       * So, for these pointers, no a priori operation needs be performed. */
      const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                  unsigned int> *> *_target_nodesets{nullptr};

      /// the set of conditioning nodesets
      /** for details, see _target_nodesets */
      const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                  unsigned int> *> *_conditioning_nodesets{nullptr};

      /// the a priori
      std::vector<std::vector<float, CountAlloc>> _apriori_counts;

      /// copy constructor
      Apriori(const Apriori<IdSetAlloc, CountAlloc> &from);

      /// move constructor
      Apriori(Apriori<IdSetAlloc, CountAlloc> &&from);

      private:
      // ##########################################################################
      // ##########################################################################

      /// prevent copy operator
      Apriori<IdSetAlloc, CountAlloc> &
      operator=(const Apriori<IdSetAlloc, CountAlloc> &) = delete;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the template implementation
#include <agrum/learning/aprioris/apriori.tcc>

#endif /* GUM_LEARNING_A_PRIORI_H */
