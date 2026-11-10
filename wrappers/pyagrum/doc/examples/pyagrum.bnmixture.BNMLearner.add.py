import pyagrum as gum
import pyagrum.bnmixture as bnm

gum.initRandom(42)
bn = gum.fastBN("A->B->C")
gum.generateSample(bn, 1000, "data1.csv")
gum.generateSample(gum.fastBN("A->B;A->C"), 1000, "data2.csv")
lrn = bnm.BNMLearner([0.7, 0.3], ["data1.csv", "data2.csv"], template=bn)
gum.generateSample(gum.fastBN("C->B->A"), 1000, "data3.csv")
lrn.add("data3.csv", 0.5, algorithm="MIIC")  # with its own learning parameters
mix = lrn.learnBNM()
mix.size()
# 3
