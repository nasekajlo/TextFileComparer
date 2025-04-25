import os

from pkg_resources import Environment
from SCons.Script import Environment, Default
from SCons.Script import Default

env = Environment()


env['CXXFLAGS'] = ['-std=c++17']

# path to the cpp files
sources = ['src/file_parser.cpp', 'src/comparer.cpp', 'src/matcher.cpp', 'src/gui.cpp', 'src/merger.cpp']
env.Program(target='TextFileComparer', source=sources)

# libs add
env.Append(LIBS=['boost_system', 'boost_filesystem', 'Qt5Widgets', 'icu'])

Default('TextFileComparer')
