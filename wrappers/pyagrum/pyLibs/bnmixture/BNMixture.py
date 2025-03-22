from typing import Dict, Optional, Union, List, Set
from tempfile import mkdtemp, TemporaryDirectory
from zipfile import ZipFile

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

import os
import logging
import pyagrum
import shutil


class IMixture:
  _bns: Dict[str, pyagrum.BayesNet]
  _weights: Dict[str, float]
  _refBN: pyagrum.BayesNet
  _refName: str

  def __init__(self):
    raise NotImplemented("class IMixture is not supposed to be initialized !")

  def __repr__(self):
    nameref = self._refBN.property("name")
    ret = f"(reference BN : {nameref}), "
    for name in self.names():
      ret += "(" + name + ", w=" + str(self._weights[name]) + "), "
    if len(ret) > 1:
      ret = ret[:-2]
    return ret

  def names(self) -> List[str]:
    """
    Returns
    -------
    List[str]
        The list of names of the BNs in the model (reference BN not included).
    """
    return list(self._bns.keys()).copy()

  def size(self) -> int:
    """
    Returns
    -------
    int
        The number of BNs in the model (reference BN not included).
    """
    return len(self._bns)

  def setWeight(self, name: str, w: float):
    """
    Changes the weight of a BN in the model.

    Parameters
    ----------
    name : str
        Name of the BN to modify.
    w : float
        Value of the new weight.

    Raises
    ------
    pyagrum.InvalidArgument
        If the weight is negative.
    pyagrum.NotFound
        If the given name doesn't correspond to the name of a BN in the model.
    """
    if w < 0:
      raise pyagrum.InvalidArgument("weight is negative !")
    if name not in self._bns:
      raise pyagrum.NotFound(f"'{name}' isn't in the model !")
    self._weights[name] = w

  def weight(self, name: str) -> float:
    """
    Parameters
    ----------
    name : str
        Name of the BN.

    Returns
    -------
    float
        The weight of the BN with name ``name``.

    Raises
    ------
    pyagrum.NotFound
        If the given name doesn't correspond to the name of a BN in the model.
    """
    if name not in self._bns:
      raise pyagrum.NotFound(f"{name} isn't in the model")
    return self._weights[name]

  def weights(self) -> Dict[str, float]:
    """
    Returns
    -------
    Dict[str, float]
        The weights of all the BNs in the model.
    """
    return self._weights.copy()

  def add(self, name: str, bn: pyagrum.BayesNet, w: float = 1):
    """
    Adds a BN to the model. If the model doesn't have a reference BN when trying to add an element,
    the BN (before adding new element) with highest weight becomes the new reference.

    Parameters
    ----------
    name : str
        Name of the BN to add.
    bn : pyagrum.BayesNet
        BN to add.
    w : float
        Weight of the BN.

    Raises
    ------
    pyagrum.InvalidArgument
        If the weight is negative.
    pyagrum.InvalidArgument
        If the names of the variables in the BN to add are differents from the one in the reference BN.
    pyagrum.InvalidArgument
        If the variables in the BN to add are differents from the one in the reference BN.
    pyagrum.InvalidArgument
        If the name in argument is the same as the reference BN's name.
    pyagrum.ArgumentError
        If the name in argument already exists for a different BN in the model.
    """
    if w < 0:
      raise pyagrum.InvalidArgument("weight is negative !")
    if name == self._refName:
      raise pyagrum.InvalidArgument("Can't give name used by reference BN")

    if self._refBN is None:
      self._refBN = pyagrum.BayesNet(bn)
      self._refBN.setProperty("name", "refBN")
      # self._refTrueName = name
      self._bns[name] = pyagrum.BayesNet(bn)
      self._bns[name].setProperty("name", name)
      self._weights[name] = w
      return

    if bn.names() != self._refBN.names():
      raise pyagrum.InvalidArgument("variables names are different from the reference BN variables")
    if set([bn.variable(name) for name in bn.names()]) != set(
       [self._refBN.variable(name) for name in self._refBN.names()]):
      raise pyagrum.InvalidArgument("variables are different from those in the reference BN")

    if name in self._bns and bn == self._bns[name]:
      self._weights[name] += w
    elif name not in self._bns and bn in self._bns.values():
      # logging.warning("bn already exists with a different name")
      for tmp in self._bns:
        if bn == self._bns[tmp]:
          self._weights[tmp] += w
          break
    elif name not in self._bns and bn not in self._bns.values():
      self._bns[name] = pyagrum.BayesNet(bn)
      self._bns[name].setProperty("name", name)
      self._weights[name] = w
    else:
      raise pyagrum.ArgumentError("name already exists for different bn")

  def remove(self, name: str):
    """
    Removes a BN from the model.

    Parameters
    ----------
    name : str
        Name of the BN to remove.

    Raises
    ------
    pyagrum.NotFound
        If the given name doesn't correspond to the name of a BN in the model.
    """
    if name not in self._bns:
      raise pyagrum.NotFound(f"'{name}' isn't in the model !")
    self._bns.pop(name)
    self._weights.pop(name)

  def isNormalized(self) -> bool:
    """
    Checks if the model is normalized (the sum of the weights equals 1).
    """
    return round(sum(self._weights.values()), 14) == 1

  def normalize(self):
    """
    Normalizes the weights.
    """
    total = sum(self._weights.values())
    for name in self._bns.keys():
      self.setWeight(name, self.weight(name) / total)

  def isValid(self) -> bool:
    """
    Checks if all the weights are equal to 0. Valid if sum of the weights is not 0.

    Returns
    -------
    bool
        True if weights are valid. False otherwise.
    """
    return sum(self._weights.values()) != 0

  def BN(self, name: str) -> "pyagrum.BayesNet":
    """
    Parameters
    ----------
    name : str
        Name of the variable.

    Returns
    -------
    pyagrum.BayesNet
        A copy of the BN with name ``name`` in the model.

    Raises
    ------
    pyagrum.NotFound
        If the given name doesn't correspond to the name of a BN in the model.
    """
    if name not in self._bns:
      raise pyagrum.NotFound(f"{name} isn't in the mixture")
    res = pyagrum.BayesNet(self._bns[name])
    res.setProperty("name", name)
    return res

  def BNs(self) -> List[pyagrum.BayesNet]:
    """
    Returns
    -------
    List[pyagrum.BayesNet]
        A list containing a copy of all BNs in the model.
    """
    return [self.BN(name) for name in self.names()]

  def zeroBNs(self) -> Set[str]:
    """
    Returns
    -------
    Set[str]
        The names of the BNs in the model that have weight with value 0.
    """
    return {name for name in self.names() if self.weight(name) == 0}

  def existsArc(self, a, b):
    """
    Counts the number of time arc ``a`` -> ``b`` appears among all BNs in the model.

    Parameters
    ----------
    a : str | int
        Tail of the arc.
    b : str | int
        Head of the arc.

    Returns
    -------
    int
        The number of time arc ``a`` -> ``b`` appears.
    """
    count = 0
    for bn in self.BNs():
      if bn.existsArc(a, b):
        count += 1
    return count

  def variable(self, name: str):
    """
    Parameters
    ----------
    name : str
        Name of the variable.

    Returns
    -------
    pyagrum.LabelizedVariable
        The corresponding variable.
    """
    if self._refBN is None:
      raise pyagrum.NotFound("Reference BN is None")
    return self._refBN.variable(name)

  def saveBIF(self, fname: str):
    """
    Saves a Mixture in BIF format and zip it.

    Parameters
    ----------
    fname : str
        Name of the file (without extenstion).
    """
    with TemporaryDirectory("", fname, None) as temp_dir:
      txt_name = f"{temp_dir}/weights.txt"
      with open(txt_name, "w+") as f_txt:
        for e in self.weights():
          f_txt.write(f"{e}:{self.weight(e)}\n")
      ref_name = "ref_" + self._refBN.property("name")
      self._refBN.saveBIF(f"{temp_dir}/{ref_name}.bif")
      for name in self.names():
        file_name = f"{temp_dir}/{name}.bif"
        self.BN(name).saveBIF(file_name)
      shutil.make_archive(fname, 'zip', temp_dir)


class BNMixture(IMixture):
  """
  Experimental model. The class contains a list of BNs. Each BN has its own weight.
  """

  def __init__(self):
    self._bns = {}
    self._weights = {}
    self._refBN = None
    self._refName = "refBN"

  def updateRef(self):
    """
    Updates the reference BN. The new reference BN is the one with maximum weight.
    """
    highest = max(list(self._weights.keys()), key=lambda x: self._weights[x])
    newRef = pyagrum.BayesNet(self._bns[highest])
    newRef.setProperty("name", self._refName)
    # self._refTrueName = highest
    self._refBN = newRef

  @staticmethod
  def loadBIF(filename):
    """
    Retrieve a BNMixture from a file.

    Parameters
    ----------
    filename : str
        Zip file containing the mixture.

    Returns
    -------
    BNMixture
        The stored BNMixture
    """
    with TemporaryDirectory("", "tmp", None) as temp_dir:
      # temp_dir = mkdtemp("", "tmp", None)
      with ZipFile(filename, 'r') as zf:
        zf.extractall(temp_dir)
      weights = {}
      with open(f"{temp_dir}/weights.txt", 'r') as wf:
        for line in wf:
          name, w = line.split(':')
          weights[name] = float(w)
      bnm = BNMixture()
      for path in os.listdir(temp_dir):
        name, _ = path.split('.')
        if name in weights:
          bni = pyagrum.BayesNet()
          bni.loadBIF(f"{temp_dir}/{path}")
          bnm.add(name, bni, w=weights[name])
      bnm.updateRef()
      return bnm


class BootstrapMixture(IMixture):
  """
  Experimental model base on bootstraping. The class contains reference BN and a list of BNs. Each BN has its own weight except for the reference BN.
  The reference BN is used so that every other BN added later contains the same variables as the reference BN.

  Parameters
  ----------
  name : str
      Name of the first BN to add to the model. It is used for reference.
  bn : pyagrum.BayesNet
      BN to add. Adding new BNs to the model is allowed only if the variables are the same in the first and new BN.
  """

  def __init__(self, name: str, bn: pyagrum.BayesNet):
    self._refBN = pyagrum.BayesNet(bn)
    self._refBN.setProperty("name", name)
    self._bns = {}
    self._weights = {}
    self._refName = name

  @staticmethod
  def loadBIF(filename):
    """
    Retrieve a BootstrapMixture from a file.

    Parameters
    ----------
    filename : str
        Zip file containing the mixture.

    Returns
    -------
    BootstrapMixture
        The stored BootstrapMixture.
    """
    with TemporaryDirectory("", "tmp", None) as temp_dir:
      with ZipFile(filename, 'r') as zf:
        zf.extractall(temp_dir)
      weights = {}
      with open(f"{temp_dir}/weights.txt", 'r') as wf:
        for line in wf:
          name, w = line.split(':')
          weights[name] = float(w)
      bn_dict = {}
      refBN = None
      for path in os.listdir(temp_dir):
        name, form = path.split('.')
        if form != 'bif':
          continue
        bni = pyagrum.BayesNet()
        bni.loadBIF(f"{temp_dir}/{path}")
        if name in weights:
          bn_dict[name] = bni
          bn_dict[name].setProperty("name", name)
        else:
          refBN = bni
          refBN.setProperty("name", name)
      bnm = BootstrapMixture(refBN.property("name"), refBN)
      for name in bn_dict:
        bnm.add(name, bn_dict[name], weights[name])
      return bnm
