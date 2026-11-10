import pyagrum as gum

gum.initRandom(42)
bn = gum.fastBN("A->B")
cn = gum.CredalNet(bn)
cn.bnToCredal(0.8, False)
cn.isSeparatelySpecified()
# True
cn.epsilonMax() > 0
# True
