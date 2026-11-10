import pyagrum as gum

gum.initRandom(42)
gen = gum.IDGenerator()
diag = gen.generate(10, 0.3, 0.6, 0.2, 3)
diag.size()
# 10
diag.decisionOrderExists()
# True
