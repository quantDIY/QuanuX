---
name: python_packaging
description: Guidelines for creating robust Python extension setup.py files that survive CI environments.
---
# Python Packaging in QuanuX

When creating or modifying `setup.py` files for Python extensions (especially those using Cython, PyBind11, or Numpy), you must ensure they are robust against missing build-time dependencies.

## The Problem
CI/CD systems (like GitHub's Dependency Submission Action) often run in environments where build dependencies like `Cython` or `numpy` are NOT installed. If `setup.py` unconditionally imports them, the dependency graph generation fails.

## The Solution: Robust Setup Pattern
Always wrap build-time imports in `try-except` blocks. If they are missing, the script should still run successfully but yield an empty list of extensions.

### Template

```python
import os
from setuptools import setup, Extension

# 1. Wrap build dependencies
try:
    from Cython.Build import cythonize
    import numpy 
except ImportError:
    cythonize = None
    numpy = None

ext_modules = []

# 2. Conditional Build Logic
if cythonize and numpy:
    # Define extensions here
    extensions = [
        Extension(
            "my_extension",
            sources=["src/my_extension.pyx"],
            include_dirs=[numpy.get_include()],
            # ...
        )
    ]
    ext_modules = cythonize(extensions, language_level=3)
else:
    print("Build dependencies (Cython/Numpy) not found. Skipping extension build.")
    # This print is important for debugging but allows the script to exit with code 0

# 3. Setup Call
setup(
    name="my_package",
    ext_modules=ext_modules, # Will be empty if deps missing
    # ... standard metadata ...
)
```

## Checklist
- [ ] Import `Cython.Build.cythonize` inside `try-except ImportError`.
- [ ] Import `numpy` inside `try-except ImportError` (if used).
- [ ] Check for existence of required directories (e.g. `_deps`, `sdk`) before adding them to `include_dirs`.
- [ ] Ensure `python setup.py --name` runs successfully in a bare environment.
