"""
The purpose of this module is to provide tools for computing different scores from a BN.
"""
# (c) Copyright by Pierre-Henri Wuillemin, 2022
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

import csv
import math

import pyAgrum as gum


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
    if not field in fields:
      raise gum.DatabaseError(f"** At least, field '{field}' is missing in {csv_name}")
    res[bn.idFromName(field)] = fields[field]

  return res


def computeScores(bn_name, csv_name, visible=False):
  """
  Compute scores (likelihood, aic, bic, mdl, etc.) from a bn w.r.t to a csv

  Parameters
  ----------
  bn_name : pyAgrum.BayesNet | str
    a gum.BayesianNetwork or a filename for a BN
  csv_name : str
     a filename for the CSV database
  visible: bool
    do we show the progress

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

  with open(csv_name, "r") as csvfile:
    dialect = csv.Sniffer().sniff(csvfile.read(1024))

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

      for i in range(inst.nbrDim()):
        try:
          inst.chgVal(i, _getIdLabel(inst, i, data[positions[i]]))
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
  if isinstance(label,int):  # an idLabel
    return label

  return inst.variable(i)[label]
