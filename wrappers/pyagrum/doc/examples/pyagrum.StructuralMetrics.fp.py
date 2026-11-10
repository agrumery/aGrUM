import pyagrum as gum

ref = gum.fastDAG("A->B->C;A->D")
test = gum.fastDAG("A->B;C->B;A->D")  # B->C is reversed
sm = gum.StructuralMetrics()
sm.compare(ref, test)
sm.fp()  # arcs of test not in ref (the reversed one)
# 1.0
