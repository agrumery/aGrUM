# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
gnb.showInference(bn, evs={"A": 1}, targets={"C"})
