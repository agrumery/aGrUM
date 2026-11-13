# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
html = gnb.getBN(bn, size="3")
html.startswith("<svg")
# True
