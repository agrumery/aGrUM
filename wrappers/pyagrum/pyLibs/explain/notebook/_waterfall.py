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
from typing import Callable, Dict
import numpy as np

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon


def _POSTERIOR(y: int, func: str) -> str:
  return f"logit($p(y={y} \\mid x)$)" if func == "_logit" else f"$p(y={y} \\mid x)$"


def _JOIN(func: str) -> str:
  return "log($p(x \\mid \\theta)$)" if func == "_log" else "$p(x \\mid \\theta)$"


def _FMT(func: str) -> str:
  return ".2e" if func == "_identity" else ".2f"


def waterfall(explanation: Explanation, y: int = 1, ax=None, real_values: Dict = None):
  """
  Plots a waterfall chart of the SHAP/SHALL values.

  Parameters:
  ----------
  explanation : Explanation
      The explanation object containing the SHAP/SHALL values.
  y : int, optional
      If the values type of the explanation is SHALL, then y is ignored.
      Else it is the class for which to plot the SHAP values.
  ax : matplotlib.Axes, optional
      The matplotlib Axes object to plot on (default is None, which creates a new figure).
  real_values : Dict, optional
      Dictionary used to display custum values for each feature.
      For example, useful when continuous values have been discretized but you still want to show the original continuous values from the database.
      The keys of the dictionary must match the keys in the Explanation object, and the values are the values you want to display on the plot.

  Raises :
  ------
  TypeError : If `explanation` is not an Explanation object or if `y` is not an integer.
  IndexError : If `y` is an integer but out of bounds for the explanation keys.
  """

  if not isinstance(explanation, Explanation):
    raise TypeError("`explanation` must be an Explanation object but got {}".format(type(explanation)))
  if explanation.values_type == "SHAP":
    if isinstance(y, int):
      if y < min(explanation.keys()) or y > max(explanation.keys()):
        raise IndexError(f"Target index y={y} is out of bounds; expected 0 <= y < {max(explanation.keys()) + 1}.")
    else:
      raise TypeError("`y`must be an integer but got {}".format(y))
    values = explanation[y]
    baseline = explanation.baseline[y]
  elif explanation.values_type == "SHALL":
    values = explanation._values
    baseline = explanation.baseline
  else:
    raise ValueError(f"Wrong values type, expected SHAP/SHALL but got {explanation.values_type}")

  # Computing arrow width
  arrow_width_base = 0.08 * np.max(np.abs(np.array(list(values.values()))))

  # Tri des SHAP values par importance décroissante
  features = [feature for feature in sorted(values.keys(), key=lambda x: abs(values.get(x)), reverse=True)]
  y_positions = np.arange(len(values) * 0.25, 0, -0.25)

  # Create the figure and axis if not provided
  if ax == None:
    _, ax = plt.subplots()

  # Ligne de base :
  ax.plot([baseline, baseline], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle="--", color="gray")
  if explanation.values_type == "SHAP":
    ax.text(
      baseline,
      y_positions[0] + 0.5,
      f"E({_POSTERIOR(y, explanation.func)}) = {baseline:{_FMT(explanation.func)}}",
      ha="center",
      va="bottom",
      color="gray",
    )
  else:
    ax.text(
      baseline,
      y_positions[0] + 0.5,
      f"E({_JOIN(explanation.func)}) = {baseline:{_FMT(explanation.func)}}",
      ha="center",
      va="bottom",
      color="gray",
    )

  # Lignes de shapes-values
  current_x = min_x = max_x = baseline

  for i, feature in enumerate(features):
    delta = values[feature]
    x_start = current_x
    x_end = current_x + delta
    z = y_positions[i]
    height = 0.2
    arrow_width = min(0.4 * abs(delta), arrow_width_base)
    facecolor, edgecolor, alpha = (
      (gum.config["notebook", "tensor_color_0"], "#D98383", 1)
      if values[feature] <= 0
      else (gum.config["notebook", "tensor_color_1"], "#82D882", -1)
    )

    # Dessin du polygon
    polygon = Polygon(
      [
        (x_end + alpha * arrow_width, z - height / 2),
        (x_start, z - height / 2),
        (x_start, z + height / 2),
        (x_end + alpha * arrow_width, z + height / 2),
        (x_end, z),  # pointe
      ],
      closed=True,
      facecolor=facecolor,
      edgecolor=edgecolor,
      alpha=0.8,
      linewidth=2,
    )

    ax.add_patch(polygon)

    current_x = x_end
    min_x = min(min_x, current_x)
    max_x = max(max_x, current_x)

  # Ligne de sortie du modèle
  ax.plot([current_x, current_x], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle="--", color="Black")
  if explanation.func == "_logit":
    ax.text(
      current_x,
      y_positions[-1] - 0.5,
      f"{_POSTERIOR(y, explanation.func)} = {current_x:{_FMT(explanation.func)}}",
      ha="center",
      va="bottom",
      color="Black",
    )
  else:
    ax.text(
      current_x,
      y_positions[-1] - 0.5,
      f"{_JOIN(explanation.func)} = {current_x:{_FMT(explanation.func)}}",
      ha="center",
      va="bottom",
      color="Black",
    )

  y_tickslabels = []

  for feature in features:
    feat_shap_value = values[feature]
    if real_values is not None:
      value = real_values[feature]
      if isinstance(real_values[feature], float):
        value = round(value, 2)
    else:
      value = explanation.data[explanation.feature_names.index(feature)]

    y_tickslabels.append(f"{feature} = {value} [{feat_shap_value:{_FMT(explanation.func)}}]")

  ax.set_yticks(y_positions)
  ax.set_yticklabels(y_tickslabels)

  # Setting the style
  ax.grid(axis="x", linestyle=":", alpha=0.5)
  ax.grid(axis="y", alpha=0.5)
  ax.spines["top"].set_visible(False)
  ax.spines["bottom"].set_visible(False)
  ax.spines["left"].set_visible(False)
  ax.spines["right"].set_visible(False)
  ax.figure.set_facecolor("White")

  plt.ylim(min(y_positions) - 1, max(y_positions) + 1)
  delta = max_x - min_x
  plt.xlim(min_x - 0.05 * delta, max_x + 0.05 * delta)
