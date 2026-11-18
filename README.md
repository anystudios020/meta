# METAPHYSICAL v0.1 — Glass Plate Prototype

**ANY Studios · Physical modelling instrument**

This is the first source-code milestone for METAPHYSICAL. It contains a JUCE plugin project targeting macOS VST3 and AU, plus a standalone test for the plate-mode model.

## Current prototype scope

- Rectangular, homogeneous, isotropic glass plate.
- Kirchhoff–Love thin-plate modal frequencies for a simply-supported rectangular plate.
- Spatial impact coupling based on the mode shape at the selected impact point.
- Velocity/mass-based impulse excitation from a simplified metal-rod contact model.
- Modal decay controlled by a loss-factor approximation.
- MIDI note-on and the GUI STRIKE OBJECT button trigger an impact.
- Adjustable width, height, thickness, impact position, rod velocity, rod mass, modal loss and output.
- Minimal dark ANY Studios interface with a plate illustration and impact marker.

## Important scientific limits

This is a **first analytical prototype**, not yet a validated full physical simulation. The plate modes currently assume simply-supported boundaries and thin-plate Kirchhoff–Love theory. The contact event is an impulse approximation; it does not yet solve nonlinear Hertzian contact, rod bending, finite contact duration, air loading, structural support coupling, fracture, crack propagation or fragment collisions.

The model intentionally clamps dimensions and material parameters to numerical safety limits. A 400 m plate is therefore an exploratory parameter case, not a claim that a real 400 m glass plate can exist as a self-supporting object. Structural plausibility and gravity-induced stress analysis are future work. The `isPlausible()` check currently flags only the thin-plate aspect-ratio assumption.

The default glass constants are representative values, not a calibrated dataset for a particular glass composition or tempering process. Modal amplitudes and output scaling require calibration against measured impulse responses before scientific accuracy can be claimed.

## Build on macOS

Requirements:
- Xcode Command Line Tools / Xcode
- CMake 3.22+
- Git
- Internet access on the first configure, to fetch JUCE 8.0.6

From this folder:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target METAPHYSICAL_VST3 METAPHYSICAL_AU
```

The resulting bundles will be under the CMake build tree, in the JUCE-generated plugin build directories. Copy the VST3 bundle to `~/Library/Audio/Plug-Ins/VST3/` and the AU component to `~/Library/Audio/Plug-Ins/Components/`, then rescan plugins in Ableton Live. Do not copy until the build completes successfully.

## Run the standalone model test

```bash
cmake --build build --config Release --target PlateModelTests
ctest --test-dir build --output-on-failure
```

## Suggested first Ableton test

1. Create a MIDI track and load METAPHYSICAL.
2. Create a MIDI clip with a short note; every note-on triggers one plate impact.
3. Alternatively press **STRIKE OBJECT** in the plugin.
4. Change width and height to hear the modal pattern move with geometry.
5. Move IMPACT X/Y away from the centre to change which modes are excited.

## Next development stages

1. Replace the impulse approximation with a finite-duration nonlinear contact model (rod tip geometry, contact stiffness, effective mass and restitution).
2. Add selectable support conditions and validate modal frequencies against analytical references and measured plates.
3. Add physically informed radiation / pickup modelling and calibrated amplitude units.
4. Add stress, gravity and plausibility analysis.
5. Add fracture initiation, crack propagation and fragment collision as a separate energy-conserving event system.
6. Add other geometries, materials and coupled objects.

## Project status

Source project created. The plugin has **not** been compiled or tested in Ableton in this environment. A successful local macOS build and host test are required before calling this an installable release.

### Known v0.1 scale limitation

The current implementation retains a low-order mode set (orders 1–12, capped at 64 modes) and ignores modes below 8 Hz. A very large plate can therefore have no represented modes in this first prototype. This is deliberately exposed by the model test rather than hidden with artificial pitch shifting. Supporting very large structures requires a higher-order / adaptive modal basis, a better radiation model, and structural stability analysis; this is a priority for the next physics iteration.
