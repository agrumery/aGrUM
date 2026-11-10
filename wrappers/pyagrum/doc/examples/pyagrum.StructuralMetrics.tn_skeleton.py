import pyagrum as gum

ref = gum.fastDAG("A->B->C;A->D")
test = gum.fastDAG("A->B;C->B;A->D")  # B->C is reversed
sm = gum.StructuralMetrics()
sm.compare(ref, test)
sm.tn()
# 9.0
sm.tn_skeleton()  # unordered pairs without edge in both skeletons
# 3.0
