#! /usr/bin/env python
# encoding: utf-8

import sys
import os
import inspect
import StringIO
import urllib
import zipfile
import tarfile
import shutil
from waflib import Context


top = '.'
out = 'build'


#################################
# "Global" functions and vars
#################################

def add_to_context(f):
    setattr(Context.Context, f.__name__, f)

@add_to_context
def g(ctx, key):
    return GLOBALS[key]

# a more helpful c/c++ checking function that doesn't
# spew garbage into the DEFINES variable
@add_to_context
def custom_check(self,**kwargs):
    header = kwargs.pop('header_name',None)
    function = kwargs.pop('function_name',None)
    if header:
        if function:
            snippet = "#include <"+header+">\nint main(){void *p; p=(void*)("+function+"); return 0;}\n"
        else:
            snippet = "#include <"+header+">\nint main(){return 0;}\n"
        if not kwargs.get('lib') and not kwargs.get('msg'):
            if function:
                kwargs['msg'] = 'Checking for function {}'.format(function)
            else:
                kwargs['msg'] = 'Checkinf for header {}'.format(header)
    return self.check(**kwargs)

@add_to_context
def include_dirs(self, suffix=None):
    dd = self.root.find_node(Context.top_dir).find_node('include')
    if suffix:
        dd = dd.make_node(suffix)
    return [dd, dd.get_bld()]


################################
# Standard waf stuffs
################################

def options(self):
    self.load('compiler_cxx')

    self.add_option('--enable-debug', default=False, action='store_true', dest='debug',
            help='to compile in debug mode')

    self.recurse('src')

def configure(self):
    self.load('compiler_cxx')

    #c++ compiler debug flags
    if self.options.debug:
        self.env.append_value('CXXFLAGS', '-g')
    else:
        self.env.append_value('CXXFLAGS', '-O3')
    #if GNU, then use -Wall
    if self.env.CXX_NAME == 'gcc':
        self.env.append_value('CXXFLAGS', '-Wall') 

    self.env.LIB_INSTALL_PATH = self.env.LIBDIR + "audio_test"

    #get the system copy command
    if sys.platform == 'win32':
        self.env.COPY = 'copy'
    else:
        self.env.COPY = 'cp'

    self.recurse('src')

def build(self):
    self.recurse('src')

    #copy the executable to the root of the build directory
    self(rule="${COPY} ${SRC} ${TGT}", source=self.path.make_node('src/main/app').get_bld(), target='audio_test')

def run(self):
    p = self.root.find_node(Context.top_dir).find_node('build/audio_test').abspath()
    os.system(p)
