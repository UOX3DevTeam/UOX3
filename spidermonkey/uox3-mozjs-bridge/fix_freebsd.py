import glob
import os
import re

home = os.path.expanduser("~")

# 1. Patch ExclusiveData.h (Mozilla Bug 1894423)
exclusive_paths = glob.glob(f"{home}/.cargo/git/checkouts/mozjs-*/fb8225e/mozjs-sys/mozjs/js/src/threading/ExclusiveData.h")
for path in exclusive_paths:
    print(f"Patching {path}...")
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()
    patched = re.sub(r'ExclusiveData\(ExclusiveData&&[^{]+\{[^}]+\}', '', content)
    with open(path, "w", encoding="utf-8") as f:
        f.write(patched)

# 2. Patch Locale.cpp (char_traits<unsigned char>)
locale_paths = glob.glob(f"{home}/.cargo/git/checkouts/mozjs-*/fb8225e/mozjs-sys/mozjs/js/src/builtin/intl/Locale.cpp")
for path in locale_paths:
    print(f"Patching {path}...")
    with open(path, "r", encoding="utf-8") as f:
        lines = f.readlines()
    patch_line = "namespace std { template<> struct char_traits<unsigned char> { static const unsigned char* find(const unsigned char* s, size_t n, unsigned char a) { return static_cast<const unsigned char*>(memchr(s, a, n)); } }; }\n"
    if patch_line not in lines:
        lines.insert(30, patch_line)
        with open(path, "w", encoding="utf-8") as f:
            f.writelines(lines)

# 3. Patch jsglue.cpp (unsupported platform on FreeBSD)
jsglue_paths = glob.glob(f"{home}/.cargo/git/checkouts/mozjs-*/fb8225e/mozjs-sys/src/jsglue.cpp")
for path in jsglue_paths:
    print(f"Patching {path}...")
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()
    patched = content.replace("defined(__linux__)", "(defined(__linux__) || defined(__FreeBSD__))")
    with open(path, "w", encoding="utf-8") as f:
        f.write(patched)

print("FreeBSD patches applied successfully!")
