# -*- coding: utf-8 -*-
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

"""
tools for BN qualitative analysis
"""
import matplotlib as mpl

import pyAgrum as gum
from pyAgrum.lib.bn2graph import BN2dot

_cdict = {
  'red': ((0.0, 0.1, 0.3),
          (1.0, 0.6, 1.0)),
  'green': ((0.0, 0.0, 0.0),
            (1.0, 0.6, 0.8)),
  'blue': ((0.0, 0.0, 0.0),
           (1.0, 1, 0.8))
}
_INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap', _cdict, 256)

def independenceListForPairs(bn):
  """
  returns a list of triples `(i,j,k)` for each non arc `(i,j)` such that `i` is independent of `j` given `k`.

  Parameters
  ----------
  bn: gum.BayesNet
    the Bayesian Network

  Returns
  -------
  List[(str,str,List[str])]
    the list of independence found in the structure of BN.
  """
  import itertools

  def powerset(iterable):
    xs = list(iterable)
    # note we return an iterator rather than a list
    return itertools.chain.from_iterable(itertools.combinations(xs, n) for n in range(len(xs) + 1))

  # testing every d-separation
  l = []
  nams = sorted(bn.names())
  for i in sorted(bn.names()):
    nams.remove(i)
    for j in nams:
      if not (bn.existsArc(i, j) or bn.existsArc(j, i)):
        for k in powerset(sorted(bn.names() - {i, j})):
          if bn.isIndependent(i, j, k):
            l.append((i, j, k))
            break
  return l


def plotIndependenceListForPairs(bn, filename, alphabetic=False):
  """
  plot the p-value of the chi2 test of a (as simple as possible) independence proposition for every non arc.

  Parameters
  ----------
  bn : gum.BayesNet
    the Bayesian network

  filename : str
    the name of the csv database

  alphabetic : bool
    if True, the list is alphabetically sorted else it is sorted by the p-value

  Returns
  -------
    matplotlib.Figure
  """
  import pylab

  learner = gum.BNLearner(filename, bn)
  vals = {}
  for indep in independenceListForPairs(bn):
    key = "$" + indep[0] + " \\perp " + indep[1]
    if len(indep[2]) > 0:
      key += " \\mid " + ",".join(indep[2])
    key += "$"
    vals[key] = learner.chi2(*indep)[1]

  if not alphabetic:
    sortedkeys = sorted(vals, key=vals.__getitem__, reverse=False)
  else:
    sortedkeys = list(vals.keys())

  fig = pylab.figure(figsize=(10, 0.25 * len(vals)))
  ax = fig.add_subplot(1, 1, 1)
  ax.plot([vals[k] for k in sortedkeys], sortedkeys, "o")
  ax.grid(True)
  ax.vlines(x=0.05, ymin=0, ymax=len(vals) - 1, colors='purple')
  ax.add_patch(mpl.patches.Rectangle((0, 0), 0.05, len(vals) - 1, color="yellow"))
  return fig


def _normalizeVals(vals, hilightExtrema=False):
  """
  normalisation if vals is not a proba (max>1)
  """
  ma = float(max(vals.values()))
  mi = float(min(vals.values()))
  if ma == mi:
    return None
  else:
    if not hilightExtrema:
      vmi = 0.01
      vma = 0.99
    else:
      vmi = 0
      vma = 1

    try:
      items = vals.items()
    except AttributeError:
      items = vals.iteritems()

    return {name: vmi + (val - mi) * (vma - vmi) / (ma - mi) for name, val in items}


def _reprInformation(bn, evs, size, cmap, asString):
  """
  repr a bn annoted with results from inference : Information and mutual informations

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the HTML string
  """
  # fix DeprecationWarning of base64.encodestring()
  try:
    from base64 import encodebytes
  except ImportError:  # 3+
    from base64 import encodestring as encodebytes

  import IPython.display
  import IPython.core.pylabtools
  from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

  ie = gum.LazyPropagation(bn)
  ie.setEvidence(evs)
  ie.makeInference()

  idEvs = {bn.idFromName(name) for name in evs}
  nodevals = {bn.variable(n).name(): ie.H(n)
              for n in bn.nodes() if not n in idEvs}
  arcvals = {(x, y): ie.I(x, y) for x, y in bn.arcs()}
  gr = BN2dot(bn, size, nodeColor=_normalizeVals(nodevals, hilightExtrema=False), arcWidth=arcvals, cmapNode=cmap,
              cmapArc=cmap,
              showMsg=nodevals)

  mi = min(nodevals.values())
  ma = max(nodevals.values())

  fig = mpl.figure.Figure(figsize=(8, 3))
  canvas = fc(fig)
  ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
  norm = mpl.colors.Normalize(vmin=mi, vmax=ma)
  cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=cmap,
                                  norm=norm,
                                  orientation='horizontal')
  cb1.set_label('Entropy')
  png = IPython.core.pylabtools.print_figure(canvas.figure, "png")  # from IPython.core.pylabtools
  png_legend = "<img style='vertical-align:middle' src='data:image/png;base64,%s'>" % encodebytes(png).decode(
    'ascii')

  gsvg = IPython.display.SVG(gr.create_svg())

  sss = "<div align='center'>" + gsvg.data + "</div>"
  sss += "<div align='center'>"
  sss += "<font color='" + \
         gum._proba2bgcolor(0.01, cmap) + "'>" + str(mi) + "</font>"
  sss += png_legend
  sss += "<font color='" + \
         gum._proba2bgcolor(0.99, cmap) + "'>" + str(ma) + "</font>"
  sss += "</div>"

  if asString:
    return sss
  else:
    return IPython.display.display(IPython.display.HTML(sss))


def getInformation(bn, evs=None, size=None, cmap=_INFOcmap):
  """
  get a HTML string for a bn annotated with results from inference : entropy and mutual information

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the HTML string
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  return _reprInformation(bn, evs, size, cmap, asString=True)


def showInformation(bn, evs=None, size=None, cmap=_INFOcmap):
  """
  show a bn annotated with results from inference : entropy and mutual information

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the graph
  """
  if evs is None:
    evs = {}

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  return _reprInformation(bn, evs, size, cmap, asString=False)
