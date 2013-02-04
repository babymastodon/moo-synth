#! /usr/bin/env python
# encoding: utf-8

import os
import sys
import shutil
from waflib import Context
from waflib.Configure import conf

def options(self):
    self.add_option('--stk-path', action='store', default=None, dest='stk_path',
            help="path to the STK source code (default ./downloads/stk)")

    if sys.platform == 'linux2':
        self.add_option('--with-alsa', default=False, action='store_true', dest='stk_alsa',
                help='to choose native ALSA API support (default, linux only)')
        self.add_option('--with-pulse', default=False, action='store_true', dest='stk_pulse',
                help='to choose native Pulseaudio API support (linux only)')
        self.add_option('--with-oss', default=False, action='store_true', dest='stk_oss',
                help='to choose native OSS audio API support (linux only, no native OSS MIDI support)')

    if sys.platform == 'darwin':
        self.add_option('--with-core', default=False, action='store_true', dest='stk_core',
                help='to choose Core Audio API support (Macintosh OS-X)')

    if sys.platform == 'linux2' or sys.platform == 'darwin':
        self.add_option('--with-jack', default=False, action='store_true', dest='stk_jack',
                help='to choose native JACK API support (linux and Macintosh OS-X)')

    if sys.platform == 'win32':
        self.add_option('--with-ds', default=False, action='store_true', dest='stk_ds',
                help='to choose DirectSound support (default, Windows only)')
        self.add_option('--with-asio', default=False, action='store_true', dest='stk_asio',
                help='to choose ASIO support (Windows only)')





def configure(self):
    #check for the STK libraries
    if not self.options.stk_path:
        stk_dir = self.download_dir().find_node('stk')
    else:
        stk_dir = self.root.find_node(self.options.stk_path)
    if not stk_dir:
        self.fatal('STK source code could not be found. Use the --stk-path configuration option or run "waf download_stk"')

    #default rawwave path
    self.env.append_value('DEFINES_STK_STUFF','RAWWAVE_PATH="./resources/rawwaves"')

    #math library
    self.custom_check(header_name='cmath', lib='m', uselib_store="STK_STUFF")

    #time library
    self.custom_check(function_name='gettimeofday',header_name='sys/time.h', uselib_store="STK_STUFF")

    #networking
    self.custom_check(function_name='select',header_name='sys/socket.h', uselib_store="STK_STUFF")
    self.custom_check(function_name='socket',header_name='sys/select.h', uselib_store="STK_STUFF")

    #stk debug messages
    if self.options.debug:
        for flag in ['_STK_STUFF_DEBUG_','__RTAUDIO_DEBUG__','__RTMIDI_DEBUG__']:
            self.env.append_value('DEFINES_STK_STUFF_DEBUG', flag)

    #operating system dependent selfiguration options
    if sys.platform == 'linux2':
        #all linux selfigurations require the asound library
        self.custom_check(
                header_name='alsa/asoundlib.h',
                lib='asound',
                function_name='send_pcm_open',
                defines=['__LINUX_ALSA__'],
                uselib_store="STK_STUFF")
        if self.options.stk_oss:
            #TODO: oss build is broken
            self.env.append_value('DEFINES_STK_STUFF','__LINUX_OSS__=1')
        elif self.options.stk_jack:
            self.custom_check(
                    header_name='jack/jack.h',
                    lib='jack',
                    function_name='jack_client_open',
                    defines=['__UNIX_JACK__'],
                    uselib_store="STK_STUFF")
        elif self.options.stk_pulse:
            self.custom_check(
                    header_name='pulse/simple.h',
                    lib='pulse',
                    defines=['__LINUX_PULSE__'],
                    uselib_store="STK_STUFF")

    elif sys.platform == 'win32':
        #TODO: test windows
        self.env.append_value('DEFINES_STK_STUFF','__WINDOWS_MM__')
        self.env.append_value('LIB_STK_STUFF',['winmm', 'ole32', 'Wsock32'])
        if self.options.stk_asio:
            self.env.append_value('INCLUDES_STK_STUFF',self.path.find_node('src/include'))
            self.env.append_value('DEFINES_STK_STUFF',['__WINDOWS_ASIO__'])
        else:
            self.custom_check(
                    header_name='dsound.h',
                    lib='dsound',
                    defines=['__WINDOWS_DS__'],
                    uselib_store='STK_STUFF')

    elif sys.platform == 'darwin':
        #TODO: test mac osx
        self.custom_check(
                header_name='pthread.h',
                lib='pthread',
                uselib_store='STK_STUFF')
        if self.options.stk_jack:
            self.custom_check(
                    header_name='jack/jack.h',
                    lib='jack',
                    function_name='jack_client_new',
                    defines=['__UNIX_JACK__'],
                    uselib_store="STK_STUFF")
        else:
            self.custom_check(
                    header_name='CoreAudio/CoreAudio.h',
                    lib='CoreAudio CoreFoundation CoreMidi',
                    defines=['__MACOSX_CORE__'],
                    uselib_store="STK_STUFF")

    else:
        raise self.errors.ConfigurationError("operating system not supported")

    #Little Endian vs Big Endian
    if sys.byteorder == 'little':
        self.define('__LITTLE_ENDIAN__', 1)

    #pop the selfiguration environment
    self.setenv('')




@conf
def build_stk(self):
    #TODO: build the asio library on windows
    #compile the STK toolkit
    self.stlib(
        source = self.download_dir('stk').ant_glob('src/*.c??'),
        includes = self.download_dir('stk').find_node('include').abspath(),
        target = 'stk',
        use = 'STK_STUFF',
        )
    #copy the header files to the "stk" subfolder of the include directory
    for node in self.download_dir('stk').ant_glob('include/*'):
        source = node
        target = self.include_dir('stk').make_node(os.path.basename(node.abspath())).get_bld()
        self(rule = '${COPY} ${SRC} ${TGT}', source=source, target=target)
