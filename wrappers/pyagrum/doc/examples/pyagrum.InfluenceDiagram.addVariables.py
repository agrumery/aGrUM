import pyagrum as gum

diag = gum.InfluenceDiagram()
diag.addVariables(["C{low|high}", "*D[3]", "$U"])
# [0, 1, 2]
diag.isChanceNode("C"), diag.isDecisionNode("D"), diag.isUtilityNode("U")
# (True, True, True)
