import sys

if __name__ == "__main__":
    sys.stdout.write(f"[echo_args({sys.argv.__len__()})] ");
    for i in range(0, sys.argv.__len__()):
        sys.stdout.write(f"[{i}]{sys.argv[i]} ")
    sys.stdout.write(f";\n")
    exit(0)