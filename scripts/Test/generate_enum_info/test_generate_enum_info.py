import subprocess
import os

def main():
    os.chdir(os.path.dirname(__file__))
    enum_file_name = "MyEnum"
    enum_file_path = os.path.abspath(f"{enum_file_name}.h")
    cpp_file_dir = "Gitignored/"
    cpp_file_path = os.path.abspath(f"{cpp_file_dir}{enum_file_name}_generated.cpp")
    encoding = "UTF-8"
    subprocess.run(f"python ../../generate_enum_info.py {enum_file_path} {cpp_file_path} {encoding}")
    exit(0)

if __name__ == "__main__":
    main();