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
import math
import numpy as np

import matplotlib.pyplot as plt
from matplotlib.ticker import (MultipleLocator, AutoMinorLocator)

import pyAgrum as gum

LEFT_ARROW = u"\u2190"
RIGHT_ARROW = u"\u2192" 

def _stats(pot):
  mu = 0.0
  mu2 = 0.0
  v = pot.variable(0)
  for i, p in enumerate(pot.tolist()):
    x = v.numerical(i)
    mu += p * x
    mu2 += p * x * x
  return mu, math.sqrt(mu2 - mu * mu)


def _getTitleHisto(p, show_mu_sigma=True):
  var = p.variable(0)

  if var.varType() == 1 or not show_mu_sigma:  # type=1 is for gum.LabelizedVariable
    return var.name()

  (mu, std) = _stats(p)
  if std == 0.0:
    return var.name()
  else:
    return f"{var.name()}\n$\\mu={mu:.2f}$; $\\sigma={std:.2f}$"


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
    if v[i] != 0 and nzmin is None:
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
  if mi > 0:
    mi -= 1
  if ma < l:
    ma += 1

  res = range(mi, ma + 1)
  lres = la[mi:ma + 1]
  #if nzmin not in [-1, 0, None]:
  if mi>1:
    lres[0] = LEFT_ARROW + var.label(mi)
  #if nzmax not in [-1, var.domainSize()-1, None]:
  if ma<var.domainSize()-2:
    lres[-1] = var.label(ma+1) + RIGHT_ARROW

  return res, [v[i] for i in res], lres


def _getProbaLine(p, scale=1.0, txtcolor="black"):
  """
  compute the representation of a matplotlib.fill_between for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    scale : float
      the scale
    txtcolor : str
      color for text

  Returns
  -------
  matplotlib.Figure
    a matplotlib figure for a Potential p.
  """

  var = p.variable(0)
  if gum.config['notebook', 'histogram_mode'] == "compact":
    ra, v, lv = __limits(p)
  else:
    lv = [var.label(int(i)) for i in np.arange(var.domainSize())]
    v = p.tolist()
    ra = range(var.domainSize())

  if var.domainSize() > 15:
    nbr_step=15
    max_step=var.domainSize()-1
    while True:
      if nbr_step==2 or max_step%nbr_step==0:
        step = max_step // nbr_step
        break
      nbr_step-=1
  else:
    step = 1

  fig = plt.figure()
  fig.set_figwidth(min(scale * 6, scale * len(v) / 4.0))
  fig.set_figheight(scale * 2)

  ax = fig.add_subplot(111)
  ax.set_xticks(ra[::step])
  ax.set_xticklabels(lv[::step], rotation='vertical')
  ax.fill_between(ra, v, color=gum.config['notebook', 'histogram_color'])

  ax.set_ylim(bottom=0, top=1.05 * p.max())
  ax.set_title(_getTitleHisto(p, True), color=txtcolor)

  ax.get_xaxis().grid(True)
  ax.get_yaxis().grid(True)
  ax.margins(0)

  ax.set_facecolor('w')
  return fig


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
      coef = -1 if gum.config.asBool["influenceDiagram", "utility_show_loss"] else 1
      fmt = f".{gum.config.asInt['influenceDiagram', 'utility_visible_digits']}f"
      lv = [f"{var.label(int(i))} [{coef * lu[i]:{fmt}}]"
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

  if gum.config.asBool['notebook', 'histogram_use_percent']:
    perc = 100
    suffix = "%"
  else:
    perc = 1
    suffix = ""
  for b in bars:
    if b.get_height() != 0:
      txt = f"{b.get_height() * perc:.{gum.config.asInt['notebook', 'histogram_vertical_visible_digits']}f}{suffix}"
      ax.text(b.get_x() + 0.5, ma, txt, ha='center', va='top', rotation='vertical',alpha=0.7)

  ax.set_ylim(bottom=0, top=p.max())
  ax.set_yticklabels([])
  ax.set_xticks(ra)
  ax.set_xticklabels(lv, rotation='vertical', color=txtcolor)
  # even if utility, now we do show the mean/sigma of the distribution.
  ax.set_title(_getTitleHisto(p, True), color=txtcolor)
  ax.get_yaxis().grid(True)
  ax.margins(0)
  ax.set_facecolor('w')

  return fig


def _getProbaH(p, scale=1.0, util=None, txtcolor="black"):
  """
  compute the representation of a horizontal histogram for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    scale : scale for the size of the graph
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
    fmt = f'.{gum.config.asInt["influenceDiagram", "utility_visible_digits"]}f'

    if gum.config.asBool["influenceDiagram", "utility_show_loss"]:
      vx = [f"{var.label(int(i))} [{-lu[i] if lu[i] != 0 else 0:{fmt}}]" for i in ra_reverse]
    else:
      vx = [f"{var.label(int(i))} [{lu[i]:{fmt}}]" for i in ra_reverse]
  else:
    vx = [var.label(int(i)) for i in ra_reverse]

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

  if gum.config.asBool['notebook', 'histogram_use_percent']:
    perc = 100
    suffix = "%"
  else:
    perc = 1
    suffix = ""
  for b in bars:
    if b.get_width() != 0:
      txt = f"{b.get_width() * perc:.{gum.config.asInt['notebook', 'histogram_horizontal_visible_digits']}f}{suffix}"
      ax.text(1, b.get_y(), txt, ha='right', va='bottom',alpha=0.7)

  ax.set_xlim(0, 1)
  ax.set_yticks(np.arange(var.domainSize()))
  ax.set_yticklabels(vx, color=txtcolor)
  ax.set_xticklabels([])
  # ax.set_xlabel('Probability')
  # Even if utility, now we do show the mean/sigma of the distribution.
  ax.set_title(_getTitleHisto(p, True), color=txtcolor)
  ax.get_xaxis().grid(True)
  ax.margins(0)

  return fig


def _getHistoForDiscretized(p, scale=1, txtcolor="Black"):
  var = p.variable(0)
  vx = var.ticks()
  widths = [vx[i + 1] - vx[i] for i in range(len(vx) - 1)]
  vals = [v / w for v, w in zip(p.tolist(), widths)]

  lim1 = 0
  lim2 = len(vals) - 1
  if gum.config['notebook', 'histogram_mode'] == "compact":
    while vals[lim1] <= gum.config.asFloat['notebook', 'histogram_epsilon']:
      lim1 += 1
    if lim1 > 0:
      lim1 -= 1

    while vals[lim2] <= gum.config.asFloat['notebook', 'histogram_epsilon']:
      lim2 -= 1
    if lim2 < len(vals) - 1:
      lim2 += 1
    if lim2 >= len(vx):
      lim2 -= 1

  fig = plt.figure()
  fw=scale * max(10, (lim2 - lim1)) / 8
  fig.set_figwidth(fw)
  fig.set_figheight(scale)

  ax = fig.add_subplot(111)
  ax.set_facecolor('white')
  delta = 0.025 * (vx[lim2 + 1] - vx[lim1])
  ax.set_xlim(vx[lim1] - delta, vx[lim2 + 1] + delta)
  ax.set_xticks([vx[lim1], (vx[lim2 + 1] + vx[lim1]) / 2, vx[lim2 + 1]])
  ax.set_xticklabels([f"{vx[lim1]:.2f}", f"{(vx[lim2 + 1] + vx[lim1]) / 2:.2f}", f"{vx[lim2 + 1]:.2f}"],
                     color=txtcolor, rotation='vertical' if fw<1.3 else 'horizontal')
  ax.xaxis.set_minor_locator(AutoMinorLocator())
  ax.tick_params(which="minor", length=4)
  ax.set_ylim(bottom=0, top=1.05 * max(vals))
  ax.get_xaxis().grid(True, which="both", zorder=0)

  bars = ax.bar(vx[lim1:1 + lim2], height=vals[lim1:1 + lim2], width=widths[lim1:1 + lim2],
                align='edge',
                color=gum.config['notebook', 'histogram_color'],
                edgecolor=gum.config['notebook', 'histogram_edge_color'], zorder=33)

  # Even if utility, now we do show the mean/sigma of the distribution.
  ax.set_title(_getTitleHisto(p, True), color=txtcolor)
  ax.margins(0)

  return fig


def proba2histo(p, scale=None, util=None, txtcolor="Black"):
  """
  compute the representation of a histogram for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    scale : float
      scale for the size of the graph
    util : pyAgrum.Potential
      an (optional) secondary Potential (values in labels)
    txtcolor : str
      color for text

  Returns
  -------
  matplotlib.Figure
    a matplotlib histogram for a Potential p.
  """
  #if util is not None:
  #  if scale is None:
  #    scale = 1.0
  #  return _getProbaH(p, scale, util=util, txtcolor=txtcolor)

  isev = p.min()==0.0 and p.max()==1.0 and p.sum()==1.0

  if p.variable(0).varType() == gum.VarType_Discretized:
    if gum.config['notebook', 'histogram_discretized_visualisation'] == "histogram":
      if scale is None:
        scale=gum.config.asFloat['notebook', 'histogram_discretized_scale']
      return _getHistoForDiscretized(p, scale, txtcolor)

  if scale is None:
    scale = 1.0

  if p.variable(0).domainSize() > int(gum.config['notebook', 'histogram_line_threshold']) and not(isev):
    return _getProbaLine(p, scale, txtcolor=txtcolor)

  if p.variable(0).domainSize() > int(gum.config['notebook', 'histogram_horizontal_threshold']):
    return _getProbaV(p, scale, txtcolor=txtcolor)

  return _getProbaH(p, scale, util=util, txtcolor=txtcolor)


def saveFigProba(p, filename, util=None, bgcolor=None, txtcolor="Black"):
  """
  save a figure  which is the representation of a histogram for a mono-dim Potential

  Parameters
  ----------
    p : pyAgrum.Potential
      the mono-dimensional Potential
    filename: str
      the name of the saved file
    util : pyAgrum.Potential
      an (optional) secondary Potential (values in labels)
    bgcolor: str
      color for background (transparent if None)
    txtcolor : str
      color for text
  """
  fig = proba2histo(p, util=util, txtcolor=txtcolor)

  if bgcolor is None:
    fc = gum.config["notebook", "figure_facecolor"]
  else:
    fc = bgcolor

  fig.savefig(filename, bbox_inches='tight', transparent=False, facecolor=fc,
              pad_inches=0.05, dpi=fig.dpi, format=gum.config["notebook", "graph_format"])
  plt.close(fig)


def probaMinMaxH(pmin, pmax, scale=1.0, txtcolor="black"):
  """
  compute the representation of a horizontal histogram for a mono-dim Potential

  Parameters
  ----------
    pmin,pmax : pyAgrum.Potential
      two mono-dimensional Potential
    scale : float
      scale for the size of the graph
    txtcolor : str
      color for text

  Returns
  -------
  matplotlib.Figure
    a matplotlib histogram for a bi-Potential pmin,pmax.
  """
  var = pmin.variable(0)
  ra = np.arange(var.domainSize())

  ra_reverse = np.arange(var.domainSize() - 1, -1, -1)  # reverse order
  vx = [var.label(int(i)) for i in ra_reverse]

  fig = plt.figure()
  fig.set_figheight(scale * var.domainSize() / 4.0)
  fig.set_figwidth(scale * 2)

  ax = fig.add_subplot(111)
  ax.set_facecolor('white')

  vmin = pmin.tolist()
  vmin.reverse()
  vmax = pmax.tolist()
  vmax.reverse()
  barsmax = ax.barh(ra, vmax,
                    align='center',
                    color="#BBFFAA")
  barsmin = ax.barh(ra, vmin,
                    align='center',
                    color=gum.config['notebook', 'histogram_color'])

  if gum.config.asBool['notebook', 'histogram_use_percent']:
    perc = 100
    suffix = "%"
  else:
    perc = 1
    suffix = ""

  for b in barsmax:
    txt = f"{b.get_width() * perc:.{gum.config.asInt['notebook', 'histogram_horizontal_visible_digits']}f}{suffix}"
    ax.text(1, b.get_y(), txt, ha='right', va='bottom')
  for b in barsmin:
    txt = f"{b.get_width() * perc:.{gum.config.asInt['notebook', 'histogram_horizontal_visible_digits']}f}{suffix}"
    ax.text(0, b.get_y(), txt, ha='left', va='bottom')

  ax.set_xlim(0, 1)
  ax.set_yticks(np.arange(var.domainSize()))
  ax.set_yticklabels(vx, color=txtcolor)
  ax.set_xticklabels([])
  ax.set_title(pmin.variable(0).name(), color=txtcolor)
  ax.get_xaxis().grid(True)
  ax.margins(0)

  return fig


def saveFigProbaMinMax(pmin, pmax, filename, bgcolor=None, txtcolor="Black"):
  """
  save a figure  which is the representation of a histogram for a bi-Potential (min,max)

  Parameters
  ----------
    pmin : pyAgrum.Potential
      the mono-dimensional Potential for min values
    pmax : pyAgrum.Potential
      the mono-dimensional Potential for max value
    filename : str
      the name of the saved file
    bgcolor: str
      color for background (transparent if None)
    txtcolor : str
      color for text
  """
  fig = probaMinMaxH(pmin, pmax, txtcolor=txtcolor)

  if bgcolor is None:
    fc = gum.config["notebook", "figure_facecolor"]
  else:
    fc = bgcolor

  fig.savefig(filename, bbox_inches='tight', transparent=False, facecolor=fc,
              pad_inches=0.05, dpi=fig.dpi, format=gum.config["notebook", "graph_format"])
  plt.close(fig)
