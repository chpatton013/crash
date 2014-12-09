solution('crash')
language('C++')

defines({'GLM_FORCE_RADIANS'})
includedirs({
   'include',
   'external/boost',
   'external/glm',
})
libdirs({
   'lib',
   'external/boost/stage/lib',
})
buildoptions({
   '--pipe',
   '--std=c++1y',
   '-Wno-unused-result',
})
linkoptions({})

configurations('debug', 'profile', 'release')

configuration('debug')
defines({'DEBUG', 'NPROFILE', 'NRELEASE'})
flags({'ExtraWarnings', 'Optimize', 'Symbols'})

configuration('profile')
defines({'NDEBUG', 'PROFILE', 'NRELEASE'})
flags({'OptimizeSpeed', 'Symbols'})

configuration('release')
defines({'NDEBUG', 'NPROFILE', 'RELEASE'})
flags({'OptimizeSpeed'})

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
   'crash_util',
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

project('crash_test')
kind('ConsoleApp')
targetdir('bin')
files({
   'tests/**.cpp',
   'tests/**.hpp',
   'tests/**.inl'
})
links({
   'crash_util',
   'crash_math',
   'crash_space',
   'boost_filesystem',
   'boost_system',
})
linkoptions({})
