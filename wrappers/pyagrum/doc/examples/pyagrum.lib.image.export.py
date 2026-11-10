# requires: matplotlib, pydot
import os

import pyagrum as gum
import pyagrum.lib.image as gimg

bn = gum.fastBN("A->B->C")
gimg.export(bn, "bn.png")  # the format is given by the extension
os.path.exists("bn.png")
# True
gimg.export(bn).ndim  # without file name, an image as a numpy array
# 3
