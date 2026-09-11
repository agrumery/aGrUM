#!/usr/bin/env python3
"""Phase 0 smoke test for the pyAgrum modularization plan (branch
`modularizePyAgrum`, docs: md_docs/modularizePyAgrum.md and
md_docs/GUM_PUBLIC.md).

Question this answers, on whatever platform you run it: with BASE/BN built
as *shared* libraries and NO `GUM_PUBLIC` annotation anywhere in the code
(the situation today), does a C++ object created in one independently
loaded shared library get correctly recognized (`dynamic_cast`, `typeid`,
virtual dispatch) by a second, separately loaded shared library that never
constructed it? That's exactly the mechanism `_pyagrum.so`/`.dll` and
`_mrf.so`/`.dll` would depend on if pyAgrum is split into per-module
extensions.

Already run and PASSED on macOS/clang (2026-08-11, see the audit journal in
both .md files above). This script reproduces the same two checks so you
can run it on Linux and Windows and report the output back.

What it does, in order:
  1. `act test release aGrUM -m BASE+BN` — builds BASE+BN as shared
     libraries (the project's default, see CMakeLists.txt) and runs the
     real doctest suite against that shared build. A link failure or test
     failure here is itself a meaningful answer for this platform — no
     need to go further if this fails.
  2. A tiny two-"plugin" harness that mimics Python's import machinery:
     `pluginA` (stands in for `_pyagrum.so`) constructs a
     `gum::BayesNet<double>` and hands out an opaque pointer; `pluginB`
     (stands in for `_mrf.so`) — compiled and linked *independently*, never
     seeing pluginA's translation unit — receives that pointer and does
     `typeid`/`dynamic_cast` up and down the `BayesNet`/`IBayesNet`
     hierarchy plus a virtual call through the base pointer. Both are
     loaded by a small host program via dlopen()/LoadLibrary(), exactly
     the way CPython loads two separate extension modules.

Usage:
    # Linux/macOS, from an activated project venv, at the repo root:
    python phase0_smoke.py

    # Windows: run from a "x64 Native Tools Command Prompt for VS" (so
    # `cl.exe` is on PATH) with the project venv activated, at the repo
    # root:
    python phase0_smoke.py

Flags:
    --skip-build     Skip step 1 (act test), go straight to the harness.
                      Useful if BASE+BN were already built/tested.
    --keep-tmp        Don't delete the temp dir used for the harness build
                      (path is printed) — handy to inspect a failure.

Nothing here touches CMakeLists.txt, act, or SWIG. It only reads the
existing build tree (build/aGrUM/release) and compiles a few throwaway
files in a temp directory.
"""

import argparse
import platform
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent
BUILD_DIR = REPO_ROOT / "build" / "aGrUM" / "release"
SRC_DIR = REPO_ROOT / "src"

SYSTEM = platform.system()  # "Linux", "Windows", "Darwin"

# ---------------------------------------------------------------------------
# Embedded harness sources (kept identical in spirit to the macOS run;
# platform differences are isolated behind #ifdef _WIN32).
# ---------------------------------------------------------------------------

PLUGIN_EXPORT = r"""
#if defined(_WIN32)
#  define PHASE0_EXPORT extern "C" __declspec(dllexport)
#else
#  define PHASE0_EXPORT extern "C"
#endif
"""

PLUGIN_A_SRC = (
    PLUGIN_EXPORT
    + r"""
// Phase 0 smoke test -- plugin A, mimics _pyagrum.so: creates a
// gum::BayesNet<double> and hands it out as an opaque pointer, exactly
// like a Python extension module would hand a wrapped C++ object to
// another independently loaded extension.
#include <agrum/BN/BayesNet.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <cstdio>

PHASE0_EXPORT void* create_bn() {
  auto* bn = new gum::BayesNet< double >("phase0-smoke");
  bn->add(gum::LabelizedVariable("A", "", 2));
  bn->add(gum::LabelizedVariable("B", "", 2));
  bn->addArc(bn->idFromName("A"), bn->idFromName("B"));

  std::printf(
      "[pluginA] created gum::BayesNet<double> @ %p, size=%u (this module's own view)\n",
      static_cast< void* >(bn),
      (unsigned)bn->size());

  return static_cast< void* >(bn);
}
"""
)

PLUGIN_B_SRC = (
    PLUGIN_EXPORT
    + r"""
// Phase 0 smoke test -- plugin B, mimics _mrf.so: receives an opaque
// pointer to an object it never constructed itself, and checks that its
// own compiled view of gum::BayesNet<double>/gum::IBayesNet<double>
// (typeid, vtable, dynamic_cast) agrees with the one baked into
// agrumBN by whichever module (here: pluginA, a different shared
// library) actually built the object.
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/IBayesNet.h>

#include <cstdio>
#include <typeinfo>

PHASE0_EXPORT int check_bn(void* raw) {
  auto* bn = static_cast< gum::BayesNet< double >* >(raw);

  const bool typeid_ok = (typeid(*bn) == typeid(gum::BayesNet< double >));

  auto*      base  = dynamic_cast< gum::IBayesNet< double >* >(bn);
  const bool up_ok  = (base != nullptr);

  auto*      back    = base ? dynamic_cast< gum::BayesNet< double >* >(base) : nullptr;
  const bool down_ok = (back == bn);

  const unsigned size = base ? (unsigned)base->size() : 0;

  std::printf(
      "[pluginB] typeid match=%d  dynamic_cast up (Bayes->IBayes)=%d  "
      "dynamic_cast round-trip=%d  size(via base ptr)=%u\n",
      typeid_ok,
      up_ok,
      down_ok,
      size);

  return (typeid_ok && up_ok && down_ok && size == 2) ? 1 : 0;
}
"""
)

HOST_SRC_UNIX = r"""
// Phase 0 smoke test -- host, mimics the Python interpreter: dlopen()'s
// two independent extension modules (pluginA ~ _pyagrum.so, pluginB ~
// _mrf.so), each dynamically linked (not statically) against the SAME
// agrumBASE/agrumBN shared libraries, and passes an object created by
// one to a function in the other.
#include <dlfcn.h>

#include <cstdio>

using create_fn = void* (*)();
using check_fn  = int (*)(void*);

int main() {
  void* pluginA = dlopen("./PLUGIN_A_NAME", RTLD_NOW | RTLD_GLOBAL);
  if (!pluginA) {
    std::fprintf(stderr, "dlopen(pluginA) failed: %s\n", dlerror());
    return 1;
  }
  void* pluginB = dlopen("./PLUGIN_B_NAME", RTLD_NOW | RTLD_GLOBAL);
  if (!pluginB) {
    std::fprintf(stderr, "dlopen(pluginB) failed: %s\n", dlerror());
    return 1;
  }

  auto create_bn = reinterpret_cast< create_fn >(dlsym(pluginA, "create_bn"));
  auto check_bn  = reinterpret_cast< check_fn >(dlsym(pluginB, "check_bn"));
  if (!create_bn || !check_bn) {
    std::fprintf(stderr, "dlsym failed: %s\n", dlerror());
    return 1;
  }

  void* obj = create_bn();
  int   ok  = check_bn(obj);

  std::printf("=== PHASE 0 SMOKE TEST: %s ===\n", ok ? "PASS" : "FAIL");
  return ok ? 0 : 1;
}
"""

HOST_SRC_WINDOWS = r"""
// Phase 0 smoke test -- host, mimics the Python interpreter: LoadLibrary()s
// two independent extension modules (pluginA ~ _pyagrum.pyd, pluginB ~
// _mrf.pyd), each dynamically linked (not statically) against the SAME
// agrumBASE.dll/agrumBN.dll, and passes an object created by one to a
// function in the other. agrumBASE.dll/agrumBN.dll are pre-loaded
// explicitly first so both plugins resolve to the SAME loaded instance
// (Windows identifies a loaded DLL by path within a process).
#include <windows.h>

#include <cstdio>

using create_fn = void* (*)();
using check_fn  = int (*)(void*);

static HMODULE must_load(const char* name) {
  HMODULE h = LoadLibraryA(name);
  if (!h) {
    std::fprintf(stderr, "LoadLibrary(%s) failed, GetLastError=%lu\n", name, GetLastError());
  }
  return h;
}

int main() {
  if (!must_load("agrumBASE.dll")) return 1;
  if (!must_load("agrumBN.dll")) return 1;

  HMODULE pluginA = must_load("PLUGIN_A_NAME");
  if (!pluginA) return 1;
  HMODULE pluginB = must_load("PLUGIN_B_NAME");
  if (!pluginB) return 1;

  auto create_bn = reinterpret_cast< create_fn >(GetProcAddress(pluginA, "create_bn"));
  auto check_bn  = reinterpret_cast< check_fn >(GetProcAddress(pluginB, "check_bn"));
  if (!create_bn || !check_bn) {
    std::fprintf(stderr, "GetProcAddress failed, GetLastError=%lu\n", GetLastError());
    return 1;
  }

  void* obj = create_bn();
  int   ok  = check_bn(obj);

  std::printf("=== PHASE 0 SMOKE TEST: %s ===\n", ok ? "PASS" : "FAIL");
  return ok ? 0 : 1;
}
"""


def run(cmd, **kw):
    print("$ " + " ".join(str(c) for c in cmd))
    kw.setdefault("text", True)
    kw.setdefault("capture_output", True)
    proc = subprocess.run(cmd, **kw)
    if proc.stdout:
        print(proc.stdout)
    if proc.stderr:
        print(proc.stderr, file=sys.stderr)
    return proc


def banner(title):
    print()
    print("=" * 78)
    print(title)
    print("=" * 78)


# ---------------------------------------------------------------------------
# Step 1 -- build BASE+BN shared and run the real test suite against it.
# ---------------------------------------------------------------------------


def step1_build_and_test():
    banner("STEP 1 -- act test release aGrUM -m BASE+BN")
    cmd = [sys.executable, str(REPO_ROOT / "act"), "test", "release", "aGrUM", "-m", "BASE+BN"]
    proc = run(cmd, cwd=REPO_ROOT)
    ok = proc.returncode == 0 and "[doctest] Status: SUCCESS!" in (proc.stdout or "")
    return ok


# ---------------------------------------------------------------------------
# Step 2 -- locate the shared libs, compile the harness, run it.
# ---------------------------------------------------------------------------


def find_first(patterns):
    for pattern in patterns:
        matches = sorted(BUILD_DIR.rglob(pattern))
        if matches:
            return matches[0]
    return None


def locate_libs():
    """Returns (base_lib_dir, bn_lib_dir, link_names_or_paths) depending on platform."""
    if SYSTEM == "Windows":
        base_dll = find_first(["agrumBASE.dll"])
        bn_dll = find_first(["agrumBN.dll"])
        base_lib = find_first(["agrumBASE.lib"])
        bn_lib = find_first(["agrumBN.lib"])
        return {"base_dll": base_dll, "bn_dll": bn_dll, "base_lib": base_lib, "bn_lib": bn_lib}
    elif SYSTEM == "Darwin":
        base = find_first(["libagrumBASE.dylib", "libagrumBASE.*.dylib"])
        bn = find_first(["libagrumBN.dylib", "libagrumBN.*.dylib"])
        return {"base": base, "bn": bn}
    else:  # Linux and friends
        base = find_first(["libagrumBASE.so", "libagrumBASE.so.*"])
        bn = find_first(["libagrumBN.so", "libagrumBN.so.*"])
        return {"base": base, "bn": bn}


def compiler_unix():
    for c in ("g++", "clang++", "c++"):
        if shutil.which(c):
            return c
    return None


def compile_unix(tmp, libs):
    cxx = compiler_unix()
    if not cxx:
        print("ERROR: no C++ compiler found (looked for g++, clang++, c++).")
        return None

    base_lib = libs.get("base")
    bn_lib = libs.get("bn")
    if not base_lib or not bn_lib:
        print(f"ERROR: could not find libagrumBASE/libagrumBN under {BUILD_DIR}")
        print("       run step 1 first (or drop --skip-build).")
        return None

    ext = ".dylib" if SYSTEM == "Darwin" else ".so"
    lib_dirs = {base_lib.parent, bn_lib.parent}
    common = [
        "-std=c++20",
        f"-I{SRC_DIR}",
        f"-I{BUILD_DIR}",
    ]

    def link_flags():
        flags = []
        for d in lib_dirs:
            flags += [f"-L{d}", f"-Wl,-rpath,{d}"]
        flags += ["-lagrumBASE", "-lagrumBN"]
        if SYSTEM == "Linux":
            flags += ["-ldl"]
        return flags

    for name, src in (("pluginA", PLUGIN_A_SRC), ("pluginB", PLUGIN_B_SRC)):
        (tmp / f"{name}.cpp").write_text(src)
        out = tmp / f"lib{name}{ext}"
        cmd = [cxx, *common, "-shared", "-fPIC", str(tmp / f"{name}.cpp"), *link_flags(), "-o", str(out)]
        proc = run(cmd)
        if proc.returncode != 0 or not out.exists():
            print(f"ERROR: failed to build {name}")
            return None

    host_src = HOST_SRC_UNIX.replace("PLUGIN_A_NAME", f"libpluginA{ext}").replace(
        "PLUGIN_B_NAME", f"libpluginB{ext}"
    )
    (tmp / "host.cpp").write_text(host_src)
    host_out = tmp / "host"
    cmd = [cxx, "-std=c++20", str(tmp / "host.cpp"), "-o", str(host_out)]
    proc = run(cmd)
    if proc.returncode != 0 or not host_out.exists():
        print("ERROR: failed to build host")
        return None

    return host_out


def compile_windows(tmp, libs):
    cl = shutil.which("cl")
    if not cl:
        print("ERROR: cl.exe not found on PATH.")
        print("       Run this script from a 'x64 Native Tools Command Prompt for VS'")
        print("       (or any shell where `vcvarsall.bat x64` has been called).")
        return None

    base_dll, bn_dll = libs.get("base_dll"), libs.get("bn_dll")
    base_lib, bn_lib = libs.get("base_lib"), libs.get("bn_lib")
    if not all([base_dll, bn_dll, base_lib, bn_lib]):
        print(f"ERROR: could not find agrumBASE/agrumBN .dll and .lib under {BUILD_DIR}")
        print("       run step 1 first (or drop --skip-build).")
        print(f"       found so far: {libs}")
        return None

    lib_dirs = {base_lib.parent, bn_lib.parent}
    common = [
        "/nologo",
        "/std:c++20",
        "/EHsc",
        "/MD",
        "/DNOMINMAX",
        "/D_CRT_SECURE_NO_WARNINGS",
        "/D_CRT_NONSTDC_NO_DEPRECATE",
        f"/I{SRC_DIR}",
        f"/I{BUILD_DIR}",
    ]

    for name, src in (("pluginA", PLUGIN_A_SRC), ("pluginB", PLUGIN_B_SRC)):
        (tmp / f"{name}.cpp").write_text(src)
        cmd = [
            cl,
            *common,
            "/LD",
            str(tmp / f"{name}.cpp"),
            f"/Fe:{tmp / (name + '.dll')}",
            f"/Fo:{tmp}\\",
            "/link",
            *[f"/LIBPATH:{d}" for d in lib_dirs],
            "agrumBASE.lib",
            "agrumBN.lib",
        ]
        proc = run(cmd, cwd=tmp)
        if proc.returncode != 0 or not (tmp / f"{name}.dll").exists():
            print(f"ERROR: failed to build {name}.dll")
            print(
                "       if the error is 'unresolved external symbol' for gum:: symbols, "
                "that IS the expected Windows-without-GUM_PUBLIC failure signature -- "
                "please paste the full output back."
            )
            return None

    # Put agrumBASE.dll/agrumBN.dll next to the plugins/host so the default
    # DLL search order finds them without extra PATH/SetDllDirectory setup.
    shutil.copy(base_dll, tmp / base_dll.name)
    shutil.copy(bn_dll, tmp / bn_dll.name)

    host_src = HOST_SRC_WINDOWS.replace("PLUGIN_A_NAME", "pluginA.dll").replace(
        "PLUGIN_B_NAME", "pluginB.dll"
    )
    (tmp / "host.cpp").write_text(host_src)
    cmd = [
        cl,
        "/nologo",
        "/std:c++20",
        "/EHsc",
        str(tmp / "host.cpp"),
        f"/Fe:{tmp / 'host.exe'}",
        f"/Fo:{tmp}\\",
    ]
    proc = run(cmd, cwd=tmp)
    if proc.returncode != 0 or not (tmp / "host.exe").exists():
        print("ERROR: failed to build host.exe")
        return None

    return tmp / "host.exe"


def step2_dlopen_harness(keep_tmp):
    banner("STEP 2 -- dlopen()/LoadLibrary() cross-module identity harness")
    libs = locate_libs()
    print(f"Located libraries under {BUILD_DIR}:")
    for k, v in libs.items():
        print(f"  {k}: {v}")

    tmp_ctx = tempfile.TemporaryDirectory(prefix="phase0_smoke_")
    tmp = Path(tmp_ctx.name)
    try:
        if SYSTEM == "Windows":
            host_bin = compile_windows(tmp, libs)
        else:
            host_bin = compile_unix(tmp, libs)

        if host_bin is None:
            return False

        banner("Running host")
        proc = run([str(host_bin)], cwd=tmp)
        ok = proc.returncode == 0 and "PHASE 0 SMOKE TEST: PASS" in (proc.stdout or "")
        return ok
    finally:
        if keep_tmp:
            print(f"\n(--keep-tmp) harness build kept at: {tmp}")
            tmp_ctx._finalizer.detach()  # prevent auto-cleanup
        else:
            tmp_ctx.cleanup()


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--skip-build", action="store_true", help="skip step 1 (act test)")
    parser.add_argument("--keep-tmp", action="store_true", help="keep the harness temp dir for inspection")
    args = parser.parse_args()

    print(f"Platform: {SYSTEM} / {platform.machine()}")
    print(f"Repo root: {REPO_ROOT}")

    ok1 = None
    if not args.skip_build:
        ok1 = step1_build_and_test()
    else:
        print("(--skip-build) skipping step 1")

    ok2 = step2_dlopen_harness(args.keep_tmp)

    banner("PHASE 0 SMOKE TEST -- SUMMARY (please paste this block back)")
    print(f"platform            : {SYSTEM} / {platform.machine()}")
    step1_label = "SKIPPED" if ok1 is None else ("PASS" if ok1 else "FAIL")
    print(f"step 1 (act test BASE+BN) : {step1_label}")
    print(f"step 2 (dlopen harness)   : {'PASS' if ok2 else 'FAIL'}")
    overall_ok = ok2 and (ok1 or ok1 is None)
    print(f"overall                   : {'PASS' if overall_ok else 'FAIL'}")

    sys.exit(0 if overall_ok else 1)


if __name__ == "__main__":
    main()
