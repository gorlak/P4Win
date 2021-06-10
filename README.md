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

Now, build dependencies and generate the main solution:

    premake vs2019

You can open the main solution from the command prompt by doing:

    start Build\P4Win.sln

### License

Please refer to
[LICENSE](LICENSE.md).
