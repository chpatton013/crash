-- Define confirations here. Valid options are:
--    kind (ConsoleApp, WindowedApp, StaticLib, or SharedLib)
--    flags (list of premake flags)
--    includedirs (list of paths to search for header files)
--    buildoptions (list of literal options to pass to the compiler)
--    links (list of libraries to link in binary targets)
--    libdirs (list of paths to search for libraries)
--    linkoptions (list of literal options to pass to the linker)

shared = {
   flags = {},
   defines = {'GLM_FORCE_RADIANS'},
   includedirs = {'./include', './external/boost', './external/glm'},
   buildoptions = {'--pipe', '--std=c++1y', '-Wno-unused-result'},
   links = {},
   libdirs = {'./lib', './external/boost/stage/lib'},
   linkoptions = {},
   headers = './include/crash'
}

primary = {
   kind = 'SharedLib',
   flags = {},
   includedirs = {},
   buildoptions = {},
   links = {},
   libdirs = {},
   linkoptions = {},
   sources = './src'
}

testing = {
   kind = 'ConsoleApp',
   flags = {},
   includedirs = {'./external/Catch/single_include'},
   buildoptions = {},
   links = {},
   libdirs = {},
   linkoptions = {},
   sources = './tests'
}

modules = {
   util = {
      links = {'boost_filesystem', 'boost_system'}
   },
   math = {}
}

-- Utility functions.
function set_join(first_table, second_table)
   first_table = first_table or {}
   second_table = second_table or {}

   -- Merge values in first and second table into accumulator.
   -- Use keys to ensure unique elements.
   local accumulator = {}
   for k,v in pairs(first_table) do
      accumulator[v] = 1
   end
   for k,v in pairs(second_table) do
      accumulator[v] = 1
   end

   -- Flatten accumulator into list.
   local list = {}
   for k,v in pairs(accumulator) do
      table.insert(list, k)
   end

   return list
end

function config_merge(template, module_config, key)
   return set_join(set_join(shared[key], template[key]), module_config[key]);
end

function configure_project(module_name, module_config, template)
   language('C++')

   local project_kind = module_config.kind or template.kind or 'SharedLib'
   kind(project_kind)
   if project_kind == 'SharedLib' or project_kind == 'StaticLib' then
      targetdir('./lib')
   else
      targetdir('./bin')
   end

   local headers = module_config.headers or template.headers or shared.headers
   local sources = module_config.sources or template.sources or shared.sources
   files({
      string.format('%s/%s/**.cpp', sources, module_name),
      string.format('%s/%s/**.hpp', headers, module_name),
      string.format('%s/%s/**.inl', headers, module_name)
   })

   flags(config_merge(template, module_config, 'flags'))
   defines(config_merge(template, module_config, 'defines'))

   includedirs(config_merge(template, module_config, 'includedirs'))
   buildoptions(config_merge(template, module_config, 'buildoptions'))

   links(config_merge(template, module_config, 'links'))
   libdirs(config_merge(template, module_config, 'libdirs'))
   linkoptions(config_merge(template, module_config, 'linkoptions'))
end

-- Makefile generation.
solution('crash')
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

for module_name,module_config in pairs(modules) do
   local target = 'crash_' .. module_name
   -- primary target
   project(target)
   configure_project(module_name, module_config, primary)

   -- testing target
   table.insert(module_config.links or {}, target)
   project(string.format('%s_test', target))
   configure_project(module_name, module_config, testing)
end
