# SpiderMonkey

This is the public headers for the SpiderMonkey project that we're using, focusing
on delivering an ESR release rather than cumulative individual releases.

It is built from ESR 115.

All subfolders except `build` are source ones for header files.

## Steps to build

The focus here is to use the [Mozilla provided instructions](https://firefox-source-docs.mozilla.org/js/build.html)
as the starting point.  The instructions at [GitHub for embedding SpiderMonkey](https://github.com/mozilla-spidermonkey/spidermonkey-embedding-examples/blob/esr102/docs/Building%20SpiderMonkey.md)
also provide information about disabling JEMalloc which can lead to errors.

If you run into trouble with not being able to find an SDK under Windows, you can
create a symlink to your existing location if you have one, and try again.

```powershell
Push-Location "$($env:USERPROFILE)/.mozbuild/vs"
New-item -type Junction -Path '.\Windows Kits' -Target 'C:\Program Files (x86)\Windows Kits\'
Pop-Location
```

Note that if using the ESR branch, it may detect a system CLang rather than the bootstrapped one.

In this event, ensure that CLang 16+ is installed.