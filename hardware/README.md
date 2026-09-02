# /hardware

SolidWorks source files and exports go here once you're ready to commit them. Suggested layout:

```
hardware/
├── solidworks/        # .SLDPRT, .SLDASM, .SLDDRW source files
├── step/               # .STEP exports — for anyone opening this in non-SolidWorks CAD
└── stl/                # .STL exports, ready to slice — one subfolder per version (v1/, v2/)
```

Notes:
- SolidWorks binary files don't diff well in git — see the root `.gitignore` for temp/backup file patterns to exclude (`~$*`, `*.SLDPRT.1`, etc.)
- Keep STEP exports in sync with source files on every version bump so non-SolidWorks contributors can still open things
- STLs should be per-version so old prints stay reproducible even after the design moves on
