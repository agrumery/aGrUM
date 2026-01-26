#!/usr/bin/env python3
"""
Convert existing TEST_CASE_FIXTURE to GUM_TEST macro.

This script:
1. Extracts MODULE and SUITE from filepath (module_X/YTestSuite.h -> X, Y)
2. Adds #define GUM_CURRENT_SUITE and GUM_CURRENT_MODULE
3. Replaces TEST_CASE_FIXTURE(...) { testXxx(); } with GUM_TEST(Xxx)

Usage:
    python convert_to_gum_test.py                    # Convert all test files
    python convert_to_gum_test.py --module module_BN # Convert specific module
    python convert_to_gum_test.py file.h             # Convert single file
"""

import re
import sys
from pathlib import Path


def extract_module_suite(filepath, content=None):
    """Extract MODULE and SUITE from filepath pattern module_X/YTestSuite.h

    If content is provided, extracts the struct name from the file content
    to handle cases where filename capitalization differs from struct name.
    """
    filepath_str = str(filepath)

    # Extract module from path
    module_match = re.search(r'module_(\w+)/', filepath_str)
    module = module_match.group(1) if module_match else None
    # Use GUMBASE instead of BASE to avoid conflict with lrslib's #define BASE
    if module == 'BASE':
        module = 'GUMBASE'

    # Try to extract suite from struct name in content (more reliable)
    suite = None
    if content:
        struct_match = re.search(r'struct\s+(\w+)TestSuite\s*\{', content)
        if struct_match:
            suite = struct_match.group(1)

    # Fallback to filename
    if not suite:
        filename = Path(filepath).stem
        suite_match = re.match(r'(\w+)TestSuite$', filename)
        suite = suite_match.group(1) if suite_match else None

    return module, suite


def convert_file(filepath):
    """Convert a single file to use GUM_TEST macro."""
    filepath = Path(filepath)

    with open(filepath, 'r') as f:
        content = f.read()

    original = content

    # Check if already converted
    if 'GUM_CURRENT_SUITE' in content:
        return False

    # Check if has TEST_CASE_FIXTURE to convert
    if 'TEST_CASE_FIXTURE' not in content:
        return False

    module, suite = extract_module_suite(filepath, content)
    if not module or not suite:
        print(f"  Warning: Could not extract module/suite from {filepath}")
        return False

    # Add defines after last #include
    include_matches = list(re.finditer(r'#\s*include\s*[<"][^>"]+[>"]', content))
    if include_matches:
        insert_pos = include_matches[-1].end()
        defines = f'\n\n#define GUM_CURRENT_SUITE {suite}\n#define GUM_CURRENT_MODULE {module}\n'
        content = content[:insert_pos] + defines + content[insert_pos:]

    # Replace TEST_CASE_FIXTURE with GUM_TEST
    # Pattern: TEST_CASE_FIXTURE(XxxTestSuite, "[MODULE][Suite] TestName") { testTestName(); }
    pattern = re.compile(
        r'TEST_CASE_FIXTURE\s*\(\s*\w+TestSuite\s*,\s*"[^"]+"\s*\)\s*\{\s*test(\w+)\s*\(\s*\)\s*;\s*\}',
        re.MULTILINE
    )

    def replace_fixture(match):
        test_name = match.group(1)
        return f'GUM_TEST({test_name})'

    content = pattern.sub(replace_fixture, content)

    # Also handle multi-line TEST_CASE_FIXTURE
    pattern_multiline = re.compile(
        r'TEST_CASE_FIXTURE\s*\(\s*\w+TestSuite\s*,\s*"[^"]+"\s*\)\s*\{\s*\n\s*test(\w+)\s*\(\s*\)\s*;\s*\n\s*\}',
        re.MULTILINE
    )
    content = pattern_multiline.sub(replace_fixture, content)

    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        return True

    return False


def process_directory(base_dir, pattern='module_*/*TestSuite*.h'):
    """Process all test files in a directory."""
    base_path = Path(base_dir)
    test_files = list(base_path.glob(pattern))

    print(f"Found {len(test_files)} test files")

    converted = 0
    for f in sorted(test_files):
        try:
            if convert_file(f):
                print(f"  Converted: {f.name}")
                converted += 1
        except Exception as e:
            print(f"  Error processing {f.name}: {e}")

    print(f"\nConverted {converted}/{len(test_files)} files")
    return converted


def process_module(module_name, base_dir='.'):
    """Process all test files in a specific module."""
    base_path = Path(base_dir)
    pattern = f'{module_name}/*TestSuite*.h'
    test_files = list(base_path.glob(pattern))

    print(f"Found {len(test_files)} files in {module_name}")

    converted = 0
    for f in sorted(test_files):
        try:
            if convert_file(f):
                print(f"  Converted: {f.name}")
                converted += 1
        except Exception as e:
            print(f"  Error processing {f.name}: {e}")

    print(f"\nConverted {converted}/{len(test_files)} files")
    return converted


def main():
    if len(sys.argv) < 2:
        # Convert all files
        process_directory('.')
    elif sys.argv[1] == '--module':
        if len(sys.argv) < 3:
            print("Usage: convert_to_gum_test.py --module <module_name>")
            sys.exit(1)
        process_module(sys.argv[2])
    else:
        # Single file
        filepath = sys.argv[1]
        if convert_file(filepath):
            print(f"Converted: {filepath}")
        else:
            print(f"No changes needed: {filepath}")


if __name__ == '__main__':
    main()
