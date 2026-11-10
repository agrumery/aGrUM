import pyagrum as gum
import pyagrum.bnmixture as bnm

gum.initRandom(42)
bn = gum.fastBN("A->B->C;A->D")
gum.generateSample(bn, 1000, "data.csv")
lrn = bnm.BNMBootstrapLearner("data.csv", template=bn, N=5)
lrn.useGreedyHillClimbing()
learner = gum.BNLearner("data.csv")
learner.useMIIC()
lrn.updateState(learner)  # no argument: copies the state of the reference learner
learner.isConstraintBased()
# False
lrn.updateState(learner, algorithm="Greedy Hill Climbing", score="BIC")
[line for line in str(learner).splitlines() if line.startswith(("Algorithm", "Score"))]
# ['Algorithm      : Greedy Hill Climbing', 'Score          : BIC']
