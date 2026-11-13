# requires: pandas
import re

import pyagrum.clg as gclg

gclg.SEM.FIND_FLOAT
# '^([0-9]*\\.?[0-9]*)$'
re.match(gclg.SEM.FIND_FLOAT, "0.5").group(1)
# '0.5'
