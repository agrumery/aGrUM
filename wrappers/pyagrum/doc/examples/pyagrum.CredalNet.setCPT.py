import pyagrum as gum

cn = gum.CredalNet()
a = cn.addVariable("A", 2)
b = cn.addVariable("B", 2)
cn.addArc(a, b)
cn.setCPTs(a, [[[0.5, 0.5]]])

# credal set of B given A=1 (index 1): two vertices
cn.setCPT(b, 1, [[0.3, 0.7], [0.5, 0.5]])

# credal set of B given A=0, selected by an instantiation of the parents
inst = gum.Instantiation(cn.src_bn().cpt(b))
inst.chgVal("A", 0)
cn.setCPT(b, inst, [[0.1, 0.9]])
cn.credalNet_srcCpt()[b]
# [[[0.1, 0.9]], [[0.3, 0.7], [0.5, 0.5]]]
