# requires: IPython
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
html = gnb.getTensor(bn.cpt("B"), digits=2)
html.startswith("<table")
# True
