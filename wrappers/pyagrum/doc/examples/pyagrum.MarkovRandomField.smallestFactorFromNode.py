import pyagrum as gum

mrf = gum.fastMRF("A--B--C;C--D")
mrf.smallestFactorFromNode(mrf.idFromName("C"))
# {2, 3}
