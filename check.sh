python3 run_clang_format.py -r include src tests samples

find src -name "*.cpp" -not -path "*/build/_deps/*" | xargs clang-tidy -p build --quiet
