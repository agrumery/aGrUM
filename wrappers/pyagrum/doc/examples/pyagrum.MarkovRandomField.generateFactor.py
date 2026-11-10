import pyagrum as gum

gum.initRandom(42)
mrf = gum.fastMRF("A--B;B--C")
before = mrf.factor(["A", "B"]).tolist()
mrf.generateFactor({0, 1})
mrf.factor(["A", "B"]).tolist() != before
# True
