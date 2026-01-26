#!/usr/bin/env python3
"""
Comprehensive migration script from CxxTest to doctest for aGrUM test suites.

This script performs all necessary transformations:
1. Convert CxxTest class-based tests to doctest struct-based fixtures
2. Replace TS_ASSERT_* macros with doctest CHECK equivalents
3. Add missing semicolons after assertions
4. Fix complex CHECK expressions with || or &&
5. Restore setUp/tearDown as constructors/destructors

Usage:
    python migration.py <file>           # Migrate a single file
    python migration.py --all [dir]      # Migrate all test files in directory
    python migration.py --module <name>  # Migrate a specific module (e.g., module_CM)
"""

import re
import subprocess
import sys
from pathlib import Path


# =============================================================================
# UTILITY FUNCTIONS
# =============================================================================

def find_matching_paren(content, start):
  """Find position of closing paren matching the one at start."""
  depth = 0
  i = start
  in_string = False
  string_char = None

  while i < len(content):
    c = content[i]
    if c in '"\'':
      if not in_string:
        in_string = True
        string_char = c
      elif c == string_char and (i == 0 or content[i - 1] != '\\'):
        in_string = False
    elif not in_string:
      if c == '(':
        depth += 1
      elif c == ')':
        depth -= 1
        if depth == 0:
          return i
    i += 1
  return -1


def find_matching_brace(content, start):
  """Find position of closing brace matching the one at start."""
  depth = 0
  i = start
  in_string = False
  string_char = None
  in_comment = False
  in_line_comment = False

  while i < len(content):
    if not in_string:
      if i + 1 < len(content) and content[i:i + 2] == '//':
        in_line_comment = True
      elif content[i] == '\n':
        in_line_comment = False
      elif i + 1 < len(content) and content[i:i + 2] == '/*':
        in_comment = True
      elif i + 1 < len(content) and content[i:i + 2] == '*/':
        in_comment = False
        i += 2
        continue

    if in_comment or in_line_comment:
      i += 1
      continue

    c = content[i]
    if c in '"\'':
      if not in_string:
        in_string = True
        string_char = c
      elif c == string_char and (i == 0 or content[i - 1] != '\\'):
        in_string = False
    elif not in_string:
      if c == '{':
        depth += 1
      elif c == '}':
        depth -= 1
        if depth == 0:
          return i
    i += 1
  return -1


def extract_args(content, start):
  """Extract arguments from a macro call starting at '('."""
  paren_end = find_matching_paren(content, start)
  if paren_end == -1:
    return None, -1

  args_str = content[start + 1:paren_end]

  args = []
  current = []
  depth = 0
  in_string = False
  string_char = None

  for i, c in enumerate(args_str):
    if c in '"\'':
      if not in_string:
        in_string = True
        string_char = c
      elif c == string_char and (i == 0 or args_str[i - 1] != '\\'):
        in_string = False
      current.append(c)
    elif in_string:
      current.append(c)
    elif c in '([{':
      depth += 1
      current.append(c)
    elif c in ')]}':
      depth -= 1
      current.append(c)
    elif c == ',' and depth == 0:
      args.append(''.join(current).strip())
      current = []
    else:
      current.append(c)

  if current:
    args.append(''.join(current).strip())

  return args, paren_end


def run_git_show(filepath, base_dir):
  """Get file content from git HEAD."""
  try:
    result = subprocess.run(
      ['git', 'show', f'HEAD:{filepath}'],
      capture_output=True,
      text=True,
      cwd=base_dir
    )
    if result.returncode == 0:
      return result.stdout
  except:
    pass
  return None


# =============================================================================
# STEP 1: CONVERT CXXTEST CLASS TO DOCTEST STRUCT
# =============================================================================

def convert_class_to_struct(content):
  """Convert CxxTest class definition to doctest struct."""

  # Replace class GUM_TEST_SUITE(Name) with struct NameTestSuite
  def replace_class(match):
    name = match.group(1)
    return f'struct {name}TestSuite {{'

  content = re.sub(
    r'class\s+GUM_TEST_SUITE\s*\(\s*(\w+)\s*\)',
    replace_class,
    content
  )

  # Replace GUM_ACTIVE_TEST(name) with void testname()
  def replace_test(match):
    name = match.group(1)
    return f'void test{name}()'

  content = re.sub(
    r'GUM_ACTIVE_TEST\s*\(\s*(\w+)\s*\)',
    replace_test,
    content
  )

  # Remove 'override' from setUp/tearDown
  content = re.sub(r'void\s+setUp\s*\(\s*\)\s*override', 'void setUp()', content)
  content = re.sub(r'void\s+tearDown\s*\(\s*\)\s*override', 'void tearDown()', content)

  # Remove 'final' from setUp/tearDown
  content = re.sub(r'void\s+setUp\s*\(\s*\)\s*final', 'void setUp()', content)
  content = re.sub(r'void\s+tearDown\s*\(\s*\)\s*final', 'void tearDown()', content)

  return content


# =============================================================================
# STEP 2: REPLACE TS_ASSERT MACROS
# =============================================================================

def replace_simple_macros(content):
  """Replace simple TS_* macros with doctest equivalents."""

  replacements = [
    (r'\bTS_ASSERT\s*\(', 'CHECK('),
    (r'\bTS_FAIL\s*\(', 'FAIL('),
  ]

  for pattern, replacement in replacements:
    content = re.sub(pattern, replacement, content)

  return content


def replace_binary_macro(content, macro_name, replacement_fn):
  """Replace a binary macro (2 arguments) with a custom replacement."""
  pattern = re.compile(rf'\b{macro_name}\s*\(')

  result = []
  i = 0

  while i < len(content):
    match = pattern.match(content, i)
    if match:
      paren_start = match.end() - 1
      args, paren_end = extract_args(content, paren_start)

      if args is not None and len(args) >= 2:
        replacement = replacement_fn(args[0], args[1])
        result.append(replacement)
        i = paren_end + 1
        continue

    result.append(content[i])
    i += 1

  return ''.join(result)


def replace_ternary_macro(content, macro_name, replacement_fn):
  """Replace a ternary macro (3 arguments) with a custom replacement."""
  pattern = re.compile(rf'\b{macro_name}\s*\(')

  result = []
  i = 0

  while i < len(content):
    match = pattern.match(content, i)
    if match:
      paren_start = match.end() - 1
      args, paren_end = extract_args(content, paren_start)

      if args is not None and len(args) >= 3:
        replacement = replacement_fn(args[0], args[1], args[2])
        result.append(replacement)
        i = paren_end + 1
        continue

    result.append(content[i])
    i += 1

  return ''.join(result)


def replace_throws_macros(content):
  """Replace TS_ASSERT_THROWS* macros."""

  # TS_ASSERT_THROWS(expr, ExType) -> CHECK_THROWS_AS(expr, ExType)
  content = replace_binary_macro(
    content, 'TS_ASSERT_THROWS',
    lambda expr, ex: f'CHECK_THROWS_AS({expr}, {ex})'
  )

  # TS_ASSERT_THROWS_NOTHING(expr) -> CHECK_NOTHROW(expr)
  pattern = re.compile(r'\bTS_ASSERT_THROWS_NOTHING\s*\(')
  result = []
  i = 0
  while i < len(content):
    match = pattern.match(content, i)
    if match:
      paren_start = match.end() - 1
      paren_end = find_matching_paren(content, paren_start)
      if paren_end != -1:
        inner = content[paren_start + 1:paren_end]
        result.append(f'CHECK_NOTHROW({inner})')
        i = paren_end + 1
        continue
    result.append(content[i])
    i += 1
  content = ''.join(result)

  # TS_ASSERT_THROWS_ANYTHING(expr) -> CHECK_THROWS(expr)
  pattern = re.compile(r'\bTS_ASSERT_THROWS_ANYTHING\s*\(')
  result = []
  i = 0
  while i < len(content):
    match = pattern.match(content, i)
    if match:
      paren_start = match.end() - 1
      paren_end = find_matching_paren(content, paren_start)
      if paren_end != -1:
        inner = content[paren_start + 1:paren_end]
        result.append(f'CHECK_THROWS({inner})')
        i = paren_end + 1
        continue
    result.append(content[i])
    i += 1
  content = ''.join(result)

  return content


def replace_comparison_macros(content):
  """Replace TS_ASSERT comparison macros."""

  # TS_ASSERT_EQUALS -> CHECK((x) == (y))
  content = replace_binary_macro(
    content, 'TS_ASSERT_EQUALS',
    lambda a, b: f'CHECK(({a}) == ({b}))'
  )
  content = replace_binary_macro(
    content, 'GUM_CHECK_ASSERT_EQUALS',
    lambda a, b: f'CHECK(({a}) == ({b}))'
  )

  # TS_ASSERT_DIFFERS -> CHECK((x) != (y))
  content = replace_binary_macro(
    content, 'TS_ASSERT_DIFFERS',
    lambda a, b: f'CHECK(({a}) != ({b}))'
  )
  content = replace_binary_macro(
    content, 'GUM_CHECK_ASSERT_DIFFERS',
    lambda a, b: f'CHECK(({a}) != ({b}))'
  )

  # TS_ASSERT_LESS_THAN -> CHECK((x) < (y))
  content = replace_binary_macro(
    content, 'TS_ASSERT_LESS_THAN',
    lambda a, b: f'CHECK(({a}) < ({b}))'
  )

  # TS_ASSERT_LESS_THAN_EQUALS -> CHECK((x) <= (y))
  content = replace_binary_macro(
    content, 'TS_ASSERT_LESS_THAN_EQUALS',
    lambda a, b: f'CHECK(({a}) <= ({b}))'
  )

  # TS_ASSERT_DELTA -> CHECK((x) == doctest::Approx(y).epsilon(d))
  content = replace_ternary_macro(
    content, 'TS_ASSERT_DELTA',
    lambda a, b, c: f'CHECK(({a}) == doctest::Approx({b}).epsilon({c}))'
  )

  # GUM_CHECK_ASSERT_ALMOST_EQUALS -> CHECK(fabs((x) - (y)) < 1e-5)
  content = replace_binary_macro(
    content, 'GUM_CHECK_ASSERT_ALMOST_EQUALS',
    lambda a, b: f'CHECK(fabs(({a}) - ({b})) < 1e-5)'
  )

  # GUM_CHECK_ASSERT_QUASI_EQUALS -> CHECK(fabs((x) - (y)) < 1e-10)
  content = replace_binary_macro(
    content, 'GUM_CHECK_ASSERT_QUASI_EQUALS',
    lambda a, b: f'CHECK(fabs(({a}) - ({b})) < 1e-10)'
  )

  return content


def replace_gum_macros(content):
  """Replace TS_GUM_* macros with GUM_CHECK_* / GUM_* equivalents."""

  # Simple replacements (string replace is sufficient)
  replacements = [
    # Tensor macros
    ('TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA', 'GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA'),
    ('TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS', 'GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS'),
    ('TS_GUM_TENSOR_ALMOST_EQUALS_DELTA', 'GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA'),
    ('TS_GUM_TENSOR_ALMOST_EQUALS', 'GUM_CHECK_TENSOR_ALMOST_EQUALS'),
    ('TS_GUM_TENSOR_QUASI_EQUALS', 'GUM_CHECK_TENSOR_QUASI_EQUALS'),
    ('TS_GUM_TENSOR_DIFFERS_SAMEVARS', 'GUM_CHECK_TENSOR_DIFFERS_SAMEVARS'),
    ('TS_GUM_TENSOR_DIFFERS', 'GUM_CHECK_TENSOR_DIFFERS'),
    ('TS_GUM_TENSOR_SHOW_DELTA', 'GUM_CHECK_TENSOR_SHOW_DELTA'),
    # Assert throws nothing
    ('TS_GUM_ASSERT_THROWS_NOTHING', 'GUM_CHECK_ASSERT_THROWS_NOTHING'),
    # Error constants (must be after macros that use them)
    ('TS_GUM_VERY_SMALL_ERROR', 'GUM_VERY_SMALL_ERROR'),
    ('TS_GUM_SMALL_ERROR', 'GUM_SMALL_ERROR'),
  ]

  for old, new in replacements:
    content = re.sub(rf'\b{old}\b', new, content)

  return content


def replace_all_macros(content):
  """Replace all TS_* macros with doctest equivalents."""
  content = replace_simple_macros(content)
  content = replace_throws_macros(content)
  content = replace_comparison_macros(content)
  content = replace_gum_macros(content)
  return content


# =============================================================================
# STEP 3: ADD MISSING SEMICOLONS
# =============================================================================

def add_semicolons(content):
  """Add semicolons after CHECK macros that are missing them."""

  macros = [
    'CHECK', 'CHECK_FALSE', 'CHECK_THROWS', 'CHECK_THROWS_AS',
    'CHECK_NOTHROW', 'REQUIRE', 'FAIL', 'GUM_CHECK_ASSERT_THROWS_NOTHING',
    'GUM_CHECK_TENSOR_ALMOST_EQUALS', 'GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA',
    'GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS', 'GUM_CHECK_TENSOR_DIFFERS',
    'GUM_CHECK_TENSOR_QUASI_EQUALS'
  ]

  for macro in macros:
    pattern = re.compile(rf'(\b{re.escape(macro)}\s*\([^;]*\))(\s*)(\n|\}}|$)')

    def add_semi(match):
      call = match.group(1)
      whitespace = match.group(2)
      following = match.group(3)

      # Check if already has semicolon
      if call.rstrip().endswith(';'):
        return match.group(0)

      # Find the matching closing paren
      paren_start = call.find('(')
      if paren_start == -1:
        return match.group(0)

      return call + ';' + whitespace + following

    content = pattern.sub(add_semi, content)

  return content


# =============================================================================
# STEP 4: FIX COMPLEX CHECK EXPRESSIONS
# =============================================================================

def fix_complex_checks(content):
  """Wrap complex CHECK expressions with || or && in extra parentheses."""

  pattern = re.compile(r'\bCHECK\s*\(')
  result = []
  i = 0

  while i < len(content):
    match = pattern.match(content, i)
    if match:
      paren_start = match.end() - 1
      paren_end = find_matching_paren(content, paren_start)

      if paren_end != -1:
        inner = content[paren_start + 1:paren_end]

        has_logical_op = '||' in inner or '&&' in inner
        stripped = inner.strip()
        already_wrapped = (stripped.startswith('(') and
                           find_matching_paren('(' + stripped[1:], 0) == len(stripped) - 1)

        if has_logical_op and not already_wrapped:
          result.append(content[i:paren_start + 1])
          result.append('(')
          result.append(inner)
          result.append(')')
          result.append(')')
          i = paren_end + 1
          continue

    result.append(content[i])
    i += 1

  return ''.join(result)


# =============================================================================
# STEP 5: CONVERT setUp/tearDown TO CONSTRUCTOR/DESTRUCTOR
# =============================================================================

def extract_method_body(content, method_name):
  """Extract a method's body from content."""
  pattern = rf'void\s+{method_name}\s*\(\s*\)\s*(?:override\s*)?\{{'
  match = re.search(pattern, content)
  if not match:
    return None

  brace_start = match.end() - 1
  brace_end = find_matching_brace(content, brace_start)
  if brace_end == -1:
    return None

  body = content[brace_start + 1:brace_end].strip()
  return body if body else None


def convert_setup_teardown(content, struct_name, original_content=None):
  """Convert setUp/tearDown methods to constructor/destructor."""

  # Check if constructor/destructor already exists
  has_constructor = bool(re.search(rf'\b{struct_name}\s*\(\s*\)', content))
  has_destructor = bool(re.search(rf'~{struct_name}\s*\(\s*\)', content))

  if has_constructor and has_destructor:
    return content

  # Try to get setUp/tearDown from current content or original
  source = original_content if original_content else content
  setup_body = extract_method_body(source, 'setUp')
  teardown_body = extract_method_body(source, 'tearDown')

  if not setup_body and not teardown_body:
    return content

  # Find insertion point
  public_match = re.search(r'public:\s*\n', content)
  if not public_match:
    return content

  method_match = re.search(r'\n(\s*)void\s+test\w+\s*\(\s*\)', content[public_match.end():])
  if not method_match:
    return content

  insert_pos = public_match.end() + method_match.start()
  indent = method_match.group(1)

  new_code = "\n"

  if setup_body and not has_constructor:
    new_code += f"{indent}{struct_name}() {{\n"
    for line in setup_body.split('\n'):
      if line.strip():
        new_code += f"{indent}  {line.strip()}\n"
    new_code += f"{indent}}}\n\n"

  if teardown_body and not has_destructor:
    new_code += f"{indent}~{struct_name}() {{\n"
    for line in teardown_body.split('\n'):
      if line.strip():
        new_code += f"{indent}  {line.strip()}\n"
    new_code += f"{indent}}}\n"

  if new_code.strip():
    content = content[:insert_pos] + new_code + content[insert_pos:]

  # Remove old setUp/tearDown methods
  content = re.sub(r'\n\s*void\s+setUp\s*\(\s*\)\s*\{[^}]*\}\s*\n', '\n', content)
  content = re.sub(r'\n\s*void\s+tearDown\s*\(\s*\)\s*\{[^}]*\}\s*\n', '\n', content)

  return content


# =============================================================================
# STEP 6: ADD TEST REGISTRATIONS WITH GUM_TEST_ACTIF MACRO
# =============================================================================

def add_test_registrations(content, struct_name, filepath=None):
  """Add GUM_CURRENT_SUITE/MODULE defines and GUM_TEST_ACTIF registrations.

  Automatically extracts MODULE and SUITE from filepath pattern:
    module_X/YTestSuite.h -> MODULE=X, SUITE=Y

  Use GUM_TEST_ACTIF for active tests, GUM_TEST_INACTIF to temporarily disable
  a test (generates a compilation warning).
  """

  # Find all test methods
  test_methods = re.findall(r'void\s+(test\w+)\s*\(\s*\)', content)

  if not test_methods:
    return content

  # Find the closing brace of the struct
  struct_match = re.search(rf'struct\s+{struct_name}\s*\{{', content)
  if not struct_match:
    return content

  brace_start = struct_match.end() - 1
  brace_end = find_matching_brace(content, brace_start)
  if brace_end == -1:
    return content

  # Check if registrations already exist
  if 'TEST_CASE_FIXTURE' in content[brace_end:] or 'GUM_TEST_ACTIF(' in content[brace_end:]:
    return content

  # Extract module and suite from filepath: module_X/YTestSuite.h -> X, Y
  module_tag = 'TEST'
  suite_name = struct_name.replace('TestSuite', '')

  if filepath:
    # Extract module from path (e.g., module_BN/... -> BN)
    module_match = re.search(r'module_(\w+)/', str(filepath))
    if module_match:
      module_tag = module_match.group(1)
      # Use GUMBASE instead of BASE to avoid conflict with lrslib's #define BASE
      if module_tag == 'BASE':
        module_tag = 'GUMBASE'

    # Extract suite from filename (e.g., BayesNetTestSuite.h -> BayesNet)
    filename = Path(filepath).stem  # Get filename without extension
    suite_match = re.match(r'(\w+)TestSuite$', filename)
    if suite_match:
      suite_name = suite_match.group(1)

  # Add GUM_CURRENT_SUITE and GUM_CURRENT_MODULE defines after includes
  if 'GUM_CURRENT_SUITE' not in content:
    # Find a good insertion point (after last #include or after #pragma once)
    include_matches = list(re.finditer(r'#\s*include\s*[<"][^>"]+[>"]', content))
    if include_matches:
      insert_pos = include_matches[-1].end()
    else:
      pragma_match = re.search(r'#\s*pragma\s+once', content)
      insert_pos = pragma_match.end() if pragma_match else 0

    defines = f'\n\n#undef GUM_CURRENT_SUITE\n#undef GUM_CURRENT_MODULE\n#define GUM_CURRENT_SUITE {suite_name}\n#define GUM_CURRENT_MODULE {module_tag}\n'
    content = content[:insert_pos] + defines + content[insert_pos:]

    # Recalculate brace_end after insertion
    struct_match = re.search(rf'struct\s+{struct_name}\s*\{{', content)
    brace_start = struct_match.end() - 1
    brace_end = find_matching_brace(content, brace_start)

  # Build registrations using the compact GUM_TEST_ACTIF macro
  registrations = '\n\n'
  for method in test_methods:
    # Convert testMethodName to TestName (remove 'test' prefix)
    name = method[4:]  # Remove 'test' prefix
    registrations += f'GUM_TEST_ACTIF({name})\n'

  # Insert after struct closing brace
  content = content[:brace_end + 1] + registrations + content[brace_end + 1:]

  return content


# =============================================================================
# MAIN MIGRATION FUNCTION
# =============================================================================

def migrate_file(filepath, base_dir=None):
  """Perform full migration of a single test file."""

  filepath = Path(filepath)
  if not filepath.exists():
    print(f"File not found: {filepath}")
    return False

  with open(filepath, 'r') as f:
    content = f.read()

  original = content

  # Get original content from git if available (for setUp/tearDown)
  git_original = None
  if base_dir:
    try:
      rel_path = filepath.relative_to(base_dir)
      git_original = run_git_show(str(rel_path), str(base_dir))
    except:
      pass

  # Step 1: Convert class to struct
  content = convert_class_to_struct(content)

  # Step 2: Replace all TS_* macros
  content = replace_all_macros(content)

  # Step 3: Add missing semicolons
  content = add_semicolons(content)

  # Step 4: Fix complex CHECK expressions
  content = fix_complex_checks(content)

  # Step 5: Find struct name and convert setUp/tearDown
  struct_match = re.search(r'struct\s+(\w+TestSuite)\s*\{', content)
  if struct_match:
    struct_name = struct_match.group(1)
    content = convert_setup_teardown(content, struct_name, git_original)

    # Step 6: Add test registrations if not present
    if 'TEST_CASE_FIXTURE' not in content and 'GUM_TEST_ACTIF(' not in content:
      content = add_test_registrations(content, struct_name, filepath)

  if content != original:
    with open(filepath, 'w') as f:
      f.write(content)
    return True

  return False


def process_directory(base_dir, pattern='module_*/*TestSuite*.h'):
  """Process all test files in a directory."""
  base_path = Path(base_dir)
  test_files = list(base_path.glob(pattern))

  print(f"Found {len(test_files)} files")

  count = 0
  for f in sorted(test_files):
    try:
      if migrate_file(f, base_path):
        print(f"Migrated: {f.name}")
        count += 1
    except Exception as e:
      print(f"Error processing {f.name}: {e}")

  print(f"\nMigrated {count}/{len(test_files)} files")
  return count


def process_module(module_name, base_dir='.'):
  """Process all test files in a specific module."""
  base_path = Path(base_dir)
  pattern = f'{module_name}/*TestSuite*.h'
  test_files = list(base_path.glob(pattern))

  print(f"Found {len(test_files)} files in {module_name}")

  count = 0
  for f in sorted(test_files):
    try:
      if migrate_file(f, base_path):
        print(f"Migrated: {f.name}")
        count += 1
    except Exception as e:
      print(f"Error processing {f.name}: {e}")

  print(f"\nMigrated {count}/{len(test_files)} files")
  return count


# =============================================================================
# MAIN ENTRY POINT
# =============================================================================

def main():
  if len(sys.argv) < 2:
    print(__doc__)
    sys.exit(1)

  if sys.argv[1] == '--all':
    base_dir = sys.argv[2] if len(sys.argv) > 2 else '.'
    process_directory(base_dir)
  elif sys.argv[1] == '--module':
    if len(sys.argv) < 3:
      print("Usage: migration.py --module <module_name> [base_dir]")
      sys.exit(1)
    module_name = sys.argv[2]
    base_dir = sys.argv[3] if len(sys.argv) > 3 else '.'
    process_module(module_name, base_dir)
  else:
    filepath = sys.argv[1]
    base_dir = sys.argv[2] if len(sys.argv) > 2 else None
    if migrate_file(filepath, base_dir):
      print(f"Migrated: {filepath}")
    else:
      print(f"No changes needed: {filepath}")


if __name__ == '__main__':
  main()
