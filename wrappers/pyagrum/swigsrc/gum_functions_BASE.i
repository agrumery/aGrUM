/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/








%pythoncode %{
import warnings

# seed is chosen randomly :)
initRandom(0)

def _update_config_core():
  # hook to control some parameters for core params
  setNumberOfThreads(config.typed['core', 'default_maxNumberOfThreads'])

# configuration object
from .config import PyAgrumConfiguration
config = PyAgrumConfiguration()
config.add_hook(_update_config_core)
config.run_hooks()

try:
  # load custom configuration if any
  config.load()
except FileNotFoundError:
  pass


def log2(p):
  """Compute p.log2() in a new Tensor without modifying p

  Parameters
  ----------
  p : pyagrum.Tensor
    The tensor on which to apply log2 function

  Returns
  -------
    a pyagrum.Tensor
  """
  return Tensor(p).log2()


def fastGraph(desc: str):
  """
  Build a graph from a dot-like syntax such as ``'A->B->C;B-D<-E;'``, picking a concrete
  graph type from a quick read of `desc`.

  If `desc` only contains directed arcs (``'->'``/``'<-'``), a :class:`pyagrum.DAG` is
  attempted first, falling back to a :class:`pyagrum.DiGraph` if that would create a
  directed cycle. If `desc` mixes arcs and undirected edges (``'-'``), a
  :class:`pyagrum.PDAG` is attempted first, falling back to a :class:`pyagrum.MixedGraph`
  on the same condition. If `desc` only contains undirected edges, a
  :class:`pyagrum.UndiGraph` is returned.

  For explicit control over the returned type, use :func:`pyagrum.fastDiGraph`,
  :func:`pyagrum.fastUndiGraph`, :func:`pyagrum.fastMixedGraph`, :func:`pyagrum.fastDAG`,
  or :func:`pyagrum.fastPDAG` directly.

  Parameters
  ----------
  desc : str
    the string containing the dot-like specification

  Returns
  -------
  pyagrum.DAG or pyagrum.DiGraph or pyagrum.PDAG or pyagrum.MixedGraph or pyagrum.UndiGraph
  """
  import re

  is_arc = ("->" in desc) or ("<-" in desc)
  is_edge = re.search(r"(?<!<)-(?!>)", desc) is not None

  if not is_arc:
    return fastUndiGraph(desc)

  if is_edge:
    try:
      return fastPDAG(desc)
    except InvalidDirectedCycle:
      return fastMixedGraph(desc)

  try:
    return fastDAG(desc)
  except InvalidDirectedCycle:
    return fastDiGraph(desc)
%}

%pythoncode %{
import os.path as ospath

def _gum_pickle_load(filename):
  """Load a model from a pickle file."""
  import pickle
  with open(filename, "rb") as f:
    return pickle.load(f)

def _gum_pickle_save(model, filename):
  """Save a model to a pickle file."""
  import pickle
  with open(filename, "wb") as f:
    pickle.dump(model, f, pickle.HIGHEST_PROTOCOL)

def _gum_set_name_property(model, filename):
  """Set the 'name' property from the filename stem if not already set."""
  model.setProperty("name", model.propertyWithDefault("name", ospath.splitext(ospath.basename(filename))[0]))
%}

%pythoncode %{

####################################################################################
def Potential(*args, **kwargs):
  warnings.warn("** pyAgrum : The class pyagrum.Potential is deprecated since `pyAgrum>=2.0.0`. A pyagrum.Tensor will be returned instead."
                , DeprecationWarning, stacklevel=2)
  return Tensor(*args, **kwargs)

%}
