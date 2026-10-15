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


#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <agrum/base/core/dot/dotColor.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct DotColorTestSuite {};

  namespace {
    gum::dot::Rgb dotColorOf(const std::string& color) {
      gum::dot::Rgb rgb{0, 0, 0};
      CHECK(gum::dot::parseColor(color, rgb));
      return rgb;
    }

    bool dotColorIs(const std::string& color, int red, int green, int blue) {
      gum::dot::Rgb rgb{0, 0, 0};
      return gum::dot::parseColor(color, rgb) && rgb.red == red && rgb.green == green
          && rgb.blue == blue;
    }
  }   // namespace

  GUM_TEST(HexadecimalColors) {
    CHECK(dotColorIs("#000000", 0, 0, 0));
    CHECK(dotColorIs("#FFFFFF", 255, 255, 255));
    CHECK(dotColorIs("#404040", 64, 64, 64));
    CHECK(dotColorIs("#4A4A4A", 74, 74, 74));
    CHECK(dotColorIs("#a08080", 160, 128, 128));   // any case
    CHECK(dotColorIs("#A0a0Ff", 160, 160, 255));
    // 3 digits: each digit is doubled (#abc is #aabbcc)
    CHECK(dotColorIs("#abc", 170, 187, 204));
    CHECK(dotColorIs("#F00", 255, 0, 0));
    // the transparency is ignored
    CHECK(dotColorIs("#11223344", 17, 34, 51));
    CHECK(dotColorIs("#abcd", 170, 187, 204));
    // surrounding double quotes, as in a dot attribute
    CHECK(dotColorIs("\"#808080\"", 128, 128, 128));
    CHECK(dotColorIs("\"red\"", 255, 0, 0));
  }

  GUM_TEST(NotInterpretableColors) {
    for (const std::string& c: std::vector< std::string >{"",
                                                          "\"\"",
                                                          "#",
                                                          "#1",
                                                          "#12",
                                                          "#12345",
                                                          "#1234567",
                                                          "#123456789",
                                                          "#GGGGGG",
                                                          "#12345G",
                                                          "#1234567G",
                                                          "##123456",
                                                          "transparent",
                                                          "notacolor",
                                                          "red ",
                                                          " red",
                                                          "/blues9/3",
                                                          "0.5 0.5 0.5",
                                                          "0.650,0.200,1.000"}) {
      INFO("color '" << c << "'");
      gum::dot::Rgb rgb{1, 2, 3};
      CHECK_FALSE(gum::dot::parseColor(c, rgb));
      // the result is untouched
      CHECK_EQ(rgb.red, 1);
      CHECK_EQ(rgb.green, 2);
      CHECK_EQ(rgb.blue, 3);
    }
  }

  GUM_TEST(NamedColorsAreCaseInsensitive) {
    CHECK(dotColorIs("burlywood", 0xDE, 0xB8, 0x87));
    CHECK(dotColorIs("Burlywood", 0xDE, 0xB8, 0x87));
    CHECK(dotColorIs("BURLYWOOD", 0xDE, 0xB8, 0x87));
    CHECK(dotColorIs("AliceBlue", 0xF0, 0xF8, 0xFF));
    CHECK(dotColorIs("aliceblue", 0xF0, 0xF8, 0xFF));
    CHECK(dotColorIs("DarkBlue", 0, 0, 139));
    CHECK(dotColorIs("lightcyan", 0xE0, 0xFF, 0xFF));
    CHECK(dotColorIs("palegreen", 0x98, 0xFB, 0x98));
    CHECK(dotColorIs("coral", 0xFF, 0x7F, 0x50));
  }

  GUM_TEST(AllTheHtmlColors) {
    // also checks the order of the table (a binary search is used)
    struct Named {
      const char*   name;
      std::uint32_t rgb;
    };

    const std::vector< Named > colors{
        {"aliceblue", 0xF0F8FF},
        {"antiquewhite", 0xFAEBD7},
        {"aqua", 0x00FFFF},
        {"aquamarine", 0x7FFFD4},
        {"azure", 0xF0FFFF},
        {"beige", 0xF5F5DC},
        {"bisque", 0xFFE4C4},
        {"black", 0x000000},
        {"blanchedalmond", 0xFFEBCD},
        {"blue", 0x0000FF},
        {"blueviolet", 0x8A2BE2},
        {"brown", 0xA52A2A},
        {"burlywood", 0xDEB887},
        {"cadetblue", 0x5F9EA0},
        {"chartreuse", 0x7FFF00},
        {"chocolate", 0xD2691E},
        {"coral", 0xFF7F50},
        {"cornflowerblue", 0x6495ED},
        {"cornsilk", 0xFFF8DC},
        {"crimson", 0xDC143C},
        {"cyan", 0x00FFFF},
        {"darkblue", 0x00008B},
        {"darkcyan", 0x008B8B},
        {"darkgoldenrod", 0xB8860B},
        {"darkgray", 0xA9A9A9},
        {"darkgreen", 0x006400},
        {"darkgrey", 0xA9A9A9},
        {"darkkhaki", 0xBDB76B},
        {"darkmagenta", 0x8B008B},
        {"darkolivegreen", 0x556B2F},
        {"darkorange", 0xFF8C00},
        {"darkorchid", 0x9932CC},
        {"darkred", 0x8B0000},
        {"darksalmon", 0xE9967A},
        {"darkseagreen", 0x8FBC8F},
        {"darkslateblue", 0x483D8B},
        {"darkslategray", 0x2F4F4F},
        {"darkslategrey", 0x2F4F4F},
        {"darkturquoise", 0x00CED1},
        {"darkviolet", 0x9400D3},
        {"deeppink", 0xFF1493},
        {"deepskyblue", 0x00BFFF},
        {"dimgray", 0x696969},
        {"dimgrey", 0x696969},
        {"dodgerblue", 0x1E90FF},
        {"firebrick", 0xB22222},
        {"floralwhite", 0xFFFAF0},
        {"forestgreen", 0x228B22},
        {"fuchsia", 0xFF00FF},
        {"gainsboro", 0xDCDCDC},
        {"ghostwhite", 0xF8F8FF},
        {"gold", 0xFFD700},
        {"goldenrod", 0xDAA520},
        {"gray", 0x808080},
        {"green", 0x008000},
        {"greenyellow", 0xADFF2F},
        {"grey", 0x808080},
        {"honeydew", 0xF0FFF0},
        {"hotpink", 0xFF69B4},
        {"ivory", 0xFFFFF0},
        {"khaki", 0xF0E68C},
        {"lavender", 0xE6E6FA},
        {"lavenderblush", 0xFFF0F5},
        {"lawngreen", 0x7CFC00},
        {"lemonchiffon", 0xFFFACD},
        {"lightblue", 0xADD8E6},
        {"lightcoral", 0xF08080},
        {"lightcyan", 0xE0FFFF},
        {"lightgoldenrodyellow", 0xFAFAD2},
        {"lightgray", 0xD3D3D3},
        {"lightgreen", 0x90EE90},
        {"lightgrey", 0xD3D3D3},
        {"lightpink", 0xFFB6C1},
        {"lightsalmon", 0xFFA07A},
        {"lightseagreen", 0x20B2AA},
        {"lightskyblue", 0x87CEFA},
        {"lightslategray", 0x778899},
        {"lightslategrey", 0x778899},
        {"lightsteelblue", 0xB0C4DE},
        {"lightyellow", 0xFFFFE0},
        {"lime", 0x00FF00},
        {"limegreen", 0x32CD32},
        {"linen", 0xFAF0E6},
        {"magenta", 0xFF00FF},
        {"maroon", 0x800000},
        {"mediumaquamarine", 0x66CDAA},
        {"mediumblue", 0x0000CD},
        {"mediumorchid", 0xBA55D3},
        {"mediumpurple", 0x9370DB},
        {"mediumseagreen", 0x3CB371},
        {"mediumslateblue", 0x7B68EE},
        {"mediumspringgreen", 0x00FA9A},
        {"mediumturquoise", 0x48D1CC},
        {"mediumvioletred", 0xC71585},
        {"midnightblue", 0x191970},
        {"mintcream", 0xF5FFFA},
        {"mistyrose", 0xFFE4E1},
        {"moccasin", 0xFFE4B5},
        {"navajowhite", 0xFFDEAD},
        {"navy", 0x000080},
        {"oldlace", 0xFDF5E6},
        {"olive", 0x808000},
        {"olivedrab", 0x6B8E23},
        {"orange", 0xFFA500},
        {"orangered", 0xFF4500},
        {"orchid", 0xDA70D6},
        {"palegoldenrod", 0xEEE8AA},
        {"palegreen", 0x98FB98},
        {"paleturquoise", 0xAFEEEE},
        {"palevioletred", 0xDB7093},
        {"papayawhip", 0xFFEFD5},
        {"peachpuff", 0xFFDAB9},
        {"peru", 0xCD853F},
        {"pink", 0xFFC0CB},
        {"plum", 0xDDA0DD},
        {"powderblue", 0xB0E0E6},
        {"purple", 0x800080},
        {"rebeccapurple", 0x663399},
        {"red", 0xFF0000},
        {"rosybrown", 0xBC8F8F},
        {"royalblue", 0x4169E1},
        {"saddlebrown", 0x8B4513},
        {"salmon", 0xFA8072},
        {"sandybrown", 0xF4A460},
        {"seagreen", 0x2E8B57},
        {"seashell", 0xFFF5EE},
        {"sienna", 0xA0522D},
        {"silver", 0xC0C0C0},
        {"skyblue", 0x87CEEB},
        {"slateblue", 0x6A5ACD},
        {"slategray", 0x708090},
        {"slategrey", 0x708090},
        {"snow", 0xFFFAFA},
        {"springgreen", 0x00FF7F},
        {"steelblue", 0x4682B4},
        {"tan", 0xD2B48C},
        {"teal", 0x008080},
        {"thistle", 0xD8BFD8},
        {"tomato", 0xFF6347},
        {"turquoise", 0x40E0D0},
        {"violet", 0xEE82EE},
        {"wheat", 0xF5DEB3},
        {"white", 0xFFFFFF},
        {"whitesmoke", 0xF5F5F5},
        {"yellow", 0xFFFF00},
        {"yellowgreen", 0x9ACD32},
    };
    CHECK(colors.size() == 146);
    for (const auto& c: colors) {
      INFO("color " << c.name);
      CHECK(dotColorIs(c.name, (c.rgb >> 16) & 0xFF, (c.rgb >> 8) & 0xFF, c.rgb & 0xFF));
    }
    // names next to the ones of the table
    gum::dot::Rgb rgb{0, 0, 0};
    for (const char* c: {"a", "zzz", "yellowgreen2", "aliceblu", "yellowgree"}) {
      INFO("color " << c);
      CHECK_FALSE(gum::dot::parseColor(c, rgb));
    }
  }

  GUM_TEST(BrightnessAndContrast) {
    CHECK_EQ(gum::dot::brightness({0, 0, 0}), 0.0);
    CHECK_EQ(gum::dot::brightness({255, 255, 255}), 255.0);
    CHECK_LT(std::abs(gum::dot::brightness({255, 0, 0}) - 76.245), 1e-9);
    CHECK_LT(std::abs(gum::dot::brightness({0, 255, 0}) - 149.685), 1e-9);
    CHECK_LT(std::abs(gum::dot::brightness({0, 0, 255}) - 29.07), 1e-9);

    CHECK_EQ(gum::dot::contrastColor(dotColorOf("black")), "white");
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("white")), "black");
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("#404040")),
             "white");   // the default background of the nodes
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("#A08080")),
             "white");   // latent nodes: brightness 137
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("burlywood")), "black");
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("palegreen")), "black");
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("coral")), "black");
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("#808080")), "white");
    // the threshold: 153 is "white", 154 is "black"
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("#999999")), "white");
    CHECK_EQ(gum::dot::contrastColor(dotColorOf("#9A9A9A")), "black");
  }

  GUM_TEST(ResolveFgColor) {
    // a color which is not 'adaptive' is returned as is, even if it is not a color
    CHECK_EQ(gum::dot::resolveFgColor("red", "white", "pink"), "red");
    CHECK_EQ(gum::dot::resolveFgColor("#123456", "#FFFFFF", "pink"), "#123456");
    CHECK_EQ(gum::dot::resolveFgColor("whatever", "transparent", "pink"), "whatever");
    CHECK_EQ(gum::dot::resolveFgColor("", "black", "pink"), "");

    // 'adaptive' follows the background
    CHECK_EQ(gum::dot::resolveFgColor("adaptive", "#404040", "pink"), "white");
    CHECK_EQ(gum::dot::resolveFgColor("adaptive", "burlywood", "pink"), "black");
    CHECK_EQ(gum::dot::resolveFgColor("adaptive", "\"#404040\"", "pink"), "white");
    CHECK_EQ(gum::dot::resolveFgColor("Adaptive", "#404040", "pink"), "white");
    CHECK_EQ(gum::dot::resolveFgColor("ADAPTIVE", "lightyellow", "pink"), "black");
    CHECK_EQ(gum::dot::resolveFgColor(gum::dot::ADAPTIVE_COLOR, "#808080", "pink"), "white");

    // a background which is not interpretable gives the fallback
    CHECK_EQ(gum::dot::resolveFgColor("adaptive", "transparent", "#AAAAAA"), "#AAAAAA");
    CHECK_EQ(gum::dot::resolveFgColor("adaptive", "nocolor", "#4A4A4A"), "#4A4A4A");
    CHECK_EQ(gum::dot::resolveFgColor("adaptive", "", "black"), "black");
  }

}   // namespace gum_tests
