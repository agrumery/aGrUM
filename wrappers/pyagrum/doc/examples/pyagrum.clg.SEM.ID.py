# requires: pandas
import re

import pyagrum.clg as gclg

gclg.SEM.ID
# '[a-zA-Z_]\\w*'
re.findall(gclg.SEM.ID, "B = 1 + 0.5*A + C")
# ['B', 'A', 'C']
