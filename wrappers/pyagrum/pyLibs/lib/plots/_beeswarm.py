# Les imports
import pyagrum as gum

import numpy as np
import pandas as pd
import random

import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib import colors
import seaborn as sns

# TODO: - Clean up the code, remove unused imports and variables.
#       - Add title to Figure.
# FIXME: When a column is not numeric, the program crashes. We should handle this case.

def beeswarm(
    shap_values,
    features=None,
    feature_names=None,
    ax=None,
    max_display=20,
    color_bar=True,
    sort=True
    ) :

    if isinstance(shap_values, pd.DataFrame) :
        shap_values = shap_values.to_numpy()
    elif isinstance(shap_values, list) :
        shap_values = np.array( shap_values )
    elif isinstance(shap_values, np.ndarray) :
        pass
    else :
        raise TypeError("shap_values must be a DataFrame, a list or a numpy array")
    
    if shap_values.ndim != 2 :
        raise ValueError("shap_values must be a 2D array")
    
    if features is not None :
        if isinstance(features, pd.DataFrame) :
            features = features.to_numpy()
        elif isinstance(features, list) :
            features = np.array( features )
        elif isinstance(features, np.ndarray) :
            pass
        else :
            raise TypeError("features must be a DataFrame, a list or a numpy array")
        
        if features.ndim != 2 :
            raise ValueError("features must be a 2D array")
        if shap_values.shape[0] != features.shape[0] :
            raise ValueError("shap_values and features must have the same number of rows")
    else :
        features = np.zeros(shap_values.shape)
    
    if feature_names is not None :
        if isinstance(feature_names, list) :
            feature_names = np.array(feature_names)
        elif isinstance(feature_names, np.ndarray) :
            pass
        else :
            raise Exception("feature_names must be a list or a numpy array")
        
        if shap_values.shape[1] != len(feature_names) :
            raise Exception("shap_values and feature_names must have the same number of columns")
    else :
        feature_names = [f"Feature {i}" for i in range(shap_values.shape[1])]
            
    sns.set(style="whitegrid", context="notebook")
    _, ax = plt.subplots(figsize=(10, 4)) if ax is None else (None, ax)
    y_positions = np.arange( min(max_display, len(feature_names)), 0, -1 )

    ax.plot([0, 0], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle='--', color='gray')
    color1 = gum.config["notebook", "tensor_color_0"]
    color2 = gum.config["notebook", "tensor_color_1"]
    cmap = colors.LinearSegmentedColormap.from_list("custom_red_green", [color1, color2])

    if sort :
        shap_contributions = np.mean(np.abs(shap_values), axis=0)
        indices = np.argsort(shap_contributions)[::-1][:max_display]
    else :
        indices = np.arange(min(max_display, shap_values.shape[1]))
        

    for k, j in enumerate(indices) :
        base = y_positions[k]
        sequence = np.arange(shap_values.shape[0])
        np.random.shuffle(sequence)

        shapes = shap_values[sequence, j]
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
    ax.set_yticklabels(feature_names[indices])
    ax.grid(axis='x', linestyle=':', alpha=0.5)
    ax.grid(axis='y', linestyle=':', alpha=0.5)
    sns.despine(left=True, bottom=True)

    if color_bar:
        cbar = plt.colorbar(sm, ax=ax)
        cbar.set_label("Feature value")
        cbar.set_ticks([minimum, maximum])
        cbar.set_ticklabels(['Low', 'High'])

    ax.set_ylim(y_positions[-1] - 0.5, y_positions[0] + 0.5)
    ax.set_xlabel("Impact on model Output", fontsize=12)
    ax.set_ylabel("Features", fontsize=12)
    ax.set_title("Shapley value (Impact on model Output)", fontsize=16)