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
 * @brief A class unifying the type of the DBCells for each column of the database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/database/DBTransformUnifier.h>

namespace gum {

  namespace learning {

    /// default constructor
    DBTransformUnifier::DBTransformUnifier() { GUM_CONSTRUCTOR(DBTransformUnifier); }

    /// copy constructor
    DBTransformUnifier::DBTransformUnifier(const DBTransformUnifier &from)
        : DBTransform(from) {
      GUM_CONS_CPY(DBTransformUnifier);
    }

    /// move constructor
    DBTransformUnifier::DBTransformUnifier(DBTransformUnifier &&from)
        : DBTransform(std::move(from)) {
      GUM_CONS_MOV(DBTransformUnifier);
    }

    /// destructor
    DBTransformUnifier::~DBTransformUnifier() { GUM_DESTRUCTOR(DBTransformUnifier); }

    /// copy operator
    DBTransformUnifier &DBTransformUnifier::operator=(const DBTransformUnifier &) {
      return *this;
    }

    /// move operator
    DBTransformUnifier &DBTransformUnifier::operator=(DBTransformUnifier &&) {
      return *this;
    }

    /// indicates whether a BDCell contains a missing value
    bool DBTransformUnifier::isMissing(const DBCell &cell,
                                       const std::vector<std::string> &miss) const
        noexcept {
      if (cell.type() == DBCell::EltType::MISSING)
        return true;
      if (cell.type() == DBCell::EltType::STRING) {
        for (const auto &str : miss) {
          if (cell.getString() == str)
            return true;
        }
      }
      return false;
    }

    /// transforms a vector of DBrows to unify the types of its columns
    bool DBTransformUnifier::transform(std::vector<DBRow> &db,
                                       std::vector<std::string> miss) const {
      if (!db.size())
        return true;

      // transform each column
      bool conversion_ok = true;
      unsigned int cols = db[0].size();
      for (unsigned int i = 0; i < cols; ++i) {
        // count all the observations to determine whether there exists
        // a type which appears much more frequently than the others
        std::vector<unsigned int> counts(sizeof(DBCell::EltType), 0);

        // parse the database and fill counts
        for (DBRow &row : db) {
          if (isMissing(row[i], miss)) {
            row[i].setMissingSafe();
          }
          ++counts[row[i].type()];
        }

        // get the type with the highest number of observations
        unsigned int pos = 0;
        for (unsigned int j = 1; j < counts.size(); ++j) {
          if ((j != DBCell::EltType::MISSING) && (counts[j] > counts[pos]))
            pos = j;
        }
        DBCell::EltType new_type = (DBCell::EltType)pos;

        // change the type of the non-missing elements to the new type
        for (auto &row : db) {
          DBCell::EltType type = row[i].type();
          if ((type != new_type) && (type != DBCell::EltType::MISSING)) {
            if (!row[i].convertType(new_type)) {
              conversion_ok = false;
            }
          }
        }
      }

      return conversion_ok;
    }

  } /* namespace learning */

} /* namespace gum */
