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
import matplotlib.cm as cm
from matplotlib import colors


def beeswarm(
  explanation: Explanation, y: int = 1, max_display: int = 20, color_bar: bool = True, ax=None, sort: bool = True
):
  """
  Plots a beeswarm plot of the Shapley values for a given target class.
  Parameters:
  ----------
  explanation : Explanation
      The explanation object containing the SHAP/SHALL values.
  y : int
      If the values type of the explanation is SHALL, then y is ignored.
      Else it is the class for which to plot the SHAP values.
  max_display : int, optional
      The maximum number of features to display in the beeswarm plot (default is 20).
  color_bar : bool, optional
      If True, adds a color bar to the plot (default is True).
  ax : plt.Axes, optional
      The matplotlib Axes object to plot on (default is None, which creates a new figure).
  sort : bool, optional
      If True, sorts the features by their importance before plotting (default is True).

  Raises:
  ------
  TypeError
      If `explanation` is not an Explanation object, if `y` is not an integer or if the explanation is not global (i.e., does not contain lists of contributions for each feature).
  IndexError
      If `y` is out of bounds for the explanation keys.
  """
  # Check parameters
  if not isinstance(explanation, Explanation):
    raise TypeError("`explanation` must be an Explanation object but got {}".format(type(explanation)))

  # Determine if The explanation object is a SHALL or SHAP explanation
  if explanation.values_type == "SHAP":
    if not isinstance(y, int):
      raise TypeError("`y` must be an integer but got {}".format(type(y)))
    if y < min(explanation.keys()) or y > max(explanation.keys()):
      raise IndexError(f"Target index y={y} is out of bounds; expected 0 <= y < {max(explanation.keys()) + 1}.")
    contributions = explanation[y]
    importances = explanation.importances[y]
  elif explanation.values_type == "SHALL":
    contributions = explanation
    importances = explanation.importances
  else:
    raise ValueError(f"Wrong values type, expected SHAP/SHALL but got {explanation.values_type}")

  feature_names = explanation.feature_names
  if not isinstance(list(contributions.values())[0], list):
    raise TypeError("For beeswarm plot, explanation must be global.")
  values = np.array([contributions[k] for k in feature_names]).T
  features = explanation.data

  # Create the figure and axis if not provided
  if ax == None:
    _, ax = plt.subplots()

  # Prepare the y-axis positions
  y_positions = np.arange(min(max_display, len(feature_names)), 0, -1)

  # Plot the beeswarm
  ax.plot([0, 0], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle="--", color="gray")
  color1 = gum.config["notebook", "tensor_color_0"]
  color2 = gum.config["notebook", "tensor_color_1"]
  cmap = colors.LinearSegmentedColormap.from_list("custom_red_green", [color1, color2])

  if sort:
    indices = [feature_names.index(feat) for feat in sorted(importances, key=importances.get, reverse=True)]
  else:
    indices = np.arange(min(max_display, values.shape[1]))

  for k, j in enumerate(indices):
    base = y_positions[k]
    sequence = np.arange(values.shape[0])
    np.random.shuffle(sequence)

    shapes = values[sequence, j]
    rounded_x = np.round(shapes, 2)
    (unique, counts) = np.unique(rounded_x, return_counts=True)
    density_map = dict(zip(unique, counts))
    densities = np.array([density_map[val] for val in rounded_x])

    sigmas = (densities / np.max(densities)) * 0.1
    ords = np.random.normal(loc=base, scale=sigmas)

    vals = features[sequence, j]

    minimum = vals.min()
    maximum = vals.max()
    norm = colors.Normalize(vmin=minimum, vmax=maximum)
    sm = cm.ScalarMappable(cmap=cmap, norm=norm)
    sm.set_array([])

    ax.scatter(
      shapes,
      ords,
      c=vals,
      cmap=cmap,
      s=7,
    )

  ax.set_yticks(y_positions)
  ax.set_yticklabels([feature_names[i] for i in indices])
  if color_bar:
    norm = colors.Normalize(vmin=0.0, vmax=1.0)
    sm = cm.ScalarMappable(cmap=cmap, norm=norm)
    sm.set_array([])
    cbar = plt.colorbar(sm, ax=ax)
    cbar.set_label("Feature value")
    cbar.set_ticks([0, 1])
    cbar.set_ticklabels(["Low", "High"])

  ax.set_ylim(y_positions[-1] - 0.5, y_positions[0] + 0.5)
  ax.set_xlabel("Impact on model Output", fontsize=12)
  ax.set_ylabel("Features", fontsize=12)
  ax.set_title(f"{explanation.values_type} value (Impact on model Output)", fontsize=16)

  # Setting the style
  ax.grid(axis="x", linestyle=":", alpha=0.5)
  ax.grid(axis="y", linestyle=":", alpha=0.5)
  ax.spines["top"].set_visible(False)
  ax.spines["bottom"].set_visible(False)
  ax.spines["left"].set_visible(False)
  ax.spines["right"].set_visible(False)
  ax.figure.set_facecolor("white")
