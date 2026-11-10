import pyagrum as gum

gum.initRandom(42)
cn = gum.CredalNet(gum.fastBN("A->B"))
cn.bnToCredal(0.8, False)
eps = cn.epsilonMean()
0 < eps < 1
# True
cn.epsilonMin() <= cn.epsilonMean() <= cn.epsilonMax()
# True
