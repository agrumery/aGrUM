import pyagrum as gum

ref = gum.fastDAG("A->B->C;A->D")
test = gum.fastDAG("A->B;C->B;A->D")  # B->C is reversed
sm = gum.StructuralMetrics()
sm.compare(ref, test)
sm.fn()  # arcs of ref missing in test: the reversed arc is counted as a false positive
# 0.0
