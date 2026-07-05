/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once

/**
 * @file
 * @brief Inline implementation of non-template classes in O3prmrContext.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6), Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */

#include <agrum/PRM/o3prmr/O3prmrContext.h>   // to ease IDE parser

namespace gum {
  namespace prm {
    namespace o3prmr {

      /* O3prmrCommand */

      INLINE O3prmrCommand::O3prmrCommand(int line) : line(line) {}

      INLINE O3prmrCommand::O3prmrCommand(const O3prmrCommand& c) : line(c.line) {}

      /* ImportCommand */

      INLINE
      ImportCommand::ImportCommand(int line, std::string_view value, std::string_view alias) :
          line(line), value(value), alias(alias) {}

      INLINE ImportCommand::ImportCommand(const ImportCommand& c) :
          line(c.line), value(c.value), alias(c.alias) {}

      INLINE std::string ImportCommand::toString() const {
        return "import " + value + (alias.empty() ? "" : "as " + alias) + ";";
      }

      /* SetEngineCommand */

      INLINE SetEngineCommand::SetEngineCommand(int line, std::string_view value) :
          O3prmrCommand(line), value(value) {}

      INLINE SetEngineCommand::SetEngineCommand(const SetEngineCommand& c) :
          O3prmrCommand(c), value(c.value) {}

      INLINE O3prmrCommand::RequestType SetEngineCommand::type() const {
        return RequestType::SetEngine;
      }

      INLINE std::string SetEngineCommand::toString() const { return "engine " + value + ";"; }

      /* SetGndEngineCommand */

      INLINE SetGndEngineCommand::SetGndEngineCommand(int line, std::string_view value) :
          O3prmrCommand(line), value(value) {}

      INLINE SetGndEngineCommand::SetGndEngineCommand(const SetGndEngineCommand& c) :
          O3prmrCommand(c), value(c.value) {}

      INLINE O3prmrCommand::RequestType SetGndEngineCommand::type() const {
        return RequestType::SetGndEngine;
      }

      INLINE std::string SetGndEngineCommand::toString() const {
        return "grd_engine " + value + ";";
      }

    }   // namespace o3prmr
  }   // namespace prm
}   // namespace gum
