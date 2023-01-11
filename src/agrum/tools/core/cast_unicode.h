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
 * @brief Unicode helper functions.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef CAST_UNICODE_H
#define CAST_UNICODE_H

#include <iostream>
#include <locale>
#include <sstream>
#include <string>

/**
 * @brief Cast a std::string into a std::wstring.
 * @param str The std::string to cast.
 * @return Returns the std::wstring of str.
 */
std::wstring widen(const std::string& str);

/**
 * @brief Cast a std::wstring into a std::string.
 * @param str The std::wstring to cast.
 * @return Returns the std::string of str.
 */
std::string narrow(const std::wstring& str);

#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/cast_unicode_inl.h>
#endif

#endif   // CAST_UNICODE_H
