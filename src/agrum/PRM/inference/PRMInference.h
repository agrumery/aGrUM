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
 * @brief Headers of PRMInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PRM_INFERENCE_H
#define GUM_PRM_INFERENCE_H

#include <string>

#include <agrum/core/hashTable.h>
#include <agrum/multidim/potential.h>

#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {
    /**
     * @class PRMInference PRMInference.h <agrum/PRM/PRMInference.h>
     * @brief This abstract class is used as base class for all inference class
     *        on PRM<GUM_SCALAR>.
     *
     * The main purpose of this class is to give a common interface between all
     * inference algorithms and to handle evidences.
     */
    template <typename GUM_SCALAR>
    class PRMInference {
      public:
      /// Code alias.
      typedef std::pair<const PRMInstance<GUM_SCALAR>*,
                        const PRMAttribute<GUM_SCALAR>*>
          Chain;

      /// Code alias.
      typedef NodeProperty<const Potential<GUM_SCALAR>*> EMap;

      /// Code alias.
      typedef typename NodeProperty<const Potential<GUM_SCALAR>*>::iterator_safe
          EMapIterator;

      /// Code alias.
      typedef
          typename NodeProperty<const Potential<GUM_SCALAR>*>::const_iterator_safe
              EMapConstIterator;

      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      PRMInference( const PRM<GUM_SCALAR>&       prm,
                    const PRMSystem<GUM_SCALAR>& system );

      /// Copy constructor.
      PRMInference( const PRMInference& source );

      /// Destructor.
      virtual ~PRMInference();

      /// Copy operator.
      PRMInference& operator=( const PRMInference& source );

      /// @}
      // ========================================================================
      /// @name Getters & setters.
      // ========================================================================
      /// @{

      /// Returns the name of the current inference algorithm
      virtual std::string name() const = 0;

      /// @}
      // ========================================================================
      // ========================================================================
      /// @name Query methods.
      // ========================================================================
      /// @{

      /**
       * Compute the marginal of the formal attribute pointed by chain and
       * stores it in m.
       *
       * @param chain A string of the form instance.attribute.
       * @param m An empty CPF which will be filed by the marginal of chain.
       * @throw NotFound Raised if chain is invalid.
       * @throw WrongType Raised if chain does not point to an
       *PRMAttribute<GUM_SCALAR>.
       * @throw OperationNotAllowed Raise if m is not empty.
       */
      void marginal( const Chain& chain, Potential<GUM_SCALAR>& m );

      /**
       * Compute the joint probability of the formals attributes pointed by
       * chains and stores it in m.
       *
       * @param chains A Set of strings of the form instance.attribute.
       * @param j An empty CPF which will be filed by the joint probability
       *        over chains.
       * @throw NotFound Raised if some chain in chains does not point to a
       *                 formal attribute.
       * @throw OperationNotAllowed Raise if m is not empty.
       */
      void joint( const std::vector<Chain>& chains, Potential<GUM_SCALAR>& j );

      /// @}
      // ========================================================================
      /// @name Evidence handling.
      // ========================================================================
      /// @{

      /// Returns EMap of evidences over i.
      /// @throw NotFound if i has no evidence.
      EMap& evidence( const PRMInstance<GUM_SCALAR>& i );

      /// Returns EMap of evidences over i.
      /// @throw NotFound if i has no evidence.
      EMap& evidence( const PRMInstance<GUM_SCALAR>* i );

      /// Returns EMap of evidences over i.
      /// @throw NotFound if i has no evidence.
      const EMap& evidence( const PRMInstance<GUM_SCALAR>& i ) const;

      /// Returns EMap of evidences over i.
      /// @throw NotFound if i has no evidence.
      const EMap& evidence( const PRMInstance<GUM_SCALAR>* i ) const;

      /// Returns true if i has evidence.
      bool hasEvidence( const PRMInstance<GUM_SCALAR>& i ) const;

      /// Returns EMap of evidences over i.
      bool hasEvidence( const PRMInstance<GUM_SCALAR>* i ) const;

      /// Returns true if i has evidence on PRMAttribute<GUM_SCALAR> a.
      bool hasEvidence( const Chain& chain ) const;

      /// Returns true if i has evidence on PRMAttribute<GUM_SCALAR> a.
      bool hasEvidence() const;

      /// Add an evidence to the given instance's elt.
      /// @param chain The variable being observed.
      /// @param p The Potential added (by copy) as evidence.
      ///
      /// @throw NotFound Raised if elt does not belong to i.
      /// @throw OperationNotAllowed Raised if p is inconsistent with elt.
      void addEvidence( const Chain& chain, const Potential<GUM_SCALAR>& p );

      /// Remove evidence on the given instance's elt.
      /// @param chain The variable being observed.
      ///
      /// @throw NotFound Raised if the given names are not found.
      /// @throw WrongType Raised if the elt is not an PRMAttribute<GUM_SCALAR>.
      void removeEvidence( const Chain& chain );

      /// Remove all evidences.
      void clearEvidence();

      /// @}
      protected:
      // ========================================================================
      /// @name Protected members.
      // ========================================================================
      /// @{

      /// This method is called whenever an evidence is added, but AFTER
      /// any processing made by PRMInference.
      virtual void _evidenceAdded( const Chain& chain ) = 0;

      /// This method is called whenever an evidence is removed, but BEFORE
      /// any processing made by PRMInference.
      virtual void _evidenceRemoved( const Chain& chain ) = 0;

      /// @brief Generic method to compute the marginal of given element.
      /// @param chain
      /// @param m CPF filled with the marginal of elt. It is initialized
      ///          properly.
      virtual void _marginal( const Chain& chain, Potential<GUM_SCALAR>& m ) = 0;

      /// @brief Generic method to compute the marginal of given element.
      /// @param queries Set of pairs of PRMInstance<GUM_SCALAR> and
      /// PRMAttribute<GUM_SCALAR>.
      /// @param j CPF filled with the joint probability of queries. It is
      ///          initialized properly.
      virtual void _joint( const std::vector<Chain>& queries,
                           Potential<GUM_SCALAR>&    j ) = 0;

      /// The PRM<GUM_SCALAR> on which inference is done.
      PRM<GUM_SCALAR> const* _prm;

      /// The Model on which inference is done.
      PRMSystem<GUM_SCALAR> const* _sys;

      /// @}

      private:
      // ========================================================================
      /// @name Private evidence handling methods and members.
      // ========================================================================
      /// @{

      /// Code alias.
      typedef typename HashTable<const PRMInstance<GUM_SCALAR>*,
                                 EMap*>::iterator_safe EvidenceIterator;
      /// Code alias.
      typedef typename HashTable<const PRMInstance<GUM_SCALAR>*,
                                 EMap*>::const_iterator_safe EvidenceConstIterator;

      /// Mapping of evidence over PRMInstance<GUM_SCALAR>'s nodes.
      HashTable<const PRMInstance<GUM_SCALAR>*, EMap*> __evidences;

      /// Private getter over __evidences, if necessary creates an EMap for i.
      EMap& __EMap( const PRMInstance<GUM_SCALAR>* i );

      /// @}
    };


    extern template class PRMInference<float>;
    extern template class PRMInference<double>;


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/PRMInference_tpl.h>

#endif /* GUM_PRM_INFERENCE_H */
