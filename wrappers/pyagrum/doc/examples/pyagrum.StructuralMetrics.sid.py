import pyagrum as gum

ref = gum.fastDAG("A->B->C")
test = gum.fastDAG("A->B;A->C;B->C")  # a supergraph of ref
sm = gum.StructuralMetrics()
sm.sid(ref, test)
# 0.0
sm.sid(test, ref)  # SID is not symmetric
# 1.0
