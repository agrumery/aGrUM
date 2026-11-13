# requires: IPython
import pyagrum as gum
import pyagrum.lib.notebook as gnb

cm = gum.CausalModel(gum.fastBN("X->Y->Z"))
gnb.showCausalImpact(cm, on="Z", doing="X")  # formula, value and explanation
