/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of PRMGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PRM_GENERATOR_H
#define GUM_PRM_GENERATOR_H
#include <sstream>
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/generator/nameGenerator.h>

namespace gum {
  namespace prm {

    /**
     * @class PRMGenerator
     * @headerfile PRMGenerator.h <agrum/PRM/generator/PRMGenerator.h>
     * @brief This class is the base class to all PRM generators.
     *
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMGenerator {
      public:
      // ========================================================================
      /// @name Constructors and destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      PRMGenerator();
      /// Copy constructor.
      PRMGenerator(const PRMGenerator& source);
      /// Destructor.
      virtual ~PRMGenerator();

      /// @}
      // ========================================================================
      /// Getters and setters.
      // ========================================================================
      /// @{

      /// Defines the NameGenerator used by this generator.
      void setNameGenerator(const NameGenerator& name_gen);

      /// Returns the NameGenerator used by this generator.
      const NameGenerator& getNameGenerator() const;

      /// Proceeds with the generation of the PRM.
      virtual PRM< GUM_SCALAR >* generate() = 0;

      /// @}
      protected:
      /// The name generator used by this class.
      NameGenerator name_gen_;
    };

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/generator/PRMGenerator_tpl.h>

#endif /* GUM_PRM_GENERATOR_H */
