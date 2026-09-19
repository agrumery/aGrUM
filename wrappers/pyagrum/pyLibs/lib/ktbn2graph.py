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

"""
The purpose of this module is to provide tools for mapping KTBN models (and inference) in order to be displayed/saved as image.
"""

import pyagrum


def plotFollowKTBN(
  m: pyagrum.KTBN, lovars: list[str], *, T: int, observations: dict = None, interventions: dict = None
):
  """
  Plots the evolution of the posterior distribution of a set of variables in a KTBN model.

  Parameters
  ----------
  m : gum.KTBN
      The KTBN model.
  lovars : list
      List of variable names to follow.
  T : int
      The number of time steps.
  observations : dict
      A dictionary of observations, where keys are variable names and values are the observed values.
  interventions: dict
      A dictionary of interventions, where keys are variable names and values are the intervened values.
  """

  # lazy import
  import math
  import numpy as np
  import pyagrum.ktbn as ktbn
  import matplotlib.pyplot as plt
  from matplotlib.patches import Rectangle

  kie = ktbn.KTBNInference(m)
  if observations is not None:
    for node, val in observations.items():
      kie.addObservation(node, val)
  if interventions is not None:
    for node, val in interventions.items():
      kie.addIntervention(node, val)

  for var in lovars:
    kie.addTarget(var)
  kie.makeInference(T)

  x = np.arange(T)

  # arrange subplots as a near-square grid rather than one figure per variable
  n = len(lovars)
  ncols = math.ceil(math.sqrt(n))
  nrows = math.ceil(n / ncols)
  fig, axes = plt.subplots(nrows, ncols, figsize=(6 * ncols, 4 * nrows), squeeze=False)
  flat_axes = axes.flatten()

  for ax, var in zip(flat_axes, lovars):
    v0 = m.variable(var, 0)
    series = kie.posteriors(var)
    lpots = [[p.tolist()[i] for p in series] for i in range(v0.domainSize())]

    ax.set_xlim(left=0, right=T - 1)
    ax.set_ylim(top=1, bottom=0)
    ax.xaxis.grid()
    ax.set_title(f"Following variable {var}", fontsize=20)
    ax.set_xlabel("time")

    stack = ax.stackplot(x, lpots)
    proxy_rects = [Rectangle((0, 0), 1, 1, fc=pc.get_facecolor()[0]) for pc in stack]
    labels = [v0.label(i) for i in range(v0.domainSize())]
    ax.legend(proxy_rects, labels, loc="center left", bbox_to_anchor=(1, 0.5), ncol=1, fancybox=True, shadow=True)

  for ax in flat_axes[n:]:
    ax.set_visible(False)

  fig.tight_layout()
  plt.show()
