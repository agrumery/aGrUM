import pyagrum as gum

ref = gum.fastDAG("A->B->C;A->D")
test = gum.fastDAG("A->B;C->B;A->D")  # B->C is reversed
sm = gum.StructuralMetrics()
sm.compare(ref, test)
sm.shd()
# 1.0
bn1 = gum.fastBN("A->B->C")
bn2 = gum.fastBN("C->B->A")  # same essential graph
sm.compare(bn1, bn2)
sm.shd()
# 0.0
