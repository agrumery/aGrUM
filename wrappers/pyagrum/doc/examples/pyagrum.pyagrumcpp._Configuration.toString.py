import pyagrum as gum

gum.config.push()
gum.config.reset()
print("\n".join(gum.config.toString().splitlines()[:3]))  # same as print(gum.config)
# [core]                                  [tensor]                                 decisionShape = box
#   maxNumberOfThreads = -1                 maxRowsConsole = 6                     decisionArcStyle = tapered, bold, dotted
#   randomSeed = 0                          columnWidthConsole = 9                 utilityArcStyle = dashed
gum.config.pop()
