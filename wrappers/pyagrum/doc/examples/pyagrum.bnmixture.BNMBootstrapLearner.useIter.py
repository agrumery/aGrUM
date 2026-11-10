import pyagrum as gum
import pyagrum.bnmixture as bnm

gum.initRandom(42)
bn = gum.fastBN("A->B->C;A->D")
gum.generateSample(bn, 1000, "data.csv")
lrn = bnm.BNMBootstrapLearner("data.csv", template=bn, N=5)  # 5 bootstrap iterations
lrn.useIter(3)
mix = lrn.learnBNM()
mix.size()
# 3
