# SPDX-FileCopyrightText: 2011-2022 Blender Authors
#
# SPDX-License-Identifier: GPL-2.0-or-later

# headless configuration, useful in for servers or renderfarms
# builds without a windowing system (X11/Windows/Cocoa).
#
# Example usage:
#   cmake -C../blender/build_files/cmake/config/blender_headless.cmake  ../blender
#

set(WITH_HEADLESS            ON  CACHE BOOL "" FORCE)

# disable audio, its possible some devs may want this but for now disable
# so the python module doesn't hold the audio device and loads quickly.
set(WITH_AUDASPACE           OFF CACHE BOOL "" FORCE)
set(WITH_CODEC_FFMPEG        OFF CACHE BOOL "" FORCE)
set(WITH_CODEC_SNDFILE       OFF CACHE BOOL "" FORCE)
set(WITH_COREAUDIO           OFF CACHE BOOL "" FORCE)
set(WITH_JACK                OFF CACHE BOOL "" FORCE)
set(WITH_OPENAL              OFF CACHE BOOL "" FORCE)
set(WITH_PULSEAUDIO          OFF CACHE BOOL "" FORCE)
set(WITH_SDL                 OFF CACHE BOOL "" FORCE)
set(WITH_WASAPI              OFF CACHE BOOL "" FORCE)

# other features which are not especially useful as a python module
set(WITH_X11_XINPUT          OFF CACHE BOOL "" FORCE)
set(WITH_INPUT_NDOF          OFF CACHE BOOL "" FORCE)

#disable cycles

set(WITH_CYCLES              OFF CACHE BOOL "" FORCE)
set(WITH_CYCLES_CUDA_BINARIES OFF CACHE BOOL "" FORCE)
set(WITH_CYCLES_DEVICE_OPENCL OFF CACHE BOOL "" FORCE)
set(WITH_CYCLES_LOGGING      OFF CACHE BOOL "" FORCE)
set(WITH_CYCLES_OSL          OFF CACHE BOOL "" FORCE)
#WITH_CYCLES_PATH_GUIDING
set(WITH_CYCLES_STANDALONE   OFF CACHE BOOL "" FORCE)
#WITH_CYCLES_EMBREE
set(WITH_CYCLES_EMBREE      OFF CACHE BOOL "" FORCE)
#WITH_CYCLES_DEVICE_CUDA
set(WITH_CYCLES_DEVICE_CUDA OFF CACHE BOOL "" FORCE)
#WITH_CUDA_DYNLOAD
set(WITH_CUDA_DYNLOAD       OFF CACHE BOOL "" FORCE)
#WITH_CYCLES_DEVICE_HIP
set(WITH_CYCLES_DEVICE_HIP OFF CACHE BOOL "" FORCE)

#WITH_MOD_FLUID
set(WITH_MOD_FLUID OFF CACHE BOOL "" FORCE)
#WITH_MOD_REMESH
set(WITH_MOD_REMESH OFF CACHE BOOL "" FORCE)
#WITH_MOD_OCEANSIM
set(WITH_MOD_OCEANSIM OFF CACHE BOOL "" FORCE)

#WITH_IMAGE_OPENEXR
set(WITH_IMAGE_OPENEXR ON CACHE BOOL "" FORCE)

#WITH_IMAGE_OPENJPEG
set(WITH_IMAGE_OPENJPEG OFF CACHE BOOL "" FORCE)

#WITH_IMAGE_CINEON
set(WITH_IMAGE_CINEON OFF CACHE BOOL "" FORCE)

#WITH_IMAGE_WEBP
set(WITH_IMAGE_WEBP OFF CACHE BOOL "" FORCE)

#WITH_CODEC_AVI
set(WITH_CODEC_AVI OFF CACHE BOOL "" FORCE)

#WITH_CODEC_FFMPEG
set(WITH_CODEC_FFMPEG OFF CACHE BOOL "" FORCE)

#WITH_CODEC_SNDFILE
set(WITH_CODEC_SNDFILE OFF CACHE BOOL "" FORCE)

#WITH_ALEMBIC
set(WITH_ALEMBIC OFF CACHE BOOL "" FORCE)

#WITH_USD
set(WITH_USD OFF CACHE BOOL "" FORCE)

#WITH_MATERIALX
set(WITH_MATERIALX OFF CACHE BOOL "" FORCE)

#WITH_HYDRA
set(WITH_HYDRA OFF CACHE BOOL "" FORCE)

#WITH_OPENCOLLADA
set(WITH_OPENCOLLADA OFF CACHE BOOL "" FORCE)

#WITH_IO_WAVEFRONT_OBJ
set(WITH_IO_WAVEFRONT_OBJ OFF CACHE BOOL "" FORCE)

#WITH_IO_PLY
set(WITH_IO_PLY OFF CACHE BOOL "" FORCE)

#WITH_IO_STL

set(WITH_IO_STL OFF CACHE BOOL "" FORCE)

#WITH_IO_GPENCIL

set(WITH_IO_GPENCIL OFF CACHE BOOL "" FORCE)

#WITH_LIBMV
set(WITH_LIBMV OFF CACHE BOOL "" FORCE)

#WITH_LIBMV_SCHUR_SPECIALIZATIONS

set(WITH_LIBMV_SCHUR_SPECIALIZATIONS OFF CACHE BOOL "" FORCE)

#WITH_FREESTYLE

set(WITH_FREESTYLE OFF CACHE BOOL "" FORCE)

#WITH_INSTALL_PORTABLE

set(WITH_INSTALL_PORTABLE OFF CACHE BOOL "" FORCE)

#WITH_PYTHON_INSTALL

set(WITH_PYTHON_INSTALL OFF CACHE BOOL "" FORCE)

#WITH_INSTALL_COPYRIGHT

set(WITH_INSTALL_COPYRIGHT OFF CACHE BOOL "" FORCE)

#WITH_PYTHON_NUMPY

set(WITH_PYTHON_NUMPY OFF CACHE BOOL "" FORCE)

#WITH_PYTHON_INSTALL_REQUESTS

set(WITH_PYTHON_INSTALL_REQUESTS OFF CACHE BOOL "" FORCE)

#WITH_PYTHON_INSTALL_ZSTANDARD

set(WITH_PYTHON_INSTALL_ZSTANDARD OFF CACHE BOOL "" FORCE)

#WITH_PYTHON

set(WITH_PYTHON OFF CACHE BOOL "" FORCE)

#WITH_PYTHON_SECURITY

set(WITH_PYTHON_SECURITY OFF CACHE BOOL "" FORCE)

#WITH_PYTHON_MODULE

set(WITH_PYTHON_MODULE OFF CACHE BOOL "" FORCE)


#WITH_BLENDER_THUMBNAILER

set(WITH_BLENDER_THUMBNAILER OFF CACHE BOOL "" FORCE)

#WITH_BLENDER_APP 暂时ON.
set(WITH_BLENDER_APP OFF CACHE BOOL "" FORCE)

set(WITH_BLENDER_WASM ON CACHE BOOL "" FORCE)

#WITH_BUILDINFO

set(WITH_BUILDINFO OFF CACHE BOOL "" FORCE)


#WITH_INTERNATIONAL

set(WITH_INTERNATIONAL OFF CACHE BOOL "" FORCE)


#WITH_OPENCOLORIO
set(WITH_OPENCOLORIO OFF CACHE BOOL "" FORCE)


#WITH_FFTW3

set(WITH_FFTW3 OFF CACHE BOOL "" FORCE)

#WITH_PUGIXML

set(WITH_PUGIXML OFF CACHE BOOL "" FORCE)

#WITH_BULLET

set(WITH_BULLET OFF CACHE BOOL "" FORCE)

#WITH_SYSTEM_BULLET

set(WITH_SYSTEM_BULLET OFF CACHE BOOL "" FORCE)

#WITH_HARU

set(WITH_HARU OFF CACHE BOOL "" FORCE)

#WITH_IK_ITASC

set(WITH_IK_ITASC OFF CACHE BOOL "" FORCE)

#WITH_IK_SOLVER

set(WITH_IK_SOLVER OFF CACHE BOOL "" FORCE)

#WITH_OPENIMAGEDENOISE

set(WITH_OPENIMAGEDENOISE OFF CACHE BOOL "" FORCE)

#WITH_POTRACE

set(WITH_POTRACE OFF CACHE BOOL "" FORCE)


#WITH_BOOST

set(WITH_BOOST OFF CACHE BOOL "" FORCE)




