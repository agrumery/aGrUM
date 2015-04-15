/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it udnder the terms of the GNU General Public License as published by *
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
#ifndef GUM_UTILS_PRM_H
#define GUM_UTILS_PRM_H

#include <string>
#include <vector>

#include <agrum/config.h>

#include <agrum/multidim/aggregators/multiDimAggregator.h>
#include <agrum/multidim/multiDimBijArray.h>
#include <agrum/multidim/multiDimBucket.h>
#include <agrum/multidim/operators/multiDimCombinationDefault.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/multidim/multiDimSparse.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/operators/projections4MultiDim.h>

#include <agrum/PRM/elements/PRMObject.h>

/*!  @ingroup prm_group
 *
 * \page prm_page How to use Probabilistic Relational Models ?
 *
 *
 *
 * \section prm_typ_inh Type inheritance.
 *
 * Type inheritance is not part of the classical PRM formalism, thus we present
 * here insights on how it works and how aGrUM manages such concepts.
 *
 * Creating subtypes is not a difficult task and the constructor's
 * gum::prm::Type::Type(const Type&, const std::vector\<Idx\>&, const
 * DiscreteVariable&) documentation should give you enough information.
 *
 * Things become more tricky when adding an Attribute (or a ClassElement
 * representing a random variable) which Type is a subtype. In this case it is
 * necessary to add what we call "casting descendants", i.e. deterministic
 * nodes which can be used to cast the random variable in the desired super
 * type.
 *
 * Suppose we have 3 Type t_1, t_2 and t_3 with t_2 being a descendant of t_1
 * and t_3 of t_2. If we add an Attribute named foo of Type t_3 to a Class C,
 * then 3 Attribute are added to C, each named foo, with types t_1, t_2 and
 * t_3. However only the Attribute of Type t_3 is associated to the name foo in
 * C's name map. To retrieve the other Attribute you must use safe type names.
 *
 * The syntax of a safe type name is "\<type\>name" they guaranty the Type of
 * the Attribute you asked for. In our example even if there is 3 Attribute
 * named foo, only the one with Type t_3 can be retrieved using foo. The others
 * are only accessible using safe type names: \<t_1\>foo and \<t_2\>foo. If you
 * really want to be sure what you are doing, you can always use safe type
 * names for Attributes since \<t_3\>foo is also added to C's name map.
 *
 * The last point is how we add casting descendants in a Class. As we said
 * casting descendants are deterministic nodes, they simple cast a random
 * variable of one Type to another. Each Attribute which Type is a subtype has
 * a child of it's Type super type added. We do that to any Attribute which
 * type is a subtype (this include Attributes added as casting descendants).
 * In our example adding foo will also add the arcs: \<t_3\>foo -\> \<t_2\>foo
 * and \<t_2\>foo -\> \<t_1\>foo.
 *
 * \section prm_ref ReferenceSlots and SlotChains.
 *
 * ReferenceSlot are the equivalent of reference in the PRM formalism. They
 * represent a relation between two Class which can be instantiated as a
 * one-to-one or a one-to-many relation. SlotChains are the equivalent of slot
 * chain (or reference chain) in the PRM formalism. Both SlotChains and
* ReferenceSlot are tightly related: a SlotChain is a sequence of ReferenceSlot
* ending by an Attribute or Aggregate.
 *
 * Reference in a PRM follows the "pull" paradigm: a ClassElement in a Class
 * requiring information defined in a different Class uses SlotChains to
 * reference such ClassElement.  This implies that parent-to-child dependencies
 * are not known from the parent since it is its children whom pull the
 * information toward them. This can be problematic while inferring in a PRM.
 * To deal with this lack of information we use the notion of inverse reference
 * and introduce a new one: inverse slotchain.
 *
 * Inverse reference are implemented as ReferenceSlot representing the inverse
 * relation defined by a ReferenceSlot. When adding a ReferenceSlot named ref
 * with slot type B to Class A, the Class A automatically adds a ReferenceSlot
 * named \<A\>ref with slot type A to B. We call ref the declared ReferenceSlot
 * and \<A\>ref the inverse ReferenceSlot. Such inverse ReferenceSlot are
 * created each time a declared ReferenceSlot is added, even if it is inherited
 * or implemented. Such inverse ReferenceSlot can then be used to know which
 * Instance are related with one another.
 *
 * If an inverse ReferenceSlot is added to a Class or Interface then all
 * sub-Class, sub-Interface and implementation receive the same inverse
 * ReferenceSlot. However the referenced inverse of a declared ReferenceSlot
 * will always be the inverse ReferenceSlot of the root Class in the
 * inheritance tree.
 *
 * Inverse SlotChain represent the reverse path defined by a SlotChain: if a
 * SlotChain rho = ref_1.ref_2.foo then its inverse is inv_rho =
 * \<B\>ref_2.\<A\>ref_1.bar where bar is the ClassElement depending on foo, A
 * the domain of ref_1 and B the domain of ref_2. Inverse SlotChain are not
 * added when a declared SlotChain is added but when a child is added to
 * declared SlotChain. Thus there is as many inverse SlotChain of a declared
 * SlotChain as the number of his children.
 *
 * The name of a SlotChain is its reference chain ended by the ClassElement it
 * points to. Since for each ClassElementContainer ClassElement name are
 * unique, the uniqueness of SlotChain name is guaranteed.
 *
 * Inverse ReferenceSlot and SlotChain are always multiple and when adding a
 * ReferenceSlot to an Interface no inverse ReferenceSlot are added since
 * implementing Class will take care of it.
 *
 * \section prm_exception Exceptions semantics
 *
 * Exceptions in PRM have specific significations:
 *
 *    - gum::NotFound: raised if an identifier (usually a string or a
 *    gum::NodeId) does not match any gum::prm::ClassElement in a given
 *    gum::prm::ClassElementContainer or gum::prm::Instance.
 *
 *    - gum::TypeError: raised if an gum::prm::PRMObject is not a valid type
 *    when using it in some context.  For example assigning an
 *    gum::prm::Instance to gum::prm::ReferenceSlot where the
 *    gum::prm::Instance type is not a subtype of the gum::prm::ReferenceSlot
 *    slot type will raise a gum::TypeError exception.
 *
 *    - gum::WrongClassElement: raised if a gum::prm::ClassElement is used
 *    where it should not be.  For example adding an gum::prm::Attribute as a
 *    parent of a gum::prm::ReferenceSlot in a gum::prm::Class will raise a
 *    gum::WrongClassElement.
 *
 *    - gum::OutOfUpperBound: raised if a bound is not respected.  For example
 *    if more than one gum::prm::Instance is assigned to a
 *    gum::prm::ReferenceSlot which is not an array a gum::OutOfUpperBound will
 *    be raised.
*/
namespace gum {
  namespace prm {

    /// Type for real numbers
    typedef float prm_float;

    /// Decompose a string in a vector of strings using "." as separators.
    void decomposePath(const std::string &path, std::vector<std::string> &v);

    /**
     * @brief Returns a copy of a Potential after applying a bijection over the
     *variables in source.
     * This copies the Potential source in a new Potential by permuting all variables
     *in source with respect
     * to bij.
     *
     * @warning This method in most case creates the new Potential using a
     *gum::MultiDimBijArray, this means
     *          that the created Potential holds a reference over source, so do not
     *delete source if you
     *          still need the created potential.
     *
     * @param bij A Bijection of DiscreteVariable where firsts are variables in
     *source and seconds variables
     *            added in the returned Potential.
     * @param source The copied Potential.
     * @return a pointer over a Potential which is a copy of source.
     * @throw FatalError raised if an unknown MultiDimImplementation is encountered.
     */
    template <typename GUM_SCALAR>
    Potential<GUM_SCALAR> *copyPotential(
        const Bijection<const DiscreteVariable *, const DiscreteVariable *> &bij,
        const Potential<GUM_SCALAR> &source);

    template <typename GUM_SCALAR>
    Potential<GUM_SCALAR> *multPotential(const Potential<GUM_SCALAR> &t1,
                                         const Potential<GUM_SCALAR> &t2);
    /**
     * @brief Proceeds with the elimination of var in pool.
     * @param var The variable eliminated from every potentials in pool.
     * @param pool A pool of potentials in wich the elimination of var is done.
     * @param trash All create potentials are inserted in this set, useful to
     *              delete later.
     */
    template <typename GUM_SCALAR>
    void eliminateNode(const DiscreteVariable *var,
                       Set<Potential<GUM_SCALAR> *> &pool,
                       Set<Potential<GUM_SCALAR> *> &trash);

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/utils_prm.tcc>

#endif // GUM_UTILS_PRM_H
