# Les imports
import pyagrum as gum

import numpy as np
import pandas as pd

import matplotlib.pyplot as plt
import seaborn as sns

def bar(
    shap_values: np.ndarray, 
    feature_names: list,
    ax=None
):
    sns.set(style="whitegrid", context="notebook")

    if isinstance(shap_values, pd.DataFrame):
        shap_values = shap_values.to_numpy()
    elif isinstance(shap_values, list):
        shap_values = np.array(shap_values)
    elif not isinstance(shap_values, np.ndarray):
        raise TypeError("shap_values must be a DataFrame, a list or a numpy array")

    if shap_values.ndim != 2:
        raise ValueError("shap_values must be a 2D array")
    
    if feature_names is not None:
        if isinstance(feature_names, list):
            feature_names = np.array(feature_names)
        elif not isinstance(feature_names, np.ndarray):
            raise TypeError("feature_names must be a list or a numpy array")
        
        if shap_values.shape[1] != len(feature_names):
            raise ValueError("shap_values and feature_names must have the same number of columns")
    else:
        feature_names = [f"Feature {i}" for i in range(shap_values.shape[1])]

    mean_abs_shap_values = np.mean(np.abs(shap_values), axis=0)
    
    importance_df = pd.DataFrame({
        'Feature': feature_names,
        'Mean_Abs_SHAP': mean_abs_shap_values
    }).sort_values(by='Mean_Abs_SHAP', ascending=False, ignore_index=True)

    if ax is None:
        _, ax = plt.subplots(figsize=(10, 6))

    sns.barplot(
        x='Mean_Abs_SHAP',
        y='Feature',
        data=importance_df,
        width=0.5,
        color=gum.config["notebook", "tensor_color_0"],
        ax=ax
    )

    for _, row in importance_df.iterrows():
        ax.text(
            row['Mean_Abs_SHAP'] + 0.01,
            row['Feature'],
            f'+{row["Mean_Abs_SHAP"]:.2f}',
            color=gum.config["notebook", "tensor_color_0"],
            ha="left", va="center", fontsize=10
        )

    ax.set_title('Feature Importance', fontsize=16)
    ax.set_xlabel('mean(|SHAP value|)', fontsize=12)
    ax.set_ylabel('Features', fontsize=12)
    ax.tick_params(axis='x', labelsize=10)
    ax.tick_params(axis='y', labelsize=10)
    ax.grid(axis='x', linestyle=':', alpha=0.6)
    ax.grid(axis='y', linestyle=':', alpha=0.3)
    sns.despine(left=True, bottom=True)