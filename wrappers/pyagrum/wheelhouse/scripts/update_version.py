import sys
import os.path


def main():
  if len(sys.argv) != 3:
    print("Please indicate project base dir and build number")
    sys.exit(1)

  base_dir = sys.argv[1]

  agrum_version_file = os.path.join(base_dir, 'VERSION.txt')

  if not os.path.isfile(agrum_version_file):
    print("Cannot find aGrUM version file.")
    sys.exit(1)

  build = sys.argv[2]
  with open(agrum_version_file, 'a') as f:
    f.write('set(AGRUM_VERSION_TWEAK "{0}")'.format(build))


if __name__ == "__main__":
  main()
