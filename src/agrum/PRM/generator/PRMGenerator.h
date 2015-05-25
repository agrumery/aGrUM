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

#ifndef GUM_PRM_GENERATOR_H
#define GUM_PRM_GENERATOR_H
#include <string>
#include <sstream>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/generator/nameGenerator.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMGenerator PRMGenerator.h <agrum/PRM/generator/PRMGenerator.h>
     * @brief This class is the base class to all PRM generators.
     *
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR> class PRMGenerator {
      public:
      // ========================================================================
      /// @name Constructors and destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      PRMGenerator();
      /// Copy constructor.
      PRMGenerator(const PRMGenerator &source);
      /// Destructor.
      virtual ~PRMGenerator();

      /// @}
      // ========================================================================
      /// Getters and setters.
      // ========================================================================
      /// @{

      /// Defines the NameGenerator used by this generator.
      void setNameGenerator(const NameGenerator &name_gen);

      /// Returns the NameGenerator used by this generator.
      const NameGenerator &getNameGenerator() const;

      /// Proceeds with the generation of the PRM.
      virtual PRM<GUM_SCALAR> *generate() = 0;

      /// @}
      protected:
      /// The name generator used by this class.
      NameGenerator _name_gen;
    };

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/generator/PRMGenerator.tcc>

#endif /* GUM_PRM_GENERATOR_H */
