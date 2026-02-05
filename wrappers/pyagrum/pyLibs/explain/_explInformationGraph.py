############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import pyagrum as gum
import pyagrum.lib._colors as gumcols
from pyagrum.lib.bn2graph import BN2dot

# Matplotlib
import matplotlib as mpl

# GL
import warnings

_cdict = {
  "red": ((0.0, 0.1, 0.3), (1.0, 0.6, 1.0)),
  "green": ((0.0, 0.0, 0.0), (1.0, 0.6, 0.8)),
  "blue": ((0.0, 0.0, 0.0), (1.0, 1, 0.8)),
}
_INFOcmap = mpl.colors.LinearSegmentedColormap("my_colormap", _cdict, 256)


def _normalizeVals(vals, hilightExtrema=False):
  """
  normalisation if vals is not a proba (max>1)
  """
  ma = float(max(vals.values()))
  mi = float(min(vals.values()))
  if ma == mi:
    return None

  if not hilightExtrema:
    vmi = 0.01
    vma = 0.99
  else:
    vmi = 0
    vma = 1

  res = {name: vmi + (val - mi) * (vma - vmi) / (ma - mi) for name, val in vals.items()}
  return res


def getInformationGraph(bn, evs=None, size=None, cmap=_INFOcmap, withMinMax=False):
  """
  Create a dot representation of the information graph for this BN

  Parameters
  ----------
  bn: gum.BayesNet
    the BN
  evs : Dict[str,str|int|List[float]]
    map of evidence
  size: str|int
    size of the graph
  cmap: matplotlib.colors.Colormap
    color map
  withMinMax: bool
    min and max in the return values ?

  Returns
  -------
  dot.Dot | Tuple[dot.Dot,float,float,float,float]
    graph as a dot representation and if asked, min_information_value, max_information_value, min_mutual_information_value, max_mutual_information_value
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  ie = gum.LazyPropagation(bn)
  ie.setEvidence(evs)
  ie.makeInference()

  idEvs = ie.hardEvidenceNodes() | ie.softEvidenceNodes()

  nodevals = dict()
  for n in bn.nodes():
    if n not in idEvs:
      v = ie.H(n)
      if v != v:  # is NaN
        warnings.warn(f"For {bn.variable(n).name()}, entropy is NaN.")
        v = 0
      nodevals[bn.variable(n).name()] = v

  arcvals = dict()
  for x, y in bn.arcs():
    v = ie.jointMutualInformation({x, y})
    if v != v:  # is NaN
      warnings.warn(f"For {bn.variable(x).name()}->{bn.variable(y).name()}, mutual information is Nan.")
      v = 0
    arcvals[(x, y)] = v

  gr = BN2dot(
    bn,
    size,
    nodeColor=_normalizeVals(nodevals, hilightExtrema=False),
    arcWidth=arcvals,
    cmapNode=cmap,
    cmapArc=cmap,
    showMsg=nodevals,
  )

  if withMinMax:
    mi_node = min(nodevals.values())
    ma_node = max(nodevals.values())
    mi_arc = min(arcvals.values())
    ma_arc = max(arcvals.values())
    return gr, mi_node, ma_node, mi_arc, ma_arc
  else:
    return gr


def _reprInformation(bn, evs=None, size=None, cmap=_INFOcmap, asString=False):
  """
  repr a bn annotated with results from inference : Information and mutual information

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  evs: Dict[str|int,str|int|List[float]]
    the observations
  size: int|str
    size of the rendered graph
  cmap: matplotlib.colours.Colormap
    the cmap
  asString: bool
    returns the string or display the HTML

  Returns
  -------
  str|None
    return the HTML string or directly display it.
  """
  import IPython.display
  import IPython.core.pylabtools
  from base64 import encodebytes
  from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  gr, mi, ma, _, _ = getInformationGraph(bn, evs, size, cmap, withMinMax=True)
  gumcols.prepareDot(gr, size=size)

  # dynamic member makes pylink unhappy
  # pylint: disable=no-member
  gsvg = IPython.display.SVG(gr.create_svg(encoding="utf-8"))
  width = (
    int(gsvg.data.split("width=")[1].split('"')[1].split("pt")[0]) / mpl.pyplot.rcParams["figure.dpi"]
  )  # pixel in inches
  if width < 5:
    width = 5

  fig = mpl.figure.Figure(figsize=(width, 1))
  fig.patch.set_alpha(0)
  canvas = fc(fig)
  ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
  norm = mpl.colors.Normalize(vmin=mi, vmax=ma)
  cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=cmap, norm=norm, orientation="horizontal")
  cb1.set_label("Entropy")
  cb1.ax.text(mi, -2, f"{mi:.4f}", ha="left", va="top", color=gumcols.proba2bgcolor(0.01, cmap))
  cb1.ax.text(ma, -2, f"{ma:.4f}", ha="right", va="top", color=gumcols.proba2bgcolor(0.99, cmap))
  png = IPython.core.pylabtools.print_figure(canvas.figure, "png")  # from IPython.core.pylabtools
  png_legend = f"<img style='vertical-align:middle' src='data:image/png;base64,{encodebytes(png).decode('ascii')}'>"

  sss = f"<div align='center'>{gsvg.data}<br/>{png_legend}</div>"

  if asString:
    return sss

  return IPython.display.display(IPython.display.HTML(sss))


def getInformation(bn, evs=None, size=None, cmap=_INFOcmap) -> str:
  """
  get a HTML string for a bn annotated with results from inference : entropy and mutual information

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  evs: Dict[str|int,str|int|List[float]]
    the observations
  size: int|str
    size of the rendered graph
  cmap: matplotlib.colours.Colormap
    the cmap

  Returns
  -------
  str
    return the HTML string
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  return _reprInformation(bn, evs, size, cmap, asString=True)


def showInformation(bn, evs=None, size=None, cmap=_INFOcmap):
  """
  diplay a bn annotated with results from inference : entropy and mutual information

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  evs: Dict[str|int,str|int|List[float]]
    the observations
  size: int|str
    size of the rendered graph
  cmap: matplotlib.colours.Colormap
    the cmap
  """
  if evs is None:
    evs = {}

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  return _reprInformation(bn, evs, size, cmap, asString=False)
