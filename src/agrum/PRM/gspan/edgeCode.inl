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
 * @brief Inline implementation of the EdgeCode class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {
    namespace gspan {

      INLINE
      EdgeCode::EdgeCode(Size my_i, Size my_j, Size my_l_i, Size my_l_ij,
                         Size my_l_j)
          : i(my_i), j(my_j), l_i(my_l_i), l_ij(my_l_ij), l_j(my_l_j) {
        GUM_CONSTRUCTOR(EdgeCode);
        std::stringstream sBuff;
        sBuff << i << j << l_i << l_ij << l_j;
        name = sBuff.str();
      }

      INLINE
      EdgeCode::EdgeCode(const EdgeCode &source)
          : i(source.i), j(source.j), l_i(source.l_i), l_ij(source.l_ij),
            l_j(source.l_j), name(source.name) {
        GUM_CONS_CPY(EdgeCode);
      }

      INLINE
      EdgeCode::~EdgeCode() { GUM_DESTRUCTOR(EdgeCode); }

      INLINE
      bool EdgeCode::isForward() const { return i < j; }

      INLINE
      bool EdgeCode::isBackward() const { return i > j; }

      INLINE
      EdgeCode &EdgeCode::operator=(const EdgeCode &source) {
        i = source.i;
        j = source.j;
        l_i = source.l_i;
        l_ij = source.l_ij;
        l_j = source.l_j;
        return *this;
      }

      INLINE
      bool EdgeCode::operator==(const EdgeCode &code) const {
        return ((i == code.i) and (j == code.j) and (l_i == code.l_i) and
                (l_ij == code.l_ij) and (l_j == code.l_j));
      }

      INLINE
      bool EdgeCode::operator!=(const EdgeCode &code) const {
        return ((i != code.i) or (j != code.j) or (l_i != code.l_i) or
                (l_ij != code.l_ij) or (l_j != code.l_j));
      }

      INLINE
      bool EdgeCode::operator<(const EdgeCode &code) const {
        if ((i == code.i) and (j == code.j)) {
          return (l_i < code.l_i) or ((l_i == code.l_i) and (l_ij < code.l_ij)) or
                 ((l_i == code.l_i) and (l_ij == code.l_ij) and (l_j < code.l_j));
        } else {
          return ((i == code.i) and (j < code.j)) or
                 ((i < code.j) and (j == code.i));
        }
      }

    } // namespace gspan
  }   // namespage prm
} // namespace gum
