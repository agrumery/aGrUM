# requires: pandas
import re

import pyagrum.clg as gclg

gclg.SEM.NUMBER
# '(?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)(?:[eE][+-]?[0-9]+)?'
re.fullmatch(gclg.SEM.NUMBER, "3.25") is not None
# True
re.fullmatch(gclg.SEM.NUMBER, "1e-05") is not None
# True
