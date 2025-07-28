############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
Color manipulations for pyagrum.lib module
"""

from typing import List, Tuple

import matplotlib as mpl
import matplotlib.colors

import pyagrum as gum
from pyagrum.lib.utils import getBlackInTheme

HTMLCOLORS = {
  "AliceBlue": "F0F8FF",
  "antiquewhite": "FAEBD7",
  "aqua": "00FFFF",
  "aquamarine": "7FFFD4",
  "azure": "F0FFFF",
  "beige": "F5F5DC",
  "bisque": "FFE4C4",
  "black": "000000",
  "blanchedalmond": "FFEBCD",
  "blue": "0000FF",
  "blueviolet": "8A2BE2",
  "brown": "A52A2A",
  "burlywood": "DEB887",
  "cadetblue": "5F9EA0",
  "chartreuse": "7FFF00",
  "chocolate": "D2691E",
  "coral": "FF7F50",
  "cornflowerblue": "6495ED",
  "cornsilk": "FFF8DC",
  "crimson": "DC143C",
  "cyan": "00FFFF",
  "darkblue": "00008B",
  "darkcyan": "008B8B",
  "darkgoldenrod": "B8860B",
  "darkgray": "A9A9A9",
  "darkgrey": "A9A9A9",
  "darkgreen": "006400",
  "darkkhaki": "BDB76B",
  "darkmagenta": "8B008B",
  "darkolivegreen": "556B2F",
  "darkorange": "FF8C00",
  "darkorchid": "9932CC",
  "darkred": "8B0000",
  "darksalmon": "E9967A",
  "darkseagreen": "8FBC8F",
  "darkslateblue": "483D8B",
  "darkslategray": "2F4F4F",
  "darkslategrey": "2F4F4F",
  "darkturquoise": "00CED1",
  "darkviolet": "9400D3",
  "deeppink": "FF1493",
  "deepskyblue": "00BFFF",
  "dimgray": "696969",
  "dimgrey": "696969",
  "dodgerblue": "1E90FF",
  "firebrick": "B22222",
  "floralwhite": "FFFAF0",
  "forestgreen": "228B22",
  "fuchsia": "FF00FF",
  "gainsboro": "DCDCDC",
  "ghostwhite": "F8F8FF",
  "gold": "FFD700",
  "goldenrod": "DAA520",
  "gray": "808080",
  "grey": "808080",
  "green": "008000",
  "greenyellow": "ADFF2F",
  "honeydew": "F0FFF0",
  "hotpink": "FF69B4",
  "indianred ": "CD5C5C",
  "indigo  ": "4B0082",
  "ivory": "FFFFF0",
  "khaki": "F0E68C",
  "lavender": "E6E6FA",
  "lavenderblush": "FFF0F5",
  "lawngreen": "7CFC00",
  "lemonchiffon": "FFFACD",
  "lightblue": "ADD8E6",
  "lightcoral": "F08080",
  "lightcyan": "E0FFFF",
  "lightgoldenrodyellow": "FAFAD2",
  "lightgray": "D3D3D3",
  "lightgrey": "D3D3D3",
  "lightgreen": "90EE90",
  "lightpink": "FFB6C1",
  "lightsalmon": "FFA07A",
  "lightseagreen": "20B2AA",
  "lightskyblue": "87CEFA",
  "lightslategray": "778899",
  "lightslategrey": "778899",
  "lightsteelblue": "B0C4DE",
  "lightyellow": "FFFFE0",
  "lime": "00FF00",
  "limegreen": "32CD32",
  "linen": "FAF0E6",
  "magenta": "FF00FF",
  "maroon": "800000",
  "mediumaquamarine": "66CDAA",
  "mediumblue": "0000CD",
  "mediumorchid": "BA55D3",
  "mediumpurple": "9370DB",
  "mediumseagreen": "3CB371",
  "mediumslateblue": "7B68EE",
  "mediumspringgreen": "00FA9A",
  "mediumturquoise": "48D1CC",
  "mediumvioletred": "C71585",
  "midnightblue": "191970",
  "mintcream": "F5FFFA",
  "mistyrose": "FFE4E1",
  "moccasin": "FFE4B5",
  "navajowhite": "FFDEAD",
  "navy": "000080",
  "oldlace": "FDF5E6",
  "olive": "808000",
  "olivedrab": "6B8E23",
  "orange": "FFA500",
  "orangered": "FF4500",
  "orchid": "DA70D6",
  "palegoldenrod": "EEE8AA",
  "palegreen": "98FB98",
  "paleturquoise": "AFEEEE",
  "palevioletred": "DB7093",
  "papayawhip": "FFEFD5",
  "peachpuff": "FFDAB9",
  "peru": "CD853F",
  "pink": "FFC0CB",
  "plum": "DDA0DD",
  "powderblue": "B0E0E6",
  "purple": "800080",
  "rebeccapurple": "663399",
  "red": "FF0000",
  "rosybrown": "BC8F8F",
  "royalblue": "4169E1",
  "saddlebrown": "8B4513",
  "salmon": "FA8072",
  "sandybrown": "F4A460",
  "seagreen": "2E8B57",
  "seashell": "FFF5EE",
  "sienna": "A0522D",
  "silver": "C0C0C0",
  "skyblue": "87CEEB",
  "slateblue": "6A5ACD",
  "slategray": "708090",
  "slategrey": "708090",
  "snow": "FFFAFA",
  "springgreen": "00FF7F",
  "steelblue": "4682B4",
  "tan": "D2B48C",
  "teal": "008080",
  "thistle": "D8BFD8",
  "tomato": "FF6347",
  "turquoise": "40E0D0",
  "violet": "EE82EE",
  "wheat": "F5DEB3",
  "white": "FFFFFF",
  "whitesmoke": "F5F5F5",
  "yellow": "FFFF00",
  "yellowgreen": "9ACD32",
}


def hex2rgb(vstr: str) -> List[int]:
  """
  from "#FFFFFF" to [255,255,255]
  frorm 'DarkBlue' to [0,0,139]

  Parameters
  ----------
  vstr: str
    the rbg string  or an html color
  Returns
  -------
  List[int]
    the list [r,g,b]
  """
  if vstr.startswith("#"):
    value = vstr.lstrip("#")
  else:
    vl = vstr.lower()
    if vl in HTMLCOLORS:
      value = HTMLCOLORS[vl]
    else:
      raise ValueError(f"Unknown color {vstr}")

  lv = len(value)
  return [int(value[i : i + lv // 3], 16) for i in range(0, lv, lv // 3)]


def hextuple2rgb(vtuple: List[str]) -> List[int]:
  """
  from ("FF","FF","FF") to [255,255,255]

  Parameters
  ----------
  vtuple : Tuple[str,str,str]
    the Tuple of hexa values

  Returns
  -------
  List[int,int,int]
    the list [r,g,b]
  """
  return [int(v, 16) for v in vtuple]


def rgb2brightness(r: int, g: int, b: int) -> str:
  """
  Give the fgcol for a background (r,g,b).

  Parameters
  ----------
  g: int[0,255]
  r: int[0,255]
  b: int[0,255]

  Returns
  -------
  str
    "white" or "black"
  """
  brightness = r * 0.299 + g * 0.587 + b * 0.114
  return "white" if brightness <= 153 else "black"


def proba2hex(p: float, cmap: matplotlib.colors.Colormap, withSpecialColor: bool) -> Tuple[str, str, str]:
  """
  From a proba p and cmap gives the HTML rgb color

  Parameters
  ----------
  p: float
    the proba
  cmap: matplotlib.colors.Colormap
    the cmap
  withSpecialColor: bool
    do we have special colors for p=0 or 1 ?

  Returns
  -------
  Tuple(str,str,str)
    the hex values for r,g,b.
  """
  if withSpecialColor:  # add special color for p=0 or p=1
    if p == 0.0:
      return "FF", "33", "33"
    elif p == 1.0:
      return "AA", "FF", "FF"

  a, b, c, _ = cmap(p)
  return f"{int(a * 256):02x}", f"{int(b * 256):02x}", f"{int(c * 256):02x}"


def proba2color(p: float, cmap: matplotlib.colors.Colormap) -> str:
  """
  From a proba p and cmap gives the HTML rgb color

  Parameters
  ----------
  p: float
    a value in [0,1]
  cmap: matplotlib.colors.Colormap

  Returns
  -------
  str
    the html representation of the color
  """
  r, g, b = proba2hex(p, cmap, withSpecialColor=False)
  return "#" + r + g + b


def proba2bgcolor(p: float, cmap: matplotlib.colors.Colormap) -> str:
  """
  From a proba p and cmap gives the HTML rgb color (with special colors for p=0 and p=1)

  Parameters
  ----------
  p: float
    a value in [0,1]
  cmap: matplotlib.colors.Colormap

  Returns
  -------
  str
    the html representation of the background color
  """
  r, g, b = proba2hex(p, cmap, withSpecialColor=True)
  return "#" + r + g + b


def proba2fgcolor(p: float, cmap: matplotlib.colors.Colormap) -> str:
  """
  From a proba p and cmap, returns the best choice for text color for the bgcolor(p,cmap).

  Parameters
  ----------
  p: float
    a value in [0,1]
  cmap: matplotlib.colors.Colormap

  Returns
  -------
  str
    the html representation of the foreground color
  """
  a, b, c = hextuple2rgb(list(proba2hex(p, cmap, withSpecialColor=True)))
  return rgb2brightness(a, b, c)


def fontFromMatplotlib():
  """
  Find the font name and the font size ysed by matplotlib

  Returns
  -------
    fontname,size : font name and size from matplotlib
  """
  family = mpl.rcParams["font.family"][0]
  if family == "sans-serif":
    family = mpl.rcParams["font.sans-serif"][0]
  return family, mpl.rcParams["font.size"]


def prepareDot(dotgraph, **kwargs):
  if "size" in kwargs and kwargs["size"] is not None:
    dotgraph.set_size(kwargs["size"])

  # workaround for some badly parsed graph (pyparsing>=3.03)
  dotgraph.del_node('"\\n"')
  dotgraph.del_node('"\\n\\n"')

  if dotgraph.get_rankdir() is None:
    dotgraph.set_rankdir(gum.config["notebook", "graph_rankdir"])
  if dotgraph.get_layout() is None:
    dotgraph.set_layout(gum.config["notebook", "graph_layout"])

  dotgraph.set_bgcolor("transparent")
  for e in dotgraph.get_edges():
    if e.get_color() is None:
      e.set_color(getBlackInTheme())
  for n in dotgraph.get_nodes():
    if n.get_color() is None:
      n.set_color(getBlackInTheme())
    if n.get_fontcolor() is None:
      n.set_fontcolor(getBlackInTheme())

  return dotgraph
