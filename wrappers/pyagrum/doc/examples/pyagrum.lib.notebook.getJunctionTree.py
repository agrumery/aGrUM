# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
html = gnb.getJunctionTree(bn)
html.startswith("<svg")
# True
