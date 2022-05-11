# Utilities

### Automatically format C++ code using `clang-format` when running `git commit`

If you copy `pre-commit` inside `.git/hooks/` (you might need to make it executable using `chmod +x .git/hooks/pre-commit`), the C++ files you staged will be automatically formatted using `clang-format` and re-staged. A prerequisite is to have `clang-format` installed (you can install it using a package manager).

### `clang-tidy`

clang-tidy gives you automatically generated reports, on how to improve the code quality. 
To get such an report:
1. `cd build`
2. `cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
3. `python /path/to/utilities/run-clang-tidy.py -header-filter=../src
