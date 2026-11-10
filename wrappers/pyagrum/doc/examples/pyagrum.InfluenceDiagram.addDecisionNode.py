import pyagrum as gum

diag = gum.InfluenceDiagram()
diag.addDecisionNode(gum.LabelizedVariable("Umbrella", "", ["yes", "no"]))
# 0
diag.isDecisionNode("Umbrella"), diag.decisionNodeSize()
# (True, 1)
