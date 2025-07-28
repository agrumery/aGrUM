# Imports
import pyagrum as gum
from pyagrum.explain._Explanation import Explanation

import numpy as np
import pandas as pd
import random

import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib import colors

def beeswarm(
    explanation: Explanation,
    y: int,
    max_display=20,
    color_bar=True,
    ax=None,
    sort=True
    ) :
    # Check parameters ...
    if not isinstance(explanation, Explanation) :
        raise TypeError("`explanation` must be an Explanation object but got {}".format(type(explanation)))
    if not isinstance(y, int) :
        raise TypeError("`y` must be an integer but got {}".format(type(y)))
    if y < min(explanation.keys()) or y > max(explanation.keys()) :
        raise IndexError(f"Target index y={y} is out of bounds; expected 0 <= y < {max(explanation.keys()) + 1}.")
    
    contributions = explanation[y]
    feature_names = explanation.feature_names
    if not isinstance(list(contributions.values())[0], list) :
        raise TypeError("For beeswarm plot, explanation must be global.")
    values = np.array([contributions[k] for k in feature_names]).T
    features = explanation.data

    # Set the style and create the figure
    if ax == None :
        _, ax = plt.subplots(figsize=(6, 4))
    y_positions = np.arange( min(max_display, len(feature_names)), 0, -1 )

    # Plot the beeswarm
    ax.plot([0, 0], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle='--', color='gray')
    color1 = gum.config["notebook", "tensor_color_0"]
    color2 = gum.config["notebook", "tensor_color_1"]
    cmap = colors.LinearSegmentedColormap.from_list("custom_red_green", [color1, color2])

    if sort :
        indices = [feature_names.index(feat) for feat in sorted(explanation.importances[y], key=explanation.importances[y].get, reverse=True)]
    else :
        indices = np.arange(min(max_display, values.shape[1]))
        
    for k, j in enumerate(indices) :
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
    ax.grid(axis='x', linestyle=':', alpha=0.5)
    ax.grid(axis='y', linestyle=':', alpha=0.5)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)

    if color_bar:
        norm = colors.Normalize(vmin=0., vmax=1.)
        sm = cm.ScalarMappable(cmap=cmap, norm=norm)
        sm.set_array([])
        cbar = plt.colorbar(sm, ax=ax)
        cbar.set_label("Feature value")
        cbar.set_ticks([0, 1])
        cbar.set_ticklabels(['Low', 'High'])

    ax.set_ylim(y_positions[-1] - 0.5, y_positions[0] + 0.5)
    ax.set_xlabel("Impact on model Output", fontsize=12)
    ax.set_ylabel("Features", fontsize=12)
    ax.set_title("Shapley value (Impact on model Output)", fontsize=16)

    # Removing spines
    ax.spines['top'].set_visible(False)
    ax.spines['bottom'].set_visible(False)
    ax.spines['left'].set_visible(False)
    ax.spines['right'].set_visible(False)