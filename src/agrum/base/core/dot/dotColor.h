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

/**
 * @file
 * @brief Colors of the graphs drawn with graphviz: reading of a color, brightness, readable text
 * color.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DOT_COLOR_H
#define GUM_DOT_COLOR_H

#include <string>

#include <agrum/agrum.h>

#include <string_view>

namespace gum::dot {

  /// a color given by its three components, each one between 0 and 255
  struct Rgb {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
  };

  /// the special value of a font color: black or white, according to the brightness of the
  /// background
  inline constexpr std::string_view ADAPTIVE_COLOR = "adaptive";

  /**
   * @brief Reads a color as it is written in a dot attribute.
   *
   * The accepted spellings are `#RGB`, `#RGBA`, `#RRGGBB`, `#RRGGBBAA` (hexadecimal digits in any
   * case; the transparency is ignored) and the 146 names of the HTML colors (`burlywood`,
   * `DarkBlue`... whatever the case), with or without surrounding double quotes. Any other spelling
   * (`transparent`, `/blues9/3`, `0.5 0.5 0.5`, an unknown name...) is not interpretable.
   *
   * @param color the spelling of the color.
   * @param rgb receives the components if (and only if) the function returns true.
   * @return true if the color is interpretable.
   */
  GUM_SHARED_PUBLIC bool parseColor(std::string_view color, Rgb& rgb);

  /**
   * @brief Perceived brightness of a color, between 0 (black) and 255 (white): `0.299 R + 0.587 G +
   * 0.114 B`.
   */
  GUM_SHARED_PUBLIC double brightness(const Rgb& rgb);

  /**
   * @brief The text color which is readable on a background: `white` if its brightness is at most
   * 153, `black` otherwise.
   */
  GUM_SHARED_PUBLIC std::string contrastColor(const Rgb& rgb);

  /**
   * @brief The font color to use on a background, once the special value `adaptive` is resolved.
   *
   * @param fgColor the configured font color. If it is not `adaptive` (whatever the case), it is
   * returned as is.
   * @param bgColor the background color the text is drawn on.
   * @param fallback returned when `fgColor` is `adaptive` and `bgColor` is not interpretable
   * (`transparent`, an unknown name...): usually the color of the arcs, which follows the light or
   * dark theme.
   * @return `fgColor`, or `white` / `black` according to `bgColor`, or `fallback`.
   */
  GUM_SHARED_PUBLIC std::string
      resolveFgColor(std::string_view fgColor, std::string_view bgColor, std::string_view fallback);

}   // namespace gum::dot

#endif   // GUM_DOT_COLOR_H
