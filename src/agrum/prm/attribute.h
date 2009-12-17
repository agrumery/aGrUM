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
 * @brief Headers of gum::Attribute.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_ATTRIBUTE_H
#define GUM_ATTRIBUTE_H
// ============================================================================
#include <agrum/multidim/multiDimImplementation.h>
// ============================================================================
#include <agrum/prm/classElement.h>
// ============================================================================
namespace gum {
// ============================================================================
/**
 * @class Attribute attribute.h <agrum/prm/attribute.h>
 * @brief Attribute is a member of a Class in a PRM.
 *
 * A Attribute is defined by its name, its containing class, its type and
 * by a Conditional Probability Function (aka CPF but represented by a Potential).
 *
 * An attribute in a PRM is the equivalent of a random variable in a bayesian
 * network.
 *
 * This class is constructed by a gum::PRMFactory and is deleted by its
 * gum::Class.
 *
 * Built-in copies (copy constructor and copy operator) are illegal due to
 * various problems raised by redondant information.
 *
 * @see PRM PRMFactory Class ClassElement Type Potential
 */
class Attribute: public ClassElement {
  public:
  // ========================================================================
  /// @name Constructor & destructor.
  // ========================================================================
    /// @{

    /**
     * @brief Constructor used by gum::Class.
     * This will create an Attribute with only one variable: type and with the given
     * implementation.
     * @param name The name of this Attribute.
     * @param type The type of this Attribute, it is copied.
     * @param impl The MultiDimImplementation used by the internal Potential of this Attribute.
     *             it will be deleted after the call of ~Attribute.
     */
    Attribute(const std::string& name, const Type& type,
              MultiDimImplementation<prm_float>* impl = new MultiDimArray<prm_float>());

    /**
     * @brief Constructor used by gum::Instance.
     * This will create an Attribute with a ready Potential, however it will check the existence
     * of type in cpf and raise an exception if it is not found.
     * @param type The type of this attribute, it will be deleted after a call to ~Attribute.
     * @param name The name of this Attribute.
     * @param cpf The Potential of this Attribute, it will be deleted after the call of
     *            ~Attribute.
     */
    Attribute(const std::string& name, Type* type, Potential<prm_float>* cpf);

    /**
     * @brief Constructor used by gum::Instance.
     * This will create an Attribute with a ready Potential, however it will check the existence
     * of type in cpf and raise an exception if it is not found.
     * Here type is not given to this attribute: it will not be deleted after a call to
     * ~Attribute().
     * @param name The name of this Attribute.
     * @param type The type of this attribute.
     * @param cpf The Potential of this Attribute, it will be deleted after the call of
     *            ~Attribute.
     */
    Attribute(const std::string& name, Type& type, Potential<prm_float>* cpf);

    // /**
    //  * @brief Constructor of instantiating gum::Aggregate.
    //  *
    //  * @param i The Instance in which agg is instantiated.
    //  * @param agg The instantiated Aggregate.
    //  */
    // Attribute(const Instance& i, const Aggregate& agg);

    // /**
    //  * @brief Constructor when instantiating a gum::Attribute in a gum::Instance.
    //  *
    //  * @param i The Instance in which attr is instantiated.
    //  * @param attr The instantiated Attribute.
    //  */
    // Attribute(const Instance& i, const Attribute& attr);

    /// Destructor.
    virtual ~Attribute();

    /// @}
  // ========================================================================
  /// @name Getters & setters
  // ========================================================================
    /// @{

    /// See gum::ClassElement::elt_type().
    virtual ClassElementType elt_type() const;

    /// See gum::ClassElement::type().
    virtual Type& type();

    /// See gum::ClassElement::type().
    virtual const Type& type() const;

    /// See gum::ClassElement::cpf().
    virtual Potential<prm_float>& cpf();

    /// See gum::ClassElement::cpf().
    virtual const Potential<prm_float>& cpf() const;

    /// See gum::ClassElement::_addParent().
    virtual void addParent(const ClassElement& elt);

    /// See gum::ClassElement::_addChild().
    virtual void addChild(const ClassElement& elt);

    /// @}
  private:

    /// Copy constructor. Don't use it.
    Attribute(const Attribute &source);

    /// Copy operator. Don't use it.
    Attribute& operator=(const Attribute& from);

  // ========================================================================
  /// @name Private members
  // ========================================================================
    /// @{

    /// The random variable type of this attribute
    Type* __type;

    /// A pointer on the Potential of this attribute
    Potential<prm_float>* __cpf;

    /// Flag to know if we can delete type.
    bool __delete_type;

    /// @}
};
// ============================================================================
} // namespace gum
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/attribute.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_ATTRIBUTE_H */
// ============================================================================
