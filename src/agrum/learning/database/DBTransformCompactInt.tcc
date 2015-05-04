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
 * @brief A class that transforms DBCell into compact ints (to speed-up learning)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

namespace gum {

  namespace learning {

    /// transforms a vector of DBrows to unify the types of its columns
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    bool DBTransformCompactInt::transform(
        DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &filter) const {
      // put the filter at the beginning of the database
      filter.reset();

      // get the handler of the database
      DBHandler &handler = filter.handler();

      // parse the database
      try {
        while (filter.hasRows()) {
          // get the initial row
          DBRow &input_row = handler.row();

          // get the row as converted by the cell translators
          FilteredRow &output_row = filter.row();

          // assign the output row to the intput
          for (unsigned int i = 0, size = input_row.size(); i < size; ++i) {
            input_row[i].setFloat(output_row[i]);
          }
        }
      } catch (NotFound &) {
      } // NotFound is raised if the row filter does not
        // produce any output row for the last rows of
        // the database

      return true;
    }

  } /* namespace learning */

} /* namespace gum */
