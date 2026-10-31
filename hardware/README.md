# /hardware

Design files for the current version (V3.2). Older versions (V1 to V3.1) are kept in the [`archive-all-versions`](https://github.com/C182R/3DOF-Picker-Placer/tree/archive-all-versions/hardware) tag.

```
hardware/
├── solidworks-cad/  # SolidWorks source. Top assembly: palletino.SLDASM
├── step/     # STEP AP214 exports for non-SolidWorks CAD. Top assembly: palletino.STEP
├── gcode/    # sliced print plates (.gcode.3mf)
└── wiring/   # wiring diagrams (.yml source, .svg / .png renders)
```

## Naming

- lowercase kebab-case, no spaces
- `buy-` prefix = purchased part (bearings, fasteners, electronics); no prefix = part you print or machine
- rods and spacers: `rod-<diameter>x<length>mm`, `spacer-<diameter>x<thickness>mm`

The STEP assembly references its parts by file name, so keep every file in `step/` together and don't rename them by hand. In SolidWorks, rename through Pack and Go or *Rename* in the feature tree so references update.

Large files are stored with Git LFS. Install it (`git lfs install`) before cloning.
