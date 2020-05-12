@echo off
pushd ..
pushd ..
del *.o
make -f Build/Windows/makefile.os3
popd
popd
