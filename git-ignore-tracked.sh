#!/bin/bash
################################################################################
# Name         : git-ignore-tracked
# Author(s)    : Chris Lloyd
# Description  : Simple script to ignore tracked changes to specific config and
#                build files
################################################################################

git update-index --assume-unchanged xfig-3.2.6a/Makefile.in
git update-index --assume-unchanged xfig-3.2.6a/aclocal.m4
git update-index --assume-unchanged xfig-3.2.6a/compile
git update-index --assume-unchanged xfig-3.2.6a/configure
git update-index --assume-unchanged xfig-3.2.6a/depcomp
git update-index --assume-unchanged xfig-3.2.6a/doc/Makefile.in
git update-index --assume-unchanged xfig-3.2.6a/install-sh
git update-index --assume-unchanged xfig-3.2.6a/missing
git update-index --assume-unchanged xfig-3.2.6a/src/Makefile.in
git update-index --assume-unchanged xfig-3.2.6a/src/version.xbm
git update-index --assume-unchanged xfig-3.2.6a/tests/Makefile.in
