solution('crash')
language('C++')

defines({'GLM_FORCE_RADIANS'})
flags({
   'FloatFast',
   'NoPCH',
   'OptimizeSpeed',
})
includedirs({
   'include',
   'external/boost',
   'external/glm',
   'external/glfw/include',
})
libdirs({
   'lib',
   'external/boost/stage/lib',
   'external/glfw/src',
})
buildoptions({
   '--pipe',
   '--std=c++11',
   '-Wall',
   '-Wextra',
   '-Werror',
   '-Wno-unused-result',
})
linkoptions({})

configurations('debug', 'profile', 'release')

configuration('debug')
defines({'DEBUG', 'NPROFILE', 'NRELEASE'})
flags({'Symbols'})

configuration('profile')
defines({'NDEBUG', 'PROFILE', 'NRELEASE'})
flags({'Symbols'})

configuration('release')
defines({'NDEBUG', 'NPROFILE', 'RELEASE'})
flags({'NoFramePointer'})

project('crash_util')
kind('SharedLib')
targetdir('lib')
files({
   'src/util/**.cpp',
   'include/crash/util/**.hpp',
   'include/crash/util/**.inl'
})
links({
   'boost_filesystem',
   'boost_system',
})
linkoptions({})

project('crash_math')
kind('SharedLib')
targetdir('lib')
files({
   'src/math/**.cpp',
   'include/crash/math/**.hpp',
   'include/crash/math/**.inl'
})
links({})
linkoptions({})

project('crash_space')
kind('SharedLib')
targetdir('lib')
files({
   'src/space/**.cpp',
   'include/crash/space/**.hpp',
   'include/crash/space/**.inl'
})
links({
   'crash_math',
})
linkoptions({})

project('crash_window')
kind('SharedLib')
targetdir('lib')
files({
   'src/window/**.cpp',
   'include/crash/window/**.hpp',
   'include/crash/window/**.inl'
})
links({
   'glfw3'
})
linkoptions({})

project('crash_unit_test')
kind('ConsoleApp')
targetdir('bin')
files({
   'tests/unit_driver.cpp',
   'tests/*/**.cpp',
   'tests/*/**.hpp',
   'tests/*/**.inl'
})
links({
   'crash_math',
   'crash_space',
   'crash_util',
   'crash_window',
   'boost_filesystem',
   'boost_system',
   'boost_filesystem',
   'boost_system',
})
linkoptions({})
