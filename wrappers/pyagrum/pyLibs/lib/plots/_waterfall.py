# Les imports
import pyagrum as gum

import numpy as np

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import seaborn as sns

def waterfall(base, shap_values, feature_values, feature_names, target_name, target_value, function) :
    sns.set(style="whitegrid", context="notebook")

    # Tri des SHAP values par importance décroissante
    indices = np.argsort(np.abs(shap_values))[::-1]
    shap_values = shap_values[indices]
    y_positions = np.arange(len(shap_values) * 0.5, 0, -0.5)
    fig, ax = plt.subplots(figsize=(12, 6))

    # Ligne de base :
    ax.plot([base, base], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle='--', color='gray')
    if function == "_logit" :
        ax.text(base, y_positions[0] + 0.5, f'logit($p({target_name} = {target_value} \\mid \\emptyset) =$) {base:.2f}', ha='center', va='bottom', color='gray')
    else :
        ax.text(base, y_positions[0] + 0.5, f'$p({target_name} = {target_value} \\mid \\emptyset) =$ {base:.2f}', ha='center', va='bottom', color='gray')

    # Lignes de shapes-values
    current_x = min_x = max_x = base
    for i in range(len(shap_values)):
        delta = shap_values[i]
        if abs(delta) <= 0.001 :
            continue
        x_start = current_x
        x_end = current_x + delta
        y = y_positions[i]
        height = 0.4
        arrow_width = min(0.1 * abs(delta), 0.01)
        color, alpha = (gum.config["notebook", "tensor_color_0"], -1) if shap_values[i] > 0 else (gum.config["notebook", "tensor_color_1"], 1)

        # Dessin du polygon
        polygon = Polygon([
            (x_end + alpha * arrow_width, y - height / 2),
            (x_start, y - height / 2),
            (x_start, y + height / 2),
            (x_end + alpha * arrow_width, y + height / 2),
            (x_end, y)  # pointe
        ], closed=True, color=color)

        ax.add_patch(polygon)

        # Texte de la valeur SHAP
        ax.text(x_start + delta / 2, y_positions[i],
                f'{delta:.2f}', ha='center', va='center',
                color='white' if abs(delta) > 0.1 else 'black', fontsize=9)

        current_x = x_end
        min_x = min(min_x, current_x)
        max_x = max(max_x, current_x)

    # Ligne de sortie du modèle
    ax.plot([current_x, current_x], [y_positions[-1] - 0.25, y_positions[0] + 0.25], linestyle='--', color='Black')
    if function == "_logit" :
        ax.text(current_x, y_positions[-1] - 0.75, f'logit($p({target_name} = {target_value} \\mid x)) =$ {current_x:.2f}', ha='center', va='bottom', color='Black')
    else :
        ax.text(current_x, y_positions[-1] - 0.75, f'$p({target_name} = {target_value} \\mid x) =$ {current_x:.2f}', ha='center', va='bottom', color='Black')

    y_tickslabels = []
    for i in indices :
        y_tickslabels.append(f"{feature_names[i]} = {feature_values[i]}")
    ax.set_yticks(y_positions)
    ax.set_yticklabels(y_tickslabels)

    ax.grid(axis='x', linestyle=':', alpha=0.5)
    sns.despine(left=True, bottom=True)

    plt.ylim(min(y_positions) - 1, max(y_positions) + 1)

    delta = max_x - min_x 
    plt.xlim(min_x - 0.1 * delta, max_x + 0.1 * delta)

    plt.show()