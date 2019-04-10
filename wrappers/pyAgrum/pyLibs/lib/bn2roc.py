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

import csv
import os
import sys

import pyAgrum as gum

from ._utils.progress_bar import ProgressBar
from ._utils.pyAgrum_header import pyAgrum_header


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
  for line in open(filename): numlines += 1

  return numlines


def _checkCompatibility(bn, fields, csv_name):
  """
  check if variables of the bn are in the fields

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a bayesian network
  fields : list
    a list of fields
  csv_name : str
    the name of the csv file

  Returns
  -------
  list
    a list of position for variables in fiels, None otherwise.
  """
  res = {}
  isOK = True
  for field in bn.names():
    if not field in fields:
      print("** field '{0}' is missing in {1}".format(field, csv_name))
      isOK = False
    else:
      res[bn.idFromName(field)] = fields[field]

  if not isOK:
    res = None

  return res


def _computeAUC(points):
  """
  Given a set of points drawing a ROC curve, compute the AUC value

  Parameters
  ----------
  points : set
    a set of points drawing a ROC curve

  Returns
  -------
  double
    the AUC value

  """
  # computes the integral from 0 to 1
  somme = 0
  for i in range(1, len(points)):
    somme += (points[i - 1][0] - points[i][0]) * points[i][1]

  return -somme


def __computeROCpoints(bn, csv_name, target, label, visible=False,with_labels=True):
  """
  Compute the ROC curve points.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a bayesian network
  csv_name : str
    a csv filename
  target : str
    the target
  label : str
    the target's label
  visible : bool
    indicates if the resulting curve must be printed

  Returns
  -------
  tuple
    (res, totalP, totalN, idLabel)

  """
  idTarget = bn.idFromName(target)
  label=str(label)

  if not with_labels:
      idLabel = -1
      for i in range(bn.variable(idTarget).domainSize()):
        if bn.variable(idTarget).label(i) == label:
          idLabel = i
          break;
      assert (idLabel >= 0)

  engine = gum.LazyPropagation(bn)

  nbr_lines = _lines_count(csv_name) - 1

  if (sys.version_info >= (3, 0)):  # python 3
    csvfile = open(csv_name, "r")
    dialect = csv.Sniffer().sniff(csvfile.readline())
    csvfile.seek(0)
    batchReader = csv.reader(open(csv_name, 'r'), dialect)
    titre = batchReader.__next__()
  else:  # python2
    csvfile = open(csv_name, "rb")
    dialect = csv.Sniffer().sniff(csvfile.readline())
    csvfile.seek(0)
    batchReader = csv.reader(open(csv_name, 'rb'), dialect)
    titre = batchReader.next()
  fields = {}
  for i, nom in enumerate(titre):
    fields[nom] = i

  positions = _checkCompatibility(bn, fields, csv_name)

  if positions is None:
    sys.exit(1)

  if visible:
    prog = ProgressBar(csv_name + ' : ', 0, nbr_lines, 77, mode='static', char='#')
    prog.display()

  totalP = 0
  totalN = 0
  res = []
  for data in batchReader:
    if with_labels:
      if str(data[positions[idTarget]])==label:
        totalP += 1
      else:
        totalN += 1
    else:
      idTargetLine = int(data[positions[idTarget]])
      if idTargetLine == idLabel:
        totalP += 1
      else:
        totalN += 1

    engine.eraseAllEvidence()
    e = {}
    for var in bn.names():
      if not var.__eq__(target):
        if with_labels:
            e[var] = str(data[positions[bn.idFromName(var)]])
        else:
            e[var] = bn.variableFromName(var).label(int(data[positions[bn.idFromName(var)]]))

    try:
      engine.setEvidence(e)
      engine.makeInference()
      px = engine.posterior(idTarget)[{target: label}]
      if with_labels:
        res.append((px, str(data[positions[idTarget]])))
      else:
        res.append((px, int(data[positions[idTarget]])))
    except gum.OutOfBounds as err:
      print(err)
      print("erreur : " + str(e))

    if visible:
      prog.increment_amount()
      prog.display()

  if visible:
    print

  if with_labels:
    return (res,totalP,totalN,label)
  else:
    return (res, totalP, totalN, idLabel)


def _computeROC(bn, values, totalP, totalN, idLabel, modalite):
  """
  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a bayesian network
  values :
    the ROC curve values
  totalP : int
    the number of positive values
  totalN : int
    the number of negative values
  idLabel : int
    the id of the target
  modalite :
    the label of the target

  Returns
  -------
  tuple
    (points, opt, seuil)
  """

  res = sorted(values, key=lambda x: x[0])

  vp = 0.0
  fp = 0.0
  xopt = 0.0
  yopt = 0.0
  opt = 100.0
  seuilopt = 0
  points = [(0, 0)]  # first one
  old_seuil = res[0][0]
  for i in range(len(res)):
    # we add a point only if the seuil has changed
    if res[i][0] - old_seuil > 1e-6:  # the seuil allows to take computation variation into account
      x = vp / totalP
      y = fp / totalN
      d = x * x + (1 - y) * (1 - y)
      if d < opt:
        opt = d
        xopt = x
        yopt = y
        seuilopt = old_seuil
      points.append((x, y))
      old_seuil = res[i][0]

    res_id = res[i][1]
    if res_id == idLabel:
      vp += 1.0
    else:
      fp += 1.0

  points.append((1, 1))  # last one

  return points, (xopt, yopt), seuilopt


def module_help(exit_value=1, message=""):
  """
  defines help viewed if args are not OK on command line, and exit with exit_value
  """
  print(os.path.basename(sys.argv[0]), "src.{" + gum.availableBNExts() + "} data[.csv] variable label")
  print()
  print(message)
  print()
  sys.exit(exit_value)


def _drawROC(points, zeTitle, zeFilename, visible, show_fig, save_fig=True,
            special_point=None, special_value=None, special_label=None):
  """
  Draw the ROC curve and save (or not) the curve into zeFilename as a png

  Parameters
  ----------
  points :
    a set of points drawing a ROC curve
  zeTitle : str
    the title of the curve
  zeFilename : str
    the name of the file
  visible : bool
    unnecessary ?
  show_fig : bool
    indicates if the resulting curve must be printed
  save_fig : bool
    indicates if the resulting curve must be saved
  special_point :
    a special point to be highlighted
  special_value :
    a special value to be highlighted
  special_label :
    a special label to be highlighted
  """
  AUC = _computeAUC(points)
  import pylab

  pylab.clf()
  pylab.grid(color='#aaaaaa', linestyle='-', linewidth=1, alpha=0.5)

  pylab.plot([x[0] for x in points], [y[1] for y in points], '-', linewidth=3, color="#000088", zorder=3)
  pylab.fill_between([x[0] for x in points], [y[1] for y in points], 0, color='0.9')
  pylab.plot([0.0, 1.0], [0.0, 1.0], '-', color="#AAAAAA")

  pylab.ylim((-0.01, 1.01))
  pylab.xlim((-0.01, 1.01))
  pylab.xticks(pylab.arange(0, 1.1, .1))
  pylab.yticks(pylab.arange(0, 1.1, .1))
  pylab.grid(True)

  ax = pylab.gca()
  r = pylab.Rectangle((0, 0), 1, 1, edgecolor='#444444', facecolor='none', zorder=1)
  ax.add_patch(r)
  [spine.set_visible(False) for spine in ax.spines.values()]

  if len(points) < 10:
    for i in range(1, len(points) - 1):
      pylab.plot(points[i][0], points[i][1], 'o', color="#000066", zorder=6)

  pylab.xlabel('False positive rate')
  pylab.ylabel('True positive rate')

  if special_point is not None:
    pylab.plot(special_point[0], special_point[1], 'o', color="#DD9999", zorder=6)
    if special_value is not None:
      pylab.text(special_point[0] + 0.01, special_point[1] - 0.01, special_value,
                 {'color': '#DD5555', 'fontsize': 10},
                 horizontalalignment='left',
                 verticalalignment='top',
                 rotation=0,
                 clip_on=False)
  if special_label is not None:
    if special_label != "":
      labels = [special_label]
      colors = ['#DD9999']
      circles = [pylab.Circle((0, 0), 1, fc=colors[0])]
      legend_location = 'lower right'
      pylab.legend(circles, labels, loc=legend_location)

  pylab.text(0.5, 0.3, 'AUC=%f' % AUC,
             horizontalalignment='center',
             verticalalignment='center',
             fontsize=18)

  pylab.title(zeTitle)

  if save_fig:
    pylab.savefig(zeFilename, dpi=300)
    print("\n result in " + zeFilename)

  if show_fig:
    pylab.show()


def showROC(bn, csv_name, variable, label, visible=True, show_fig=False,with_labels=True):
  """
  Compute the ROC curve and save the result in the folder of the csv file.

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    a bayesian network
  csv_name : str
    a csv filename
  target : str
    the target
  label : str
    the target label
  visible : bool
    indicates if the resulting curve must be printed

  """
  (res, totalP, totalN, idTarget) = __computeROCpoints(bn, csv_name, variable, label, visible,with_labels)
  points, opt, seuil = _computeROC(bn, res, totalP, totalN, idTarget, label)

  try:
    shortname = os.path.basename(bn.property("name"))
  except gum.NotFound:
    shortname = "unnamed"

  title = shortname + " vs " + csv_name + " - " + variable + "=" + str(label)

  figname = csv_name + "-" +'ROC_' + shortname + "-" +  variable + "-" + str(label) + '.png'

  _drawROC(points, title, figname, not visible, show_fig, special_point=opt, special_value=seuil)


def _checkROCargs():
  pyAgrum_header("2011-13")

  bn_name = sys.argv[1] if len(sys.argv) > 1 else ""
  csv_name = sys.argv[2] if len(sys.argv) > 2 else ""
  variable = sys.argv[3] if len(sys.argv) > 3 else ""
  label = sys.argv[4] if len(sys.argv) > 4 else ""

  if bn_name.__eq__(""):
    module_help()

  bn = gum.loadBN(bn_name)

  if csv_name.__eq__(""):
    module_help()

  if variable.__eq__(""):
    module_help(message=" Variables : " + str(bn.names()))
  else:
    if variable not in bn.names():
      module_help(message=" Variable '" + variable + "'not found.\n Variables : " + str(bn.names()))

    if label.__eq__(""):
      module_help(message=" Labels : " + str(bn.variableFromName(variable)))
    else:
      try:
        bn.variableFromName(variable)[label]
      except gum.OutOfBounds:
        module_help(message=" Label '" + label + "' not found.\n Labels : " + str(bn.variableFromName(variable)))

  return (bn, csv_name, variable, label)


if __name__ == "__main__":
  pyAgrum_header("2011-17")

  (bn, csv_name, variable, label) = _checkROCargs()
  showROC(bn, csv_name, variable, label, True, False)
