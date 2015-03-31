solution('crash')
language('C++')

defines({'GLM_FORCE_RADIANS'})
flags({
   'FloatFast',
   'NoPCH',
})
includedirs({
   'include',
   'external/assimp/include',
   'external/boost',
   'external/glm',
   'external/glfw/include',
})
libdirs({
   'lib',
   'external/assimp/lib',
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
flags({
   'Symbols',
   'OptimizeSpeed',
})

configuration('release')
defines({'NDEBUG', 'NPROFILE', 'RELEASE'})
flags({
   'NoFramePointer',
   'OptimizeSpeed',
})

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

project('crash_render')
kind('SharedLib')
targetdir('lib')
files({
   'src/render/**.cpp',
   'include/crash/render/**.hpp',
   'include/crash/render/**.inl'
})
links({
   'crash_math',
   'assimp',
   'boost_filesystem',
   'boost_system',
   'GLEW',
})
linkoptions({})
configuration("macosx")
links({"OpenGL.framework"})
configuration("not macosx")
links({"GL"})

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
   'glfw3',
})
linkoptions({})

project('crash_unit_test')
kind('ConsoleApp')
targetdir('bin')
files({
   'tests/unit_driver.cpp',
   'tests/unit/**.cpp',
   'tests/unit/**.hpp',
   'tests/unit/**.inl'
})
links({
   'crash_math',
   'crash_space',
   'crash_util',
   'crash_window',
   'boost_filesystem',
   'boost_system',
})
linkoptions({})

project('crash_asset_test')
kind('ConsoleApp')
targetdir('bin')
files({
   'tests/asset_driver.cpp',
})
links({
   'crash_math',
   'crash_render',
   'assimp',
})
linkoptions({})

project('crash_render_test')
kind('ConsoleApp')
targetdir('bin')
files({
   'tests/render_driver.cpp',
})
links({
   'crash_math',
   'crash_space',

   'crash_render',
   'assimp',
   'boost_filesystem',
   'boost_system',

   'crash_window',
   'glfw3',
   'GLEW',

   'boost_timer',
})
linkoptions({})
configuration('macosx')
links({'OpenGL.framework'})
configuration('not macosx')
links({'GL'})

project('crash_window_test')
kind('ConsoleApp')
targetdir('bin')
files({
   'tests/window_driver.cpp',
})
links({
   'crash_window',
   'glfw3',
})
linkoptions({})
