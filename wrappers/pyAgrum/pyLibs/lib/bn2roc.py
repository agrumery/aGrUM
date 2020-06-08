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

import matplotlib.pylab as pylab

import pyAgrum as gum

try:
  from ._utils.progress_bar import ProgressBar
except ModuleNotFoundError:
  from _utils.progress_bar import ProgressBar

try:
  from ._utils.pyAgrum_header import pyAgrum_header
except ModuleNotFoundError:
  from _utils.pyAgrum_header import pyAgrum_header


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
  for line in open(filename):
    numlines += 1

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
    somme += (points[i][0] - points[i-1][0]) * (points[i-1][1]+points[i][1])

  return somme/2


def __computepoints(bn, csv_name, target, label, visible=False, with_labels=True,significant_digits=10):
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
  significant_digits:
    number of significant digits when computing probabilities

  Returns
  -------
  tuple
    (res, totalP, totalN, idLabel)

  """
  idTarget = bn.idFromName(target)
  label = str(label)

  if not with_labels:
    idLabel = -1
    for i in range(bn.variable(idTarget).domainSize()):
      if bn.variable(idTarget).label(i) == label:
        idLabel = i
        break
    assert (idLabel >= 0)

  engine = gum.LazyPropagation(bn)
  engine.eraseAllEvidence()
  mbvars = [bn.variable(i).name()
            for i in gum.MarkovBlanket(bn, target).nodes()
            if bn.variable(i).name()!=target]
  e = {}
  for var in bn.names():
    if not var.__eq__(target):
      e[var] = 0
  engine.setEvidence(e)
  engine.addTarget(idTarget)

  if visible:
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
    prog = ProgressBar(csv_name + ' : ', 0, nbr_lines,
                       77, mode='static', char='#')
    prog.display()

  totalP = 0
  totalN = 0
  res = []
  for data in batchReader:
    if with_labels:
      if str(data[positions[idTarget]]) == label:
        totalP += 1
      else:
        totalN += 1
    else:
      idTargetLine = int(data[positions[idTarget]])
      if idTargetLine == idLabel:
        totalP += 1
      else:
        totalN += 1

    if with_labels:
      for var in mbvars:
        engine.chgEvidence(var, str(data[positions[bn.idFromName(var)]]))
    else:
      for var in mbvars:
        engine.chgEvidence(var, int(data[positions[bn.idFromName(var)]]))

    try:
      engine.makeInference()
      px = round(engine.posterior(idTarget)[{target: label}],significant_digits)
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
    return (res, totalP, totalN, label)
  else:
    return (res, totalP, totalN, idLabel)


def _computeROC_PR(bn, values, totalP, totalN, idLabel, modalite):
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

  res = sorted(values, key=lambda x: x[0], reverse = True)
  
  vp = 0.0      # Number of True Positives
  fp = 0.0      # Number of False Positives
  
  xopt = 0.0    # True Postives Rate value for the best threshold
  yopt = 0.0    # False Postives Rate value for the best threshold
  seuilopt = 100.0   # temporal value for knowing the best threshold
  seuil = 0  # best threshold (euclidian distance)
  
  zf1 = 0.0     # Recall value of f1 max
  yf1 = 0.0     # Precision value of f1 max
  f1opt = 0.0   # temporal value for knowing f1 max
  seuilf1 = 0   # threshold of f1 max 
  
  
  pointsROC = [(0, 0)]  # first one
  pointsPR = [(0,1)]
  
  old_seuil = res[0][0]
  for i in range(len(res)):
    # we add a point only if the seuil has changed
    if res[i][0]<old_seuil:  # the seuil allows to take computation variation into account
      x = fp / totalN   # false positives rate
      y = vp / totalP   # true positives rate and recall
      z = vp / (vp + fp)        # precision
      
      d = x * x + (1 - y) * (1 - y)     #euclidian distance to know the best threshold
      if d < seuilopt:
        seuilopt = d
        xopt = x
        yopt = y
        seuil = (res[i][0] + old_seuil) / 2
        
      if z + y > 0:
        f = 2 * z * y / (z + y)           # f1
        if f > f1opt:
          f1opt = f
          zf1 = z
          yf1 = y
          seuilf1 = (res[i][0] + old_seuil) / 2
        
      pointsROC.append((x, y))
      pointsPR.append((y, z))
      old_seuil = res[i][0]

    res_id = res[i][1]
    if res_id == idLabel:
      vp += 1.0
    else:
      fp += 1.0

  pointsROC.append((1, 1))  # last one
  pointsPR.append((1,0))
  
  return pointsROC, (xopt, yopt), seuil, pointsPR, (yf1, zf1), seuilf1

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
  
  points, opt, seuil, _, _, _ = _computeROC_PR(bn, values, totalP, totalN, idLabel, modalite)
  
  return (points, opt, seuil)

def _computePR(bn, values, totalP, totalN, idLabel, modalite):
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
  
  _, _, _, points, opt, seuil = _computeROC_PR(bn, values, totalP, totalN, idLabel, modalite)
  
  return (points, opt, seuil)

def module_help(exit_value=1, message=""):
  """
  defines help viewed if args are not OK on command line, and exit with exit_value
  """
  print(os.path.basename(
      sys.argv[0]), "src.{" + gum.availableBNExts() + "} data[.csv] variable label")
  print()
  print(message)
  print()
  sys.exit(exit_value)


def _drawROC_PR(points, zeTitle, zeFilename, visible, show_fig, save_fig=True,
             special_point=None, special_value=None, special_label=None, 
             rate = None, showROC = True, showPR = True, ax = None):
  """
  Draw the ROC curve and save (or not) the curve into zeFilename as a png

  Parameters
  ----------
  points :
    a set of points drawing a curve
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
  rate :
    rate of Positives in the dataset (use only for Precision-Recall cruve)
  showROC : bool
    indicates if the curve is ROC
  showPR : bool
    indicates if the curve is Precision-Recall curve
  axs :
    where the curve will be plot (use only whe  both are shown)
  """
  ax = ax or pylab.gca()
  
  AUC = _computeAUC(points)
  
  ax.grid(color='#aaaaaa', linestyle='-', linewidth=1, alpha=0.5)

  ax.plot([x[0] for x in points], [y[1] for y in points], '-',
           linewidth=3, color=gum.config["ROC", "draw_color"], zorder=3)
  ax.fill_between([x[0] for x in points],
                   [y[1] for y in points], 0, color=gum.config["ROC", "fill_color"])
  
  if showROC :
    ax.plot([0.0, 1.0], [0.0, 1.0], '-', color="#AAAAAA")
  if showPR :
    ax.plot([0.0, 1.0], [rate, rate], '-', color="#AAAAAA")
  
  ax.set_ylim((-0.01, 1.01))
  ax.set_xlim((-0.01, 1.01))
  ax.set_xticks(pylab.arange(0, 1.1, .1))
  ax.set_yticks(pylab.arange(0, 1.1, .1))
  ax.grid(True)

  axs = pylab.gca()
  r = pylab.Rectangle((0, 0), 1, 1, edgecolor='#444444',
                      facecolor='none', zorder=1)
  axs.add_patch(r)
  [spine.set_visible(False) for spine in axs.spines.values()]

  if len(points) < 10:
    for i in range(1, len(points) - 1):
      ax.plot(points[i][0], points[i][1], 'o', color="#000066", zorder=6)

  if showPR :
    ax.set_xlabel('Recall')
    ax.set_ylabel('Precision')
  if showROC :
    ax.set_xlabel('False positive rate')
    ax.set_ylabel('True positive rate')

  if special_point is not None:
    ax.plot(special_point[0], special_point[1],
               'o', color="#DD9999", zorder=6)
    if special_value is not None:
      horadj = "left" if showROC else "right"
      incadj = 0.01 if showROC else -0.01
      ax.text(special_point[0] + incadj, special_point[1] - 0.01, special_value,
                 {'color': '#DD5555', 'fontsize': 10},
                 horizontalalignment=horadj,
                 verticalalignment='top',
                 rotation=0,
                 clip_on=False)
  if special_label is not None:
    if special_label != "":
      labels = [special_label]
      colors = ['#DD9999']
      circles = [ax.Circle((0, 0), 1, fc=colors[0])]
      legend_location = 'lower right'
      ax.legend(circles, labels, loc=legend_location)

  if showPR and special_point is not None:
    f1 = 2 * special_point[0] * special_point[1] / (special_point[0] + special_point[1])
    ax.text(0.5, 0.2, 'AUC=%f\nf1=%f' % (AUC,f1),
            horizontalalignment='center',
            verticalalignment='center',
            fontsize=18)
  else : 
    ax.text(0.5, 0.2, 'AUC=%f' % AUC,
            horizontalalignment='center',
            verticalalignment='center',
            fontsize=18)

  ax.set_title(zeTitle)

  if save_fig:
    pylab.savefig(zeFilename, dpi=300)

  if show_fig:
    pylab.show()
    
  if save_fig:   
    return zeFilename

def _drawROC(points, zeTitle, zeFilename, visible, show_fig, save_fig=True,
             special_point=None, special_value=None, special_label=None):
  """
  Draw the ROC curve and save (or not) the curve into zeFilename as a png

  Parameters
  ----------
  points :
    a set of points drawing a curve
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
  _drawROC_PR(points, zeTitle, zeFilename, visible, show_fig = show_fig, save_fig = save_fig, 
              special_point = special_point, special_value = special_value, special_label = special_label, 
              showROC = True, showPR = False)
  
def _drawPR(points, rate,zeTitle, zeFilename, visible, show_fig, save_fig=True,
             special_point=None, special_value=None, special_label=None):
  """
  Draw the ROC curve and save (or not) the curve into zeFilename as a png

  Parameters
  ----------
  points :  
    a set of points drawing a curve
  rate :
    rate of Positives in the dataset (use only for Precision-Recall cruve)
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
  special_point :s
    a special point to be highlighted
  special_value :
    a special value to be highlighted
  special_label :
    a special label to be highlighted
  """
  _drawROC_PR(points, zeTitle, zeFilename, visible, show_fig, save_fig = save_fig, 
              special_point = special_point, special_value = special_value, special_label = special_label, 
              rate = rate, showROC = False, showPR = True)

def showROC_PR(bn, csv_name, variable, label, visible=True, show_fig=False, with_labels=True, showROC = True, showPR = True,significant_digits=10 ):
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
  showROC : bool
    indicates if the curve is ROC
  showPR : bool
    indicates if the curve is Precision-Recall curve
  significant_digits:
    number of significant digits when computing probabilitie
    
  Returns
  -------
  tuple
    (pointsROC, seuilROC, pointsPR, seuilPR)

  """
  
  (res, totalP, totalN, idTarget) = __computepoints(
      bn, csv_name, variable, label, visible, with_labels,significant_digits)
  pointsROC, optROC, seuilROC, pointsPR, optPR, seuilPR = _computeROC_PR(bn, res, totalP, totalN, idTarget, label)
    
  try:
    shortname = os.path.basename(bn.property("name"))
  except gum.NotFound:
    shortname = "unnamed"
  
  title = shortname + " vs " + csv_name + " - " + variable + "=" + str(label)    
  rate = totalP / (totalP + totalN)
  
  if showROC and showPR :  
    figname = csv_name + "-" + 'ROCandPR_' + shortname + \
        "-" + variable + "-" + str(label) + '.png'
    fig = pylab.figure(figsize=(10,4))
    fig.suptitle(title)
    pylab.gcf().subplots_adjust(wspace = 0.1)

    ax1 = fig.add_subplot(1,2,1)
   
    _drawROC_PR(pointsROC, "ROC", figname, not visible, False,
                special_point=optROC, special_value=seuilROC, showROC=True, showPR=False, ax=ax1)
    
    
    ax2 = fig.add_subplot(1,2,2)
    ax2.yaxis.tick_right()
    ax2.yaxis.set_label_position("right")
    _drawROC_PR(pointsPR, "Precision-Recall", figname, not visible, show_fig, 
                special_point=optPR, special_value=seuilPR, rate = rate, showROC = False, showPR = True, ax = ax2)
        
  elif showROC : 
    figname = csv_name + "-" + 'ROC_' + shortname + \
        "-" + variable + "-" + str(label) + '.png'

    _drawROC(pointsROC, title, figname, not visible, show_fig,
             special_point=optROC, special_value=seuilROC)
     
  elif showPR :
    figname = csv_name + "-" + 'PrecisionRecall_' + shortname + \
        "-" + variable + "-" + str(label) + '.png'
    _drawPR(pointsPR, rate, title, figname, not visible, show_fig,
            special_point=optPR, special_value=seuilPR)
    
  return (_computeAUC(pointsROC), seuilROC, _computeAUC(pointsPR), seuilPR)
    

def showROC(bn, csv_name, variable, label, visible=True, show_fig=False, with_labels=True,significant_digits=10):
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
  significant_digits:
    number of significant digits when computing probabilities
  """
  
  return showROC_PR(bn, csv_name, variable, label, visible, show_fig, with_labels, True, False,significant_digits)
  
def showPR(bn, csv_name, variable, label, visible=True, show_fig=False, with_labels=True,significant_digits=10):
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
  significant_digits:
    number of significant digits when computing probabilities
  """
  
  return showROC_PR(bn, csv_name, variable, label, visible, show_fig, with_labels, False, True,significant_digits)  

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
      module_help(message=" Variable '" + variable +
                  "'not found.\n Variables : " + str(bn.names()))

    if label.__eq__(""):
      module_help(message=" Labels : " + str(bn.variableFromName(variable)))
    else:
      try:
        bn.variableFromName(variable)[label]
      except gum.OutOfBounds:
        module_help(message=" Label '" + label +
                    "' not found.\n Labels : " + str(bn.variableFromName(variable)))

  return (bn, csv_name, variable, label)
  

if __name__ == "__main__":
  pyAgrum_header("2011-19")

  (bn, csv_name, variable, label) = _checkROCargs()
  showROC(bn, csv_name, variable, label, True, False)
