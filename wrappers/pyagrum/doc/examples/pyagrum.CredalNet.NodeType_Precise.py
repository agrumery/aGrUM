import pyagrum as gum

bn = gum.fastBN("A->B")
cn = gum.CredalNet(bn, bn)  # same lower and upper probabilities
cn.intervalToCredal()
gum.CredalNet.NodeType_Precise
# 0
cn.nodeType(bn.idFromName("A")) == gum.CredalNet.NodeType_Precise
# True
