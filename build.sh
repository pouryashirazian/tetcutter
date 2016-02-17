#!/bin/bash

echo "This script builds the tetcutter library and examples"
mkdir bin
pushd bin
cmake ..

make

popd

echo "Make documentation"
pushd doc

doxygen

popd 
