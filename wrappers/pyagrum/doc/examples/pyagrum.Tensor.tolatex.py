# requires: pandas, jinja2
import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
print(p.tolatex())
# \begin{tabular}{lrr}
# \toprule
#  & \multicolumn{2}{r}{a} \\
#  & 0 & 1 \\
# b &  &  \\
# \midrule
# 0 & 1.000000 & 2.000000 \\
# 1 & 3.000000 & 4.000000 \\
# 2 & 5.000000 & 6.000000 \\
# \bottomrule
# \end{tabular}
