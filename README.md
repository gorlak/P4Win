
### About This Project

From Perforce Software:

> P4Win is the Perforce client for Windows that was first made available as part of the 99.1 release. It is no longer being maintained or supported by Perforce Software, but the source is now available through the Workshop. See for build instructions.

This repository is a fork that aims to pick up where Perforce Software left off, and advance the tool to have features accessible in more modern version of p4d (AKA Helix).

Energy is being put into this mainly because:
* P4V eschews efficient workflow idioms custom to windows
* P4V feature delivery is slow
* P4V has bugs and inefficiencies

### Compiling

First, grab our source tree from git and ensure that you fetch all the submodules by doing:

    git submodule update --init

Now, build dependent libs. Open a visual studio command prompt for _the architecture you want to use, Win32 or x64_.  Then:

    cd Dependencies
    openssl-build.bat

Now, generate the main solution:

    premake vs2017
    start Build\P4Win.sln

### License

Please refer to
[LICENSE](LICENSE.md).
