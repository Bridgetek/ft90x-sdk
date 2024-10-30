#!/bin/bash

# For FT90x
make -B TARGET=ft900 BUILD=Debug
make -B TARGET=ft900 BUILD=Release

# For FT93x
make -B TARGET=ft930 BUILD=Debug
make -B TARGET=ft930 BUILD=Release
