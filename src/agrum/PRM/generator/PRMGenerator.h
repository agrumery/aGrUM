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

#ifndef GUM_PRM_GENERATOR_H
#define GUM_PRM_GENERATOR_H
#include <sstream>
#include <string>

#include <agrum/PRM/generator/nameGenerator.h>
#include <agrum/PRM/PRM.h>

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
