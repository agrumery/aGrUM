# requires: pandas
import re

import pyagrum.clg as gclg

gclg.SEM.FIND_TERM
# '^((?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)(?:[eE][+-]?[0-9]+)?)\\*?([a-zA-Z_]\\w*)$'
re.match(gclg.SEM.FIND_TERM, "0.5*A").groups()  # coefficient and variable of a term
# ('0.5', 'A')
