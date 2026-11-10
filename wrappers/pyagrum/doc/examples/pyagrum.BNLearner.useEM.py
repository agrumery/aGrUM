import pyagrum as gum

with open("data.csv", "w") as f:  # "?" is a missing value
  f.write("A,B\n" + "0,0\n0,1\n1,1\n1,1\n?,0\n1,?\n" * 20)
learner = gum.BNLearner("data.csv")
learner.useEM(1e-4, 0.0)  # rate criterion, no random noise at initialization
bn = learner.learnParameters(gum.fastDAG("A->B"))
print(bn.cpt("B"))
#
#       ║  B                │
# A     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.6667  │ 0.3333  │
# 1     ║ 0.0000  │ 1.0000  │
