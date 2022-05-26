"""
Color manipulations for pyAgrum.lib module
"""

# (c) Copyright by Pierre-Henri Wuillemin, 2022
# (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
from typing import List, Tuple

import matplotlib.colors
import pyAgrum as gum


def forDarkTheme():
  print(
    "** pyAgrum : forDarkTheme() is obsolete since 0.21.0. Please use setDarkTheme() instead. setDarkTheme() called.")
  setDarkTheme()


def forLightTheme():
  print(
    "** pyAgrum : forLightTheme() is obsolete since 0.21.0. Please use setLightTheme() instead. setLightTheme() called.")
  setLightTheme()


def setDarkTheme():
  """ change the color for arcs and text in graphs to be more visible in dark theme
  """
  gum.config["notebook", "default_arc_color"] = "#AAAAAA"


def setLightTheme():
  """ change the color for arcs and text in graphs to be more visible in light theme
  """
  gum.config["notebook", "default_arc_color"] = "#202020"


def getBlackInTheme():
  """ return the color used for arc and text in graphs
  """
  return gum.config["notebook", "default_arc_color"]


def hex2rgb(vstr):
  """
  from "#FFFFFF" to [255,255,255]

  Parameters
  ----------
  vstr: str
    the rbg string
  Returns
  -------
  List[int]
    the list [r,g,b]
  """
  value = vstr.lstrip('#')
  lv = len(value)
  return [int(value[i:i + lv // 3], 16) for i in range(0, lv, lv // 3)]


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
