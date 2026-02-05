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
from pyagrum.explain._Explanation import Explanation

import numpy as np

import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap, to_rgb
from matplotlib.patches import Patch


def bar(explanation: Explanation, y: int = None, ax: plt.Axes = None, percentage: bool = False) -> plt.axis:
  """
  Plots a horizontal bar chart of the mean absolute SHAP/SHALL values for each feature in the explanation.

  Parameters:
  ----------
  explanation : Explanation
      The explanation object containing the SHAP/SHALL values.
  y : int, optional
      If the values type of the explanation is SHALL, then y is ignored.
      Else it is the class for which to plot the SHAP values (default is None, which plots multi-bar for all classes).
  ax : plt.Axes, optional
      The matplotlib Axes object to plot on (default is None, which creates a new figure).
  percentage: bool
    if True, the importance plot is shown in percent.

  Raises :
  ------
  TypeError : If `explanation` is not an Explanation object or if `y` is not an integer or None.
  IndexError : If `y` is an integer but out of bounds for the explanation keys.
  """

  if not isinstance(explanation, Explanation):
    raise TypeError(f"`explanation` must be an Explanation object but got {type(explanation)}")

  # Determine if The explanation object is a SHALL or SHAP explanation
  if explanation.values_type == "SHAP":
    if not isinstance(y, int) and y is not None:
      raise TypeError(f"`y` must be either a positive integer or None, but got {type(y)}")
    if isinstance(y, int) and (y < min(explanation.keys()) or y > max(explanation.keys())):
      raise IndexError(f"Target index y={y} is out of bounds; expected 0 <= y < {max(explanation.keys()) + 1}.")
  elif explanation.values_type == "SHALL":
    # We force y to be an integer, so we can use the same code after for both explanations
    y = 0
  else:
    raise ValueError(f"Wrong values type, expected SHAP/SHALL but got {explanation.values_type}")

  if ax is None:
    _, ax = plt.subplots(figsize=(6, 4))

  if y is not None:
    importances = explanation.importances[y] if explanation.values_type == "SHAP" else explanation.importances
    columns = [col for col in sorted(importances.keys(), key=importances.get)]
    values = [importances[feat] for feat in columns]
    if percentage:
      total = sum(values)
      values = [(v / total) * 100 for v in values]
    ax.barh(columns, values, color=gum.config["notebook", "tensor_color_0"], height=0.5, alpha=0.8)
  else:
    classes = sorted(explanation.keys())
    cmap = LinearSegmentedColormap.from_list(
      "class_cmap", [to_rgb(gum.config["notebook", "tensor_color_0"]), to_rgb(gum.config["notebook", "tensor_color_1"])]
    )
    colors = [cmap(i / (len(explanation) - 1)) for i in range(len(explanation))]

    n_features = len(explanation.feature_names)
    values = np.array([[explanation.importances[z][feat] for feat in explanation.feature_names] for z in classes])
    # Sort bars
    indices = np.argsort(np.sum(values, axis=0))
    values = values[:, indices]
    features = [explanation.feature_names[i] for i in indices]
    bottom = np.zeros(n_features)

    for i, cls in enumerate(classes):
      contribs = values[i]
      if percentage:
        total = sum(contribs)
        contribs = [(v / total) * 100 for v in contribs]
      ax.barh(
        features, contribs, height=0.5, left=bottom, color=colors[i % len(colors)], label=f"class {cls}", alpha=0.8
      )

      bottom += contribs
    legend_elements = [Patch(facecolor=colors[i], edgecolor="black", label=f"Class {i}") for i in range(len(colors))]
    ax.legend(loc="lower right", handles=legend_elements, title="Classes")

  ax.set_title("Feature Importance", fontsize=16)

  msg = " in %" if percentage else ""
  ax.set_xlabel(f"mean(|{explanation.values_type} value|){msg}", fontsize=12)
  ax.set_ylabel("Features", fontsize=12)
  ax.tick_params(axis="x", labelsize=10)
  ax.tick_params(axis="y", labelsize=10)

  # Removing spines
  ax.grid(axis="x", linestyle=":", alpha=0.6)
  ax.grid(axis="y", linestyle=":", alpha=0.3)
  ax.spines["top"].set_visible(False)
  ax.spines["bottom"].set_visible(False)
  ax.spines["left"].set_visible(False)
  ax.spines["right"].set_visible(False)
  ax.figure.set_facecolor("white")
