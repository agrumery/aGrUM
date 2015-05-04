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
 * @brief Headers of GroundedInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_GROUNDED_INFERENCE_H
#define GUM_GROUNDED_INFERENCE_H

#include <agrum/PRM/inference/PRMInference.h>
#include <agrum/BN/inference/BayesNetInference.h>

namespace gum {
  namespace prm {
    /**
     * @class GroundedInference groundedInference.h <agrum/PRM/groundedInference.h>
     * @brief This class is used to realise grounded inference in a PRM<GUM_SCALAR>.
     *
     * The best way to build this class is to use the static creation methods.
     *
     */
    template <typename GUM_SCALAR>
    class GroundedInference : public PRMInference<GUM_SCALAR> {
      public:
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      GroundedInference(const PRM<GUM_SCALAR> &prm,
                        const System<GUM_SCALAR> &system);

      /// Destructor.
      virtual ~GroundedInference();

      /// @}
      // ========================================================================
      /// @name Getters & setters.
      // ========================================================================
      /// @{

      /**
       * @brief Returns the bayesnet inference engine used by this class.
       * @return the bayesnet inference engine used by this class.
       *
       * @throw NotFound Raised if no inference engine have been defined for
       *                 this class.
       */
      BayesNetInference<GUM_SCALAR> &getBNInference();

      /**
       * @brief Defines the bayesnet inference engine used by this class.
       * The inference engine is given to this class, it will be deleted
       * when ~GroundedInference() is called.
       * @param bn_inf The bayesnet inference engine used by this class.
       *
       * @throw OperationNotAllowed If bn_inf does not inference over the
       *                            SystemBayesNet of this class.
       * @todo BayesNetInference should have copy constructors.
       */
      void setBNInference(BayesNetInference<GUM_SCALAR> *bn_inf);

      virtual std::string name() const;

      /// @}
      protected:
      // ========================================================================
      /// @name Private evidence handling methods and members.
      // ========================================================================
      /// @{

      /// This method is called whenever an evidence is added, but AFTER
      /// any processing made by PRMInference.
      virtual void
      _evidenceAdded(const typename PRMInference<GUM_SCALAR>::Chain &chain);

      /// This method is called whenever an evidence is removed, but BEFORE
      /// any processing made by PRMInference.
      virtual void
      _evidenceRemoved(const typename PRMInference<GUM_SCALAR>::Chain &chain);

      /// @brief Generic method to compute the marginal of given element.
      /// @param chain
      /// @param m CPF filled with the marginal of elt. It is initialized
      ///          properly.
      virtual void _marginal(const typename PRMInference<GUM_SCALAR>::Chain &chain,
                             Potential<GUM_SCALAR> &m);

      /// @brief Generic method to compute the marginal of given element.
      /// @param queries Set of pairs of Instance and Attribute.
      /// @param j CPF filled with the joint probability of queries. It is
      ///          initialized properly.
      virtual void
      _joint(const std::vector<typename PRMInference<GUM_SCALAR>::Chain> &queries,
             Potential<GUM_SCALAR> &j);

      /// @}
      private:
      /// Copy constructor.
      GroundedInference(const GroundedInference &source);

      /// Copy operator.
      GroundedInference &operator=(const GroundedInference &source);

      /// The bayesnet inference engine used by this class.
      BayesNetInference<GUM_SCALAR> *__inf;

      List<const Potential<GUM_SCALAR> *> __obs;
    };

    extern template class GroundedInference<double>;

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/groundedInference.tcc>

#endif /* GUM_GROUNDED_INFERENCE_H */
