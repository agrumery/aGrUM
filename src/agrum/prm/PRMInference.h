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
 * @author Lionel TORTI
 */
// ============================================================================
#include <string>
// ============================================================================
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/multidim/potential.h>
// ============================================================================
#include <agrum/prm/PRM.h>
// ============================================================================
#ifndef GUM_PRM_INFERENCE_H
#define GUM_PRM_INFERENCE_H
// ============================================================================

namespace gum {
/**
 * @class PRMInference PRMInference.h <agrum/prm/PRMInference.h>
 * @brief This abstract class is used as base class for all inference class
 *        on PRM.
 *
 * The main purpose of this class is to give a common interface between all
 * inference algorithms and to handle evidences.
 */
class PRMInference {
  public:
  // ========================================================================
  /// @name Constructor & destructor.
  // ========================================================================
    /// @{

    /// Default constructor.
    PRMInference(const PRM& prm, const Model& model);

    /// Default constructor.
    PRMInference(const PRM& prm, const std::string& model);

    /// Destructor.
    virtual ~PRMInference();

    /// @}
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
     * @throw WrongType Raised if chain does not point to an Attribute.
     * @throw OperationNotAllowed Raise if m is not empty.
     */
    void marginal(const std::string& chain, Potential<prm_float>& m);

    /**
     * Compute the joint probability of the formals attributes pointed by
     * chains and stores it in m.
     *
     * @param chain A Set of strings of the form instance.attribute.
     * @param j An empty CPF which will be filed by the joint probability
     *        over chains.
     * @throw NotFound Raised if some chain in chains does not point to a
     *                 formal attribute.
     * @throw OperationNotAllowed Raise if m is not empty.
     */
    void joint(const Set<std::string> chains, Potential<prm_float>& j);

    /// @}
  // ========================================================================
  /// @name Evidence handling.
  // ========================================================================
    /// @{

    /// Code alias.
    typedef Property<const Potential<prm_float>*>::onNodes EMap;

    /// Code alias.
    typedef Property<const Potential<prm_float>*>::onNodes::iterator EMapIterator;

    /// Code alias.
    typedef Property<const Potential<prm_float>*>::onNodes::const_iterator
            EMapConstIterator;

    /// Returns EMap of evidences over i.
    /// @throw NotFound if i has no evidence.
    EMap& evidence(const Instance& i);

    /// Returns EMap of evidences over i.
    /// @throw NotFound if i has no evidence.
    EMap& evidence(const Instance* i);

    /// Returns EMap of evidences over i.
    /// @throw NotFound if i has no evidence.
    const EMap& evidence(const Instance& i) const;

    /// Returns EMap of evidences over i.
    /// @throw NotFound if i has no evidence.
    const EMap& evidence(const Instance* i) const;

    /// Returns true if i has evidence.
    bool hasEvidence(const Instance& i) const;

    /// Returns EMap of evidences over i.
    bool hasEvidence(const Instance* i) const;

    /// Returns true if i has evidence on Attribute a.
    bool hasEvidence(const Instance& i, const Attribute& a) const;

    /// Returns true if i has evidence on Attribute a.
    bool hasEvidence(const Instance* i, const Attribute* a) const;

    /// Copy evidences from source if they share the same Model.
    /// @throw OperationNotAllowed Raised if source and this does not share
    ///                            the same Model.
    void copyEvidence(const PRMInference& source);

    /// Add an evidence to the given instance's elt.
    /// @param inst_name The Instance's name.
    /// @param elt_name The Instance's ClassElement's name.
    /// @param p The Potential added (by copy) as evidence.
    /// @throw NotFound Raised if the given names are not found.
    /// @throw OperationNotAllowed Raised if p is inconsistent with elt.
    /// @throw WrongType Raised if the elt is not an Attribute.
    void addEvidence(const std::string& inst_name,
                     const std::string& elt_name,
                     const Potential<prm_float>& p);

    /// Add an evidence to the given instance's elt.
    /// @param slot_chain A string of the form "instanceName.attrName".
    /// @param p The Potential added (by copy) as evidence.
    /// @throw NotFound Raised if the given slot chain is intractable.
    /// @throw OperationNotAllowed Raised if p is inconsistent with the
    ///                            Attribute pointed by slot_chain.
    /// @throw WrongType Raised if the ClassElement pointed by slot_chain is
    ///                  not an Attribute.
    void addEvidence(const std::string& slot_chain, const Potential<prm_float>& p);

    /// Add an evidence to the given instance's elt.
    /// @param i The Instance on which an evidence is added.
    /// @param elt The Attibute on which an evidence is added.
    /// @param p The Potential added (by copy) as evidence.
    /// @throw NotFound Raised if elt does not belong to i.
    /// @throw OperationNotAllowed Raised if p is inconsistent with elt.
    void addEvidence(const Instance& i, const Attribute& elt,
                     const Potential<prm_float>& p);

    /// Add an evidence to the given instance's elt.
    /// @param i The Instance on which an evidence is added.
    /// @param elt_id The Instance's ClassElement's id.
    /// @param p The Potential added (by copy) as evidence.
    /// @throw NotFound Raised if elt_id does not match any ClassElement in i.
    /// @throw OperationNotAllowed Raised if p is inconsistent with elt_id.
    /// @throw WrongType Raised if elt_id is not an Attribute.
    void addEvidence(const Instance& i, NodeId elt_id,
                     const Potential<prm_float>& p);

    /// Remove evidence on the given instance's elt.
    /// @param inst_name The Instance's name.
    /// @param elt_name The Instance's ClassElement's name.
    /// @throw NotFound Raised if the given names are not found.
    /// @throw WrongType Raised if the elt is not an Attribute.
    void removeEvidence(const std::string& inst_name,
                        const std::string& elt_name);

    /// Remove evidence on the given instance's elt.
    /// @param slot_chain A chain path starting by an Instance's name and
    ///                   finishing by an Attribute's name.
    /// @throw NotFound Raised if the given slot chain is intractable.
    /// @throw WrongType Raised if the ClassElement pointed by slot_chain
    ///                  is not an Attribute.
    void removeEvidence(const std::string& slot_chain);

    /// Remove evidence on the given instance's elt.
    /// @param i The Instance on which an evidence is removed.
    /// @param elt The Attibute on which an evidence is removed.
    /// @throw NotFound Raised if elt does not belong to i.
    void removeEvidence(const Instance& i, const Attribute& elt);

    /// Remove evidence on the given instance's elt.
    /// @param i The Instance on which an evidence is removed.
    /// @param elt_id The Instance's ClassElement's id.
    /// @throw NotFound Raised if elt_id does not match any ClassElement in i.
    /// @throw WrongType Raised if elt_id is not an Attribute.
    void removeEvidence(const Instance& i, NodeId elt_id);

    /// Remove all evidences.
    void clearEvidence();

    /// @}
  protected:
    /// Copy constructor.
    PRMInference(const PRMInference& source);

    /// Copy operator.
    PRMInference& operator=(const PRMInference& source);

    /// This method is called whenever an evidence is added, but AFTER
    /// any processing made by PRMInference.
    virtual void _evidenceAdded(const Instance* i, NodeId elt_id) =0;

    /// This method is called whenever an evidence is removed, but BEFORE
    /// any processing made by PRMInference.
    virtual void _evidenceRemoved(const Instance* i, NodeId elt_id) =0;

    /// @brief Generic method to compute the marginal of given element.
    /// @param i Pointer over the Instance of elt.
    /// @param elt Pointer over the Attribute for which the marginal must be
    ///            computed.
    /// @param m CPF filled with the marginal of elt. It is initialized
    ///          properly.
    virtual void _marginal(const Instance* i, const Attribute* elt,
                           Potential<prm_float>& m) =0;

    /// Code alias.
    typedef std::pair<const Instance*, const Attribute*> InstAttrPair;
    /// @brief Generic method to compute the marginal of given element.
    /// @param queries Set of pairs of Instance and Attribute.
    /// @param j CPF filled with the joint probability of queries. It is
    ///          initialized properly.
    virtual void _joint(const Set< InstAttrPair >& queries,
                        Potential<prm_float>& j) =0;

    /// The PRM on which inference is done.
    const PRM& _prm;

    /// The Model on which inference is done.
    const Model& _model;

  private:

  // ========================================================================
  /// @name Private evidence handling methods and members.
  // ========================================================================
    /// @{

    /// Code alias.
    typedef HashTable<const Instance*, EMap*>::iterator EvidenceIterator;

    /// Mapping of evidence over Instance's nodes.
    mutable HashTable<const Instance*, EMap*> __evidences;

    /// @brief Generic method to add evidence over a ClassElement in an
    ///        Instance.
    /// The Potential is added by copy and this method checks if it only
    /// contains elt's variable.
    /// @param i The instance in which an evidence is added.
    /// @param elt The ClassElement of i on which an evidence is observed.
    /// @param p The Potential representing the evidence.
    /// @throw OperationNotAllowed Raised if p isn't consistent with elt.
    void __addEvidence(const Instance* i, const Attribute* elt,
                       const Potential<prm_float>& p);

    /// @brief Generic method to remove an evidence over a ClassElement in
    ///        an Instance.
    /// The concerned Instance is updated with the suppression of the
    /// evidence (if any). If there is no evidence over elt, than nothing is
    /// done.
    void __removeEvidence(const Instance* i, const Attribute* elt);

    /// Private getter over __evidences, if necessary creates an EMap for i.
    EMap& __EMap(const Instance* i);

    /// @}
};

} /* namespace gum */
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/PRMInference.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_PRM_INFERENCE_H */
// ============================================================================
