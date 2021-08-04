"""
The purpose of this module is to provide tools for building ROC and PR from Bayesian Network.
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


import os
from typing import List, Tuple

import numpy as np

from matplotlib import pylab

import pyAgrum as gum
from pyAgrum import skbn


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


def _checkCompatibility(bn, fields, csv_name):
  """
  check if variables of the bn are in the fields

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  fields : list
    a list of fields
  csv_name : str
    the name of the csv file

  Returns
  -------
  list
    a list of position for variables in fields, None otherwise.
  """
  res = {}
  isOK = True
  for field in bn.names():
    if field not in fields:
      print(f"** field '{field}' is missing in {csv_name}")
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


def _computeF1(points, ind):
  return 2 * points[ind][0] * points[ind][1] / (points[ind][0] + points[ind][1])


def _computepoints(bn, csv_name, target, label, show_progress=True, with_labels=True, significant_digits=10):
  """
  Compute the ROC curve points.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  csv_name : str
    a csv filename
  target : str
    the target
  label : str
    the target's label
  show_progress : bool
    indicates if the resulting curve must be printed
  significant_digits:
    number of significant digits when computing probabilities

  Returns
  -------
  tuple (res, totalP, totalN)
    where res is a list of (proba,isWellClassified) for each line of csv_name.

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

  Classifier = skbn.BNClassifier(significant_digit=significant_digits)

  if show_progress:
    # tqdm is optional:
    # pylint: disable=import-outside-toplevel
    from tqdm import tqdm
    pbar = tqdm(total=_lines_count(csv_name) - 1, desc=csv_name,
                bar_format='{desc}: {percentage:3.0f}%|{bar}|')

  Classifier.fromTrainedModel(bn, target, idLabel)
  # as a Binary classifier, y will be a list of True (good classification) and False (bad one)
  X, y = Classifier.XYfromCSV(csv_name, with_labels=with_labels, target=target)
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


def _computeROC_PR(values, totalP, totalN):
  """
  Parameters
  ----------
  values :
    the curve values
  totalP : int
    the number of positive values
  totalN : int
    the number of negative values

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
        f = 2 * prec * tpr / (prec + tpr)  # f1
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

  f1_ROC = _computeF1(pointsPR, ind_ROC)
  f1_PR = _computeF1(pointsPR, ind_PR)

  return (pointsROC, ind_ROC, threshopt_ROC, AUC_ROC, f1_ROC,
          pointsPR, ind_PR, threshopt_PR, AUC_PR, f1_PR,
          thresholds)


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


def _basicDraw(ax, points, thresholds, f1, AUC, main_color, secondary_color, last_color="black",
               thresholds_to_show=None, align_threshold="left"):
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

  ax.text(AUC_x, 0.0, f'AUC={AUC:.4f}\nf1={f1:.4f}', {'color': main_color, 'fontsize': 18},
          horizontalalignment=AUC_halign,
          verticalalignment='bottom',
          fontsize=18)


def _drawROC(points, zeTitle, f1_ROC, AUC_ROC, thresholds, thresholds_to_show, ax=None):
  ax = ax or pylab.gca()

  _basicDraw(ax, points, thresholds, f1=f1_ROC, AUC=AUC_ROC, main_color='#DD5555',
             secondary_color='#120af7', thresholds_to_show=thresholds_to_show, align_threshold="left")
  ax.plot([0.0, 1.0], [0.0, 1.0], '-', color="#AAAAAA")
  ax.set_xlabel('False positive rate')
  ax.set_ylabel('True positive rate')

  ax.set_title(zeTitle)


def _drawPR(points, zeTitle, f1_PR, AUC_PR, thresholds, thresholds_to_show, rate, ax=None):
  ax = ax or pylab.gca()

  _basicDraw(ax, points, thresholds, f1=f1_PR, AUC=AUC_PR, main_color='#120af7', secondary_color='#DD5555',
             thresholds_to_show=thresholds_to_show, align_threshold="right")
  ax.plot([0.0, 1.0], [rate, rate], '-', color="#AAAAAA")
  ax.set_xlabel('Precision')
  ax.set_ylabel('Recall')

  ax.set_title(zeTitle)


def showROC_PR(bn, csv_name, target, label, show_progress=True, show_fig=True, save_fig=False, with_labels=True,
               show_ROC=True, show_PR=True, significant_digits=10):
  """
  Compute the ROC curve and save the result in the folder of the csv file.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  csv_name : str
    a csv filename
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
  show_ROC: bool
    whether we show the ROC figure
  show_PR: bool
    whether we show the PR figure
  significant_digits:
    number of significant digits when computing probabilitie

  Returns
  -------
  tuple
    (pointsROC, thresholdROC, pointsPR, thresholdPR)

  """

  (res, totalP, totalN) = _computepoints(bn, csv_name, target,
                                         label, show_progress, with_labels, significant_digits)
  (pointsROC, ind_ROC, thresholdROC, AUC_ROC, f1_ROC, pointsPR, ind_PR,
   thresholdPR, AUC_PR, f1_PR, thresholds) = _computeROC_PR(res, totalP, totalN)
  try:
    shortname = os.path.basename(bn.property("name"))
  except gum.NotFound:
    shortname = "unnamed"
  title = shortname + " vs " + csv_name + " - " + target + "=" + str(label)

  rate = totalP / (totalP + totalN)

  if show_ROC and show_PR:
    figname = f"{csv_name}-ROCandPR_{shortname}-{target}-{label}.png"
    fig = pylab.figure(figsize=(10, 4))
    fig.suptitle(title)
    pylab.gcf().subplots_adjust(wspace=0.1)

    ax1 = fig.add_subplot(1, 2, 1)
    _drawROC(points=pointsROC, zeTitle="ROC", f1_ROC=f1_ROC, AUC_ROC=AUC_ROC, thresholds=thresholds,
             thresholds_to_show=[thresholdROC, thresholdPR],
             ax=ax1)

    ax2 = fig.add_subplot(1, 2, 2)
    ax2.yaxis.tick_right()
    ax2.yaxis.set_label_position("right")
    _drawPR(points=pointsPR, zeTitle="Precision-Recall", f1_PR=f1_PR, AUC_PR=AUC_PR,
            thresholds=thresholds, thresholds_to_show=[thresholdPR, thresholdROC], rate=rate, ax=ax2)
  elif show_ROC:
    figname = f"{csv_name}-ROC_{shortname}-{target}-{label}.png"

    _drawROC(points=pointsROC, zeTitle=title, f1_ROC=f1_ROC, AUC_ROC=AUC_ROC, thresholds=thresholds,
             thresholds_to_show=[thresholdROC])
  elif show_PR:
    figname = f"{csv_name}-PR_{shortname}-{target}-{label}.png"
    _drawPR(points=pointsPR, zeTitle=title, f1_PR=f1_PR, AUC_PR=AUC_PR, thresholds=thresholds,
            thresholds_to_show=[thresholdPR], rate=rate)

  if save_fig:
    pylab.savefig(figname, dpi=300)

  if show_fig:
    pylab.show()

  return AUC_ROC, thresholdROC, AUC_PR, thresholdPR


def showROC(bn, csv_name, target, label, show_progress=True, show_fig=True, save_fig=False, with_labels=True,
            significant_digits=10):
  """
  Compute the ROC curve and save the result in the folder of the csv file.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  csv_name : str
    a csv filename
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

  return showROC_PR(bn, csv_name, target, label, show_progress=show_progress, show_fig=show_fig, save_fig=save_fig,
                    with_labels=with_labels, show_ROC=True, show_PR=False, significant_digits=significant_digits)


def showPR(bn, csv_name, target, label, show_progress=True, show_fig=True, save_fig=False, with_labels=True,
           significant_digits=10
           ):
  """
  Compute the ROC curve and save the result in the folder of the csv file.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a Bayesian network
  csv_name : str
    a csv filename
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

  return showROC_PR(bn, csv_name, target, label, show_progress=show_progress, show_fig=show_fig, save_fig=save_fig,
                    with_labels=with_labels, show_ROC=False, show_PR=True, significant_digits=significant_digits)
