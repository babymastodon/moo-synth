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


################################
# Download helpers
################################

@add_to_context
def download_and_extract(self, url, dirname, compression='zip'):
    print "Downloading {}".format(url)

    o={'counter':-1}
    def pr(numblocks, block_size, total_size):
        dl_size = numblocks*block_size
        c = 10*dl_size/total_size
        if c > o['counter']:
            o['counter']= c
            print "{} of {} bytes -- {}%".format(dl_size, total_size, 10*c)

    def rm(path):
        if os.path.isdir(path):
            shutil.rmtree(path)
        if os.path.isfile(path) or os.path.islink(path):
            os.remove(path)

    filename, headers = urllib.urlretrieve(url, reporthook=pr)
    out_dir = self.download_dir().make_node(dirname).abspath()
    rm(out_dir)
    if compression=='zip':
        with open(filename) as f:
            z = zipfile.ZipFile(f)
            z.extractall(out_dir)
    elif compression in ['gzip', 'bz2']:
        t = tarfile.open(name=filename, mode='r')
        t.extractall(out_dir)
    else:
        self.fatal("Compression format {} is not supported".format(compression))

    #flatten the extracted stuff if there's only one directory inside
    if len(os.listdir(out_dir))==1:
        inner_dirname = os.listdir(out_dir)[0]
        tmp_dir = self.download_dir().make_node('temp_dir').abspath()
        rm(tmp_dir)
        shutil.move(out_dir, tmp_dir)
        shutil.move(os.path.join(tmp_dir,inner_dirname), out_dir)
        rm(tmp_dir)

@add_to_context
def download_dir(self,suffix=None):
    dd = self.root.find_node(Context.top_dir).find_node('download')
    if suffix:
        dd = dd.make_node(suffix)
    return dd

@add_to_context
def include_dir(self,suffix=None):
    dd = self.root.find_node(Context.top_dir).find_node('include')
    if suffix:
        dd = dd.make_node(suffix)
    return dd

@add_to_context
def include_dirs(self):
    return [self.include_dir(), self.include_dir().get_bld()]


################################
# Download External libraries
################################

def download_stk(self):
    self.download_and_extract('http://ccrma.stanford.edu/software/stk/release/stk-4.4.4.tar.gz',
            'stk',compression='gzip')
    stk_dir = self.download_dir().find_node('stk')



################################
# Standard waf stuffs
################################

def options(self):
    self.load('compiler_cxx')
    self.load('load_stk',tooldir='config')

    self.add_option('--enable-debug', default=False, action='store_true', dest='debug',
            help='to compile in debug mode')

    self.recurse('src')

def configure(self):
    self.load('compiler_cxx')
    self.load('load_stk',tooldir='config')

    #c++ compiler debug flags
    if self.options.debug:
        self.env.append_value('CXXFLAGS', '-g')
    else:
        self.env.append_value('CXXFLAGS', '-O3')
    
    #if GNU, then use -Wall
    if self.env.CXX_NAME == 'gcc':
        self.env.append_value('CXXFLAGS', '-Wall')

    #set some envrironment constants
    self.env.LIB_INSTALL_PATH = self.env.LIBDIR + "audio_test"

    #get the copy command
    if sys.platform == 'win32':
        self.env.COPY = 'copy'
    else:
        self.env.COPY = 'cp'

    self.recurse('src')

def build(self):
    self.build_stk()
    self.recurse('src')

    self.install_files('${PREFIX}/lib/',
            self.path.ant_glob("resources/**"), relative_trick=True)
