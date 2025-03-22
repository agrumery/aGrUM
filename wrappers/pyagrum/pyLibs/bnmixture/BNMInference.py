from typing import Optional, List, Tuple
from numpy import quantile, array

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

import pyagrum

import pyagrum.bnmixture as BNM


def tensor2ref(ref, tens) -> "pyagrum.Tensor":
  """
  Returns a copy of ``tens`` but with a reference to ``ref`` 's variables instead. Allow to sum tensors that have the same variables
  but with different instantiations of them.

  Parameters
  ----------
  ref : pyagrum.Tensor
      Tensor containing variables of reference.
  tens : pyagrum.Tensor
      Tensor to convert.

  Returns
  -------
  pyagrum.Tensor
      The converted tensor with values of ``tens`` and variable references of ``ref``.
  """
  res = pyagrum.Tensor()
  for v in tens.names:
    res.add(ref.variable(v))
  return res.fillWith(tens)


class IMixtureInference:
  """
  Interface class for inference on Mixture models.
  """

  def __init__(self):
    self._inferences = []
    raise NotImplementedError("Interface should not be initialized !")

  def setEvidence(self, evs):
    """
    Erases all the evidences and apply addEvidence(key,value) for every pairs in ``evs``. Does this for every BN in the model, excluding reference BN.

    Parameters
    ----------
    evs : dict
        A dict of evidences.
    """
    for ie in self._inferences:
      ie.setEvidence(evs)

  def makeInference(self):
    """
    Compute inference for all BNs in the model, excluding reference BN.
    """
    for ie in self._inferences:
      ie.makeInference()

  def _posteriors(self, name: str):
    """
    Parameters
    ----------
    name : str
        Name of the variable.

    Returns
    -------
    List[pyagrum.Tensor]
        List of (not normalized) posteriors of variable ``name`` for each BN in the model, excluding reference BN.
    """
    return {ie._model.property("name"): pyagrum.Tensor(ie.posterior(name)) for ie in self._inferences}


class BNMixtureInference(IMixtureInference):
  """
  Class used for inference on BNMixtures.

  Parameters
  ----------
  bnm : BNM.BNMixture
      Model used.
  engine : Optional
      Inference tool used to compute inference. You shuold give the class and not an initialized object. Example : engine=pyagrum.LazyPropagation is ok
      but not engine=pyagrum.LazyPropagation(bnm).
  """

  def __init__(self, bnm: BNM.BNMixture, engine=pyagrum.LazyPropagation):
    """
    Parameters
    ----------
    bnm : BNM.BNMixture
      Model used.
    engine : Optional
      Inference tool used to compute inference. You shuold give the class and not an initialized object. Example : engine=pyagrum.LazyPropagation is ok
      but not engine=pyagrum.LazyPropagation(bnm).
    """
    self._bnm = bnm
    self._engine = engine
    self._inferences = [self._engine(bn) for bn in self._bnm.BNs()]

  def posterior(self, name: str) -> "pyagrum.Tensor":
    """
    Parameters
    ----------
    name : str
        Variable to compute posterior for.

    Returns
    -------
    pyagrum.Tensor
        The weighted mean (over all the BNs in the model) of the posterior of a variable.
    """
    posts = self._posteriors(name)

    ret = pyagrum.Tensor()
    for bn_name in posts:
        ret += posts[bn_name].toVarsIn(self._bnm) * self._bnm.weight(bn_name)

    return ret.normalize()


class BootstrapMixtureInference(IMixtureInference):
  """
  Class used for inference on BootstrapMixture objects. The main difference with BNMixtureInference is that the returned posterior is not a mean of all posteriors in the BNs but only
  the one of the reference BN. The other BNs are used to estimate parameters such as max, min and quantile values.

  Parameters
  ----------
  bnm : BNM.BootstrapMixture
      Model used.
  engine : Optional
      Inference tool used to compute inference. You shold give the class and not an initialized object. Example : engine=pyagrum.LazyPropagation is ok
      but not engine=pyagrum.LazyPropagation(bnm).
  """

  def __init__(self, bnm: BNM.BootstrapMixture, engine=pyagrum.LazyPropagation):
    self._bnm = bnm
    self._engine = engine
    self._ref_inference = self._engine(pyagrum.BayesNet(self._bnm._refBN))
    self._inferences = [self._engine(bn) for bn in self._bnm.BNs()]

  def setEvidence(self, evs):
    """
    Erases all the evidences and apply addEvidence(key,value) for every pairs in ``evs``. Does this for every BN in the model, including reference BN.

    Parameters
    ----------
    evs : dict
        A dict of evidences.
    """
    self._ref_inference.setEvidence(evs)
    super().setEvidence(evs)

  def makeInference(self):
    """
    Compute inference for all BNs (including reference BN) in the model.
    """
    self._ref_inference.makeInference()
    super().makeInference()

  def posterior(self, name: str) -> "pyagrum.Tensor":
    """
    Parameters
    ----------
    name : str
        Variable to compute posterior for.

    Returns
    -------
    pyagrum.Tensor
        The posterior of variable ``name`` in the reference BN.
    """
    ref_post = self._ref_inference.posterior(name)
    return ref_post.normalize()

  def quantiles(self, name: str) -> Tuple[pyagrum.Tensor, pyagrum.Tensor]:
    """
    Parameters
    ----------
    name : str
        Variable to compute quantiles for.

    Returns
    -------
    Tuple[pyagrum.Tensor, pyagrum.Tensor]
        Two tensors to store the quantile values. Each tensor stores the quantile for each state of the variable.
        The values considered are those of the BNs in the Mixture (excluding reference BN).

    Raises
    ------
    pyagrum.SizeError
        If the mixture doesn't contain any BN.
    """
    if self._bnm.size() == 0:
      raise pyagrum.SizeError("The mixture doesn't contain any BN, can't compute quantiles")
    q1 = float(pyagrum.config['bnmixture', 'left_quantile'])
    q2 = float(pyagrum.config['bnmixture', 'right_quantile'])
    var = self._bnm.variable(name)

    # init tensors to return
    ret1 = pyagrum.Tensor()
    ret1.add(var)
    ret1.fillWith(0)
    ret2 = pyagrum.Tensor()
    ret2.add(var)
    ret2.fillWith(0)
    I = pyagrum.Instantiation()
    I.add(var)

    # normalize posteriors
    posteriors = self._posteriors(name)
    for nam in posteriors:
      posteriors[nam] = posteriors[nam].toVarsIn(self._bnm).normalize()

    # set quantile values
    I.setFirst()
    while not I.end():
      values = []
      for nam in posteriors:
        values.append(posteriors[nam].get(I))
      v1 = quantile(array(values), q1)
      v2 = quantile(array(values), q2)
      ret1.set(I, v1)
      ret2.set(I, v2)
      I.inc()

    return (ret1, ret2)
