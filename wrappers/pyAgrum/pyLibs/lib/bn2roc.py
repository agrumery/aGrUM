"""
The purpose of this module is to provide tools for building ROC and PR from Bayesian Network.
"""

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


import os
from typing import List, Tuple

import numpy as np

from matplotlib import pylab

import pyAgrum as gum
from pyAgrum import skbn

CSV_TMP_SUFFIX = ".x.csv"


def _getFilename(datasrc):
  "*.CSV_TMP_SUFFIXcsv is the signature of a temp csv file"
  if datasrc.endswith(CSV_TMP_SUFFIX):
    return "dataframe"

  return datasrc


def _lines_count(filename):
  """
  Parameters
  ----------
  filename : str
    a filename

  Returns
  -------
  int
    the number of lines in the file
  """
  numlines = 0

  with open(filename) as f:
    for _ in f.readlines():
      numlines += 1

  return numlines


@gum.deprecated_arg(newA="datasrc", oldA="csvname", version="1.8.3")
def _checkCompatibility(bn, fields, datasrc):
  """
  check if variables of the bn are in the fields

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  fields : list
    a list of fields
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame

  Returns
  -------
  list
    a list of position for variables in fields, None otherwise.
  """
  res = {}
  isOK = True
  for field in bn.names():
    if field not in fields:
      print(f"** field '{field}' is missing.")
      isOK = False
    else:
      res[bn.idFromName(field)] = fields[field]

  if not isOK:
    res = None

  return res


def _computeAUC(points):
  """
  Given a set of points drawing a ROC/PR curve, compute the AUC value

  Parameters
  ----------
  points : list
    a list of points

  Returns
  -------
  double
    the AUC value

  """
  # computes the integral from 0 to 1
  somme = 0
  for i in range(1, len(points)):
    somme += (points[i][0] - points[i - 1][0]) * \
             (points[i - 1][1] + points[i][1])

  return somme / 2


def _computeFbeta(points, ind, beta=1):
  return (1 + beta ** 2) * points[ind][1] * points[ind][0] / ((beta ** 2 * points[ind][1]) + points[ind][0])


@gum.deprecated_arg(newA="datasrc", oldA="csvname", version="1.8.3")
def _computePoints(bn, datasrc, target, label, *, beta=1, show_progress=True, with_labels=True, significant_digits=10):
  """
  Compute the ROC points.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame
  target : str
    the target
  label : str
    the target's label or id
  beta : float
    the value of beta for the F-beta score
  show_progress : bool
    indicates if the resulting curve must be printed
  with_labels: bool
    whether we use label or id (especially for parameter label)
  significant_digits:
    number of significant digits when computing probabilities

  Returns
  -------
  tuple (res, totalP, totalN)
    where res is a list of (proba,isWellClassified) for each line of datasrc.

  """
  idTarget = bn.idFromName(target)
  label = str(label)

  if not with_labels:
    idLabel = -1
    for i in range(bn.variable(idTarget).domainSize()):
      if bn.variable(idTarget).label(i) == label:
        idLabel = i
        break
    assert idLabel >= 0
  else:
    idLabel = label

  Classifier = skbn.BNClassifier(beta=beta, significant_digit=significant_digits)

  if show_progress:
    # tqdm is optional:
    # pylint: disable=import-outside-toplevel
    filename = _getFilename(datasrc)
    from tqdm import tqdm
    pbar = tqdm(total=_lines_count(datasrc) - 1, desc=filename,
                bar_format='{desc}: {percentage:3.0f}%|{bar}|')

  Classifier.fromTrainedModel(bn, target, idLabel)
  # as a Binary classifier, y will be a list of True (good classification) and False (bad one)
  X, y = Classifier.XYfromCSV(datasrc, with_labels=with_labels, target=target)
  predictions = Classifier.predict_proba(X)

  totalP = np.count_nonzero(y)
  totalN = len(y) - totalP
  res = []
  for i in range(len(X)):
    px = predictions[i][1]
    res.append((px, y[i]))

    if show_progress:
      pbar.update()

  if show_progress:
    pbar.close()

  return res, totalP, totalN


def _computeROC_PR(values, totalP, totalN, beta):
  """
  Parameters
  ----------
  values :
    the curve values
  totalP : int
    the number of positive values
  totalN : int
    the number of negative values
  beta : float
    the value of beta for the F-beta score

  Returns
  -------
  tuple
    (points_ROC, ind_ROC, threshold_ROC,AUC_ROC,points_PR, ind_PR, threshold_PR, AUC_PR,thresholds)
  """

  res = sorted(values, key=lambda t: t[0], reverse=True)

  vp = 0.0  # Number of True Positives
  fp = 0.0  # Number of False Positives

  ind_ROC = 0
  dmin_ROC = 100.0  # temporal value for knowing the best threshold
  threshopt_ROC = 0  # best threshold (euclidean distance)

  ind_PR = 0
  fmax_PR = 0.0  # temporal value for knowing f1 max
  threshopt_PR = 0  # threshold of f1 max

  pointsROC = [(0, 0)]  # first one
  pointsPR = [(0, 1)]
  thresholds = [1]

  old_threshold = res[0][0]
  for r_i in res:
    # we add a point only if the threshold has changed
    cur_threshold = r_i[0]
    if cur_threshold < old_threshold:  # the threshold allows to take computation variation into account
      fpr = fp / totalN  # false positives rate
      tpr = vp / totalP  # true positives rate and recall
      prec = vp / (vp + fp)  # precision

      # euclidian distance to know the best threshold
      d = fpr * fpr + (1 - tpr) * (1 - tpr)
      if d < dmin_ROC:
        dmin_ROC = d
        ind_ROC = len(pointsROC)
        threshopt_ROC = (cur_threshold + old_threshold) / 2

      if prec + tpr > 0:
        f = (1 + beta ** 2) * prec * tpr / ((beta ** 2 * prec) + tpr)

        if f > fmax_PR:
          fmax_PR = f
          ind_PR = len(pointsPR)
          threshopt_PR = (cur_threshold + old_threshold) / 2

      pointsROC.append((fpr, tpr))
      pointsPR.append((tpr, prec))
      thresholds.append(cur_threshold)

      old_threshold = cur_threshold

    correct_prediction = r_i[1]
    if correct_prediction:
      vp += 1.0
    else:
      fp += 1.0

  # last ones
  thresholds.append(0)
  pointsROC.append((1, 1))
  pointsPR.append((1, 0))

  AUC_ROC = _computeAUC(pointsROC)
  AUC_PR = _computeAUC(pointsPR)

  fbeta_ROC = _computeFbeta(pointsPR, ind_ROC, beta)
  fbeta_PR = _computeFbeta(pointsPR, ind_PR, beta)

  return (pointsROC, ind_ROC, threshopt_ROC, AUC_ROC, fbeta_ROC,
          pointsPR, ind_PR, threshopt_PR, AUC_PR, fbeta_PR,
          thresholds)


@gum.deprecated_arg(newA="datasrc", oldA="csvname", version="1.8.3")
def getROCpoints(bn, datasrc, target, label, with_labels=True, significant_digits=10):
  """
  Compute the points of the ROC curve

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str | DataFrame
    a csv filename or a DataFrame
  target : str
    the target
  label : str
    the target's label
  with_labels: bool
    whether we use label or id (especially for parameter label)
  significant_digits:
    number of significant digits when computing probabilities

  Returns
  -------
    List[Tuple[int,int]]
      the list of points (FalsePositifRate,TruePositifRate)
  """
  if type(datasrc) is not str:
    if hasattr(datasrc, "to_csv") or hasattr(datasrc, "write_csv"):
      import tempfile
      csvfile = tempfile.NamedTemporaryFile(delete=False)
      tmpfilename = csvfile.name
      csvfilename = tmpfilename + CSV_TMP_SUFFIX
      csvfile.close()
      if hasattr(datasrc, "to_csv"):
        datasrc.to_csv(csvfilename, na_rep="?", index=False)
      else:
        datasrc.write_csv(csvfilename, na_rep="?", index=False)

      l = getROCpoints(bn, csvfilename, target, label, with_labels=with_labels, significant_digits=significant_digits)

      os.remove(csvfilename)
      return l
    else:
      raise TypeError("first argument must be a string or a DataFrame")

  (res, totalP, totalN) = _computePoints(bn, datasrc, target,
                                         label, show_progress=False, with_labels=with_labels,
                                         significant_digits=significant_digits)
  (pointsROC, ind_ROC, thresholdROC, AUC_ROC, fbeta_ROC, pointsPR, ind_PR,
   thresholdPR, AUC_PR, fbeta_PR, thresholds) = _computeROC_PR(res, totalP, totalN, beta=1)

  return pointsROC


@gum.deprecated_arg(newA="datasrc", oldA="csvname", version="1.8.3")
def getPRpoints(bn, datasrc, target, label, with_labels=True, significant_digits=10):
  """
  Compute the points of the PR curve

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame
  target : str
    the target
  label : str
    the target's label
  with_labels: bool
    whether we use label or id (especially for parameter label)
  significant_digits:
    number of significant digits when computing probabilities

  Returns
  -------
    List[Tuple[float,float]]
      the list of points (precision,recall)
  """
  if type(datasrc) is not str:
    if hasattr(datasrc, "to_csv") or hasattr(datasrc, "write_csv"):
      import tempfile
      csvfile = tempfile.NamedTemporaryFile(delete=False)
      tmpfilename = csvfile.name
      csvfilename = tmpfilename + CSV_TMP_SUFFIX
      csvfile.close()
      if hasattr(datasrc, "to_csv"):
        datasrc.to_csv(csvfilename, na_rep="?", index=False)
      else:
        datasrc.write_csv(csvfilename, na_rep="?", index=False)

      l = getPRpoints(bn, csvfilename, target, label, with_labels=with_labels, significant_digits=significant_digits)

      os.remove(csvfilename)
      return l
    else:
      raise TypeError("first argument must be a string or a DataFrame")

  show_progress = False
  (res, totalP, totalN) = _computePoints(bn, datasrc, target,
                                         label, show_progress=show_progress, with_labels=with_labels,
                                         significant_digits=significant_digits)
  (pointsROC, ind_ROC, thresholdROC, AUC_ROC, fbeta_ROC, pointsPR, ind_PR,
   thresholdPR, AUC_PR, fbeta_PR, thresholds) = _computeROC_PR(res, totalP, totalN, beta=1)

  return pointsPR


def _getPoint(threshold: float, thresholds: List[float], points: List[Tuple[float, float]]) -> Tuple[float, float]:
  """

  Find the point corresponding to threshold in points (annotated by thresholds)

  Parameters
  ----------
  threshold : float
    the threshold to find
  thresholds: list[float]
    the list of thresholds
  points : list[tuple]
    the list of points

  Returns
  -------
  the point corresponding to threshold
  """

  def _dichot(mi, ma, tab, v):
    mid = (mi + ma) // 2
    if mid == mi:
      return mi

    if tab[mid] == v:
      return mid
    elif tab[mid] > v:
      return _dichot(mid, ma, tab, v)
    else:
      return _dichot(mi, mid, tab, v)

  ind = _dichot(0, len(thresholds), thresholds, threshold)
  if ind == len(points) - 1:
    return points[ind]
  else:  # a threshold is between 2 points
    return (points[ind][0] + points[ind + 1][0]) / 2, (points[ind][1] + points[ind + 1][1]) / 2


def _basicDraw(ax, points, thresholds, fbeta, beta, AUC, main_color, secondary_color,
               last_color="black", thresholds_to_show=None, align_threshold="left"):
  ax.grid(color='#aaaaaa', linestyle='-', linewidth=1, alpha=0.5)

  ax.plot([x[0] for x in points], [y[1] for y in points], '-',
          linewidth=3, color=gum.config["ROC", "draw_color"], zorder=3
          )
  ax.fill_between([x[0] for x in points],
                  [y[1] for y in points], 0, color=gum.config["ROC", "fill_color"]
                  )

  ax.set_ylim((-0.01, 1.01))
  ax.set_xlim((-0.01, 1.01))
  ax.set_xticks(pylab.arange(0, 1.1, .1))
  ax.set_yticks(pylab.arange(0, 1.1, .1))
  ax.grid(True)

  axs = pylab.gca()
  r = pylab.Rectangle((0, 0), 1, 1, edgecolor='#444444',
                      facecolor='none', zorder=1
                      )
  axs.add_patch(r)
  for spine in axs.spines.values():
    spine.set_visible(False)

  if len(points) < 10:
    for i in range(1, len(points) - 1):
      ax.plot(points[i][0], points[i][1], 'o', color="#55DD55", zorder=6)

  def _show_point_from_thresh(thresh, col, shape):
    fontsize = 10 if shape == 'o' else 7
    inc_threshold = 0.01 if align_threshold == "left" else -0.01
    point = _getPoint(thresh, thresholds, points)
    ax.plot(point[0], point[1], shape, color=col, zorder=6)
    ax.text(point[0] + inc_threshold, point[1] - 0.01,
            f'{thresh:.4f}',
            {'color': col, 'fontsize': fontsize},
            horizontalalignment=align_threshold,
            verticalalignment='top',
            rotation=0,
            clip_on=False)

  if thresholds_to_show is not None:
    _show_point_from_thresh(thresholds_to_show[0], main_color, shape="o")
    if len(thresholds_to_show) > 1:
      _show_point_from_thresh(
        thresholds_to_show[1], secondary_color, shape=".")
      if len(thresholds_to_show) > 2:
        for i in range(2, len(thresholds_to_show)):
          _show_point_from_thresh(thresholds_to_show[i], last_color, shape=".")

  if align_threshold == "left":
    AUC_x = 0.95
    AUC_halign = "right"
  else:
    AUC_x = 0.05
    AUC_halign = "left"

  if beta == 1:
    ax.text(AUC_x, 0.0, f'AUC={AUC:.4f}\nF1={fbeta:.4f}', {'color': main_color, 'fontsize': 18},
            horizontalalignment=AUC_halign,
            verticalalignment='bottom',
            fontsize=18)
  else:
    ax.text(AUC_x, 0.0, f'AUC={AUC:.4f}\nF-{beta:g}={fbeta:.4f}', {'color': main_color, 'fontsize': 18},
            horizontalalignment=AUC_halign,
            verticalalignment='bottom',
            fontsize=18)


def _drawROC(points, zeTitle, fbeta_ROC, beta, AUC_ROC, thresholds, thresholds_to_show, ax=None):
  ax = ax or pylab.gca()

  _basicDraw(ax, points, thresholds, fbeta=fbeta_ROC, beta=beta, AUC=AUC_ROC, main_color='#DD5555',
             secondary_color='#120af7', thresholds_to_show=thresholds_to_show, align_threshold="left")
  ax.plot([0.0, 1.0], [0.0, 1.0], '-', color="#AAAAAA")
  ax.set_xlabel('False positive rate')
  ax.set_ylabel('True positive rate')

  ax.set_title(zeTitle)


def _drawPR(points, zeTitle, fbeta_PR, beta, AUC_PR, thresholds, thresholds_to_show, rate, ax=None):
  ax = ax or pylab.gca()

  _basicDraw(ax, points, thresholds, fbeta=fbeta_PR, beta=beta, AUC=AUC_PR, main_color='#120af7',
             secondary_color='#DD5555',
             thresholds_to_show=thresholds_to_show, align_threshold="right")
  ax.plot([0.0, 1.0], [rate, rate], '-', color="#AAAAAA")
  ax.set_xlabel('Recall')
  ax.set_ylabel('Precision')

  ax.set_title(zeTitle)


@gum.deprecated_arg(newA="datasrc", oldA="csvname", version="1.8.3")
def showROC_PR(bn, datasrc, target, label, *, beta=1, show_progress=True, show_fig=True, save_fig=False,
               with_labels=True, show_ROC=True, show_PR=True, significant_digits=10, bgcolor=None):
  """
  Compute the ROC curve and save the result in the folder of the csv file.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame
  target : str
    the target
  label : str
    the target label
  beta : float
    the value of beta for the F-beta score
  show_progress : bool
    indicates if the progress bar must be printed
  save_fig:
    save the result
  show_fig:
    plot the resuls
  with_labels:
    labels in csv
  show_ROC: bool
    whether we show the ROC figure
  show_PR: bool
    whether we show the PR figure
  significant_digits:
    number of significant digits when computing probabilities
  bgcolor:
    HTML background color for the figure (default: None if transparent)

  Returns
  -------
  tuple
    (pointsROC, thresholdROC, pointsPR, thresholdPR)

  """
  if type(datasrc) is not str:
    if hasattr(datasrc, "to_csv") or hasattr(datasrc, "write_csv"):
      import tempfile
      csvfile = tempfile.NamedTemporaryFile(delete=False)
      tmpfilename = csvfile.name
      csvfilename = tmpfilename + CSV_TMP_SUFFIX
      csvfile.close()
      if hasattr(datasrc, "to_csv"):
        datasrc.to_csv(csvfilename, na_rep="?", index=False)
      else:
        datasrc.write_csv(csvfilename, na_rep="?", index=False)

      showROC_PR(bn, csvfilename, target, label, beta=beta, show_progress=show_progress, show_fig=show_fig,
                 save_fig=save_fig, with_labels=with_labels, show_ROC=show_ROC, show_PR=show_PR,
                 significant_digits=significant_digits)

      os.remove(csvfilename)
      return
    else:
      raise TypeError("first argument must be a string or a DataFrame")

  if bgcolor is not None:
    oldcol = gum.config["notebook", "figure_facecolor"]
    gum.config["notebook", "figure_facecolor"] = bgcolor

  filename = _getFilename(datasrc)
  (res, totalP, totalN) = _computePoints(bn, datasrc, target,
                                         label, beta=beta, show_progress=show_progress, with_labels=with_labels,
                                         significant_digits=significant_digits)
  (pointsROC, ind_ROC, thresholdROC, AUC_ROC, fbeta_ROC, pointsPR, ind_PR,
   thresholdPR, AUC_PR, fbeta_PR, thresholds) = _computeROC_PR(res, totalP, totalN, beta)
  try:
    shortname = os.path.basename(bn.property("name"))
  except gum.NotFound:
    shortname = "unnamed"
  title = shortname + " vs " + filename + " - " + target + "=" + str(label)

  rate = totalP / (totalP + totalN)

  if show_ROC and show_PR:
    figname = f"{filename}-ROCandPR_{shortname}-{target}-{label}.png"
    fig = pylab.figure(figsize=(10, 4))
    fig.suptitle(title)
    pylab.gcf().subplots_adjust(wspace=0.1)

    ax1 = fig.add_subplot(1, 2, 1)
    _drawROC(points=pointsROC, zeTitle="ROC", fbeta_ROC=fbeta_ROC, beta=beta, AUC_ROC=AUC_ROC, thresholds=thresholds,
             thresholds_to_show=[thresholdROC, thresholdPR],
             ax=ax1)

    ax2 = fig.add_subplot(1, 2, 2)
    ax2.yaxis.tick_right()
    ax2.yaxis.set_label_position("right")
    _drawPR(points=pointsPR, zeTitle="Precision-Recall", fbeta_PR=fbeta_PR, beta=beta, AUC_PR=AUC_PR,
            thresholds=thresholds, thresholds_to_show=[thresholdPR, thresholdROC], rate=rate, ax=ax2)
  elif show_ROC:
    figname = f"{filename}-ROC_{shortname}-{target}-{label}.png"

    _drawROC(points=pointsROC, zeTitle=title, fbeta_ROC=fbeta_ROC, beta=beta, AUC_ROC=AUC_ROC, thresholds=thresholds,
             thresholds_to_show=[thresholdROC])
  elif show_PR:
    figname = f"{filename}-PR_{shortname}-{target}-{label}.png"
    _drawPR(points=pointsPR, zeTitle=title, fbeta_PR=fbeta_PR, beta=beta, AUC_PR=AUC_PR, thresholds=thresholds,
            thresholds_to_show=[thresholdPR], rate=rate)

  if save_fig:
    pylab.savefig(figname, dpi=300, transparent=(bgcolor is None))

  if show_fig:
    pylab.show()

  if bgcolor is not None:
    gum.config["notebook", "figure_facecolor"] = oldcol

  return AUC_ROC, thresholdROC, AUC_PR, thresholdPR


@gum.deprecated_arg(newA="datasrc", oldA="csvname", version="1.8.3")
def showROC(bn, datasrc, target, label, show_progress=True, show_fig=True, save_fig=False, with_labels=True,
            significant_digits=10):
  """
  Compute the ROC curve and save the result in the folder of the csv file.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame
  target : str
    the target
  label : str
    the target label
  show_progress : bool
    indicates if the progress bar must be printed
  save_fig:
    save the result
  show_fig:
    plot the resuls
  with_labels:
    labels in csv
  significant_digits:
    number of significant digits when computing probabilities
  """

  return showROC_PR(bn, datasrc, target, label, show_progress=show_progress, show_fig=show_fig, save_fig=save_fig,
                    with_labels=with_labels, show_ROC=True, show_PR=False, significant_digits=significant_digits)


@gum.deprecated_arg(newA="datasrc", oldA="csvname", version="1.8.3")
def showPR(bn, datasrc, target, label, *, beta=1, show_progress=True, show_fig=True, save_fig=False,
           with_labels=True, significant_digits=10):
  """
  Compute the ROC curve and save the result in the folder of the csv file.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame
  target : str
    the target
  label : str
    the target label
  show_progress : bool
    indicates if the progress bar must be printed
  save_fig:
    save the result ?
  show_fig:
    plot the resuls ?
  with_labels:
    labels in csv ?
  significant_digits:
    number of significant digits when computing probabilities
  """

  return showROC_PR(bn, datasrc, target, label, show_progress=show_progress, show_fig=show_fig, save_fig=save_fig,
                    with_labels=with_labels, show_ROC=False, show_PR=True, significant_digits=significant_digits)


def animROC(bn, datasrc, target="Y", label="1"):
  """
  Interactive selection of a threshold using TPR and FPR for BN and data

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame
  target : str
    the target
  label : str
    the target label
  """
  import ipywidgets as widgets
  import matplotlib.pyplot as plt
  import matplotlib.ticker as mtick

  class DisplayROC:
    def __init__(self, points):
      self._x = [i / len(points) for i in range(len(points))]
      self._y1, self._y2 = zip(*points)
      self._points = points

    def display(self, threshold):
      rate = threshold / 100.0
      indexes = int((len(self._points) - 1) * rate)

      plt.rcParams["figure.figsize"] = (4, 3)

      fig, (ax1, ax2) = plt.subplots(nrows=2)
      ax1.plot(viewer._x, viewer._y1, "g")
      ax1.plot(viewer._x, viewer._y2, "r")
      ax1.plot([rate, rate], [0, 1])
      ax1.xaxis.set_major_formatter(mtick.PercentFormatter(1.0))

      ax2.barh([0, 1], self._points[indexes], color=["g", "r"])
      ax2.set_yticks(ticks=[0, 1], labels=["FPR", "TPR"])
      ax2.annotate(f" {self._points[indexes][0]:.1%}", xy=(1, 0), xytext=(1, -0.2))
      ax2.annotate(f" {self._points[indexes][1]:.1%}", xy=(1, 1), xytext=(1, 0.8))
      ax2.set_xlim(0, 1)

      plt.tight_layout()
      plt.show()

  viewer = DisplayROC(getROCpoints(bn, datasrc, target=target, label=label))

  def interactive_view(rate: float):
    viewer.display(rate)

  # widgets.interact(interactive_view, rate=(0,100,1))
  interactive_plot = widgets.interactive(interactive_view, rate=(0, 100, 1))
  output = interactive_plot.children[-1]
  output.layout.height = '250px'
  return interactive_plot


def animPR(bn, datasrc, target="Y", label="1"):
  """
  Interactive selection of a threshold using TPR and FPR for BN and data

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  datasrc : str|DataFrame
    a csv filename or a pandas.DataFrame
  target : str
    the target
  label : str
    the target label
  """
  import ipywidgets as widgets
  import matplotlib.pyplot as plt
  import matplotlib.ticker as mtick

  class DisplayPR:
    def __init__(self, points):
      self._x = [i / len(points) for i in range(len(points))]
      self._y1, self._y2 = zip(*points)
      self._points = points

    def display(self, threshold):
      rate = threshold / 100.0
      indexes = int((len(self._points) - 1) * rate)

      plt.rcParams["figure.figsize"] = (4, 3)

      fig, (ax1, ax2) = plt.subplots(nrows=2)
      ax1.plot(viewer._x, viewer._y1, "r")
      ax1.plot(viewer._x, viewer._y2, "g")
      ax1.plot([rate, rate], [0, 1])
      ax1.xaxis.set_major_formatter(mtick.PercentFormatter(1.0))

      ax2.barh([1, 0], self._points[indexes], color=["r", "g"])
      ax2.set_yticks(ticks=[0, 1], labels=["Precision", "Recall"])
      ax2.annotate(f" {self._points[indexes][1]:.1%}", xy=(1, 0), xytext=(1, -0.2))
      ax2.annotate(f" {self._points[indexes][0]:.1%}", xy=(1, 1), xytext=(1, 0.8))
      ax2.set_xlim(0, 1)

      plt.tight_layout()
      plt.show()

  viewer = DisplayPR(getPRpoints(bn, datasrc, target=target, label=label))

  def interactive_view(rate: float):
    viewer.display(rate)

  # widgets.interact(interactive_view, rate=(0,100,1))
  interactive_plot = widgets.interactive(interactive_view, rate=(0, 100, 1))
  output = interactive_plot.children[-1]
  output.layout.height = '250px'
  return interactive_plot
