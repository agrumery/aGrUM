#!/usr/bin/python
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

from __future__ import print_function

import time

import matplotlib.pyplot as plt
import numpy as np
import pyAgrum as gum
import pydotplus as dot
import shutil


def _proba2rgb(p=0.99, cmap=None):
  if cmap is None:
    cmap=plt.get_cmap('summer')

  if p == 0.0:
    r, g, b = "FF", "33", "33"
  elif p == 1.0:
    r, g, b = "AA", "FF", "FF"
  else:
    (r, g, b, _) = cmap(p)
    r = "%02x" % int(r * 256)
    g = "%02x" % int(g * 256)
    b = "%02x" % int(b * 256)

  return r, g, b


def _proba2bgcolor(p, cmap=None):
  if cmap is None:
    cmap=plt.get_cmap('summer')

  r, g, b = _proba2rgb(p, cmap)
  return "#" + r + g + b


def _proba2fgcolor(p, cmap=None):
  if cmap is None:
    cmap=plt.get_cmap('summer')

  if max([eval("0x" + s[0]) for s in _proba2rgb(p, cmap)]) <= 12:
    return "#FFFFFF"
  else:
    return "#000000"


def BN2dot(bn, size="4", arcvals=None, vals=None, cmap=None, showValues=None):
  """
  create a pydotplus representation of the BN

  :param pyAgrum.BayesNet bn:
  :param string size: size of the rendered graph
  :param dictionnary evs: map of evidence
  :param set targets: set of targets. If targets={} then each node is a target
  :param vals: a nodeMap of values to be shown as color nodes (with special colors for 0 and 1)
  :param arcvals: a arcMap of values to be shown as bold arcs
  :param cmap: color map to show the vals
  :param showValues: a nodeMap of values to be shown as tooltip
  :return: the desired representation of the inference
  """
  if cmap is None:
    cmap=plt.get_cmap('summer')

  if arcvals is not None:
    minarcs = min(arcvals.values())
    maxarcs = max(arcvals.values())

  graph = dot.Dot(graph_type='digraph',bgcolor="transparent")

  for n in bn.names():
    if vals is None or n not in vals:
      bgcol = "#444444"
      fgcol = "#FFFFFF"
      res = ""
    else:
      bgcol = _proba2bgcolor(vals[n], cmap)
      fgcol = _proba2fgcolor(vals[n], cmap)
      res = " : {0:2.5f}".format(vals[n] if showValues is None else showValues[n])

    node = dot.Node('"' + n + '"', style="filled",
                    fillcolor=bgcol,
                    fontcolor=fgcol,
                    tooltip='"({0}) {1}{2}"'.format(bn.idFromName(n), n, res))
    graph.add_node(node)

  for a in bn.arcs():
    if arcvals is None:
      pw = 1
      av = ""
    else:
      if a in arcvals:
        pw = 0.1 + 5 * (arcvals[a] - minarcs) / (maxarcs - minarcs)
        av = arcvals[a]
      else:
        pw = 1

    edge = dot.Edge('"' + bn.variable(a[0]).name() + '"', '"' + bn.variable(a[1]).name() + '"',
                    penwidth=pw,
                    tooltip="{} : {}".format(a, av))
    graph.add_edge(edge)

  graph.set_size(size)
  return graph


def _getProbaV(p):
  """
  compute the representation of an histogram for a mono-dim Potential

  :param p: the mono-dim Potential
  :return: a matplotlib bar (vertical histogram) for a Potential p.

  """
  var = p.variable(0)
  ra = np.arange(var.domainSize())

  fig = plt.figure()
  fig.set_figwidth(var.domainSize() / 4.0)
  fig.set_figheight(2)

  ax = fig.add_subplot(111)

  ax.bar(ra, p.tolist(), align='center')
  # ax.set_ylim(0,1)
  ax.set_xticks(ra)
  ax.set_xticklabels([])
  # ax.set_xlabel('Probability')
  ax.set_title(var.name())
  ax.get_yaxis().grid(True)
  return fig


def _getProbaH(p):
  """
  compute the representation of an histogram for a mono-dim Potential

  :param p: the mono-dim Potential
  :return: a matplotlib barh (horizontal histogram) for a Potential p.
  """
  var = p.variable(0)
  ra = np.arange(var.domainSize())
  ra_reverse = np.arange(var.domainSize() - 1, -1, -1)  # reverse order
  vx = ["{0}:{1:1.4f}".format(var.label(int(i)), p[i]) for i in ra_reverse]

  fig = plt.figure()
  fig.set_figheight(var.domainSize() / 4.0)
  fig.set_figwidth(2)

  ax = fig.add_subplot(111)

  vals = p.tolist()
  vals.reverse()
  ax.barh(ra, vals, align='center')
  ax.set_xlim(0, 1)
  ax.set_yticks(np.arange(var.domainSize()))
  ax.set_yticklabels(vx)
  ax.set_xticklabels([])
  # ax.set_xlabel('Probability')
  ax.set_title(var.name())
  ax.get_xaxis().grid(True)
  return fig


def proba2histo(p):
  """
  compute the representation of an histogram for a mono-dim Potential

  :param pyAgrum.Potential p: the mono-dim Potential
  :return: a matplotlib histogram for a Potential p.
  """
  if p.variable(0).domainSize() > 8:
    return _getProbaV(p)
  else:
    return _getProbaH(p)


def _saveFigProba(p, filename, format="svg"):
  fig = proba2histo(p)
  fig.savefig(filename, bbox_inches='tight', transparent=True, pad_inches=0, dpi=fig.dpi, format=format)
  plt.close(fig)


def BNinference2dot(bn, size="4",engine=None, evs={}, targets={}, format='png', vals=None, arcvals=None,cmap=None):
  """
  create a pydotplus representation of an inference in a BN

  :param pyAgrum.BayesNet bn:
  :param string size: size of the rendered graph
  :param pyAgrum Inference engine: inference algorithm used. If None, LazyPropagation will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets. If targets={} then each node is a target
  :param string format: render as "png" or "svg"
  :param vals: a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  :param arcvals: a arcMap of values to be shown as bold arcs
  :param cmap: color map to show the vals
  :return: the desired representation of the inference
  """
  if cmap is None:
    cmap=plt.get_cmap('summer')

  startTime = time.time()
  if engine is None:
    ie = gum.LazyPropagation(bn)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  from tempfile import mkdtemp
  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "digraph structs {\n  bgcolor=\"transparent\";"
  dotstr += "  label=\"Inference in {:6.2f}ms\";\n".format(1000 * (stopTime - startTime))
  dotstr += "  node [fillcolor=floralwhite, style=filled,color=grey];\n"

  for nid in bn.nodes():
    name = bn.variable(nid).name()

    if vals is None or name not in vals or nid not in vals:
      bgcol = "sandybrown" if name in evs or nid in evs else "#FFFFFF"
      fgcol = "#000000"
    else:
      bgcol = _proba2bgcolor(vals[name], cmap)
      fgcol = _proba2fgcolor(vals[name], cmap)
    colorattribute = 'fillcolor="{}", fontcolor="{}", color="#000000"'.format(bgcol, fgcol)

    if len(targets) == 0 or name in targets or nid in targets:
      filename = temp_dir + name + "." + format
      _saveFigProba(ie.posterior(name), filename, format=format)
      fill = ", " + colorattribute
      dotstr += ' "{0}" [shape=rectangle,image="{1}",label="" {2}];\n'.format(name, filename, fill)
    else:
      dotstr += ' "{0}" [{1}]'.format(name, colorattribute)

  if arcvals is not None:
    minarcs = min(arcvals.values())
    maxarcs = max(arcvals.values())

  for a in bn.arcs():
    (n, j) = a
    if arcvals is None:
      pw = 1
      av = ""
    else:
      if (n, j) in arcvals:
        pw = 0.1 + 5 * (arcvals[a] - minarcs) / (maxarcs - minarcs)
        av = arcvals[a]
      else:
        pw = 1

    dotstr += ' "{0}"->"{1}" [penwidth={2},tooltip="{3}:{4}"];'.format(bn.variable(n).name(), bn.variable(j).name(),
                                                                       pw, a, av)
  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)

  shutil.rmtree(temp_dir)

  g.set_size(size)
  return g


def dotize(aBN, name, format='pdf'):
  """
  From a bn, creates an image of the BN

  :param pyAgrum.BayesNet bn: the bayes net to show
  :param string name: the filename (without extension) for the image
  :param string format: format in ['pdf','png','fig','jpg','svg']
  """
  if format not in ['pdf', 'png', 'fig', 'jpg', 'svg']:
    raise Exception("<%s> in not a correct style ([pdf,png,fig,jpg,svg])" % style)

  if isinstance(aBN, str):
    bn = gum.loadBN(aBN)
  else:
    bn = aBN

  imgfile = name + '.' + format
  BN2dot(bn).write(imgfile, format=format)


def pngize(aBN, name):
  """
  From a bn, creates a png of the BN

  :param pyAgrum.BayesNet bn: the bayes net to show
  :param string name: the filename (without extension) for the image
  """
  dotize(aBN, name, 'png')


def pdfize(aBN, name):
  """
  From a bn, creates a pdf of the BN

  :param pyAgrum.BayesNet bn: the bayes net to show
  :param string name: the filename (without extension) for the image
  """
  dotize(aBN, name, 'pdf')


if __name__ == "__main__":
  #  pyAgrum_header("2011-17")##
  #
  #  if len(sys.argv) < 2:
  #    print(os.path.basename(sys.argv[0]), "file.{" + gum.availableBNExts() + "}")
  #  else:
  #    base, ext = os.path.splitext(sys.argv[1])
  #    pngize(sys.argv[1], base)
  bn = gum.fastBN("a->b->d;a->c->d->e;f->b")
  g = BNinference2dot(bn,
                      targets=['f', 'd'],
                      vals={'a': 1, 'b': 0.3, 'c': 0.3, 'd': 0.1, 'e': 0.1, 'f': 0.3},
                      arcvals={(0, 1): 2, (0, 2): 0.5})
  g.write("test.pdf", format='pdf')
  # pdfize(bn, "test")
