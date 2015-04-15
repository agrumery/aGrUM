/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Base class for labelized discrete random variables
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_LABELIZED_DISCRETE_VARIABLE_H
#define GUM_LABELIZED_DISCRETE_VARIABLE_H

#include <iostream>
#include <string>
#include <sstream>

#include <agrum/config.h>

#include <agrum/variables/discreteVariable.h>
#include <agrum/core/sequence.h>

namespace gum {
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                        GUM_LABELIZED_VARIABLE                        === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** class LabelizedVariable
   * @brief Base class for labelized discrete random variables
   * @ingroup multidim_group
   */
  /* ============================================================================ */

  class LabelizedVariable : public DiscreteVariable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** @brief Default constructor
    * @param aName the name
    * @param aDesc the Description
    * @param nbrLabel the domainSize (2 by default)
    */

    LabelizedVariable(const std::string &aName, const std::string &aDesc = "",
                      const int nbrLabel = 2);

    /// Copy constructor
    /**
    * @param aLDRV the variable we copy
    */

    LabelizedVariable(const LabelizedVariable &aLDRV);

    /// destructor

    virtual ~LabelizedVariable();

    /// a virtual clone ...
    virtual DiscreteVariable *clone() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param aLDRV the labelized discrete random variable we copy */

    const LabelizedVariable &operator=(const LabelizedVariable &aLDRV);

    /// returns the index of a given label
    /** @param aLabel searched label
     * @return the index of this label
     * @throw NotFound */
    Idx operator[](const std::string &aLabel) const;
    virtual Idx index(const std::string &label) const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// indicates whether the variable already has the label passed in argument
    /** @param aLabel
     * @return true if the label already exists */
    bool isLabel(const std::string &aLabel) const;

    /// add a label with a new index (we assume that we will NEVER remove a label)
    /** @param aLabel the label to be added to the labelized variable
       * @throw DuplicateElement is raised if the variable already contains
       * the label
       * @return *this which allows : v.addLabel("1").addLabel("2")...;
       */
    LabelizedVariable &addLabel(const std::string aLabel);

    /// change a label for this index
    /** @param pos the index of the label to be changed
      * @param aLabel the label to be added to the labelized variable
       * @throw DuplicateElement is raised if the variable already contains
       * @throw OutOfBounds
       * @warning Since this change does not affect the variable domain but only the
     * variable representation,
       * this method is allowed on const LabelizedVariable.
       */
    void changeLabel(Idx pos, const std::string aLabel) const;

    /// erase all the labels
    void eraseLabels(void);

    /// returns the ith label
    /** @param i
     * @return the ith label */
    virtual const std::string label(Idx i) const;

    /// get a numerical representation of he indice-the value.
    virtual double numerical(Idx indice) const;

    /// returns the size of the random discrete variable domain
    virtual Size domainSize() const;

    /// returns the set of labels of the variable

    const Sequence<std::string> &labels() const;

    /// returns the type of variable

    virtual VarType varType(void) const;

    /// @}

    protected:
    /// (protected) Default constructor
    LabelizedVariable() { GUM_CONSTRUCTOR(LabelizedVariable); };

    /// copies the content of aLDRV

    void _copy(const LabelizedVariable &aLDRV);

    private:
    /// the set of labels contained in the variable
    mutable Sequence<std::string> __labels;
  };

} /* namespace gum */

/* ============================================================================== */
/* ============================================================================== */
/* ===                    LabelizedVariable IMPLEMENTATION                    === */
/* ============================================================================== */
/* ============================================================================== */
#ifndef GUM_NO_INLINE
#include <agrum/variables/labelizedVariable.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LABELIZED_DISCRETE_VARIABLE_H */
