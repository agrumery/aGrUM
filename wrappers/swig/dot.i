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

/*
 * The look of the graphs drawn with graphviz (see agrum/base/core/dot/) :
 *  - gum::dot::resolveFgColor : the resolution of the special font color `adaptive`, used by the language layers for
 *    the graphs they build themselves. It is private (underscore) : the language layers wrap it (see
 *    swigsrc/gum_functions_BASE.i for pyAgrum) ;
 *  - gum::dot::parseColor (and gum::dot::Rgb which receives its result) : the reading of a color, private too
 *    (underscore), for the language layers which need the components of a color ;
 *  - gum::dot::contrastColor : the text color (white or black) readable on a background given by its components,
 *    private too (underscore) ;
 *  - gum::dot::configFgColor : a font color of the configuration resolved against its background color, private too
 *    (underscore) ;
 *  - gum::dot::DotOptions : what the caller gives to a toDot() beyond the configuration. The class is declared by
 *    hand (its header also declares the structures of the options, which have no meaning in a target language) :
 *    the nodes are designated by their identifiers, the arcs by the identifiers of their tail and head, and a
 *    factor of a factor graph by the identifiers of its variables.
 */

%{
#include <agrum/base/core/dot/dotColor.h>
#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/base/core/dot/dotStyle.h>
#include <agrum/base/graphs/graphElements.h>
%}

%rename(_resolveFgColor) gum::dot::resolveFgColor;
%rename(_Rgb) gum::dot::Rgb;
%rename(_parseColor) gum::dot::parseColor;
%rename(_contrastColor) gum::dot::contrastColor;
%rename(_configFgColor) gum::dot::configFgColor;

namespace gum {
  namespace dot {
    struct Rgb {
      unsigned char red;
      unsigned char green;
      unsigned char blue;
    };

    bool parseColor(std::string_view color, Rgb& rgb);
    std::string contrastColor(const Rgb& rgb);
    std::string configFgColor(std::string_view section, std::string_view key);
    std::string resolveFgColor(std::string_view fgColor, std::string_view bgColor, std::string_view fallback);

    class DotOptions {
      public:
      DotOptions();

      void setSize(const std::string& size);
      void setRankDir(const std::string& rankDir);
      void setLayout(const std::string& layout);
      void setLabel(const std::string& label);
      const std::string& size() const;
      const std::string& rankDir() const;
      const std::string& layout() const;
      const std::string& label() const;

      void setNodeBgColor(gum::Size node, const std::string& color);
      void setNodeFgColor(gum::Size node, const std::string& color);
      void setNodeLabel(gum::Size node, const std::string& label);
      void setNodeTooltip(gum::Size node, const std::string& tooltip);
      void setNodeAttribute(gum::Size node, const std::string& key, const std::string& value, bool verbatim = false);

      void setArcColor(gum::Size tail, gum::Size head, const std::string& color);
      void setArcWidth(gum::Size tail, gum::Size head, double width);
      void setArcLabel(gum::Size tail, gum::Size head, const std::string& label);
      void setArcTooltip(gum::Size tail, gum::Size head, const std::string& tooltip);
      void setArcAttribute(gum::Size tail,
                           gum::Size head,
                           const std::string& key,
                           const std::string& value,
                           bool verbatim = false);

      bool empty() const;
    };
  }   // namespace dot
}   // namespace gum

// the factors are designated by a set of identifiers (the typemaps of gum::NodeSet of the language layer apply)
%extend gum::dot::DotOptions {
  void setFactorBgColor(const gum::NodeSet& variables, const std::string& color) {
    self->setFactorBgColor(std::vector< gum::Size >(variables.begin(), variables.end()), color);
  }
  void setFactorFgColor(const gum::NodeSet& variables, const std::string& color) {
    self->setFactorFgColor(std::vector< gum::Size >(variables.begin(), variables.end()), color);
  }
  void setFactorLabel(const gum::NodeSet& variables, const std::string& label) {
    self->setFactorLabel(std::vector< gum::Size >(variables.begin(), variables.end()), label);
  }
  void setFactorTooltip(const gum::NodeSet& variables, const std::string& tooltip) {
    self->setFactorTooltip(std::vector< gum::Size >(variables.begin(), variables.end()), tooltip);
  }
  void setFactorAttribute(const gum::NodeSet&  variables,
                          const std::string&   key,
                          const std::string&   value,
                          bool                 verbatim = false) {
    self->setFactorAttribute(std::vector< gum::Size >(variables.begin(), variables.end()), key, value, verbatim);
  }
}
