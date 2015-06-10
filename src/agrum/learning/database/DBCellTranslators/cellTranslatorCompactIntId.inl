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
 * @brief An identity function cell translator on integers that assumes the
 * integers it reads forms a compact (interval { 0 ,..., n } )
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE
    CellTranslatorCompactIntId::CellTranslatorCompactIntId(bool check_database)
        : __check_database(check_database) {}

    /// copy constructor
    INLINE CellTranslatorCompactIntId::CellTranslatorCompactIntId(
        const CellTranslatorCompactIntId &from)
        : DBCellTranslator<1, 1>(from), __values(from.__values),
          __check_database(from.__check_database) {}

    /// move constructor
    INLINE CellTranslatorCompactIntId::CellTranslatorCompactIntId(
        CellTranslatorCompactIntId &&from)
        : DBCellTranslator<1, 1>(std::move(from)),
          __values(std::move(from.__values)),
          __check_database(std::move(from.__check_database)) {}

    /// virtual copy constructor
    INLINE CellTranslatorCompactIntId *CellTranslatorCompactIntId::copyFactory() {
      return new CellTranslatorCompactIntId(*this);
    }

    /// destructor
    INLINE CellTranslatorCompactIntId::~CellTranslatorCompactIntId() {}

    /// copy operator
    INLINE CellTranslatorCompactIntId &CellTranslatorCompactIntId::
    operator=(const CellTranslatorCompactIntId &from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(from);
        __values = from.__values;
        __check_database = from.__check_database;
      }
      return *this;
    }

    /// move operator
    INLINE CellTranslatorCompactIntId &CellTranslatorCompactIntId::
    operator=(CellTranslatorCompactIntId &&from) {
      if (this != &from) {
        DBCellTranslator<1, 1>::operator=(std::move(from));
        __values = std::move(from.__values);
        __check_database = std::move(from.__check_database);
      }
      return *this;
    }

    /// perform the translation
    ALWAYS_INLINE void CellTranslatorCompactIntId::translate() {
      out(0) = in(0).getFloat();
    }

    /// initialize the cell translator by a first database parsing
    ALWAYS_INLINE void CellTranslatorCompactIntId::initialize() {
      if (__check_database) {
        const int nb = in(0).getFloat();
        if (nb < 0) {
          GUM_ERROR(WrongType, "the CellTranslatorCompactIntId has read "
                               "a negative value");
        }
        if (!__values.exists(nb))
          __values.insert(nb);
      }
    }

    /// perform a post initialization after the database parsing
    INLINE void CellTranslatorCompactIntId::postInitialize() {
      if (__check_database) {
        // check that the values form a compact
        unsigned int max_val = 0;
        for (const auto &val : __values) {
          if (val > max_val)
            max_val = val;
        }
        if (max_val >= __values.size()) {
          GUM_ERROR(WrongType, "The CellTranslatorCompactIntId is applied on a "
                               "non-compact set of integers");
        }
      }
    }

    /// add the number of modalities discovered in the database into a vector
    INLINE void
    CellTranslatorCompactIntId::modalities(std::vector<unsigned int> &modal) const
        noexcept {
      if (__check_database) {
        modal.push_back(__values.size());
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "the CellTranslatorCompactIntId has not "
                  "been initialized, so getting its modalities is impossible");
      }
    }

    /// returns whether the translator needs a DB parsing to initialize itself
    INLINE bool CellTranslatorCompactIntId::requiresInitialization() const noexcept {
      return __check_database;
    }

    /// returns a given value as stored within the database
    INLINE std::string
    CellTranslatorCompactIntId::translateBack(unsigned int col,
                                              unsigned int translated_val) const {
      std::stringstream str;
      str << translated_val;
      return str.str();
    }

    /// returns the name of the variable(s) the translator has processed
    INLINE void CellTranslatorCompactIntId::variableNames(
        const std::vector<std::string> &db_var,
        std::vector<std::string> &output_vars) const {
      output_vars.push_back(db_var[_input_cols[0]]);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
