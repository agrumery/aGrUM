# requires: matplotlib, pydot
import os

import pyagrum as gum
import pyagrum.lib.image as gimg

bn = gum.fastBN("A->B->C")
gimg.exportInference(bn, "inference.png", evs={"A": 1})
os.path.exists("inference.png")
# True
