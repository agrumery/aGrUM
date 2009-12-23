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
 * @brief Headers of gum::Interface.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_INTERFACE_H
#define GUM_INTERFACE_H
// ============================================================================
#include <agrum/prm/classElementContainer.h>
// ============================================================================
namespace gum {
namespace prm {

class Interface: public ClassElementContainer {
  public:
  // ========================================================================
  /// @name Constructor & destructor.
  // ========================================================================
    /// @{

    /**
     * Default constructor.
     * @param name The Interface name.
     */
    Interface(const std::string& name);

    /**
     * Constructor for building a sub interface of super.
     * @param name The sub Interface name.
     * @param super The Interface which this is an extension.
     */
    Interface(const std::string& name, Interface& super);

    /**
     * Destructor.
     */
    virtual ~Interface();

    /// @}
  // ========================================================================
  /// @name Getters & setters
  // ========================================================================
    /// @{

    /**
     * Implementation of pure virtual method of PRMObject.
     */
    virtual ObjectType obj_type() const;

    /**
     * Returns the super of this Interface.
     * @throw NotFound Raised if this has no super.
     */
    Interface& super();

    /**
     * Returns the super of this Interface.
     * @throw NotFound Raised if this has no super.
     */
    const Interface& super() const;

    /**
     * @brief Add an attribute to this Interface.
     *
     * The pointer is "given" to this Interface, which will delete when
     * ~Interface() is called.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void add(Attribute* attr);

    /**
     * @brief Add an aggregate to this Interface.
     *
     * The pointer is "given" to this Interface, which will delete when
     * Interface() is called.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void add(Aggregate* agg);

    /**
     * @brief Add a ReferenceSlot to this Interface.
     *
     * The pointer is "given" to this Interface, which will delete when
     * Interface() is called.
     *
     * @throw DuplicateElement Raised if an element in this has the same name.
     */
    void add(ReferenceSlot* ref);

    /// @}
  protected:

    /// See ClassElementContainer::_isSubTypeOf().
    virtual bool _isSubTypeOf(const ClassElementContainer& cec) const;

  private:
    /// Copy constructor. Don't use it.
    Interface(const Interface& source);

    /// Copy operator. Don't use it.
    Interface& operator=(const Interface& source);
};

} /* namespace prm */
} // namespace gum
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/interface.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif // GUM_INTERFACE_H
// ============================================================================
