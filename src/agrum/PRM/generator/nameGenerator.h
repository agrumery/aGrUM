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

#include <sstream>
#include <string>

#include <agrum/PRM/elements/PRMObject.h>
#include <agrum/agrum.h>

#ifndef GUM_NAME_GENERATOR_H
#  define GUM_NAME_GENERATOR_H
namespace gum {
  namespace prm {

    /**
     * @class NameGenerator
     * @headerfile PRMGenerator.h <agrum/PRM/generator/PRMGenerator.h>
     * @brief This is a name generator for classes, types, systems, instances
     *and
     *        class elements.
     *
     * The generation simply use an unsigned int starting at 1 and increasing
     *for
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
      NameGenerator(const NameGenerator& source);
      /// Destructor.
      virtual ~NameGenerator();
      /// Affectation operator
      NameGenerator& operator=(const NameGenerator& source);

      /// @}
      // ========================================================================
      /// Getters and setters.
      // ========================================================================
      /// @{

      /// Returns the next name w.r.t. the given PRMType.
      std::string nextName(PRMObject::prm_type type);

      /// @}
      private:
      /// The vector of counters.
      std::vector< Size > _counters_;
    };

  } /* namespace prm */
} /* namespace gum */

#  ifndef GUM_NO_INLINE
#    include <agrum/PRM/generator/nameGenerator_inl.h>
#  endif

#endif /* GUM_NAME_GENERATOR_H */
