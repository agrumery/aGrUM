# requires: pandas, scipy, sklearn
import re

import pyagrum.clg as gclg

gclg.SEM.FIND_VAR
# '^([a-zA-Z_]\\w*)$'
re.match(gclg.SEM.FIND_VAR, "speed_1") is not None
# True
