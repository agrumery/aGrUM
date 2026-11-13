# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
gclg.SEM.saveCLG(clg, "model.sem")
clg2 = gclg.SEM.loadCLG("model.sem")
clg2.names()
# ['A', 'B', 'C']
clg2.coefArc("A", "B")
# 0.5
