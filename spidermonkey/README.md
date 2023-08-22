# SpiderMonkey

This is the public headers for the SpiderMonkey project that we're using, focusing
on delivering an ESR release rather than cumulative individual releases.

It is built from ESR 115.

All subfolders except `build` are source ones for header files.

## Steps to build

The focus here is to use the [Mozilla provided instructions](https://firefox-source-docs.mozilla.org/js/build.html)
as the starting point.  The instructions at [GitHub for embedding SpiderMonkey](https://github.com/mozilla-spidermonkey/spidermonkey-embedding-examples/blob/esr102/docs/Building%20SpiderMonkey.md)
also provide information about disabling JEMalloc which can lead to errors.

Once you have the code boostrapped, you'll want to switch to an appropriate ESR bookmark

```
hg update esr115
```

If there have been changes to the ESR branch since your last build, consider

```
hg pull && hg update
```

### Cannot find Windows SDK ###

If you run into trouble with not being able to find an SDK under Windows, you can
create a symlink to your existing location if you have one, and try again.

```powershell
Push-Location "$($env:USERPROFILE)/.mozbuild/vs"
New-item -type Junction -Path '.\Windows Kits' -Target 'C:\Program Files (x86)\Windows Kits\'
Pop-Location
```

Note that if using the ESR branch, it may detect a system CLang rather than the bootstrapped one.

In this event, ensure that CLang 16+ is installed.

### Compile errors around JSProto_LIMIT ###

If you're seeing errors like:

```
1>D:\UOX3\spidermonkey\jspubtd.h(55,7): error C2146: syntax error: missing '}' before identifier 'JSProto_LIMIT' (compiling source file ..\..\source\cConsole.cpp)
1>D:\UOX3\spidermonkey\jspubtd.h(56,1): error C2143: syntax error: missing ';' before '}' (compiling source file ..\..\source\cConsole.cpp)
1>D:\UOX3\spidermonkey\jspubtd.h(56,1): error C2059: syntax error: '}' (compiling source file ..\..\source\cConsole.cpp)
```

Then it may be that the compiler is experiencing an issue with conformance.  To resolve, update `jspubtd.h` as:

```c++
enum JSProtoKey {
#define PROTOKEY_AND_INITIALIZER(name, clasp) JSProto_##name,
  JS_FOR_EACH_PROTOTYPE(PROTOKEY_AND_INITIALIZER)
#undef PROTOKEY_AND_INITIALIZER
      ,JSProto_LIMIT // This line here
};
```