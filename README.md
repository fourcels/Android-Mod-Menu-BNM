# Introduction

Floating mod menu for il2cpp base on [ByNameModding](https://github.com/ByNameModding/BNM-Android)

![preview.gif](images/preview.gif)

# Getting Start

## Edit and Build Android-Mod-Menu-BNM with [Android Studio](https://developer.android.com/studio)

1. edit `app/com/android/support/Menu.kt`

   ```kotlin
   const val TITLE = "Modded by (YourName)"
   const val SUB_TITLE = "whatever here"
   const val START_ICON = "data:image/png;base64,..."
   ```

1. edit `app/cpp/native-lib.cpp`

   ```cpp
   // feature list
   getFeatureList() {}

   // value change
   valueChange() {}

   // hook methods
   OnLoaded() {}
   ```

1. build `app-debug.apk`

   ```
   Build > Generate App Bundles or APKs > Generate APKs
   ```

## Decoding `app-debug.apk` with [apktool](https://apktool.org/)

```zsh
apktool d -f app-debug.apk
```

## Decoding `game.apk` with [apktool](https://apktool.org/)

```zsh
apktool d -f game.apk
```

## Copy `app-debug` files to `game` fold

```zsh
cp app-debug/lib/${ARCH_ABI}/libModBNM.so game/lib/${ARCH_ABI}/libModBNM.so
cp -r app-debug/smali_classes{2,3,4,5} game/
```

## Update `game`'s main activity

1. find `main` activity in `game/AndroidManifest.xml`

   ```xml
   <activity
       android:configChanges="mcc|mnc|locale|touchscreen|keyboard|keyboardHidden|navigation|orientation|screenLayout|uiMode|screenSize|smallestScreenSize|density|layoutDirection|fontScale"
       android:exported="true" android:hardwareAccelerated="false" android:label="@string/app_name"
       android:launchMode="singleTask" android:maxAspectRatio="2.0"
       android:name="com.nutaku.unity.UnityPlayerActivity" android:resizeableActivity="false"
       android:screenOrientation="userLandscape">
       <meta-data android:name="unityplayer.UnityActivity" android:value="true" />
   </activity>
   ```

1. edit `onCreate` method in `game/smali/com/nutaku/unity/UnityPlayerActivity.smali`

   ```smali
   .method protected onCreate(Landroid/os/Bundle;)V
       # invoke Main.start(context)
       invoke-static {p0}, Lcom/android/support/Main;->start(Landroid/content/Context;)V
       ...
   .end method
   ```

## Rebuilding `game` with [apktool](https://apktool.org/)

```zsh
apktool b -f game
```

## Sign `game/dist/game.apk` with [AndroidToolKit](https://github.com/LazyIonEs/AndroidToolKit/) or [apksigner](https://developer.android.com/tools/apksigner)

![sign.png](images/sign.png)

# Credits

- [Android-Mod-Menu](https://github.com/LGLTeam/Android-Mod-Menu) Floating mod menu for il2cpp and
  other native android games.
- [ByNameModding](https://github.com/ByNameModding/BNM-Android) Modding il2cpp games by classes,
  methods, fields names on Android.
- [Dobby](https://github.com/jmpews/Dobby) a lightweight, multi-platform, multi-architecture hook framework.
