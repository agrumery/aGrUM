import pyagrum as gum

cn = gum.CredalNet()
a = cn.addVariable("A", 2)
b = cn.addVariable("B", 2)
cn.addArc(a, b)
cn.setCPTs(a, [[[0.2, 0.8], [0.6, 0.4]]])
cn.setCPTs(b, [[[0.1, 0.9]], [[0.3, 0.7], [0.5, 0.5]]])
cn.credalNet_srcCpt()[a]  # the 2 vertices of the credal set of A
# [[[0.2, 0.8], [0.6, 0.4]]]
