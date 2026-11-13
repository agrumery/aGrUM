# requires: pandas
import pyagrum as gum
import pyagrum.explain as expl

gum.initRandom(42)
bn = gum.fastBN("A->B->Y;C->Y")
df, _ = gum.generateSample(bn, 200)
explainer = expl.MarginalShallValues(bn, (df, True))  # df as background data
explanation = explainer.compute(df.head(5))  # explains the log-likelihood of the first 5 rows
explanation.values_type
# 'SHALL'
{v: round(imp, 4) for v, imp in sorted(explanation.importances.items())}
# {'A': 0.3982, 'B': 0.1687, 'C': 0.1762, 'Y': 0.1304}
