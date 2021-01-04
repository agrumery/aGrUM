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
import math
import hashlib

import matplotlib.pyplot as plt
import numpy as np
import pyAgrum as gum

def _stats(p):
  mu = 0.0
  mu2 = 0.0
  v = p.variable(0)
  for i, p in enumerate(p.tolist()):
    x = v.numerical(i)
    mu += p * x
    mu2 += p * x * x
  return (mu, math.sqrt(mu2 - mu * mu))


def _getTitleHisto(p, showMuSigma=True):
  var = p.variable(0)
  if var.varType() == 1 or not (showMuSigma):  # Labelized
    return "{}".format(var.name())

  (mu, std) = _stats(p)
  return "{}\n$\mu={:.2f}$; $\sigma={:.2f}$".format(var.name(), mu, std)


def __limits(p):
  """return vals and labs to show in the histograme

  Parameters
  ----------
    p : gum.Potential
      the marginal to analyze
  """
  var = p.variable(0)
  la = [var.label(int(i)) for i in np.arange(var.domainSize())]
  v = p.tolist()
  nzmin = None
  nzmax = None
  l = len(v) - 1
  for i in range(l + 1):
    if v[i] != 0:
      if nzmin is None:
        if i > 0:
          nzmin = i - 1
        else:
          nzmin = -1
    if v[l - i] != 0:
      if nzmax is None:
        if i > 0:
          nzmax = l - i + 1
        else:
          nzmax = -1

  mi = 0 if nzmin in [-1, None] else nzmin
  ma = l if nzmax in [-1, None] else nzmax

  res = range(mi, ma + 1)
  lres = la[mi:ma + 1]
  if nzmin not in [-1, None]:
    lres[0] = "..."
  if nzmax not in [-1, None]:
    lres[-1] = "..."

  return res, [v[i] for i in res], lres


def _getProbaV(p, scale=1.0, util=None, txtcolor="black"):
  """
  compute the representation of a vertical histogram for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    util : pyAgrum.Potential
      an (optional) secondary Potential (values in labels)
    txtcolor : str
      color for text

  Returns
  -------
  matplotlib.Figure
    a matplotlib histogram for a Potential p.

  """
  if gum.config['notebook', 'histogram_mode'] == "compact":
    ra, v, lv = __limits(p)
  else:
    var = p.variable(0)
    if util is not None:
      lu = util.toarray()
      fmt = gum.config["influenceDiagram", "utility_format_number"]
      lv = ["{} [{:." + fmt + "f}]".format(var.label(int(i)), lu[i])
            for i in np.arange(var.domainSize())]
    else:
      lv = [var.label(int(i)) for i in np.arange(var.domainSize())]
    v = p.tolist()
    ra = range(len(v))

  fig = plt.figure()
  fig.set_figwidth(scale * len(v) / 4.0)
  fig.set_figheight(scale * 2)

  ax = fig.add_subplot(111)

  bars = ax.bar(ra, v,
                align='center',
                color=gum.config['notebook', 'histogram_color'])
  ma = p.max()

  for bar in bars:
    if bar.get_height() != 0:
      # ".2%" for instance
      txt_format = "{:." + \
                   str(
                     int(gum.config['notebook', 'vertical_histogram_visible_digits'])) + "%}"
      txt = txt_format.format(bar.get_height())
      ax.text(bar.get_x(), ma, txt, ha='left', va='top', rotation='vertical')

  ax.set_ylim(bottom=0, top=p.max())
  ax.set_xticks(ra)
  ax.set_xticklabels(lv, rotation='vertical', color=txtcolor)
  # if utility, we do not show the mean/sigma of the proba.
  ax.set_title(_getTitleHisto(p, util is None), color=txtcolor)
  ax.get_yaxis().grid(True)
  ax.margins(0)
  ax.set_facecolor('w')

  return fig


def _getProbaH(p, scale=1.0, util=None, txtcolor="black"):
  """
  compute the representation of an horizontal histogram for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    util : pyAgrum.Potential
      an (optional) secondary Potential (values in labels)
    txtcolor : str
      color for text

  Returns
  -------
  matplotlib.Figure
    a matplotlib histogram for a Potential p.
  """
  var = p.variable(0)
  ra = np.arange(var.domainSize())

  ra_reverse = np.arange(var.domainSize() - 1, -1, -1)  # reverse order

  if util is not None:
    lu = util.toarray()
    vx = ["{} [{:.1f}]".format(var.label(int(i)), lu[i]) for i in ra_reverse]
  else:
    vx = ["{0}".format(var.label(int(i))) for i in ra_reverse]

  fig = plt.figure()
  fig.set_figheight(scale * var.domainSize() / 4.0)
  fig.set_figwidth(scale * 2)

  ax = fig.add_subplot(111)
  ax.set_facecolor('white')

  vals = p.tolist()
  vals.reverse()
  bars = ax.barh(ra, vals,
                 align='center',
                 color=gum.config['notebook', 'histogram_color'])

  for bar in bars:
    if bar.get_width() != 0:
      # ".2%" for instance
      txt_format = "{:." + \
                   str(
                     int(gum.config['notebook', 'horizontal_histogram_visible_digits'])) + "%}"
      txt = txt_format.format(bar.get_width())
      ax.text(1, bar.get_y(), txt, ha='right', va='bottom')

  ax.set_xlim(0, 1)
  ax.set_yticks(np.arange(var.domainSize()))
  ax.set_yticklabels(vx, color=txtcolor)
  ax.set_xticklabels([])
  # ax.set_xlabel('Probability')
  # if utility, we do not show the mean/sigma of the proba.
  ax.set_title(_getTitleHisto(p, util is None), color=txtcolor)
  ax.get_xaxis().grid(True)
  ax.margins(0)

  return fig


def proba2histo(p, scale=1.0, util=None, txtcolor="Black"):
  """
  compute the representation of an histogram for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    util : pyAgrum.Potential
      an (optional) secondary Potential (values in labels)
    txtcolor : str
      color for text

  Returns
  -------
  matplotlib.Figure
    a matplotlib histogram for a Potential p.
  """
  if util is None and p.variable(0).domainSize() > 8:
    return _getProbaV(p, scale, txtcolor=txtcolor)
  else:
    return _getProbaH(p, scale, util=util, txtcolor=txtcolor)


def saveFigProba(p, filename, util=None, bgcol=None, txtcolor="Black"):
  """
  save a figure  which is the representation of an histogram for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    util : pyAgrum.Potential
      an (optional) secondary Potential (values in labels)
    bgcolor: str
      color for background (transparent if None)
    txtcolor : str
      color for text
  """
  fig = proba2histo(p, util=util, txtcolor=txtcolor)

  if bgcol is None:
    fc = gum.config["notebook", "figure_facecolor"]
  else:
    fc = bgcol

  fig.savefig(filename, bbox_inches='tight', transparent=False, facecolor=fc,
              pad_inches=0.05, dpi=fig.dpi, format=gum.config["notebook", "graph_format"])
  plt.close(fig)
