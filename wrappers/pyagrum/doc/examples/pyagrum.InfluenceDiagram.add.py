import pyagrum as gum

diag = gum.InfluenceDiagram()
diag.add(gum.LabelizedVariable("C", "chance", 2))
# 0
diag.add("*D{go|stay}")  # a decision node in fast syntax
# 1
diag.add("$U")  # a utility node
# 2
diag.isDecisionNode("D"), diag.isUtilityNode("U")
# (True, True)
