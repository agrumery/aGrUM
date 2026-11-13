# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
ie = gclg.CLGVariableElimination(clg)
ie.updateEvidence({"C": 9.0})
post = ie.posterior("A")
round(float(post.mu()), 4), round(float(post.sigma()), 4)
# (2.381, 0.9759)
