import subprocess
import os

def main():
    os.chdir(os.path.dirname(__file__))
    file_name = "hoge"
    source_path = os.path.abspath(f"Source/{file_name}.cpp")
    generated_path = os.path.abspath(f"Gitignored/{file_name}_generated.cpp")
    encoding = "UTF-8"
    subprocess.run(f"python ../../code_generator.py {source_path} {generated_path} {encoding}")
    exit(0)

if __name__ == "__main__":
    main();