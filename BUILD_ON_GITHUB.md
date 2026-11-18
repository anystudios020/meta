# Build the Apple Silicon VST3 without installing Xcode locally

This project includes a GitHub Actions workflow at `.github/workflows/build-vst3.yml`.
It builds an ARM64 VST3 targeting macOS 12 Monterey or later, runs the standalone
plate-model tests, and packages the plugin into a drag-and-drop DMG.

## One-time setup

1. Sign in to GitHub and create a **new repository**. A public repository is easiest
   for this workflow because standard hosted runners are free for public repositories.
2. Upload the contents of this project folder to the repository (not the ZIP file
   itself). Make sure the `.github/workflows/build-vst3.yml` file is included.
3. Open the repository's **Actions** tab.
4. Select **Build METAPHYSICAL VST3 (Apple Silicon)** and click **Run workflow**.
5. Wait for the run to finish. If it fails, open the run and download or copy the
   error log so it can be diagnosed.
6. On a successful run, open the run's **Artifacts** section and download
   `METAPHYSICAL-Apple-Silicon-DMG`. GitHub downloads it as a ZIP; unzip it to
   reveal `METAPHYSICAL-Apple-Silicon.dmg`.

## Install on the Mac

Open the DMG, then follow `INSTALL.txt` inside it. This is a drag-and-drop disk
image, not a signed/notarized commercial installer. macOS may warn because the
plugin is unsigned. The plugin is built for Apple Silicon and targets macOS 12+.

## Important

The workflow builds the VST3 only. It does not sign or notarize the plugin.
The code remains an early analytical prototype; a successful build does not
mean the physical model has been experimentally validated or tested in Ableton.
