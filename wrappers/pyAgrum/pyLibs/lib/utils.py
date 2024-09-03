# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
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
"""
collection of utilities for pyAgrum.lib
"""

from typing import Dict, Tuple
import csv
import pydot as dot

import pyAgrum as gum


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
    print(filename.split(".")[-1])
    await page.screenshot(path=filename, type=filename.split(".")[-1])
  return
