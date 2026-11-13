# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
html = gnb.getInference(bn, evs={"A": 1})
html.startswith("<svg")
# True
