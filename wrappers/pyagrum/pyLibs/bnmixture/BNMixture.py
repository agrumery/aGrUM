from tempfile import TemporaryDirectory
from zipfile import ZipFile

############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import json
import math
import os
import pyagrum


class IMixture:
  _bns: dict[str, pyagrum.BayesNet]
  _weights: dict[str, float]
  _refBN: pyagrum.BayesNet | None
  _refName: str

  def __init__(self):
    raise NotImplementedError("class IMixture is not supposed to be initialized !")

  def __repr__(self):
    nameref = self._refBN.property("name") if self._refBN is not None else "<none>"
    ret = f"(reference BN : {nameref}), "
    for name in self.names():
      ret += "(" + name + ", w=" + str(self._weights[name]) + "), "
    if len(ret) > 1:
      ret = ret[:-2]
    return ret

  @property
  def refBN(self) -> "pyagrum.BayesNet | None":
    """Read-only access to the reference BN."""
    return self._refBN

  @property
  def refName(self) -> str:
    """Read-only access to the reference BN name."""
    return self._refName

  def names(self) -> list[str]:
    """
    Returns
    -------
    list[str]
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

  def setWeight(self, name: str, w: float) -> None:
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

  def weights(self) -> dict[str, float]:
    """
    Returns
    -------
    dict[str, float]
        The weights of all the BNs in the model.
    """
    return self._weights.copy()

  def add(self, name: str, bn: pyagrum.BayesNet, w: float = 1) -> None:
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
      self._bns[name] = pyagrum.BayesNet(bn)
      self._bns[name].setProperty("name", name)
      self._weights[name] = w
      return

    if set(bn.names()) != set(self._refBN.names()):
      raise pyagrum.InvalidArgument("variables names are different from the reference BN variables")
    for vname in self._refBN.names():
      if bn.variable(vname).domain() != self._refBN.variable(vname).domain():
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

  def remove(self, name: str) -> None:
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
    return math.isclose(sum(self._weights.values()), 1.0)

  def normalize(self) -> None:
    """
    Normalizes the weights.

    Raises
    ------
    pyagrum.InvalidArgument
        If all weights are zero (cannot normalize).
    """
    total = sum(self._weights.values())
    if total == 0:
      raise pyagrum.InvalidArgument("Cannot normalize: all weights are zero.")
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

  def BN(self, name: str) -> pyagrum.BayesNet:
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

  def BNs(self) -> list[pyagrum.BayesNet]:
    """
    Returns
    -------
    list[pyagrum.BayesNet]
        A list containing a copy of all BNs in the model.
    """
    return [self.BN(name) for name in self.names()]

  def zeroBNs(self) -> set[str]:
    """
    Returns
    -------
    Set[str]
        The names of the BNs in the model that have weight with value 0.
    """
    return {name for name in self.names() if self.weight(name) == 0}

  def existsArc(self, a: str | int, b: str | int) -> int:
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
    for bn in self._bns.values():
      if bn.existsArc(a, b):
        count += 1
    return count

  def variable(self, name: str) -> pyagrum.DiscreteVariable:
    """
    Parameters
    ----------
    name : str
        Name of the variable.

    Returns
    -------
    pyagrum.DiscreteVariable
        The corresponding variable.
    """
    if self._refBN is None:
      raise pyagrum.NotFound("Reference BN is None")
    return self._refBN.variable(name)


class BNMixture(IMixture):
  """
  A mixture of Bayesian networks where each network carries a positive weight.

  The **reference BN** (``refBN``) is the BN with the highest weight; it is
  recomputed by :meth:`updateRef`.  It serves as the structural template for
  visualisation and variable look-ups, but plays no special role during inference:
  :class:`~pyagrum.bnmixture.BNMixtureInference` returns the weight-averaged
  posterior over *all* BNs in the mixture.

  Notes
  -----
  This is an experimental model.
  """

  def __init__(self):
    self._bns = {}
    self._weights = {}
    self._refBN = None
    self._refName = "refBN"

  def updateRef(self) -> None:
    """
    Updates the reference BN. The new reference BN is the one with maximum weight.
    """
    if not self._weights:
      raise pyagrum.InvalidArgument("Cannot update reference: mixture is empty.")
    highest = max(list(self._weights.keys()), key=lambda x: self._weights[x])
    newRef = pyagrum.BayesNet(self._bns[highest])
    newRef.setProperty("name", self._refName)
    self._refBN = newRef


class BootstrapMixture(IMixture):
  """
  A mixture of Bayesian networks built by Bayesian bootstrapping a single database.

  The **reference BN** (``refBN``) is learned from the *original* (uniformly
  weighted) database.  It is the primary estimate: its posteriors are the ones
  returned by :class:`~pyagrum.bnmixture.BootstrapMixtureInference`.

  The other BNs are learned from Bayesian bootstrap resamples of the same database
  (Dirichlet-drawn record weights simulate resampling with replacement).  They are
  used exclusively to quantify the stability of the reference estimate: arc-confidence
  scores and quantile intervals on posteriors.

  Parameters
  ----------
  name : str
      Name given to the reference BN.  Acts as a guard: no BN added later may
      carry this same name.
  bn : pyagrum.BayesNet
      The reference BN.  Every BN added later must share the same variables.

  Notes
  -----
  This is an experimental model.  Use :class:`~pyagrum.bnmixture.BNMBootstrapLearner`
  to build one automatically from a database.
  """

  def __init__(self, name: str, bn: pyagrum.BayesNet):
    self._refBN = pyagrum.BayesNet(bn)
    self._refBN.setProperty("name", name)
    self._bns = {}
    self._weights = {}
    self._refName = name


def saveBNM(bnm: "IMixture", fname: str, fmt: str = "jgum") -> None:
  """
  Save a BNMixture or BootstrapMixture to a file.

  The file is created at the exact path ``fname`` (no extension is appended).
  Use :func:`loadBNM` with the same name to reload it.

  Parameters
  ----------
  bnm : IMixture
      The mixture model to save.
  fname : str
      Exact output filename.
  fmt : str
      Format for individual BNs: ``'jgum'`` (JSON, default) or ``'bgum'`` (binary).

  Raises
  ------
  pyagrum.InvalidArgument
      If ``fmt`` is neither ``'jgum'`` nor ``'bgum'``.
  """
  if fmt not in ("jgum", "bgum"):
    raise pyagrum.InvalidArgument(f"Unknown format '{fmt}'. Use 'jgum' or 'bgum'.")
  safe_ref_file = f"ref_bn.{fmt}"
  name_map = {f"bn_{i}": name for i, name in enumerate(bnm.names())}
  with TemporaryDirectory() as temp_dir:
    with open(f"{temp_dir}/manifest.json", "w") as f:
      json.dump(
        {"type": type(bnm).__name__, "ref_file": safe_ref_file, "weights": bnm.weights(), "name_map": name_map}, f
      )
    pyagrum.saveBN(bnm._refBN, f"{temp_dir}/{safe_ref_file}")
    for stem, name in name_map.items():
      pyagrum.saveBN(bnm.BN(name), f"{temp_dir}/{stem}.{fmt}")
    with ZipFile(fname, "w") as zf:
      for f in os.listdir(temp_dir):
        zf.write(f"{temp_dir}/{f}", arcname=f)


def loadBNM(fname: str) -> "IMixture":
  """
  Load a BNMixture or BootstrapMixture from a file saved with :func:`saveBNM`.

  Requires a ``manifest.json`` in the archive (produced by current :func:`saveBNM`).
  For archives saved by older versions of pyAgrum, use :func:`loadRetroCompatibleBNM`.

  Parameters
  ----------
  fname : str
      Exact filename to read.

  Returns
  -------
  IMixture
      A :class:`BNMixture` or :class:`BootstrapMixture` depending on what was saved.

  Raises
  ------
  pyagrum.InvalidArgument
      If ``manifest.json`` is absent from the archive.
  """
  with TemporaryDirectory() as temp_dir:
    with ZipFile(fname, "r") as zf:
      zf.extractall(temp_dir)
    manifest_path = f"{temp_dir}/manifest.json"
    if not os.path.exists(manifest_path):
      raise pyagrum.InvalidArgument(
        "No manifest.json found in the archive. "
        "Use loadRetroCompatibleBNM() for archives saved by older versions of pyAgrum."
      )
    with open(manifest_path) as f:
      manifest = json.load(f)
    type_name = manifest["type"]
    ref_file = manifest["ref_file"]
    weights = manifest["weights"]
    name_map = manifest.get("name_map")  # {safe_stem: original_name}, absent in old archives

    def resolve(stem: str) -> "str | None":
      name = name_map.get(stem) if name_map else stem
      return name if name in weights else None

    if type_name == "BootstrapMixture":
      members: dict[str, pyagrum.BayesNet] = {}
      refBN = None
      for path in os.listdir(temp_dir):
        stem, ext = os.path.splitext(path)
        if ext not in (".jgum", ".bgum"):
          continue
        if path == ref_file:
          refBN = pyagrum.loadBN(f"{temp_dir}/{path}")
        elif (name := resolve(stem)) is not None:
          members[name] = pyagrum.loadBN(f"{temp_dir}/{path}")
      if refBN is None:
        raise pyagrum.InvalidArgument(f"Reference file '{ref_file}' listed in manifest not found in archive.")
      bnm = BootstrapMixture(refBN.property("name"), refBN)
      for name, bn in members.items():
        bnm.add(name, bn, weights[name])
      return bnm
    else:
      bnm = BNMixture()
      for path in os.listdir(temp_dir):
        stem, ext = os.path.splitext(path)
        if ext not in (".jgum", ".bgum"):
          continue
        if (name := resolve(stem)) is not None:
          bnm.add(name, pyagrum.loadBN(f"{temp_dir}/{path}"), w=weights[name])
      bnm.updateRef()
      return bnm


def loadRetroCompatibleBNM(fname: str) -> "IMixture":
  """
  Load a BNMixture or BootstrapMixture from an archive saved by older versions of
  pyAgrum (before the ``manifest.json`` format was introduced).

  Uses ``type.txt``, ``weights.txt``, and the ``ref_`` filename prefix heuristic.
  For archives saved by current pyAgrum, prefer :func:`loadBNM`.

  Parameters
  ----------
  fname : str
      Exact filename to read.

  Returns
  -------
  IMixture
      A :class:`BNMixture` or :class:`BootstrapMixture` depending on what was saved.
  """
  with TemporaryDirectory() as temp_dir:
    with ZipFile(fname, "r") as zf:
      zf.extractall(temp_dir)
    type_name = "BNMixture"
    type_file = f"{temp_dir}/type.txt"
    if os.path.exists(type_file):
      with open(type_file) as f:
        type_name = f.read().strip()
    weights = {}
    with open(f"{temp_dir}/weights.txt") as wf:
      for line in wf:
        name, w = line.rsplit(":", 1)
        weights[name] = float(w)
    if type_name == "BootstrapMixture":
      bn_dict = {}
      refBN = None
      for path in os.listdir(temp_dir):
        stem, ext = os.path.splitext(path)
        if ext not in (".jgum", ".bgum"):
          continue
        bni = pyagrum.loadBN(f"{temp_dir}/{path}")
        if stem in weights:
          bn_dict[stem] = bni
        elif stem.startswith("ref_"):
          refBN = bni
          refBN.setProperty("name", stem[4:])
      if refBN is None:
        raise pyagrum.InvalidArgument("No reference BN found in the archive (no 'ref_*' file).")
      bnm = BootstrapMixture(refBN.property("name"), refBN)
      for name, bn in bn_dict.items():
        bnm.add(name, bn, weights[name])
      return bnm
    else:
      bnm = BNMixture()
      for path in os.listdir(temp_dir):
        stem, ext = os.path.splitext(path)
        if ext not in (".jgum", ".bgum") or stem not in weights:
          continue
        bnm.add(stem, pyagrum.loadBN(f"{temp_dir}/{path}"), w=weights[stem])
      bnm.updateRef()
      return bnm
