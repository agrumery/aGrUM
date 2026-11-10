# requires: IPython, matplotlib, pydot
import pyagrum as gum
import pyagrum.lib.notebook as gnb

cm = gum.CausalModel(gum.fastBN("X->Y->Z"))
gnb.showCausalModel(cm)
