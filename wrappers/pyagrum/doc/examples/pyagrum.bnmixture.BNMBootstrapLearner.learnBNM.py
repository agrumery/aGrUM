# requires: pandas
import pyagrum as gum
import pyagrum.bnmixture as bnm

gum.initRandom(42)
bn = gum.fastBN("A->B->C;A->D")
gum.generateSample(bn, 1000, "data.csv")
lrn = bnm.BNMBootstrapLearner("data.csv", template=bn, N=5)  # 5 bootstrap iterations
mix = lrn.learnBNM()  # a BootstrapMixture
mix.refName
# 'bn0'
mix.names()
# ['bn1', 'bn2', 'bn3', 'bn4', 'bn5']
mix.existsArc("A", "B") + mix.existsArc("B", "A")  # how many bootstrapped BNs link A and B
# 3
