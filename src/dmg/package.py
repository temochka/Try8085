#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys, os, shutil
from os import path
from subprocess import call

if len(sys.argv) != 4:
    print "Usage: %s <project_root> <executable path> <release path>" % sys.argv[0]
    sys.exit(0)

_, project_root, executable_path, release_path = sys.argv
dmg_path = '%s.dmg' % release_path

if path.exists(release_path):
    shutil.rmtree(release_path)

if path.exists(dmg_path):
    os.remove(dmg_path)

os.mkdir(release_path)

shutil.copy(path.join(project_root, 'COPYING'), release_path)
shutil.copytree(path.join(project_root, 'examples'), path.join(release_path, 'examples'))
shutil.copytree(executable_path, path.join(release_path, path.basename(executable_path)))
os.symlink('/Applications', path.join(release_path, 'Applications'))
call(['hdiutil', 'create', '-volname', 'Try8085', '-srcfolder', release_path, '-ov', '-format', 'UDZO', dmg_path])
