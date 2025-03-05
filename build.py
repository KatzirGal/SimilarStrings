import subprocess
import os
import sys
import argparse
import platform
import shutil

def is_unix_environment():
    """Check if the script is running on a Unix-like system."""
    return platform.system() != "Windows"

def run_command(command, cwd=None):
    try:
        result = subprocess.run(command, cwd=cwd, check=True, shell=True, text=True, stdout=sys.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error while executing command: {e.cmd}")
        print(e.stderr)
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Build Gutsy using CMake.")
    parser.add_argument("--config", choices=["Release", "Debug", "RelWithDebInfo"], default="Release", help="Build configuration (default: Release)")
    args = parser.parse_args()

    cmake_build_folder = "CMAKE_BUILD"

    # Create build directory
    os.makedirs(cmake_build_folder, exist_ok=True)

    # Step 1: Call cmake to generate project
    print("Running CMake to generate the project...")
    if is_unix_environment():
        def find_unix_compiler():
            for compiler in ['g++-13']:
                if shutil.which(compiler):
                    return shutil.which(compiler)
            raise RuntimeError("No suitable compiler found (Should be g++-13)")
        unix_compiler = find_unix_compiler()

        poco_cmake_dir = os.path.abspath(f'Dependencies/CMAKE_BUILD/install/{args.config}/lib/cmake/Poco')
        
        run_command(f'cmake -B {cmake_build_folder} -G "Unix Makefiles" -DCMAKE_BUILD_TYPE={args.config} -DCMAKE_CXX_COMPILER={unix_compiler} -DCMAKE_PREFIX_PATH={poco_cmake_dir} -DPoco_DIR={poco_cmake_dir} -DPocoFoundation_DIR={poco_cmake_dir}', cwd='Dependencies')
        run_command(f'cmake --build {cmake_build_folder} --config {args.config} --target Poco', cwd='Dependencies')
        run_command(f'cmake -B {cmake_build_folder} -G "Unix Makefiles" -DCMAKE_BUILD_TYPE={args.config} -DCMAKE_CXX_COMPILER={unix_compiler} -DCMAKE_PREFIX_PATH={poco_cmake_dir} -DPoco_DIR={poco_cmake_dir} -DPocoFoundation_DIR={poco_cmake_dir}')
    else:
        poco_cmake_dir = os.path.abspath(f'Dependencies/CMAKE_BUILD/install/{args.config}/cmake')
        run_command(f'cmake -B {cmake_build_folder} -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE={args.config} -DPoco_DIR={poco_cmake_dir}', cwd='Dependencies')
        run_command(f'cmake --build {cmake_build_folder} --config {args.config} --target Poco', cwd='Dependencies')
        run_command(f'cmake -B {cmake_build_folder} -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE={args.config} -DPoco_DIR={poco_cmake_dir}')



    # Step 2: Call cmake --build
    print("Building the project using CMake...")
    run_command(f'cmake --build {cmake_build_folder} --config {args.config}')

    build_folder = "build" if is_unix_environment() else f'x64\\{args.config}'

    # Step 3: Print how to execute SimilarStrings
    print("\nBuild complete!")
    print("To execute the application, use the following command:")
    print(f'/{build_folder}/SimilarStrings <path to dictionary>' if is_unix_environment() else f'{build_folder}\\SimilarStrings.exe <path to dictionary>')

if __name__ == "__main__":
    main()
