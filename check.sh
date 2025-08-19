set -e

python3 run_clang_format.py -r include src tests samples

find src -name *.cpp | xargs clang-tidy -p build --quiet
