# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
cn = gum.CredalNet(bn, bn)
gnb.showCN(cn)
