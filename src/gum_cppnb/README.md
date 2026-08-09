# gum_cppnb

IPython extension providing a `%%cpp` cell magic to compile and run C++ code
against an installed aGrUM, from a Jupyter notebook.

```python
%load_ext gum_cppnb
```

```cpp
%%cpp
#include <agrum/bn.h>
int main() {
  auto net = gum::BayesNet<double>::fastPrototype("A->B->C");
  std::cout << net.toString();
  cppnb_add_dot(net.toDot());
}
```

Installed automatically by `act install aGrUM` when a Python environment
(virtualenv/conda site-packages, or user site-packages) is detected at the
target prefix — see `acttools/ActBuilderAgrum.py`. It has **no dependency on
pyAgrum**: it never imports `pyagrum`, only aGrUM's CMake package
(`aGrUMConfig.cmake` / `agrum.pc`, produced by `act install aGrUM` alone).

## Design

Each `%%cpp` cell is compiled as a **complete, standalone translation unit**
(with its own `main()`) using the system compiler, linked against the
installed aGrUM libraries, then executed as a child process. There is no
persistent interpreter state between cells — every cell starts from a clean
slate.

This is a deliberate choice, not an oversight.

### Why not xeus-cling

[xeus-cling](https://github.com/jupyter-xeus/xeus-cling) wraps
[cling](https://github.com/root-project/cling), a C++ interpreter built on a
frozen fork of Clang/LLVM. Two problems rule it out for aGrUM:

- **C++ standard support**: cling's Clang fork lags behind upstream; aGrUM
  requires C++20 and is template-heavy, an area where interpreter-grade
  standard compliance tends to fall short first.
- **ABI risk**: cling bundles its own LLVM/libc++ (or libstdc++) runtime,
  which may not match the toolchain that built the installed aGrUM shared
  libraries. Loading a precompiled `.so` through a JIT with a different ABI
  is exactly the failure mode `find_working_compiler()` in this package is
  built to detect and avoid — cling reintroduces it with far less control.
- The project itself has stalled; QuantStack (its maintainers) moved their
  effort to xeus-cpp instead.

### Why not xeus-cpp

[xeus-cpp](https://github.com/jupyter-xeus/xeus-cpp) is the actively
developed successor, built on **Clang-Repl** (the incremental-compiler work
now upstream in LLVM) — a real Clang frontend, so C++20 support tracks much
closer to mainline Clang than cling's fork does. It is a better long-term
bet, but as of this writing it is still young: alpha/beta maturity, sparse
documentation, weak Windows support, and a much smaller user base exercising
it against large, template-heavy libraries. It carries the same class of
JIT/ABI risk as cling, just less of it. Worth revisiting once it matures.

### Trade-off accepted here

Compile-and-run-a-full-program per cell gives up REPL-style state across
cells, in exchange for using the *exact* system compiler and linker that
built aGrUM — the same toolchain, the same ABI, nothing bundled or
JIT-mediated. `find_working_compiler()` doesn't just look a compiler up on
`PATH`: it compiles, links, and *runs* a small aGrUM-linked probe before
trusting a candidate, specifically to catch ABI mismatches (e.g. libc++ vs
libstdc++) that a plain `PATH` lookup would miss.

## Known limitations

- **No state between cells**: every `%%cpp` cell needs its own `main()`.
  Not a REPL.
- **No Windows/MSVC support**: `find_working_compiler()`'s probe and the
  flags returned by `agrum_build_flags()` assume GCC/Clang-style syntax
  (`-std=`, `-I`, `-L`, `-Wl,-rpath,...`). MSVC needs different flags
  (`/std:`, `/I`, `/LIBPATH:`) and a different runtime DLL resolution
  strategy (no rpath equivalent — `PATH` injection or DLL copying instead).
  aGrUM's own generated `AGRUM_CXX_FLAGS` (see `src/cmake/Pack.agrum.cmake`)
  is Unix-flavored too, so this is a prerequisite fix upstream of this
  package, not just here. Tracked as future work.
- Re-detecting the compiler/aGrUM configuration after an environment change
  is done via `%reload_ext gum_cppnb`, not by re-running `import`.
