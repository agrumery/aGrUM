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
/** @file
 * @brief The classes that represents database columns and their increments
 *
 * This file contains essentially classes Col<> and Incr<> which represent
 * columns and increments respectively. For instance, Col<3,6> represents the
 * pair of columns 3 and 6. Incr<1> represents an increment of a column by 1.
 * These classes are used to specify on which columns database translators
 * will be applied.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_COLUMNS_H
#define GUM_LEARNING_COLUMNS_H

#include <iostream>
#include <string>
#include <sstream>

namespace gum {

  namespace learning {

// ============================================================================
// COLUMN IMPLEMENTATION
// ============================================================================

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /// the general type for the columns
    template <int Idx, int... NextIdx> struct Col;

    /// the specialized type for the specification of 1 column
    template <int Idx> struct Col<Idx> {
      /// the number of columns specified by this class
      static constexpr unsigned int size = 1;

      /// saves the column into an array
      static void toArray(unsigned int array[]) noexcept { *array = Idx; }

      /// to display the content of the column
      std::string toString() const noexcept {
        std::stringstream str;
        str << Idx;
        return str.str();
      }
    };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /** @class Col
     * @brief A generic class to specify columns in a database
     * @ingroup learning_group
     *
     * Learning algorithms use row filters to read the content of tabular databases
     * (like CSVs or MySQL databases). These row filters use translators and
     * generators. The former translate the "generic" content of the columns of
     * the database into integer-encoded contents. To specify on which columns
     * each translator will work, you should use class Col. For instance, if you
     * wish to express that a translator should be applied on columns 1, 7, 4 in
     * order to produce one translated column, you just need to indicate that it
     * will be applied on Col<1,7,4>. Col<>s can have an arbitrary number of
     * column numbers in argument.
     */
    template <int Idx, int... NextIdx> struct Col : public Col<NextIdx...> {
      /// the number of columns specified by this class
      static constexpr unsigned int size = 1 + sizeof...(NextIdx);

      /// saves the columns into an array of integers
      static void toArray(unsigned int array[]) noexcept {
        *array = Idx;
        Col<NextIdx...>::toArray(array + 1);
      }

      /// to display the content of the columns
      std::string toString() {
        std::stringstream str;
        str << Idx << " " << Col<NextIdx...>::toString();
        return str.str();
      }
    };

// ============================================================================
// COLUMNS ADDITIONS
// ============================================================================

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /// the generic concatenation of columns
    template <typename, typename> struct ConcatCols;

    /// The class used to concatenate two Cols
    template <int Idx1, int Idx2, int... NextIdx2>
    struct ConcatCols<Col<Idx1>, Col<Idx2, NextIdx2...>> {
      using type = Col<Idx1, Idx2, NextIdx2...>;
    };

    /// the generic class for adding two Cols
    template <typename Col1, typename Col2> struct AddCols;

    /// the specialized addition of two columns
    template <int Idx1, int Idx2> struct AddCols<Col<Idx1>, Col<Idx2>> {
      using type = Col<Idx1 + Idx2>;
    };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /** @class AddCols
     * @ingroup learning_group
     * @brief The class used to add (by metaprogramming) the contents of two
     * Col<>s of the same size (usually, the second is an increment).
     *
     * AddCols takes as template parameters two Cols<> of the same size and adds
     * pointwise their values. For instance, AddCols<Col<1,3>,Cols<2,4>> will
     * produce a new Col<3,7>, since 3 = 1 + 2 and 7 = 3 + 4. The result of the
     * addition is available through a "type" typedef, i.e., trhough
     * AddCols<Col<1,3>,Cols<2,4>>::type
     */
    template <int Idx1, int Idx2, int... NextIdx1, int... NextIdx2>
    struct AddCols<Col<Idx1, NextIdx1...>, Col<Idx2, NextIdx2...>> {
      using type = typename ConcatCols<
          Col<Idx1 + Idx2>,
          typename AddCols<Col<NextIdx1...>, Col<NextIdx2...>>::type>::type;
    };

// ============================================================================
// COLUMNS INCREMENTS
// ============================================================================

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    // make a default increment for a given Col
    template <typename> struct Make_Default_Incr;

    // the specialization of Make_Default_Incr for 1 column
    template <int Idx> struct Make_Default_Incr<Col<Idx>> { using type = Col<1>; };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    /** @class Make_Default_Incr
     * @ingroup learning_group
     * @brief A Metaprogramming class for creating default increments to Col<>s
     *
     * This class takes in argument a Col<> and computes the Col<> that should
     * result from an addition by 1 to each of its columns. For instance,
     * Make_Default_Incr< Col<1,8,2> > will produce a Col<2,9,3>. The result is
     * accessible through a "type" typedef, i.e., through
     * Make_Default_Incr< Col<1,8,2> >::type.
     */
    template <int Idx, int... NextIdx>
    struct Make_Default_Incr<Col<Idx, NextIdx...>>
        : public Make_Default_Incr<Col<NextIdx...>> {
      using NextDefaultIncr = Make_Default_Incr<Col<NextIdx...>>;
      using type = typename ConcatCols<Col<1>, typename NextDefaultIncr::type>::type;
    };

    /** @class Incr
     * @ingroup learning_group
     * @brief The class for specifying increments of Col<>s
     *
     * To specify that you want to increment Col<2,3,4> by <1,6,3>, just create
     * an Incr<1,6,3>.
     */
    template <int Idx, int... NextIdx> using Incr = Col<Idx, NextIdx...>;

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_COLUMNS_H */
