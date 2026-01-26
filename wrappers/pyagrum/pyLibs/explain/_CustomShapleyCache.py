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

class CustomShapleyCache:
  def __init__(self, max_capacity: int = 1000):
    if max_capacity <= 0:
      raise ValueError("max_capacity must be a positive integer.")
    self._cache = {}  # Structure: {longueur_str: { (entier, str): valeur }}
    self._current_k_index = -1  # Pour suivre l'indice k actuel
    self._max_capacity = max_capacity  # Capacité maximale du cache en nombre d'éléments
    self._current_size = 0  # Taille actuelle du cache

  @staticmethod
  def generate_keys(bn, target, feat, nodes):
    key1 = tuple(nodes)  # Key1 represents the coalition.
    key2 = tuple(
      n for n in nodes if n != feat
    )  # Key2 represents the coalition without the feature which is being evaluated.
    if target is None:  # For SHALL Values
      return key1, key2, None
    # Key3 is the coalition without ONE ! node which is not in the minimal conditional set.
    minimal = bn.minimalCondSet(
      target, nodes
    )  # Calculates the minimal conditional set for the target node given the nodes.
    diff = next((n for n in nodes if n not in minimal), None)
    if diff is not None:
      key3 = tuple(n for n in nodes if n != diff)
    else:
      key3 = key1
    return key1, key2, key3

  def get(self, int_key: int, tuple_key: tuple):
    tuple_len = len(tuple_key)
    return self._cache[tuple_len].get((int_key, tuple_key), None)

  def set(self, int_key: int, tuple_key: tuple, value):
    tuple_len = len(tuple_key)
    if tuple_len not in self._cache:
      self._cache[tuple_len] = {}
      self._current_k_index = tuple_len

    # If it is the first time we add an entry of this length, we initialize the cache for this length.
    if (int_key, tuple_key) not in self._cache[tuple_len]:
      self._current_size += 1
    self._cache[tuple_len][(int_key, tuple_key)] = value

    # Check if we need to purge the cache
    if self._current_size > self._max_capacity:
      self._perform_purge()

  def _perform_purge(self):
    if self._current_k_index < 2:
      return

    str_len_to_purge = self._current_k_index - 2

    # Purge all entries with length <= str_len_to_purge
    for length in list(self._cache.keys()):
      if length <= str_len_to_purge:
        self._current_size -= len(self._cache[length])
        del self._cache[length]

  def __len__(self):
    return self._current_size

  def __str__(self):
    # Affiche le cache par longueur de clé pour une meilleure visibilité
    items_by_length = {k: len(v) for k, v in self._cache.items()}
    return (
      f"Cache (k={self._current_k_index}, size={self._current_size}/{self._max_capacity}): "
      f"Contents by length: {items_by_length}"
    )
