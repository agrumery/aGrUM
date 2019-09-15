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
import math
import os
import sys

import pyAgrum as gum

from ._utils.progress_bar import ProgressBar
from ._utils.pyAgrum_header import pyAgrum_header


def lines_count(filename):
  """ count lines in a file
  """
  numlines = 0
  for line in open(filename): numlines += 1

  return numlines


def checkCompatibility(bn, fields, csv_name):
  """check if variables of the bn are in the fields

  if not : return None
  if compatibilty : return a list of position for variables in fields
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


def computeScores(bn_name, csv_name, visible=False, transforme_label=None):
  if isinstance(bn_name, str):
    bn = gum.loadBN(bn_name)
  else:
    bn = bn_name

  nbr_lines = lines_count(csv_name) - 1

  csvfile = open(csv_name, "r")
  dialect = csv.Sniffer().sniff(csvfile.read(1024))
  csvfile.seek(0)

  batchReader = csv.reader(open(csv_name, 'r'), dialect)

  titre = next(batchReader)
  fields = {}
  for i, nom in enumerate(titre):
    fields[nom] = i

  positions = checkCompatibility(bn, fields, csv_name)
  if positions is None:
    sys.exit(1)

  inst = bn.completeInstantiation()

  if visible:
    prog = ProgressBar(csv_name + ' : ', 0, nbr_lines, 77, mode='static', char='#')
    prog.display()

  nbr_insignificant = 0
  num_ligne = 0
  likelihood = 0.0
  for data in batchReader:
    num_ligne += 1

    for i in range(inst.nbrDim()):
      try:
        inst.chgVal(i, getNumLabel(inst, i, data[positions[i]], transforme_label))
      except gum.OutOfBounds:
        print("out of bounds", i, positions[i], data[positions[i]], inst.variable(i))

    p = bn.jointProbability(inst)
    if p == 0.0:
      print(str(num_ligne) + ":" + str(inst))
      nbr_insignificant += 1
    else:
      likelihood += math.log(p, 2)
    if visible:
      prog.increment_amount()
      prog.display()

  if visible:
    print

  nbr_arcs = 1.0 * bn.sizeArcs()
  dim = 1.0 * bn.dim()

  aic = likelihood - dim
  aicc = 2 * aic - 2 * dim * (dim + 1) / (nbr_lines - dim + 1) if (nbr_lines - dim + 1 > 0) else "undefined"
  bic = likelihood - dim * math.log(nbr_lines, 2)
  mdl = likelihood - nbr_arcs * math.log(nbr_lines, 2) - 32 * dim  # 32=nbr bits for a params

  return ((nbr_lines - nbr_insignificant) * 100.0 / nbr_lines,
          {'likelihood': likelihood, 'aic': aic, 'aicc': aicc, 'bic': bic, 'mdl': mdl})


def module_help(exit_value=1):
  """
  defines help viewed if args are not OK on command line, and exit with exit_value
  """
  print(os.path.basename(sys.argv[0]), "src.{" + gum.availableBNExts() + "} [data[.csv]]")
  sys.exit(exit_value)


def getNumLabel(inst, i, label, transforme_label):
  if transforme_label is not None:
    label = transforme_label(label)

  if label.isdigit():  # an indice
    return int(label)
  else:
    return inst.variable(i)[label]


def stringify(s):
  return '"' + s + '"'


if __name__ == "__main__":
  pyAgrum_header("2011-19")

  if len(sys.argv) < 2:
    # module_help()
    bn_name = "../resources/alarm.dsl"
    csv_name = "alarm.csv"
  else:
    bn_name = sys.argv[1]

    if len(sys.argv) < 3:
      base, ext = os.path.splitext(bn_name)
      csv_name = base + '.csv'
    else:
      csv_name = sys.argv[2]
      base, ext = os.path.splitext(csv_name)

  print('"{0}" vs "{1}"'.format(bn_name, csv_name))
  print()
  (nbr, LL) = computeScores(bn_name, csv_name, visible=True)
  print()
  print('{0}% of base is significant.\nscores : {1}'.format(nbr, LL))
