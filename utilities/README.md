# Utilities

### Automatically format C++ code using `clang-format` when running `git commit`

If you copy `pre-commit` inside `.git/hooks/` (you might need to make it executable using `chmod +x .git/hooks/pre-commit`), the C++ files you staged will be automatically formatted using `clang-format` and re-staged. A prerequisite is to have `clang-format` installed (you can install it using a package manager).