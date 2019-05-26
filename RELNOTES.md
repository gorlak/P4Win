[![Build Status](https://ci.appveyor.com/api/projects/status/github/gorlak/P4Win?branch=master&svg=true)](https://ci.appveyor.com/project/GeoffEvans/p4win)

### Releases

#### 2008.1.2

* Update to p4api 2018.2, a major release that includes the new client-side scripts (lua)
* Update p4api build configuration to similar libs found in Perforce Software's build
* Turn on warnings as errors

#### 2008.1.1

Initial release of the final release of P4Win from Perforce Software open source

* Minimal functional changes
* Added "awareness" of stream depots, compatible but no stream depot features
* Updated for x64 native compile
* Included appropriate p4.exe built from the same source
* Removed resource DLLs (no Japan codepage resources at the current time)
* Implement AppVeyor CI + Github release deployment
