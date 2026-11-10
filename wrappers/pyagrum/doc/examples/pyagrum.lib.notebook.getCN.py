# requires: IPython, matplotlib, pydot, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
cn = gum.CredalNet(bn, bn)
html = gnb.getCN(cn)
html.startswith("<svg")
# True
