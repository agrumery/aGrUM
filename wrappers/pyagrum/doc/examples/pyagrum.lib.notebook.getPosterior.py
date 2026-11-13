# requires: IPython
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
html = gnb.getPosterior(bn, evs={"A": 1}, target="C")  # the histogram as an HTML image
html.startswith("<img")
# True
