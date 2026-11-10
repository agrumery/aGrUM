import pyagrum as gum

ref = gum.fastDAG("A->B->C;A->D")
test = gum.fastDAG("A->B;C->B;A->D")  # B->C is reversed
sm = gum.StructuralMetrics()
sm.compare(ref, test)
sm.tp()  # A->B and A->D
# 2.0
