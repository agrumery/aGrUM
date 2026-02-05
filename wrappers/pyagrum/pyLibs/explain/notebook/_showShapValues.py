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

import pyagrum as gum
import pyagrum.lib.notebook as gnb
from pyagrum.lib.bn2graph import BN2dot
from pyagrum.explain import Explanation

import matplotlib.pyplot as plt


def showShapValues(bn: gum.BayesNet, expl: Explanation | dict, cmap="plasma", y=1):
  """
  Show the Shap values in the DAG of the BN
  Parameters
  ----------
  bn : pyagrum.BayesNet
      The Bayesian network
  expl: Explanation | dict[str,float]
      The Shap values to each variable
  cmap: str
      Name of the Matplotlib colormap used for coloring the nodes.
  y: int
      The target class for which the Shap values are computed (default is 1).
      y is ignored if `expl` is a dict.

  Raises
  ------
  TypeError
      If bn is not a gum.BayesNet, if expl is neither an Explanation nor a dict, or if expl is an Explanation and y is not an integer.
  IndexError
      If expl is an Explanation and y is outside the valid class range.
  """

  if not isinstance(bn, gum.BayesNet):
    raise TypeError(f"The parameter bn must be a gum.BayesNet but got {type(bn)}")
  if isinstance(expl, Explanation):
    if isinstance(y, int):
      if y < min(expl.keys()) or y > max(expl.keys()):
        raise IndexError(
          f"Target index y={y} is out of bounds; expected {min(expl.keys())} <= y < {max(expl.keys()) + 1}."
        )
    else:
      raise TypeError("`y`must be an integer but got {}".format(y))
    importances = expl.importances[y]

  elif isinstance(expl, dict):
    importances = expl

  else:
    raise TypeError(f"The parameter expl must be either an Explanation object or a dict but got {type(expl)}")

  norm_color = {}
  raw = list(importances.values())
  norm = [float(i) / sum(raw) for i in raw]
  for i, feat in enumerate(list(importances.keys())):
    norm_color[feat] = norm[i]
  cm = plt.get_cmap(cmap)
  g = BN2dot(bn, nodeColor=norm_color, cmapNode=cm)
  gnb.showGraph(g)
