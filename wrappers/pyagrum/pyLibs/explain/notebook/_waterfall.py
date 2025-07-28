# Les imports
import pyagrum as gum
from pyagrum.explain._Explanation import Explanation

import numpy as np

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

def waterfall(explanation: Explanation,
              y: int,
              ax: plt.axis= None) :
    if not isinstance(explanation, Explanation) :
        raise TypeError("`explanation` must be an Explanation object but got {}".format(type(explanation)))
    if isinstance(y, int) :
        if y < min(explanation.keys()) or y > max(explanation.keys()) :
            raise IndexError(f"Target index y={y} is out of bounds; expected 0 <= y < {max(explanation.keys()) + 1}.")
    else :
        raise TypeError("`y`must be an integer but got {}".format(y))
    
    shap_values= explanation[y]
    arrow_width_base= 0.08 * np.max( np.abs(np.array(list(shap_values.values()))) )

    # Tri des SHAP values par importance décroissante
    features = [feature for feature in sorted(shap_values.keys(), key=lambda x: abs(shap_values.get(x)), reverse=True)]
    y_positions = np.arange(len(shap_values) * 0.25, 0, -0.25)
    if ax is None :
        _, ax = plt.subplots(figsize=(5, 5))

    # Ligne de base :
    ax.plot([explanation.baseline[y], explanation.baseline[y]], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle='--', color='gray')
    if explanation.func == "_logit" :
        ax.text(explanation.baseline[y], y_positions[0] + 0.5, f'logit($p(y = {y} \\mid \\emptyset) =$) {explanation.baseline[y]:.2f}', ha='center', va='bottom', color='gray')
    else :
        ax.text(explanation.baseline[y], y_positions[0] + 0.5, f'$p(y = {y} \\mid \\emptyset) =$ {explanation.baseline[y]:.2f}', ha='center', va='bottom', color='gray')

    # Lignes de shapes-values
    current_x = min_x = max_x = explanation.baseline[y]

    for i, feature in enumerate(features) :
        delta = shap_values[feature]
        x_start = current_x
        x_end = current_x + delta
        z = y_positions[i]
        height = 0.2
        arrow_width = min(0.4 * abs(delta), arrow_width_base)
        facecolor, edgecolor, alpha = (gum.config["notebook", "tensor_color_0"], '#D98383', -1) if shap_values[feature] > 0 else (gum.config["notebook", "tensor_color_1"], '#82D882', 1)

        # Dessin du polygon
        polygon = Polygon([
            (x_end + alpha * arrow_width, z - height / 2),
            (x_start, z - height / 2),
            (x_start, z + height / 2),
            (x_end + alpha * arrow_width, z + height / 2),
            (x_end, z)  # pointe
        ], closed=True, facecolor=facecolor, edgecolor=edgecolor, alpha=0.8, linewidth=2)

        ax.add_patch(polygon)

        current_x = x_end
        min_x = min(min_x, current_x)
        max_x = max(max_x, current_x)

    # Ligne de sortie du modèle
    ax.plot([current_x, current_x], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle='--', color='Black')
    if explanation.func == "_logit" :
        ax.text(current_x, y_positions[-1] - 0.5, f'logit($p(y = {y} \\mid x)) =$ {current_x:.2f}', ha='center', va='bottom', color='Black')
    else :
        ax.text(current_x, y_positions[-1] - 0.5, f'$p(y = {y} \\mid x) =$ {current_x:.2f}', ha='center', va='bottom', color='Black')

    y_tickslabels = []
    for feature in features :
        y_tickslabels.append(f"{feature} = {explanation.data[explanation.feature_names.index(feature)]} $[{shap_values[feature]:.2f}]$")
    ax.set_yticks(y_positions)
    ax.set_yticklabels(y_tickslabels)

    ax.grid(axis='x', linestyle=':', alpha=0.5)
    ax.grid(axis='y', alpha=0.5)

    # Removing spines
    ax.spines['top'].set_visible(False)
    ax.spines['bottom'].set_visible(False)
    ax.spines['left'].set_visible(False)
    ax.spines['right'].set_visible(False)

    plt.ylim(min(y_positions) - 1, max(y_positions) + 1)

    delta = max_x - min_x 
    plt.xlim(min_x - 0.05 * delta, max_x + 0.05 * delta)

    plt.show()