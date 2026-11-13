# requires: IPython
import pyagrum as gum
import pyagrum.lib.notebook as gnb

bn = gum.fastBN("A->B->C")
gnb.sideBySide(bn, bn.cpt("B"), captions=["the BN", "the CPT of B"])
