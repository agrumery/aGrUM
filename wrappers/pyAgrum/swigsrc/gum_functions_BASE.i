/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
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
  setNumberOfThreads(config.asInt['core', 'default_maxNumberOfThreads'])

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


def _gum_add_properties_while_getstate_(model):
  if not hasattr(model, "setProperty"):
    return

  if config.asBool["Pickle", "add_version"]:
    model.setProperty("version", f"pyAgrum {__version__}")
  if config.asBool["Pickle", "add_date"]:
    from datetime import datetime
    model.setProperty("creation", model.propertyWithDefault("creation", datetime.now().strftime("%Y-%m-%d %H:%M:%S%z")))
    model.setProperty("lastModification", datetime.now().strftime("%Y-%m-%d %H:%M:%S%z"))


def log2(p):
  """Compute p.log2() in a new Potential without modifying p

  Parameters
  ----------
  p : pyAgrum.Potential
    The potential on which to apply log2 function

  Returns
  -------
    a pyAgrum.Potential
  """
  return Potential(p).log2()


def fastGraph(msg:str):
  """
  Create a graph with a dot-like syntax which specifies the structure 'a->b->c;b-d<-e;' where a,b,c,d,e,.. are unsigned int

  Warnings
  --------
    The choice of "-" for edges is unambiguous because "-" is not a valid character for a node id (unsigned int).
    Moreover, "--" is already used by `pyAgrum.fastMRF` to specify factors.

  Parameters
  ----------
    msg : str
      the string containing the specification

  Returns
  -------
    pyAgrum.DiGraph ou pyAgrum.UndiGraph ou pyAgrum.MixedGraph
  """
  # regexp to recognize strings with only unsigned int, ";", "-" followed by ">" or ">"


  import re
  if not re.match(r"^[\d;\-><]+$", msg):
    raise InvalidArgument("fastGraph only accepts strings with only unsigned int, ';', '->' and '<-'")

  is_arc="->" in msg or "<-" in msg
  is_edge="-" in msg
  if is_arc:
    if is_edge:
      m=pyAgrum.MixedGraph()
    else:
      m=pyAgrum.DiGraph()
  else:
    m=pyAgrum.UndiGraph()

  def addEdgeIn(m,msg):
    t=msg.split("-")
    if len(t)==1:
      n1=int(msg)
      deb=n1
      if not m.existsNode(n1):
        m.addNodeWithId(n1)
    else:
      deb,n1=addEdgeIn(m,t[0])
      for i in range(1,len(t)):
        d,f=addEdgeIn(m,t[i])
        m.addEdge(n1,d)
        n1=f
    return deb,n1
  def addRevArcsIn(m,msg):
    t=msg.split("<-")
    if len(t)==1:
      deb,n1=addEdgeIn(m,msg)
    else:
      deb,fin=addEdgeIn(m,t[0])
      n1=fin
      for i in range(1,len(t)):
        d,f=addEdgeIn(m,t[i])
        m.addArc(d,n1)
        n1=f
    return deb,n1
  def addArcsIn(m,msg):
    t=msg.split("->")
    if len(t)==1:
      deb,n1=addRevArcsIn(m,msg)
    else:
      deb,fin=addRevArcsIn(m,t[0])
      n1=fin
      for i in range(1,len(t)):
        d,f=addRevArcsIn(m,t[i])
        m.addArc(n1,d)
        n1=f
    return deb,n1

  for l in msg.split(";"):
    if len(l)>0:
      t=addArcsIn(m,l)

  return m
%}
