# requires: IPython, matplotlib, pydot
import pyagrum as gum
import pyagrum.lib.notebook as gnb

cm = gum.CausalModel(gum.fastBN("X->Y->Z"))
html = gnb.getCausalImpact(cm, on="Z", doing="X")  # an IPython HTML object: formula, value and explanation
"backdoor" in html.data
# True
