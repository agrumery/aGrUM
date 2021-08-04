"""
Color manipulations for pyAgrum.lib module
"""

# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
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
import pyAgrum as gum

def forDarkTheme():
  print("** pyAgrum : forDarkTheme() is obsolete since 0.21.0. Please use setDarkTheme() instead. setDarkTheme() called.")
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
  :param vstr: the rbg string
  :return: the list
  """
  value = vstr.lstrip('#')
  lv = len(value)
  return [int(value[i:i + lv // 3], 16) for i in range(0, lv, lv // 3)]


def hextuple2rgb(vtuple):
  """
  from ("FF","FF","FF") to [255,255,255]
  """
  return [int(v, 16) for v in vtuple]


def rgb2brightness(r, g, b):
  """
  Give the fgcol for a background (r,g,b).

  :param r: int[0,255]
  :param g: int[0,255]
  :param b: int[0,255]
  :return: "white" or "black"
  """
  brightness = r * 0.299 + g * 0.587 + b * 0.114
  return "white" if brightness <= 153 else "black"


def proba2hex(p, cmap, withSpecialColor):
  """
  From a proba p and cmap gives the HTML rgb color

  :param p: the proba
  :param cmap: the cmap
  :param withSpecialColor: do we have a special color when p=0 or 1 ?
  :return: a string
  """
  (r, g, b, _) = cmap(p)
  r = "%02x" % int(r * 256)
  g = "%02x" % int(g * 256)
  b = "%02x" % int(b * 256)

  if withSpecialColor:  # add special color for p=0 or p=1
    if p == 0.0:
      r, g, b = "FF", "33", "33"
    elif p == 1.0:
      r, g, b = "AA", "FF", "FF"

  return r, g, b


def proba2color(p, cmap):
  """
  From a proba p and cmap gives the HTML rgb color
  :param p: the proba
  :param cmap: the cmap
  :return: a string
  """
  r, g, b = proba2hex(p, cmap, withSpecialColor=False)
  return "#" + r + g + b


def proba2bgcolor(p, cmap):
  """
  From a proba p and cmap gives the HTML rgb color (with special colors for p=0 and p=1)
  :param p: the proba
  :param cmap: the cmap
  :return: a string
  """
  r, g, b = proba2hex(p, cmap, withSpecialColor=True)
  return "#" + r + g + b


def proba2fgcolor(p, cmap):
  """
  From a proba p and cmap, returns the best choice for text color for the bgcolor(p,cmap).
  :param p: the proba
  :param cmap: the cmap
  :return: a string
  """
  return rgb2brightness(*hextuple2rgb(proba2hex(p, cmap, withSpecialColor=True)))
