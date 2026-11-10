import pyagrum as gum
import pyagrum.bnmixture as bnm

gum.initRandom(42)
bn = gum.fastBN("A->B->C")
gum.generateSample(bn, 1000, "data1.csv")
gum.generateSample(gum.fastBN("A->B;A->C"), 1000, "data2.csv")
lrn = bnm.BNMLearner([0.7, 0.3], ["data1.csv", "data2.csv"], template=bn)
learner = gum.BNLearner("data1.csv")
lrn.updateState(learner, algorithm="MIIC", correction="NML")
learner.isConstraintBased()
# True
[line for line in str(learner).splitlines() if line.startswith(("Algorithm", "Correction"))]
# ['Algorithm      : MIIC', 'Correction     : NML']
