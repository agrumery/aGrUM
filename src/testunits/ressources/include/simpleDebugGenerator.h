/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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

/** @file
 * @brief A DBRowGenerator class that returns exactly k times the rows it gets
 * in input
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_SIMPLEDEBUGGENERATOR_H
#define GUM_LEARNING_SIMPLEDEBUGGENERATOR_H

#include <agrum/agrum.h>

#include <agrum/base/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    class SimpleDebugGenerator: public DBRowGenerator {
      public:
      // ##########################################################################
      // @name Constructors / Destructors
      // ##########################################################################

      /// default constructor
      SimpleDebugGenerator(const std::vector< DBTranslatedValueType > column_types,
                           const std::size_t                          nb_duplicates) :
          DBRowGenerator(column_types, DBRowGeneratorGoal::OTHER_THINGS_THAN_REMOVE_MISSING_VALUES),
          _nb_duplicates_(nb_duplicates) {
        GUM_CONSTRUCTOR(SimpleDebugGenerator);
      }

      /// copy constructor
      SimpleDebugGenerator(const SimpleDebugGenerator& from) :
          DBRowGenerator(from), _input_row_(from._input_row_),
          _nb_duplicates_(from._nb_duplicates_) {
        GUM_CONS_CPY(SimpleDebugGenerator);
      }

      /// move constructor
      SimpleDebugGenerator(SimpleDebugGenerator&& from) :
          DBRowGenerator(std::move(from)), _input_row_(from._input_row_),
          _nb_duplicates_(from._nb_duplicates_) {
        GUM_CONS_MOV(SimpleDebugGenerator);
      }

      /// virtual copy constructor
      virtual SimpleDebugGenerator* clone() const { return new SimpleDebugGenerator(*this); }

      /// destructor
      ~SimpleDebugGenerator() { GUM_DESTRUCTOR(SimpleDebugGenerator); }

      // ##########################################################################
      // @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      SimpleDebugGenerator& operator=(const SimpleDebugGenerator& from) {
        DBRowGenerator::operator=(from);
        _input_row_     = from._input_row_;
        _nb_duplicates_ = from._nb_duplicates_;
        return *this;
      }

      /// move operator
      SimpleDebugGenerator& operator=(SimpleDebugGenerator&& from) {
        DBRowGenerator::operator=(std::move(from));
        _input_row_     = from._input_row_;
        _nb_duplicates_ = from._nb_duplicates_;
        return *this;
      }

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// generates new lines from those the generator gets in input
      virtual const DBRow< DBTranslatedValue >& generate() final {
        this->decreaseRemainingRows();
        return *_input_row_;
      }


      protected:
      /// computes the rows it will provide in output
      virtual std::size_t computeRows_(const DBRow< DBTranslatedValue >& row) final {
        _input_row_ = &row;
        return _nb_duplicates_;
      }


      private:
      /// the row used as input to generate the output DBRows
      const DBRow< DBTranslatedValue >* _input_row_{nullptr};

      /// the number of times we return each input row
      std::size_t _nb_duplicates_{std::size_t(1)};
    };

  } /* namespace learning */

} /* namespace gum */


#endif /* GUM_LEARNING_SIMPLEDEBUGGENERATOR_H */
