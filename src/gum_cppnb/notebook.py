############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import base64
import subprocess

from IPython.display import HTML, display


def _dot_png_b64(path: str, config: dict[str, str]) -> str:
  '''Render a .dot file to PNG and return a base64 data-URI (portable across platforms).'''
  r = subprocess.run([config['dotexe'], '-Tpng', path], capture_output=True)
  if not r.stdout:
    print('dot error:', r.stderr.decode()[:200])
    return ''
  return base64.b64encode(r.stdout).decode()


def show_dot(path: str, config: dict[str, str]) -> None:
  b64 = _dot_png_b64(path, config)
  if b64:
    display(HTML(f'<img src="data:image/png;base64,{b64}"/>'))


def show_dots(*pairs: tuple[str, str], config: dict[str, str]) -> None:
  '''pairs = (path, title), ... rendered side by side as PNG data-URIs.'''
  tds = ''.join(
    f'<td style="padding:8px;vertical-align:top;text-align:center"><b>{t}</b><br>'
    f'<img src="data:image/png;base64,{_dot_png_b64(p, config)}"/></td>'
    for p, t in pairs)
  display(HTML(f'<table><tr>{tds}</tr></table>'))
