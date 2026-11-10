import pyagrum as gum

gum.initRandom(42)
mrf = gum.fastMRF("A--B;B--C")
before = [mrf.factor(f).tolist() for f in mrf.factors()]
mrf.generateFactors()
[mrf.factor(f).tolist() for f in mrf.factors()] != before
# True
