import pyagrum as gum

bn = gum.fastBN("A->B")
cn = gum.CredalNet(bn, bn)
cn.fillConstraint(cn.src_bn().idFromName("A"), 0, [0.2, 0.6], [0.4, 0.8])
cn.intervalToCredal()
sorted(cn.credalNet_currentCpt()[cn.src_bn().idFromName("A")][0])
# [[0.2, 0.8], [0.4, 0.6]]
