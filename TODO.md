# TODO_CLAUDE.md

Actions à réaliser lors d'une prochaine session.

**Règle** : ne jamais traiter un item de cette liste sans l'avoir signalé à l'utilisateur et obtenu son accord explicite.

**Scores** : Importance et Difficulté sur 5.

---

## 1. Typage Python | I:4 D:3

À traiter dans l'ordre (chaque étape débloque la suivante).

1. **`wrappers/pyagrum/pyLibs/` — Déplacer `_types.py`** | I:4 D:1

   `pyLibs/causal/_types.py` contient `NodeId`, `NodeSet`, `NameSet`, etc., utiles à l'ensemble de `pyLibs/`. Le déplacer en `pyLibs/_types.py` et mettre à jour tous les imports. Vérifier l'absence de redéfinitions ailleurs.

2. **`wrappers/pyagrum/pyLibs/**/*.py` — Compléter les annotations de type** | I:3 D:2

   Parcourir tous les fichiers et ajouter les annotations manquantes (paramètres et retours). S'appuyer sur `pyLibs/_types.py`. Ne pas modifier les docstrings NumPy existantes.

3. **`wrappers/pyagrum/` — Stubs `.pyi` pour les classes SWIG** | I:4 D:4 (multi-étapes)

   1. **Génération** : lancer `stubgen -p pyagrum` à chaque `act install pyAgrum`, sortie dans `pyagrum-stubs/`.
   2. **Post-traitement** : script remplaçant les `Any` génériques par les vrais types, en lisant `gum_typemaps.i` et les annotations de `pyLibs/`.
   3. **Marqueur** : ajouter `py.typed` dans le package une fois les stubs opérationnels.
   4. **Test CI** : vérifier via `inspect` qu'aucune méthode publique n'a disparu des stubs entre deux builds.

---

## 2. Modernisation C++ | I:3 D:3

- **`src/agrum/` — Intégration `std::string_view`** | I:3 D:3

  Remplacer les `const std::string&` (paramètres en entrée uniquement, sans copie retournée) par `std::string_view`. Priorité aux interfaces publiques fréquemment utilisées. Vérifier la compatibilité avec les bindings SWIG et typemaps en sortie.

---

## 3. SWIG | I:3 D:3

- **`wrappers/pyagrum/swigsrc/*.i` + `pyagrum.i` — Simplifier la structure des includes** | I:3 D:3

  Analyser l'ordre des `%include` dans `pyagrum.i` et `aGrUM_wrap_*.i` / `swigsrc/*.i`. Vérifier l'absence de redondances et que la convention "swigsrc avant aGrUM_wrap" est appliquée partout.

- **`wrappers/pyagrum/swigsrc/*.i` — Audit des typemaps manquants** | I:3 D:3

  Identifier les blocs `%extend` / code inline qui appellent manuellement des helpers de conversion (`populateHashTableStrStrFromPyDict`, `populateStringSetFromPySequence`, etc.) alors qu'un typemap existe ou pourrait exister. Remplacer par le typemap correspondant.

---

## 4. Documentation | I:2 D:1

- **`CHANGELOG.md` — Découper par version majeure** | I:2 D:1

  Scinder `CHANGELOG.md` en `CHANGELOG-0.x.md`, `CHANGELOG-1.x.md`, `CHANGELOG-2.x.md`. Remplacer `CHANGELOG.md` par un index pointant vers ces fichiers.

---

## 5. Nettoyage | I:2 D:2

- **`src/agrum/` — Namespaces anonymes** | I:3 D:2

  Utiliser `namespace { ... }` pour toute fonction/variable interne à un `.cpp` (au lieu de `static` ou d'une exposition accidentelle dans le namespace global).

- **`src/testunits/` — Remplacer `CHECK` par les macros précises** | I:2 D:1

  Remplacer `CHECK((a) == (b))` et similaires par `GUM_CHECK_EQ`, `GUM_CHECK_NE`, `GUM_CHECK_LT`, etc. Traiter module par module.

- **`wrappers/pyagrum/swigsrc/*.i` — Audit des `%ignore` redondants** | I:2 D:2

  Vérifier pour chaque `%ignore` si le retrait produit un warning SWIG ou une API non désirée. Candidats prioritaires : `%ignore gum::MultiDimWithOffset/MultiDimImplementation/MultiDimInterface/MultiDimDecorator/MultiDimArray` dans `tensor.i`.
