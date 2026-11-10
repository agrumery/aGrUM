import contextlib
import io

import pyagrum as gum

gum.config.push()
gum.config.reset()
gum.config["tensor", "visibleDigits"] = 6
with contextlib.redirect_stdout(io.StringIO()) as out:
  gum.config.diff()  # diff() prints its result
print(out.getvalue().strip())
# [tensor]
#   visibleDigits = 6
gum.config.pop()
