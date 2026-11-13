# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
ie = gclg.CLGVariableElimination(clg)
ie.updateEvidence({"C": 7.0})
ie.hasEvidence("C")
# True
ie.hasEvidence("A")
# False
