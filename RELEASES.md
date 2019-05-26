[![Build Status](https://ci.appveyor.com/api/projects/status/github/gorlak/P4Win?branch=master&svg=true)](https://ci.appveyor.com/project/GeoffEvans/p4win)

### Releases

To make a release:

* Update version in .appveyor.yml
* Update version in Source/gui/resource.h
* Commit
* `git tag -a -m "<version>" && git push --tags`
* Push draft release on github.com
