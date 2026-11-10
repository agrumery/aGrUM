import pyagrum as gum

mr = gum.MeekRules()
dag = mr.propagateToDAG(gum.fastMixedGraph("A-B-C"))  # no rule applies: arbitrary orientations
len(mr.choices())  # the edges oriented arbitrarily
# 2
mr.propagateToDAG(gum.fastMixedGraph("A->B-C"))  # rule R1 orients B->C
mr.choices()
# []
