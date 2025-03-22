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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
collection of utilities for pyagrum.lib
"""

from typing import Dict, Tuple
import csv
import pydot as dot

import pyagrum as gum


def setDarkTheme():
  """ change the color for arcs and text in graphs to be more visible in dark theme
  """
  gum.config["notebook", "default_arc_color"] = "#AAAAAA"


def setLightTheme():
  """ change the color for arcs and text in graphs to be more visible in light theme
  """
  gum.config["notebook", "default_arc_color"] = "#4A4A4A"


def getBlackInTheme():
  """ return the color used for arc and text in graphs
  """
  return gum.config["notebook", "default_arc_color"]


from collections import namedtuple

DotPoint = namedtuple("DotPoint", ["x", "y"])


def dot_layout(g: dot.Dot) -> Dict[str, DotPoint]:
  """
  extract the layout from a dot graph

  Parameters
  ----------
  g : pydot.Dot
    the graph

  Returns
  -------
  Dict[str,Tuple[float, float]]
     the layout i.e. the position of each node, identified by their name.
  """
  return {l[1]: DotPoint(float(l[2]), float(l[3]))
          for l in csv.reader(g.create(format="plain")
                              .decode("utf8")
                              .split("\n")
                              , delimiter=' '
                              , quotechar='"')
          if len(l) > 3 and l[0] == "node"}


def apply_dot_layout(g: dot.Dot, layout: Dict[str, DotPoint]):
  """
  apply a layout to a dot graph

  Parameters
  ----------
  g : pydot.Dot
    the graph

  layout : Dict[str,Tuple[float, float]]
     the layout i.e. the position of each node, identified by their name.
  """
  g.set_layout("fdp")  # force directed placement
  for n in g.get_nodes():
    name = n.get_name()
    if name in {"edge", "node"}:  # default attributes for all following nodes or edges
      continue
    if name[0] == "\"":
      pos = layout[name[1:-1]]
    else:
      pos = layout[name]
    n.set_pos(f'"{pos.x:.2f},{pos.y:.2f}!"')


async def async_html2image(htmlcontent: str, filename: str):
  """
  convert an html content to an image. The format is determined by the extension of the filename.
  This function is asynchronous and uses playwright. To install playwright, use `pip install playwright` and then `playwright install`.

  Parameters
  ----------
  htmlcontent : str
    the html content to convert
  filename: str
    the filename of the image to create. The format is determined by the extension of the filename (at least pdf and png).
  """
  from playwright.async_api import async_playwright

  playwright = await async_playwright().start()
  browser = await playwright.chromium.launch(headless=True)
  page = await browser.new_page()
  await page.set_content(htmlcontent)
  if filename.endswith(".pdf"):
    await page.pdf(path=filename, scale=1, print_background=True, width="0")
  else:
    await page.screenshot(path=filename, type=filename.split(".")[-1])
  return
