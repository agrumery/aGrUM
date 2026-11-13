# requires: IPython
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
gnb.showPosterior(bn, evs={"A": 1}, target="C")
