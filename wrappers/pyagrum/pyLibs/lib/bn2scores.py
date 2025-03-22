############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
The purpose of this module is to provide tools for computing different scores from a BN.
"""

import csv
import math

import pyagrum as gum


def lines_count(filename):
  """ count lines in a file
  """
  with open(filename) as f:
    count = sum(1 for _ in f)

  return count


def checkCompatibility(bn, fields, csv_name):
  """
  check if the variables of the bn are in the fields

  Parameters
  ----------
  bn: gum.BayesNet
    the model
  fields: Dict[str,int]
    Dict of name,position in the file
  csv_name: str
    name of the csv file

  Raises
  ------
  gum.DatabaseError
    if a BN variable is not in fields

  Returns
  -------
  Dict[int,str]
    return a dictionary of position for BN variables in fields
  """
  res = {}
  for field in bn.names():
    if field not in fields:
      raise gum.DatabaseError(f"** At least, field '{field}' is missing in {csv_name}")
    res[bn.idFromName(field)] = fields[field]

  return res


def computeScores(bn_name, csv_name, visible=False, dialect=None):
  """
  Compute scores (likelihood, aic, bic, mdl, etc.) from a bn w.r.t to a csv

  Parameters
  ----------
  bn_name : pyagrum.BayesNet | str
    a gum.BayesianNetwork or a filename for a BN
  csv_name : str
     a filename for the CSV database
  visible: bool
    do we show the progress
  dialect: csv.Dialect
    if not provided, dialect will be inferred using csv.Sniffer().sniff(csvfile.read(1024))

  Returns
  -------
  Tuple[float,Dict[str,float]]
    percentDatabaseUsed,scores
  """
  if isinstance(bn_name, str):
    bn = gum.loadBN(bn_name)
  else:
    bn = bn_name

  nbr_lines = lines_count(csv_name) - 1

  if dialect is None:
    with open(csv_name, "r") as csvfile:
      k = 1024
      buffer = ""
      while True:
        try:
          buffer += csvfile.read(k)
          dialect = csv.Sniffer().sniff(buffer)
          break
        except csv.Error:
          if k == -1:
            # we already tried with all the file
            raise gum.DatabaseError("csv.Sniffer could not determine delimiter even with the whole file")
          if len(buffer) > 16384:
            # we try with all the file now
            k = -1

  nbr_insignificant = 0
  num_ligne = 1
  likelihood = 0.0

  with open(csv_name, 'r') as csvfile:
    batchReader = csv.reader(csvfile, dialect)

    titre = next(batchReader)
    fields = {}
    for i, nom in enumerate(titre):
      fields[nom] = i

    positions = checkCompatibility(bn, fields, csv_name)

    inst = bn.completeInstantiation()

    if visible:
      # tqdm is optional
      # pylint: disable=import-outside-toplevel
      from tqdm import tqdm
      pbar = tqdm(total=nbr_lines, desc=csv_name, bar_format='{desc}: {percentage:3.0f}%|{bar}|')

    for data in batchReader:
      num_ligne += 1

      for i in positions.keys():
        try:
          name = bn.variable(i).name()
          inst.chgVal(name, _getIdLabel(inst, name, data[positions[i]]))
        except gum.OutOfBounds:
          print(
            f"Out of bounds for ({i},{positions[i]}) : unknown id or label '{data[positions[i]]}' for the variable {inst.variable(i)}")

      p = bn.jointProbability(inst)
      if p == 0.0:
        print(str(num_ligne) + ":" + str(inst))
        nbr_insignificant += 1
      else:
        likelihood += math.log(p, 2)
      if visible:
        pbar.update()

    if visible:
      pbar.close()

    nbr_arcs = 1.0 * bn.sizeArcs()
    dim = 1.0 * bn.dim()

    aic = likelihood - dim
    aicc = 2 * aic - 2 * dim * (dim + 1) / (nbr_lines - dim + 1) if (nbr_lines - dim + 1 > 0) else "undefined"
    bic = likelihood - dim * math.log(nbr_lines, 2)
    mdl = likelihood - nbr_arcs * math.log(nbr_lines, 2) - 32 * dim  # 32=nbr bits for a params

  return ((nbr_lines - nbr_insignificant) * 100.0 / nbr_lines,
          {'likelihood': likelihood, 'aic': aic, 'aicc': aicc, 'bic': bic, 'mdl': mdl})


def _getIdLabel(inst, i, label):
  """
  Return the idLabel. if label is an int, it already is an idLabel

  Parameters
  ----------
  inst : gum.Instantiation
    the inst to find the idLabel
  i: int
    the id to find the variable
  label: int|str
    the label or id of label

  Returns
  -------
  int
    the id of label
  """
  if isinstance(label, int):  # an idLabel
    return label

  return inst.variable(i)[label]
