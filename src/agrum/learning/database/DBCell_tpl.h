
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


/** @file
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    // checks whether a string correspond to a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBCell::isMissing(
       const std::string&                                      str,
       const std::vector< std::string, ALLOC< std::string > >& missingVals) {
      for (auto missing : missingVals) {
        if (str == missing) return true;
      }
      return false;
    }

    // returns the best type to store a given element encoded as a string
    template < template < typename > class ALLOC >
    INLINE DBCell::EltType DBCell::bestType(
       const std::string&                                      str,
       const std::vector< std::string, ALLOC< std::string > >& missingVals) {
      if (isMissing(str, missingVals)) return EltType::MISSING;
      if (isInteger(str)) return EltType::INTEGER;
      if (isReal(str)) return EltType::REAL;
      return EltType::STRING;
    }


    // returns the DBCell with the best type for an element encoded as a string
    template < template < typename > class ALLOC >
    INLINE DBCell DBCell::bestDBCell(
       const std::string&                                      str,
       const std::vector< std::string, ALLOC< std::string > >& missingVals) {
      if (isMissing(str, missingVals)) return DBCell();
      if (isInteger(str)) return DBCell(std::stoi(str));
      if (isReal(str)) return DBCell(std::stof(str));

      return DBCell(str);
    }


    /// returns the content of the DBCell as a string, whatever its type
    template < template < typename > class ALLOC >
    std::string DBCell::toString(
       const std::vector< std::string, ALLOC< std::string > >& missingVals) const {
      switch (__type) {
        case EltType::STRING: return __strings().first(__val_index);

        case EltType::REAL: {
          char buffer[100];
          sprintf(buffer, "%g", __val_real);
          return std::string(buffer);
        }

        case EltType::INTEGER: return std::to_string(__val_integer);

        case EltType::MISSING:
          if (missingVals.size())
            return missingVals[0];
          else
            GUM_ERROR(UndefinedElement, "no missing value symbol found");

        default:
          GUM_ERROR(NotImplementedYet, "type not supported by DBCell toString");
      }
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
