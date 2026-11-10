import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.exists(mrf.idFromName("C"))
# True
mrf.exists(42)
# False
