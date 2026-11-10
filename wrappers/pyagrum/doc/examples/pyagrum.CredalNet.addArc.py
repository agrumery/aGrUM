import pyagrum as gum

cn = gum.CredalNet()
a = cn.addVariable("A", 2)
b = cn.addVariable("B", 2)
cn.addArc(a, b)
cn.src_bn().arcs()
# {(0, 1)}
