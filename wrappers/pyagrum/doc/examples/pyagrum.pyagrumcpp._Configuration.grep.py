import contextlib
import io

import pyagrum as gum

gum.config.push()
gum.config.reset()
with contextlib.redirect_stdout(io.StringIO()) as out:
  gum.config.grep("visibleDigits")  # grep() prints its result
print(out.getvalue().strip())
# [tensor]
#   visibleDigits = 4
# [influenceDiagram]
#   utilityVisibleDigits = 2
# [histogram]
#   horizontalVisibleDigits = 2
#   verticalVisibleDigits = 2
gum.config.pop()
