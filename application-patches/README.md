# Termux application patches

Various patches for the [Termux application](https://github.com/termux/termux-app/).
They may contain improvements, new features and fixes that probably will not
be available in the official version.

## How to apply patches

1. Pick a patch you want and read its name. For example, patch with the
following name
   ```
   0000-termux-v0.68-merge-styling-addon.git.patch
   ```
   means that it:
   * Should be applied on Termux application with exact version v0.68.
   * Will merge Termux:Styling addon with termux-app sources.

2. Checkout necessary version of Termux application. In our case it is v0.68:
   ```
   git clone https://github.com/termux/termux-app
   cd ./termux-app
   git checkout v0.68
   ```

3. Apply git patch:
   ```
   git am ../0000-termux-v0.68-merge-styling-addon.git.patch
   ```
   Once command finishes successfully, a new commit should be added.
