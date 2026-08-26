# SpiderMonkey 115.13 configured for UOX3

Herein is configured a minimum set of files from **SpiderMonkey 115.13** required to compile a static library on Windows, Linux/FreeBSD and macOS, to be linked with [Ultima Offline eXperiment 3](https://github.com/UOX3DevTeam/UOX3/) as a scripting engine. Extraneous build files and folders have been stripped from this configuration. For the official SpiderMonkey project visit https://spidermonkey.dev.

Archives with prebuilt static libraries for each platform are included to reduce one-time build dependencies. These archives are automatically extracted and referenced by UOX3's build files, but SpiderMonkey can also be built manually from source by following the optional instructions below (also available in the [main UOX3 README](https://github.com/UOX3DevTeam/UOX3/)):

<details>
  <summary>(Optional) Build <strong>SpiderMonkey 115.13</strong> from source (Windows)</summary>

  1. Install [Rust](https://rustup.rs/), [Python 3.11](https://www.python.org/downloads/windows/), [LLVM/Clang 14 or newer](https://releases.llvm.org/), and [MozTools 4.0](https://github.com/servo/servo-build-deps/releases/download/msvc-deps/moztools-4.0.zip). In Visual Studio Installer, also install the Windows SDK, C++ ATL, and C++ MFC components.
  2. Open Developer Powershell for VS and navigate to `spidermonkey\uox3-mozjs-bridge`, update the paths for your installed tools, then build:

     ```powershell
     $env:MOZJS_FROM_SOURCE = "1"
     Remove-Item Env:MOZJS_ARCHIVE -ErrorAction SilentlyContinue
     $env:CC = "clang-cl"
     $env:CXX = "clang-cl"
     $env:LD = "lld-link"
     $env:PYTHON = "C:\Python311\python.exe"
     $env:PYTHON3 = "C:\Python311\python.exe"
     $env:MOZTOOLS_PATH = "C:\path\to\moztools-4.0"
     $env:LIBCLANG_PATH = "C:\path\to\LLVM\lib"
     cargo build --release
     ```

  3. Create the drop-in archive:

     ```powershell
     Copy-Item target\release\uox3_mozjs_bridge.lib mozjs-115.13-windows-x64-clangcl.lib
     Compress-Archive mozjs-115.13-windows-x64-clangcl.lib mozjs-115.13-windows-x64-clangcl.zip
     ```

  Replace `spidermonkey\mozjs-115.13-windows-x64-clangcl.zip` in your local UOX3 repository with the ZIP you created, then follow normal Windows build instructions.
</details>

---

<details>
  <summary>(Optional) Build <strong>SpiderMonkey 115.13</strong> from source (Linux/macOS/FreeBSD)</summary>

  1. Install dependencies:
     * **Linux / macOS**: Install Python 3.11, Clang and libclang from your preferred package manager (or via Xcode / Homebrew on macOS).
     * **FreeBSD**: Run `sudo pkg install -y rust python311 llvm18 bash pkgconf`
  2. Open Terminal and run from the UOX3 repository:
  >
  > *(One-time operation, if Rust is not already installed on Linux/macOS)*:\
  > `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`\
  > `source "$HOME/.cargo/env"`\
  > \
  > `cd spidermonkey/uox3-mozjs-bridge`\
  > `export MAKE=gmake` *(Required on FreeBSD)*\
  > `export PYTHON3=python3.11` *(Required on FreeBSD)*\
  > `export CC=clang18` *(Required on FreeBSD)*\
  > `export CXX=clang++18` *(Required on FreeBSD)*\
  > `cargo fetch` *(Required on FreeBSD)*\
  > `python3.11 fix_freebsd.py` *(Required on FreeBSD)*\
  > `MOZJS_FROM_SOURCE=1 cargo build --locked --release`\
  > `cd ../../`\
  > `export UOX3_MOZJS_LIBRARY="$PWD/spidermonkey/uox3-mozjs-bridge/target/release/libuox3_mozjs_bridge.a"`
  3. You can now use `./automake.sh` or the manual CMake instructions to build UOX3.

  > **Note for FreeBSD builders**:
  > * `MAKE=gmake` & `PYTHON3=python3.11`: Directs the build to use GNU Make and Python 3.11 explicitly.
  > * `CC=clang18` & `CXX=clang++18`: Ensures the build uses LLVM 18 to avoid breaking C++ standard library deprecations present in newer toolchains.
  > * `python3.11 fix_freebsd.py`: Automatically patches the downloaded `mozjs-sys` dependency to add missing FreeBSD platform support to `jsglue.cpp` and apply upstream C++ fixes in `ExclusiveData.h` (Mozilla Bug 1894423) and `Locale.cpp`.
</details>