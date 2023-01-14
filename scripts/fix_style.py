#!/usr/bin/env python3

import os
import subprocess
import sys
import argparse

os.chdir(os.path.dirname(__file__) + "/..")

def recursive_file_list(path):
	result = []
	for dirpath, _, filenames in os.walk(path):
		result += [os.path.join(dirpath, filename) for filename in filenames]
	return result

IGNORE_FILES = [
	"src/engine/client/keynames.h",
	"src/engine/keys.h",
]
def filter_ignored(filenames):
	return [filename for filename in filenames
		if filename not in IGNORE_FILES
		and not filename.startswith("src/game/generated/")]

def filter_cpp(filenames):
	return [filename for filename in filenames
		if any(filename.endswith(ext) for ext in ".c .cpp .h".split())]

def find_clang_format(version):
	for binary in (
		"clang-format",
		f"clang-format-{version}",
		f"/opt/clang-format-static/clang-format-{version}"):
		try:
			out = subprocess.check_output([binary, "--version"])
		except FileNotFoundError:
			continue
		if f"clang-format version {version}." in out.decode("utf-8"):
			return binary
	print(f"Found no clang-format {version}")
	sys.exit(-1)

clang_format_bin = find_clang_format(10)

def reformat(filenames):
	subprocess.check_call([clang_format_bin, "-i"] + filenames)

def warn(filenames):
	return subprocess.call([clang_format_bin, "-Werror", "--dry-run"] + filenames)

def main():
	p = argparse.ArgumentParser(description="Check and fix style of changed files")
	p.add_argument("-n", "--dry-run", action="store_true", help="Don't fix, only warn")
	args = p.parse_args()
	filenames = filter_ignored(filter_cpp(recursive_file_list("src")))
	if not args.dry_run:
		reformat(filenames)
	else:
		sys.exit(warn(filenames))

if __name__ == "__main__":
	main()
