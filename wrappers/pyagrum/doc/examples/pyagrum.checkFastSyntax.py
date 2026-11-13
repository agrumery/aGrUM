import pyagrum as gum

check = gum.checkFastSyntax("A->B->C")
[syntax.name for syntax in check.validSyntaxes()]
# ['DiGraph', 'MixedGraph', 'MarkedGraph', 'BayesNet', 'InfluenceDiagram']
[syntax.name for syntax in gum.checkFastSyntax("A--B--C").validSyntaxes()]
# ['MarkovRandomField']
[syntax.name for syntax in gum.checkFastSyntax("A{yes|no}->Visit Asia").validSyntaxes()]
# ['BayesNet', 'InfluenceDiagram']
bool(gum.checkFastSyntax("A->"))
# False
