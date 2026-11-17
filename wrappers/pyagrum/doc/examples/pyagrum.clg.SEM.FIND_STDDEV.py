# requires: pandas
import re

import pyagrum.clg as gclg

gclg.SEM.FIND_STDDEV
# '^\\[((?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)(?:[eE][+-]?[0-9]+)?)\\]$'
re.match(gclg.SEM.FIND_STDDEV, "[2.5]").group(1)  # the standard deviation of a SEM equation
# '2.5'
