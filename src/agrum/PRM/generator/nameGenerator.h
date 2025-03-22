/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of PRMGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/PRM/elements/PRMObject.h>

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
