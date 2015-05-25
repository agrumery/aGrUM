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
 * @brief Headers of PRMGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <string>
#include <sstream>

#include <agrum/config.h>
#include <agrum/PRM/elements/PRMObject.h>

#ifndef GUM_NAME_GENERATOR_H
#define GUM_NAME_GENERATOR_H
namespace gum {
  namespace prm {

    /**
     * @class NameGenerator PRMGenerator.h <agrum/PRM/generator/PRMGenerator.h>
     * @brief This is a name generator for classes, types, systems, instances and
     *        class elements.
     *
     * The generation simply use an unsigned int starting at 1 and increasing for
     * each new name. Different numbers are used for each different type of
     * PRMObject.
     */
    class NameGenerator {
      public:
      // ========================================================================
      /// @name Constructors and destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      NameGenerator();
      /// Copy constructor.
      NameGenerator(const NameGenerator &source);
      /// Destructor.
      virtual ~NameGenerator();
      /// Affectation operator
      NameGenerator &operator=(const NameGenerator &source);

      /// @}
      // ========================================================================
      /// Getters and setters.
      // ========================================================================
      /// @{

      /// Returns the next name w.r.t. the given Type.
      std::string nextName(PRMObject::PRMType type);

      /// @}
      private:
      /// The vector of counters.
      std::vector<unsigned int> __counters;
    };

  } /* namespace prm */
} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/PRM/generator/nameGenerator.inl>
#endif

#endif /* GUM_NAME_GENERATOR_H */
