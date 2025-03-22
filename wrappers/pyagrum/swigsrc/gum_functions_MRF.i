/****************************************************************************
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

def availableMRFExts():
  """ Give the list of all formats known by pyAgrum to save a Markov random field.

  Returns
  ------
  str
    a string which lists all suffixes for supported MRF file formats.
  """
  return "uai|pkl"


def loadMRF(filename, listeners=None, verbose=False):
  """load a MRF from a file with optional listeners and arguments

  Parameters
  ----------
  filename: str
    the name of the input file
  listeners: List[Object]
    list of functions to execute
  verbose: bool
    whether to print or not warning messages

  Returns
  -------
  pyagrum.MarkovRandomField
    a MRF from a file using one of the availableMNExts() suffixes.

  Listeners could be added in order to monitor its loading.

  pkl suffix is used to save a BN using pickle. In this case, options are ignored.

  Examples
  --------
  >>> import pyagrum as gum
  >>>
  >>> # creating listeners
  >>> def foo_listener(progress):
  >>>    if progress==200:
  >>>        print(' BN loaded ')
  >>>        return
  >>>    elif progress==100:
  >>>        car='%'
  >>>    elif progress%10==0:
  >>>        car='#'
  >>>    else:
  >>>        car='.'
  >>>    print(car,end='',flush=True)
  >>>
  >>> def bar_listener(progress):
  >>>    if progress==50:
  >>>        print('50%')
  >>>
  >>> # loadBN with list of listeners
  >>> gum.loadMRF('./bn.uai',listeners=[foo_listener,bar_listener])
  >>> # .........#.........#.........#.........#..50%
  >>> # .......#.........#.........#.........#.........#.........% | bn loaded
  """
  mn = MarkovRandomField()

  extension = filename.split('.')[-1].upper()
  if extension == "UAI":
    warns = mn.loadUAI(filename, listeners)
  elif extension == "PKL":
    import pickle
    with open(filename, "rb") as f:
      mn = pickle.load(f)
  else:
    raise InvalidArgument("extension " + filename.split('.')
    [-1] + " unknown. Please use among " + availableBNExts())

  if verbose:
    print(warns)

  mn.setProperty("name", mn.propertyWithDefault("name", ospath.splitext(ospath.basename(filename))[0]))
  return mn


def saveMRF(mn, filename):
  """
  save a MRF into a file using the format corresponding to one of the availableWriteMNExts() suffixes.

  Parameters
  ----------
  mn : pyagrum.MarkovRandomField)
    the MRF to save
  filename : str
    the name of the output file
  """
  extension = filename.split('.')[-1].upper()

  if extension == "UAI":
    mn.saveUAI(filename)
  elif extension == "PKL":
    import pickle
    with open(filename, "wb") as f:
      pickle.dump(mn, f, pickle.HIGHEST_PROTOCOL)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableMNExts())

def fastMRF(structure, domain="[2]"):
  """
  Create a Markov random field with a modified dot-like syntax which specifies:
      - the structure 'a-b-c;b-d;c-e;' where each chain 'a-b-c' specifies a factor,
      - the type of the variables with different syntax (cf documentation).

  Examples
  --------
  >>> import pyagrum as gum
  >>> bn=gum.fastMRF('A--B[1,3]--C{yes|No};C--D[2,4]--E[1,2.5,3.9]',6)

  Parameters
  ----------
  structure : str
          the string containing the specification
  domain: int or str
          the default domain size (int) or domain specification (str) for variables (default is "[2]"

  Returns
  -------
  pyagrum.MarkovRandomField
          the resulting Markov random field
  """
  return MarkovRandomField.fastPrototype(structure, domain)


def getPosterior(model, *, target, evs=None):
  """
  Compute the posterior of a single target (variable) in a BN given evidence


  getPosterior uses a VariableElimination inference.
  If more than one target is needed with the same set of evidence or if the same
  target is needed with more than one set of evidence, this function is not
  relevant since it creates a new inference engine every time it is called.

  Parameters
  ----------
  bn : pyagrum.BayesNet or pyagrum.MarkovRandomField
    The probabilistic Graphical Model
  target: string or int
    variable name or id (forced keyword argument)
  evs:  Dict[name|id:val, name|id : List[ val1, val2 ], ...]. (optional forced keyword argument)
    the (hard and soft) evidence

  Returns
  -------
    posterior (pyagrum.Tensor or other)
  """
  if isinstance(model, pyagrum.bn.BayesNet):
    inf = pyagrum.bn.VariableElimination(model)
  elif isinstance(model, MarkovRandomField):
    inf = ShaferShenoyMRFInference(model)
  else:
    raise InvalidArgument("Argument model should be a PGM (BayesNet or MarkovRandomField")

  if evs is not None:
    inf.setEvidence(evs)
  inf.addTarget(target)
  inf.makeInference()
  # creating a new Tensor from posterior(will disappear with ie)
  return pyagrum.base.Tensor(inf.posterior(target))
%}
