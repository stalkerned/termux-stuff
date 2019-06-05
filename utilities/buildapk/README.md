# APK building scripts

These scripts allow to build simple Android applications with Termux. Note
that project structure is very basic and incompatible with Android Studio
or Eclipse.

## How to use

1. Copy scripts to Termux /bin directory:
   ```
   cp buildapk create-android-app $PREFIX/bin/
   chmod 700 $PREFIX/bin/{buildapk,create-android-app}
   ```

2. Install script runtime dependencies:
   ```
   pkg install -yq aapt apksigner dx ecj findutils grep sed
   ```

3. Create new Android application project:
   ```
   create-android-app --name MyApp --package com.myapp MyAp
   ```

4. [optional] Edit source files.

5. Build APK file:
   ```
   buildapk -o MyApp.apk
   ```
   If your application uses some JARs, you may specify their path:
   ```
   buildapk -l /path/to/jars_directory -o MyApp.apk
   ```
