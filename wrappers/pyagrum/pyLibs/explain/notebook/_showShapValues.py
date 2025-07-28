import pyagrum as gum
import pyagrum.lib.notebook as gnb
from pyagrum.lib.bn2graph import BN2dot
from pyagrum.explain import Explanation

import matplotlib.pyplot as plt

def showShapValues(bn: gum.BayesNet, expl: Explanation | dict, cmap="plasma", y=1) :
    """
    Show the Shap values in the DAG of the BN
    Parameters
    ----------
    bn : pyagrum.BayesNet
    The Bayesian network
    expl: Explanation | dict[str,float]
    The Shap values to each variable
    cmap: Matplotlib.ColorMap
    The colormap used for coloring the nodes
    """

    if not isinstance(bn, gum.BayesNet) :
        raise TypeError(f'The parameter bn must be a gum.BayesNet but got {type(bn)}')
    if isinstance(expl, Explanation) :
        if isinstance(y, int) :
            if y < min(expl.keys()) or y > max(expl.keys()) :
                raise IndexError(f"Target index y={y} is out of bounds; expected {min(expl.keys())} <= y < {max(expl.keys()) + 1}.")
        else :
            raise TypeError("`y`must be an integer but got {}".format(y))
        importances = expl.importances[y]

    elif isinstance(expl, dict) :
        importances = expl
    
    else :
        raise TypeError(f'The parameter expl must be either an Explanation object or a dict but got {type(expl)}')

    norm_color = {}
    raw = list(importances.values())
    norm = [float(i) / sum(raw) for i in raw]
    for i, feat in enumerate(list(importances.keys())):
        norm_color[feat] = norm[i]
    cm = plt.get_cmap(cmap)
    g = BN2dot(bn, nodeColor=norm_color, cmapNode=cm)
    gnb.showGraph(g)