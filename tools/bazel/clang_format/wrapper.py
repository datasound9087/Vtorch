import subprocess
import sys

# Wrapper script to invoke clang-format
if __name__ == "__main__":
    # Write out params
    with open(sys.argv[6], "w") as f:
        f.write(" ".join(sys.argv[1:6]))

    ran_command = subprocess.run(sys.argv[1:5])
    sys.exit(ran_command.returncode)