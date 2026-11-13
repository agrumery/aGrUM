# requires: pandas
import pyagrum as gum
import pyagrum.bnmixture as bnm

gum.initRandom(42)
bn = gum.fastBN("A->B->C")
gum.generateSample(bn, 1000, "data1.csv")
gum.generateSample(gum.fastBN("A->B;A->C"), 1000, "data2.csv")
lrn = bnm.BNMLearner([0.7, 0.3], ["data1.csv", "data2.csv"], template=bn)
mix = lrn.learnBNM()  # a BNMixture
mix.names()
# ['bn0', 'bn1']
mix.weights()
# {'bn0': 0.7, 'bn1': 0.3}
