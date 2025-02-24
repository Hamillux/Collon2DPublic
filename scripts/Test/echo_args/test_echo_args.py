import subprocess
import os

def main():
    os.chdir(os.path.dirname(__file__))
    script_path = os.path.abspath("../../echo_args.py")
    subprocess.run(f"python {script_path} argA argB argC")
    exit(0)

if __name__ == "__main__":
    main();