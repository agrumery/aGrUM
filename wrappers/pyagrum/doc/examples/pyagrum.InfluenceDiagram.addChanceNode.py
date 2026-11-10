import pyagrum as gum

diag = gum.InfluenceDiagram()
diag.addChanceNode(gum.LabelizedVariable("Weather", "", ["sun", "rain"]))
# 0
diag.isChanceNode("Weather"), diag.chanceNodeSize()
# (True, 1)
