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

import pyagrum
import pyagrum.lib.notebook as gnb
from pyagrum.lib.bn2graph import BN2dot
from pyagrum.explain import Explanation

import matplotlib.pyplot as plt


def showShapValues(bn: pyagrum.BayesNet, expl: Explanation | dict, cmap="plasma", y=1, show_legend=False, filename: str | None = None):
  """
  Show the Shap values in the DAG of the BN

  Nodes with positive importance are colored by the colormap (brighter = more important).
  Nodes with zero importance are rendered with a white background and bold border to signal
  they are not causal players.
  The target node (absent from the explanation) uses the default theme color.

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
  show_legend: bool
      If True, display a colorbar showing the importance scale (default is False).
  filename : str, optional
      If provided and show_legend is True, save the legend figure to this path instead of displaying it (default is None).

  Raises
  ------
  TypeError
      If bn is not a pyagrum.BayesNet, if expl is neither an Explanation nor a dict, or if expl is an Explanation and y is not an integer.
  IndexError
      If expl is an Explanation and y is outside the valid class range.
  """

  if not isinstance(bn, pyagrum.BayesNet):
    raise TypeError(f"The parameter bn must be a pyagrum.BayesNet but got {type(bn)}")
  if isinstance(expl, Explanation):
    if isinstance(y, int):
      if y < min(expl.keys()) or y > max(expl.keys()):
        raise IndexError(
          f"Target index y={y} is out of bounds; expected {min(expl.keys())} <= y < {max(expl.keys()) + 1}."
        )
    else:
      raise TypeError(f"`y`must be an integer but got {y}")
    importances = expl.importances[y]

  elif isinstance(expl, dict):
    importances = expl

  else:
    raise TypeError(f"The parameter expl must be either an Explanation object or a dict but got {type(expl)}")

  import matplotlib.patches as mpatches

  # Only color nodes with positive importance; zero-importance nodes and target keep theme default
  positive = {feat: val for feat, val in importances.items() if val > 0}
  total = sum(positive.values())
  norm_color = {feat: val / total for feat, val in positive.items()} if total > 0 else {}

  cm = plt.get_cmap(cmap)
  g = BN2dot(bn, nodeColor=norm_color, cmapNode=cm)

  # Zero-importance nodes: white bg + bold border to signal "not a causal player"
  zero_names = {feat for feat, val in importances.items() if val == 0}
  for node in g.get_nodes():
    if node.get_name().strip('"').strip("'") in zero_names:
      node.set_style('"filled"')
      node.set_fillcolor('"white"')
      node.set_fontcolor('"black"')
      node.set_penwidth(3)

  # Graph first so legend appears below in the notebook
  gnb.showGraph(g)

  if show_legend:
    default_bg = pyagrum.config["notebook", "default_node_bgcolor"]
    default_fg = pyagrum.config["notebook", "default_node_fgcolor"]

    fig = plt.figure(figsize=(5, 1.0))
    fig.patch.set_alpha(0)

    # Patches for target and zero-importance nodes (above the colorbar)
    ax_leg = fig.add_axes([0.0, 0.50, 1.0, 0.45])
    ax_leg.set_axis_off()
    ax_leg.set_facecolor("none")
    patches = [
      mpatches.Patch(facecolor=default_bg, edgecolor=default_fg, label="Target node"),
      mpatches.Patch(facecolor="white", edgecolor="black", linewidth=2, label="Zero importance"),
    ]
    ax_leg.legend(handles=patches, loc="center", ncol=2, framealpha=0)

    # Colorbar (below the patches)
    ax_cb = fig.add_axes([0.05, 0.05, 0.9, 0.38])
    ax_cb.set_facecolor("none")
    plt.colorbar(
      plt.cm.ScalarMappable(norm=plt.Normalize(0, 1), cmap=cm),
      cax=ax_cb,
      orientation="horizontal",
      label="Relative importance",
    )

    if filename is not None:
      fig.savefig(filename)
      plt.close(fig)
    else:
      plt.show()
