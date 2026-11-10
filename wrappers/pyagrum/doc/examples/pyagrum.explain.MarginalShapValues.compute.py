# requires: pandas, matplotlib
import pyagrum as gum
import pyagrum.explain as expl

gum.initRandom(42)
bn = gum.fastBN("A->B->Y;C->Y")
df, _ = gum.generateSample(bn, 200)
explainer = expl.MarginalShapValues(bn, "Y", (df, True))  # df as background data
explanation = explainer.compute(df.head(5))  # explains the first 5 rows
sorted(explanation.keys())  # one explanation per value of Y
# [0, 1]
{v: round(imp, 4) for v, imp in sorted(explanation.importances[1].items())}  # A acts on Y only through B
# {'A': 0.0, 'B': 0.7737, 'C': 0.5633}
