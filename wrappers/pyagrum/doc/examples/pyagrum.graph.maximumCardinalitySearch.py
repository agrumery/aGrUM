import pyagrum as gum
import pyagrum.graph as gg

gg.maximumCardinalitySearch(gum.fastUndiGraph("A-B-C;B-D;C-D"))
# [0, 1, 3, 2]
