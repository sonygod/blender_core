# SPDX-FileCopyrightText: 2006 Blender Authors
#
# SPDX-License-Identifier: GPL-2.0-or-later

set(INC
  ../../intern/clog
  ../blender/blenkernel
  ../blender/depsgraph
  ../blender/editors/include
  ../blender/gpu
  ../blender/imbuf

  ../blender/bmesh
  ../blender/makesrna
  ../blender/render
  ../blender/windowmanager
)

set(LIB
  PRIVATE bf::blenlib
  PRIVATE bf::dna
  PRIVATE bf::intern::guardedalloc
  bf_windowmanager
)



if(WITH_HEADLESS)
  add_definitions(-DWITH_HEADLESS)
endif()



if(WITH_BINRELOC)
  list(APPEND INC ${BINRELOC_INCLUDE_DIRS})
  add_definitions(-DWITH_BINRELOC)
endif()




if(WITH_GMP)
  list(APPEND INC ${GMP_INCLUDE_DIRS})
  add_definitions(-DWITH_GMP)
endif()



# Setup the EXE sources and `buildinfo`.
set(SRC
  creator.cc
  creator_args.cc
  creator_signals.cc

  creator_intern.h
)





add_cc_flags_custom_test(blender)

# message(STATUS "Configuring blender")

  add_executable(blender ${EXETYPE} ${SRC})
  if(WIN32)
    add_executable(blender-launcher WIN32
      blender_launcher_win32.c
      ${CMAKE_SOURCE_DIR}/release/windows/icons/winblender.rc
      ${CMAKE_BINARY_DIR}/blender.exe.manifest
    )
    target_compile_definitions (blender-launcher PRIVATE -D_UNICODE -DUNICODE)
    target_link_libraries(blender-launcher Pathcch.lib)
  endif()







# -----------------------------------------------------------------------------
# Platform specific target destinations
#
# Setup version directory, libraries, `bpy` & text files.

if(UNIX AND NOT APPLE)
  if(WITH_PYTHON_MODULE)
    if(WITH_INSTALL_PORTABLE)
      set(TARGETDIR_BPY "./bpy")
      set(TARGETDIR_VER "./bpy/${BLENDER_VERSION}")
      set(TARGETDIR_LIB "./bpy/lib")
    else()
      set(TARGETDIR_BPY ${PYTHON_SITE_PACKAGES}/bpy)
      set(TARGETDIR_VER ${PYTHON_SITE_PACKAGES}/bpy/${BLENDER_VERSION})
      set(TARGETDIR_LIB ${PYTHON_SITE_PACKAGES}/bpy/lib)
    endif()
  else()
    if(WITH_INSTALL_PORTABLE)
      set(TARGETDIR_VER "./${BLENDER_VERSION}")
      set(TARGETDIR_TEXT ".")
      set(TARGETDIR_LIB "./lib")
    else()
      set(TARGETDIR_VER "./share/blender/${BLENDER_VERSION}")
      set(TARGETDIR_TEXT "./share/doc/blender")
    endif()
  endif()

elseif(WIN32)
  if(WITH_PYTHON_MODULE)
    set(TARGETDIR_BPY ${CMAKE_INSTALL_PREFIX_WITH_CONFIG}/bpy)
    set(TARGETDIR_VER ${CMAKE_INSTALL_PREFIX_WITH_CONFIG}/bpy/${BLENDER_VERSION})
    # Important the DLL's are next to `__init__.pyd` otherwise it won't load.
    set(TARGETDIR_LIB ${CMAKE_INSTALL_PREFIX_WITH_CONFIG}/bpy)
    set(TARGETDIR_EXE ${CMAKE_INSTALL_PREFIX_WITH_CONFIG}/bpy)
  else()
    set(TARGETDIR_VER "./${BLENDER_VERSION}")
    set(TARGETDIR_TEXT ".")
    set(TARGETDIR_LIB "./blender.shared")
    set(TARGETDIR_EXE ".")
  endif()
elseif(APPLE)
  if(WITH_PYTHON_MODULE)
    if(WITH_INSTALL_PORTABLE)
      set(TARGETDIR_BPY "./bpy")
      set(TARGETDIR_VER "./bpy/${BLENDER_VERSION}")
      set(TARGETDIR_LIB "./bpy/lib")
    else()
      # Paths defined in terms of site-packages since the site-packages
      # directory can be a symlink (brew for example).
      set(TARGETDIR_BPY ${PYTHON_SITE_PACKAGES}/bpy)
      set(TARGETDIR_VER ${PYTHON_SITE_PACKAGES}/bpy/${BLENDER_VERSION})
      set(TARGETDIR_LIB ${PYTHON_SITE_PACKAGES}/bpy/lib)
    endif()
  else()
    set(TARGETDIR_VER "./Blender.app/Contents/Resources/${BLENDER_VERSION}")
    set(TARGETDIR_LIB "./Blender.app/Contents/Resources/lib")
    set(TARGETDIR_TEXT "./Blender.app/Contents/Resources/text")
  endif()
  # Skip re-linking on CPACK / install.
  set_target_properties(blender PROPERTIES BUILD_WITH_INSTALL_RPATH true)
endif()


# -----------------------------------------------------------------------------
# Install Targets (Generic, All Platforms)





# localization









# -----------------------------------------------------------------------------
# Install Targets (Platform Specific)

if(UNIX AND NOT APPLE)

  if(PLATFORM_BUNDLED_LIBRARIES AND TARGETDIR_LIB)
    install(
      FILES ${PLATFORM_BUNDLED_LIBRARIES}
      DESTINATION ${TARGETDIR_LIB}
    )
  endif()

  # There are a few differences between portable and system install.
  if(WITH_PYTHON_MODULE)
    if(WITH_INSTALL_PORTABLE)
      install(
        TARGETS blender
        DESTINATION ${TARGETDIR_BPY}
      )
    else()
      install(
        TARGETS blender
        LIBRARY DESTINATION ${TARGETDIR_BPY}
      )
    endif()

    # none of the other files are needed currently
  elseif(WITH_INSTALL_PORTABLE)
    set(BLENDER_BIN "blender")
    install(
      TARGETS blender
      DESTINATION "."
    )

    install(
      FILES
        ${CMAKE_SOURCE_DIR}/release/freedesktop/blender.desktop
        ${CMAKE_SOURCE_DIR}/release/freedesktop/icons/scalable/apps/blender.svg
        ${CMAKE_SOURCE_DIR}/release/freedesktop/icons/symbolic/apps/blender-symbolic.svg
      DESTINATION "."
    )

    if(WITH_BLENDER_THUMBNAILER)
      install(
        TARGETS blender-thumbnailer
        DESTINATION "."
      )
    endif()

    # NOTE: there is a bug in CMake 3.25.1 where `LIBDIR` is reported as undefined.
    if(NOT DEFINED LIBDIR)
      # Pass.
    elseif(EXISTS ${LIBDIR}/mesa)
      install(
        DIRECTORY ${LIBDIR}/mesa/lib
        DESTINATION "./lib/mesa"
      )

      install(
        PROGRAMS
        ${CMAKE_SOURCE_DIR}/release/bin/blender-launcher
        ${CMAKE_SOURCE_DIR}/release/bin/blender-softwaregl
        DESTINATION "."
      )

      # Remove from old location, so existing builds don't start with software
      # OpenGL now that the lib/ folder is used for other libraries.
      install(
        CODE "\
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libGL.so)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libGL.so.1)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libGL.so.1.5.0)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libGLU.so)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libGLU.so.1)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libGLU.so.1.3.1)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libglapi.so)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libglapi.so.0)\n
        file(REMOVE ${CMAKE_BINARY_DIR}/bin/lib/libglapi.so.0.0.0)\n
        "
      )
    endif()
  else()
    # main blender binary
    set(BLENDER_BIN "bin/blender")
    install(
      TARGETS blender
      DESTINATION "./bin"
    )

    # Misc files.
    install(
      FILES ${CMAKE_SOURCE_DIR}/release/freedesktop/blender.desktop
      DESTINATION "./share/applications"
    )
    install(
      FILES ${CMAKE_SOURCE_DIR}/release/freedesktop/icons/scalable/apps/blender.svg
      DESTINATION "./share/icons/hicolor/scalable/apps"
    )
    install(
      FILES ${CMAKE_SOURCE_DIR}/release/freedesktop/icons/symbolic/apps/blender-symbolic.svg
      DESTINATION "./share/icons/hicolor/symbolic/apps"
    )
    if(WITH_BLENDER_THUMBNAILER)
      install(
        TARGETS blender-thumbnailer
        DESTINATION "./bin"
      )
    endif()
  endif()

  if(WITH_PYTHON AND WITH_PYTHON_INSTALL)
    # Install executable
    install(
      PROGRAMS ${PYTHON_EXECUTABLE}
      DESTINATION ${TARGETDIR_VER}/python/bin
    )

    if(DEFINED LIBDIR)
      # Precompiled libraries, copy over complete lib directory.
      install_dir(
        ${PYTHON_LIBPATH}
        ${TARGETDIR_VER}/python
      )
    else()
      # System libraries.
      install(
        PROGRAMS ${PYTHON_EXECUTABLE}
        DESTINATION ${TARGETDIR_VER}/python/bin
      )

      # On some platforms (like openSUSE) Python is linked to be used from `lib64` directory.
      # determine this from Python's libraries path.
      # Ugh, its possible `lib64` is just a symlink to 'lib' which causes incorrect use of `lib64`.
      get_filename_component(_pypath_real ${PYTHON_LIBPATH} REALPATH)
      if(${_pypath_real} MATCHES "lib64$")
        set(_target_LIB "lib64")
      else()
        set(_target_LIB "lib")
      endif()
      unset(_pypath_real)

      # Copy the systems python into the install directory:
      # install(CODE "message(\"copying a subset of the systems python...\")")
      install(
        DIRECTORY ${PYTHON_LIBPATH}/python${PYTHON_VERSION}
        DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}
        PATTERN "__pycache__" EXCLUDE               # * any cache *
        PATTERN "config-${PYTHON_VERSION}/*.a" EXCLUDE  # static lib
        PATTERN "lib2to3" EXCLUDE                   # ./lib2to3
        PATTERN "site-packages/*" EXCLUDE           # ./site-packages/*
        PATTERN "tkinter" EXCLUDE                   # ./tkinter
        PATTERN "lib-dynload/_tkinter.*" EXCLUDE    # ./lib-dynload/_tkinter.co
        PATTERN "idlelib" EXCLUDE                   # ./idlelib
        PATTERN "test" EXCLUDE                      # ./test
        PATTERN "turtledemo" EXCLUDE                # ./turtledemo
        PATTERN "turtle.py" EXCLUDE                 # ./turtle.py
        PATTERN "wininst*.exe" EXCLUDE              # from distutils, avoid malware false positive
      )

      # Needed for `distutils/pip`.
      # Get the last part of the include dir, will be `python{version}{abiflag}`.
      get_filename_component(_py_inc_suffix ${PYTHON_INCLUDE_DIR} NAME)
      install(
        FILES ${PYTHON_INCLUDE_DIR}/pyconfig.h
        DESTINATION ${TARGETDIR_VER}/python/include/${_py_inc_suffix}
      )
      unset(_py_inc_suffix)

      if(WITH_PYTHON_INSTALL_NUMPY)
        # Install to the same directory as the source, so debian-like
        # distributions are happy with their policy.
        set(_suffix "site-packages")
        if(${PYTHON_NUMPY_PATH} MATCHES "dist-packages")
          set(_suffix "dist-packages")
        endif()
        install(
          DIRECTORY ${PYTHON_NUMPY_PATH}/numpy
          DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
          PATTERN ".svn" EXCLUDE
          PATTERN "__pycache__" EXCLUDE           # * any cache *
          PATTERN "*.pyc" EXCLUDE                 # * any cache *
          PATTERN "*.pyo" EXCLUDE                 # * any cache *
          PATTERN "oldnumeric" EXCLUDE            # ./oldnumeric
          PATTERN "doc" EXCLUDE                   # ./doc
          PATTERN "tests" EXCLUDE                 # ./tests
          PATTERN "f2py" EXCLUDE                  # ./f2py - fortran/python interface code, not for blender.
          PATTERN "include" EXCLUDE               # include dirs all over, we won't use NumPy/CAPI
          PATTERN "*.h" EXCLUDE                   # some includes are not in include dirs
          PATTERN "*.a" EXCLUDE                   # ./core/lib/libnpymath.a - for linking, we don't need.
        )
        install(
          DIRECTORY ${PYTHON_NUMPY_PATH}/Cython
          DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
          PATTERN ".svn" EXCLUDE
          PATTERN "__pycache__" EXCLUDE           # * any cache *
          PATTERN "*.pyc" EXCLUDE                 # * any cache *
          PATTERN "*.pyo" EXCLUDE                 # * any cache *
        )
        install(
          FILES ${PYTHON_NUMPY_PATH}/cython.py
          DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
        )
        unset(_suffix)
      endif()

      if(WITH_USD)
        # Install to the same directory as the source, so debian-like
        # distros are happy with their policy.
        set(_suffix "site-packages")
        if(${PYTHON_USD_PATH} MATCHES "dist-packages")
          set(_suffix "dist-packages")
        endif()
        install(
          DIRECTORY ${USD_LIBRARY_DIR}/python/
          DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
          PATTERN ".svn" EXCLUDE
          PATTERN "__pycache__" EXCLUDE           # * any cache *
          PATTERN "*.pyc" EXCLUDE                 # * any cache *
          PATTERN "*.pyo" EXCLUDE                 # * any cache *
        )
        unset(_suffix)
      endif()

      if(WITH_PYTHON_INSTALL_ZSTANDARD)
        # Install to the same directory as the source, so debian-like
        # distributions are happy with their policy.
        set(_suffix "site-packages")
        if(${PYTHON_ZSTANDARD_PATH} MATCHES "dist-packages")
          set(_suffix "dist-packages")
        endif()
        install(
          DIRECTORY ${PYTHON_ZSTANDARD_PATH}/zstandard
          DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
          PATTERN ".svn" EXCLUDE
          PATTERN "__pycache__" EXCLUDE           # * any cache *
          PATTERN "*.pyc" EXCLUDE                 # * any cache *
          PATTERN "*.pyo" EXCLUDE                 # * any cache *
        )
        unset(_suffix)
      endif()

      # Copy requests, we need to generalize site-packages.
      if(WITH_PYTHON_INSTALL_REQUESTS)
        set(_suffix "site-packages")
        if(${PYTHON_REQUESTS_PATH} MATCHES "dist-packages")
          set(_suffix "dist-packages")
        endif()
        install(
          DIRECTORY ${PYTHON_REQUESTS_PATH}/requests
          DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
          PATTERN ".svn" EXCLUDE
          PATTERN "__pycache__" EXCLUDE           # * any cache *
          PATTERN "*.pyc" EXCLUDE                 # * any cache *
          PATTERN "*.pyo" EXCLUDE                 # * any cache *
        )
        # On some platforms requests does have extra dependencies.
        #
        # Either `chardet` or `charset_normalizer` is used, depending on the version of Python.
        # The code below silently skips the one that's not available, so we can list both here.
        set(_requests_deps "certifi" "chardet" "charset_normalizer" "idna" "urllib3")
        foreach(_requests_dep ${_requests_deps})
          if(EXISTS ${PYTHON_REQUESTS_PATH}/${_requests_dep})
            install(
              DIRECTORY ${PYTHON_REQUESTS_PATH}/${_requests_dep}
              DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
              PATTERN ".svn" EXCLUDE
              PATTERN "__pycache__" EXCLUDE           # * any cache *
              PATTERN "*.pyc" EXCLUDE                 # * any cache *
              PATTERN "*.pyo" EXCLUDE                 # * any cache *
            )
          endif()
        endforeach()
        if(EXISTS ${PYTHON_REQUESTS_PATH}/six.py)
          install(
            FILES ${PYTHON_REQUESTS_PATH}/six.py
            DESTINATION ${TARGETDIR_VER}/python/${_target_LIB}/python${PYTHON_VERSION}/${_suffix}
          )
        endif()
        unset(_requests_dep)
        unset(_requests_deps)
        unset(_suffix)
      endif()
      unset(_target_LIB)
    endif()
  endif()

  if(WITH_DRACO)
    install(
      PROGRAMS $<TARGET_FILE:extern_draco>
      DESTINATION ${TARGETDIR_VER}/python/lib/python${PYTHON_VERSION}/site-packages
    )
  endif()
elseif(WIN32)
  windows_install_shared_manifest(
    FILES ${LIBDIR}/epoxy/bin/epoxy-0.dll
    ALL
  )

  if(WITH_OPENMP AND MSVC_CLANG)
    windows_install_shared_manifest(
      FILES ${CLANG_OPENMP_DLL}
      ALL
    )
  endif()

  if(EXISTS ${LIBDIR}/fftw3/lib/fftw3.dll)
    set(FFTW_DLL ${LIBDIR}/fftw3/lib/fftw3.dll)
  else()
    set(FFTW_DLL ${LIBDIR}/fftw3/lib/libfftw3-3.dll)
  endif()

  windows_install_shared_manifest(
    FILES ${FFTW_DLL}
    ALL
  )
  if(EXISTS ${LIBDIR}/fftw3/lib/fftw3f-3.dll)
    windows_install_shared_manifest(
      FILES ${LIBDIR}/fftw3/lib/fftw3f-3.dll
      ALL
    )
  elseif(EXISTS ${LIBDIR}/fftw3/lib/fftw3f.dll)
    windows_install_shared_manifest(
      FILES ${LIBDIR}/fftw3/lib/fftw3f.dll
      ALL
    )
  endif()
  if(MSVC_ASAN)
    # The ASAN DLL's can be found in the same folder as the compiler,
    # this is the easiest way to find these.
    string(
      REPLACE "cl.exe" "clang_rt.asan_dynamic-x86_64.dll"
      ASAN_DLL ${CMAKE_C_COMPILER})
    string(
      REPLACE "cl.exe" "clang_rt.asan_dbg_dynamic-x86_64.dll"
      ASAN_DEBUG_DLL ${CMAKE_C_COMPILER}
    )
    if(NOT EXISTS "${ASAN_DLL}")
      message(
        FATAL_ERROR
        "ASAN is enabled, but the ASAN runtime is not detected, "
        "this is an optional component during the MSVC install, please install it"
      )
    endif()
    windows_install_shared_manifest(
      FILES ${ASAN_DLL}
      RELEASE
    )
    windows_install_shared_manifest(
      FILES ${ASAN_DEBUG_DLL}
      DEBUG
    )
    unset(ASAN_DLL)
    unset(ASAN_DEBUG_DLL)
  endif()
  if(EXISTS ${LIBDIR}/openexr/bin/Iex.dll)
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/openexr/bin/Iex.dll
        ${LIBDIR}/openexr/bin/IlmThread.dll
        ${LIBDIR}/openexr/bin/OpenEXRCore.dll
        ${LIBDIR}/openexr/bin/OpenEXRUtil.dll
        ${LIBDIR}/openexr/bin/OpenEXR.dll
        ${LIBDIR}/imath/bin/imath.dll
      RELEASE
    )
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/openexr/bin/Iex_d.dll
        ${LIBDIR}/openexr/bin/IlmThread_d.dll
        ${LIBDIR}/openexr/bin/OpenEXRCore_d.dll
        ${LIBDIR}/openexr/bin/OpenEXRUtil_d.dll
        ${LIBDIR}/openexr/bin/OpenEXR_d.dll
        ${LIBDIR}/imath/bin/imath_d.dll
      DEBUG
    )
  endif()
  if(EXISTS ${LIBDIR}/openimageio/bin/openimageio.dll)
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/openimageio/bin/openimageio.dll
        ${LIBDIR}/openimageio/bin/openimageio_util.dll
      RELEASE
    )
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/openimageio/bin/openimageio_d.dll
        ${LIBDIR}/openimageio/bin/openimageio_util_d.dll
      DEBUG
    )
  endif()

  if(EXISTS ${LIBDIR}/gmp/lib/gmp-10.dll)
    set(GMP_DLL ${LIBDIR}/gmp/lib/gmp-10.dll)
  else()
    set(GMP_DLL ${LIBDIR}/gmp/lib/libgmp-10.dll)
  endif()

  windows_install_shared_manifest(
    FILES ${GMP_DLL}
    ALL
  )
  unset(GMP_DLL)

  windows_install_shared_manifest(
    FILES ${LIBDIR}/gmp/lib/libgmpxx.dll
    RELEASE
  )
  windows_install_shared_manifest(
    FILES ${LIBDIR}/gmp/lib/libgmpxx_d.dll
    DEBUG
  )

  if(WITH_WINDOWS_RELEASE_PDB)
    if(WITH_WINDOWS_RELEASE_STRIPPED_PDB)
      # Icky hack for older CMAKE from https://stackoverflow.com/a/21198501
      # `$<CONFIG>` will work in newer CMAKE but the version currently (3.12)
      # on the build-bot does not support this endeavor.
      install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/\${CMAKE_INSTALL_CONFIG_NAME}/blender_public.pdb
        DESTINATION "."
        RENAME blender.pdb
        CONFIGURATIONS Release
      )
    else()
      install(
        FILES $<TARGET_PDB_FILE:blender>
        DESTINATION "."
        RENAME blender.pdb
        CONFIGURATIONS Release
      )
    endif()
  endif()

  windows_install_shared_manifest(
    FILES ${LIBDIR}/openvdb/bin/openvdb.dll
    RELEASE
  )
  windows_install_shared_manifest(
    FILES ${LIBDIR}/openvdb/bin/openvdb_d.dll
    DEBUG
  )

  # This will not exist for 3.4 and earlier lib folders
  # to ease the transition, support both 3.4 and 3.5 lib
  # folders.
  if(EXISTS ${LIBDIR}/openvdb/python/pyopenvdb_d.pyd)
    install(
      FILES ${LIBDIR}/openvdb/python/pyopenvdb_d.pyd
      DESTINATION ${TARGETDIR_VER}/python/lib/site-packages
      CONFIGURATIONS Debug
    )
    install(
      FILES ${LIBDIR}/openvdb/python/pyopenvdb.pyd
      DESTINATION ${TARGETDIR_VER}/python/lib/site-packages
      CONFIGURATIONS Release;RelWithDebInfo;MinSizeRel
    )
  endif()

  windows_install_shared_manifest(
    FILES
      ${LIBDIR}/materialx/bin/MaterialXCore.dll
      ${LIBDIR}/materialx/bin/MaterialXFormat.dll
      ${LIBDIR}/materialx/bin/MaterialXGenGlsl.dll
      ${LIBDIR}/materialx/bin/MaterialXGenMdl.dll
      ${LIBDIR}/materialx/bin/MaterialXGenOsl.dll
      ${LIBDIR}/materialx/bin/MaterialXGenShader.dll
    RELEASE
  )
  if(EXISTS ${LIBDIR}/materialx/bin/MaterialXRender.dll) # 3.6+
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/materialx/bin/MaterialXRender.dll
        ${LIBDIR}/materialx/bin/MaterialXRenderGlsl.dll
        ${LIBDIR}/materialx/bin/MaterialXRenderHw.dll
        ${LIBDIR}/materialx/bin/MaterialXRenderOsl.dll
      RELEASE
    )
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/materialx/bin/MaterialXRender_d.dll
        ${LIBDIR}/materialx/bin/MaterialXRenderGlsl_d.dll
        ${LIBDIR}/materialx/bin/MaterialXRenderHw_d.dll
        ${LIBDIR}/materialx/bin/MaterialXRenderOsl_d.dll
      DEBUG
    )
  endif()
  windows_install_shared_manifest(
    FILES
      ${LIBDIR}/materialx/bin/MaterialXCore_d.dll
      ${LIBDIR}/materialx/bin/MaterialXFormat_d.dll
      ${LIBDIR}/materialx/bin/MaterialXGenGlsl_d.dll
      ${LIBDIR}/materialx/bin/MaterialXGenMdl_d.dll
      ${LIBDIR}/materialx/bin/MaterialXGenOsl_d.dll
      ${LIBDIR}/materialx/bin/MaterialXGenShader_d.dll
    DEBUG
  )

  

  # Filenames change slightly between FFMPEG versions check both 6.0 and fallback to 5.0
  # to ease the transition between versions.
  if(EXISTS "${LIBDIR}/ffmpeg/lib/avcodec-60.dll")
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/ffmpeg/lib/avcodec-60.dll
        ${LIBDIR}/ffmpeg/lib/avformat-60.dll
        ${LIBDIR}/ffmpeg/lib/avdevice-60.dll
        ${LIBDIR}/ffmpeg/lib/avutil-58.dll
        ${LIBDIR}/ffmpeg/lib/swscale-7.dll
        ${LIBDIR}/ffmpeg/lib/swresample-4.dll
      ALL
    )
  else()
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/ffmpeg/lib/avcodec-59.dll
        ${LIBDIR}/ffmpeg/lib/avformat-59.dll
        ${LIBDIR}/ffmpeg/lib/avdevice-59.dll
        ${LIBDIR}/ffmpeg/lib/avutil-57.dll
        ${LIBDIR}/ffmpeg/lib/swscale-6.dll
        ${LIBDIR}/ffmpeg/lib/swresample-4.dll
      ALL
    )
  endif()
  windows_install_shared_manifest(
    FILES
      ${LIBDIR}/tbb/bin/tbb.dll
    RELEASE
  )
  windows_install_shared_manifest(
    FILES
      ${LIBDIR}/tbb/bin/tbb_debug.dll
    DEBUG
  )
  if(WITH_TBB_MALLOC_PROXY)
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/tbb/bin/tbbmalloc.dll
        ${LIBDIR}/tbb/bin/tbbmalloc_proxy.dll
      RELEASE
    )
    windows_install_shared_manifest(
      FILES
        ${LIBDIR}/tbb/bin/tbbmalloc_debug.dll
        ${LIBDIR}/tbb/bin/tbbmalloc_proxy_debug.dll
      DEBUG
    )
    list(APPEND LIB ${TBB_MALLOC_LIBRARIES})
  endif()

  if(EXISTS ${LIBDIR}/sndfile/lib/sndfile.dll)
    set(SNDFILE_DLL ${LIBDIR}/sndfile/lib/sndfile.dll)
  else()
    set(SNDFILE_DLL ${LIBDIR}/sndfile/lib/libsndfile-1.dll)
  endif()

  windows_install_shared_manifest(
    FILES ${SNDFILE_DLL}
    ALL
  )
  unset(SNDFILE_DLL)

  windows_install_shared_manifest(
    FILES ${LIBDIR}/shaderc/bin/shaderc_shared.dll
    RELEASE
  )

  windows_install_shared_manifest(
    FILES ${LIBDIR}/shaderc/bin/shaderc_shared_d.dll
    DEBUG
  )

  windows_install_shared_manifest(
    FILES
      ${LIBDIR}/openal/lib/OpenAL32.dll
    ALL
  )

  windows_install_shared_manifest(
    FILES ${LIBDIR}/sdl/lib/SDL2.dll
    ALL
  )

  if(WITH_SYSTEM_AUDASPACE)
    install(
      FILES
        ${LIBDIR}/audaspace/lib/audaspace.dll
        ${LIBDIR}/audaspace/lib/audaspace-c.dll
        ${LIBDIR}/audaspace/lib/audaspace-py.dll
      DESTINATION "."
    )
  endif()


  if(NOT WITH_PYTHON_MODULE)
    install(
      FILES
        ${CMAKE_SOURCE_DIR}/release/windows/batch/blender_debug_gpu.cmd
        ${CMAKE_SOURCE_DIR}/release/windows/batch/blender_debug_gpu_glitchworkaround.cmd
        ${CMAKE_SOURCE_DIR}/release/windows/batch/blender_debug_log.cmd
        ${CMAKE_SOURCE_DIR}/release/windows/batch/blender_factory_startup.cmd
        ${CMAKE_SOURCE_DIR}/release/windows/batch/blender_oculus.cmd
        ${CMAKE_SOURCE_DIR}/release/windows/batch/oculus.json
      DESTINATION ${TARGETDIR_EXE}
    )
  endif()

  if(WITH_BLENDER_THUMBNAILER)
    install(
      TARGETS BlendThumb
      DESTINATION "."
    )
  endif()

  if(WITH_DRACO)
    install(
      PROGRAMS $<TARGET_FILE:extern_draco>
      DESTINATION ${TARGETDIR_VER}/python/lib/site-packages
    )
  endif()

  if(WITH_PYTHON_MODULE AND TARGETDIR_BPY)
    install(
      TARGETS blender
      LIBRARY DESTINATION ${TARGETDIR_BPY}
    )
  endif()

  if(PLATFORM_BUNDLED_LIBRARIES)
    windows_process_platform_bundled_libraries("${PLATFORM_BUNDLED_LIBRARIES}")
  endif()
elseif(APPLE)
  if(NOT WITH_PYTHON_MODULE)
    # Uppercase name for app bundle.
    set_target_properties(blender PROPERTIES OUTPUT_NAME Blender)
  endif()

  set(OSX_APP_SOURCEDIR ${CMAKE_SOURCE_DIR}/release/darwin/Blender.app)

  # Setup `Info.plist`.
  execute_process(
    COMMAND date "+%Y-%m-%d"
    OUTPUT_VARIABLE BLENDER_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set_target_properties(blender PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${OSX_APP_SOURCEDIR}/Contents/Info.plist
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${BLENDER_VERSION}.${BLENDER_VERSION_PATCH}"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${BLENDER_VERSION}.${BLENDER_VERSION_PATCH} ${BLENDER_DATE}"
  )

  # Gather the date in finder-style.
  execute_process(
    COMMAND date "+%m/%d/%Y/%H:%M"
    OUTPUT_VARIABLE SETFILE_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Give the bundle actual creation/modification date.
  #
  # Note that the directory might not yet exist, which happens when CMAKE is first run.
  if(NOT EXISTS ${EXECUTABLE_OUTPUT_PATH}/Blender.app)
    file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/Blender.app)
  endif()
  execute_process(
    COMMAND SetFile -d ${SETFILE_DATE} -m ${SETFILE_DATE} ${EXECUTABLE_OUTPUT_PATH}/Blender.app
  )

  set(BLENDER_BIN "bin/blender")
  install(
    TARGETS blender
    DESTINATION "."
  )

  install(
    FILES ${OSX_APP_SOURCEDIR}/Contents/PkgInfo
    DESTINATION "./Blender.app/Contents"
  )

  install_dir(
    ${OSX_APP_SOURCEDIR}/Contents/Resources
    "./Blender.app/Contents"
  )

  if(WITH_BLENDER_THUMBNAILER)
    install(
      TARGETS blender-thumbnailer
      DESTINATION "./Blender.app/Contents/MacOS"
    )
  endif()

  if(PLATFORM_BUNDLED_LIBRARIES AND TARGETDIR_LIB)
    install(
      FILES ${PLATFORM_BUNDLED_LIBRARIES}
      DESTINATION ${TARGETDIR_LIB}
    )
  endif()

  if(WITH_VULKAN_BACKEND)
    install(
      FILES ${VULKAN_LIBRARY} ${MOLTENVK_LIBRARY}
      DESTINATION ${TARGETDIR_LIB}
    )
  endif()

  # Python.
  if(WITH_PYTHON AND NOT WITH_PYTHON_MODULE AND NOT WITH_PYTHON_FRAMEWORK)
    # Copy the python libraries into the install directory.
    install_dir(
      ${PYTHON_LIBPATH}/python${PYTHON_VERSION}
      ${TARGETDIR_VER}/python/lib
    )

    # Install Python executable.
    install(
      PROGRAMS ${PYTHON_EXECUTABLE}
      DESTINATION ${TARGETDIR_VER}/python/bin
    )

    # Needed for `distutils/pip`.
    # Get the last part of the include dir, will be `python{version}{abiflag}`.
    get_filename_component(_py_inc_suffix ${PYTHON_INCLUDE_DIR} NAME)
    install(
      FILES ${PYTHON_INCLUDE_DIR}/pyconfig.h
      DESTINATION ${TARGETDIR_VER}/python/include/${_py_inc_suffix}
    )
    unset(_py_inc_suffix)
  endif()

  if(WITH_PYTHON_MODULE AND TARGETDIR_BPY)
    install(
      TARGETS blender
      LIBRARY DESTINATION ${TARGETDIR_BPY}
    )
  endif()

  if(WITH_DRACO)
    install(
      PROGRAMS $<TARGET_FILE:extern_draco>
      DESTINATION ${TARGETDIR_VER}/python/lib/python${PYTHON_VERSION}/site-packages
    )
  endif()
endif()

# -----------------------------------------------------------------------------
# Generic Install, for all targets

if(DEFINED TARGETDIR_TEXT)

  configure_file(
    ${CMAKE_SOURCE_DIR}/release/text/readme.html
    ${CMAKE_BINARY_DIR}/release/text/readme.html
    @ONLY
  )
  list(APPEND BLENDER_TEXT_FILES
    ${CMAKE_BINARY_DIR}/release/text/readme.html
  )

  install(
    FILES ${BLENDER_TEXT_FILES}
    DESTINATION "${TARGETDIR_TEXT}"
  )

  install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/release/license
    DESTINATION "${TARGETDIR_TEXT}"
)
endif()

# Install more files specified elsewhere.
delayed_do_install(${TARGETDIR_VER})

unset(BLENDER_TEXT_FILES)
unset(TARGETDIR_TEXT)


# -----------------------------------------------------------------------------
# Geometry Icons

# Geometry icons.
get_property(_icon_names GLOBAL PROPERTY ICON_GEOM_NAMES)
set(_icon_files)
foreach(_f ${_icon_names})
  list(APPEND _icon_files
    "${CMAKE_SOURCE_DIR}/release/datafiles/icons/${_f}.dat"
  )
endforeach()
install(
  FILES ${_icon_files}
  DESTINATION ${TARGETDIR_VER}/datafiles/icons
)

unset(_icon_names)
unset(_icon_files)
unset(_f)


# -----------------------------------------------------------------------------
# Studio Lights

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/release/datafiles/studiolights
  DESTINATION ${TARGETDIR_VER}/datafiles
)


# -----------------------------------------------------------------------------
# Bundle assets

set(ASSET_BUNDLE_DIR ${CMAKE_SOURCE_DIR}/release/datafiles/assets/publish/)

if(NOT EXISTS "${ASSET_BUNDLE_DIR}")
  set(ASSET_BUNDLE_DIR ${CMAKE_SOURCE_DIR}/../lib/assets/publish/)
endif()

if(EXISTS "${ASSET_BUNDLE_DIR}")
  install(
    DIRECTORY ${ASSET_BUNDLE_DIR}
    DESTINATION ${TARGETDIR_VER}/datafiles/assets
    PATTERN ".svn" EXCLUDE
  )
endif()


# -----------------------------------------------------------------------------
# Setup link libraries

add_dependencies(blender makesdna)
target_link_libraries(blender PRIVATE ${LIB})
unset(LIB)

setup_platform_linker_flags(blender)
setup_platform_linker_libs(blender)

if(DEFINED PLATFORM_SYMBOLS_MAP)
  set_target_properties(blender PROPERTIES LINK_DEPENDS ${PLATFORM_SYMBOLS_MAP})
endif()

blender_target_include_dirs(blender ${INC})







