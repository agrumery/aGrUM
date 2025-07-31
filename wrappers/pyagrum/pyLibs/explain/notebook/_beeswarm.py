# Imports
import pyagrum as gum
from pyagrum.explain._Explanation import Explanation

import numpy as np

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

    """
    Plots a beeswarm plot of the Shapley values for a given target class.
    Parameters:
    ----------
    explanation : Explanation
        The explanation object containing the SHAP/SHALL values.
    y : int
        The target class for which to plot the SHAP/SHALL values.
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
    if not isinstance(explanation, Explanation) :
        raise TypeError("`explanation` must be an Explanation object but got {}".format(type(explanation)))
    
    # Determine if The explanation object is a SHALL or SHAP explanation
    if explanation.values_type == 'SHAP' :
        if not isinstance(y, int) :
            raise TypeError("`y` must be an integer but got {}".format(type(y)))
        if y < min(explanation.keys()) or y > max(explanation.keys()) :
            raise IndexError(f"Target index y={y} is out of bounds; expected 0 <= y < {max(explanation.keys()) + 1}.")
        contributions = explanation[y]
        importances = explanation.importances[y]
    elif explanation.values_type == 'SHALL' :
        contributions = explanation
        importances = explanation.importances
    else :
        raise ValueError(f"Wrong values type, expected SHAP/SHALL but got {explanation.values_type}")

    feature_names = explanation.feature_names
    if not isinstance(list(contributions.values())[0], list) :
        raise TypeError("For beeswarm plot, explanation must be global.")
    values = np.array([contributions[k] for k in feature_names]).T
    features = explanation.data

    # Create the figure and axis if not provided
    if ax == None :
        fig, ax = plt.subplots()
    
    # Prepare the y-axis positions
    y_positions = np.arange( min(max_display, len(feature_names)), 0, -1 )

    # Plot the beeswarm
    ax.plot([0, 0], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle='--', color='gray')
    color1 = gum.config["notebook", "tensor_color_0"]
    color2 = gum.config["notebook", "tensor_color_1"]
    cmap = colors.LinearSegmentedColormap.from_list("custom_red_green", [color1, color2])

    if sort :
        indices = [feature_names.index(feat) for feat in sorted(importances, key=importances.get, reverse=True)]
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
    ax.set_title(f"{explanation.values_type} value (Impact on model Output)", fontsize=16)

    # Setting the style
    ax.grid(axis='x', linestyle=':', alpha=0.5)
    ax.grid(axis='y', linestyle=':', alpha=0.5)
    ax.spines['top'].set_visible(False)
    ax.spines['bottom'].set_visible(False)
    ax.spines['left'].set_visible(False)
    ax.spines['right'].set_visible(False)
    fig.patch.set_facecolor('White')