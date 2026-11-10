# requires: IPython, matplotlib, pydot
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
gnb.showProba(gum.getPosterior(bn, target="C"))
