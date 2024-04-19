project('graph_lib', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20', 'optimization=3'])

compiler = meson.get_compiler('cpp')

inc = include_directories('graph_lib/include')
graphlib_dep = declare_dependency(include_directories : inc)

exe_verification = executable('exe', 'verification/main.cpp',
  install : true,
  include_directories : inc
  )

tests = [
  ['Test', 'test/test.cpp'],
]

Catch2 = dependency('Catch2', method : 'cmake', modules : ['Catch2::Catch2WithMain', 'Catch2::Catch2'])

foreach t : tests
  exe = executable(t.get(0), t.get(1),
    dependencies : [Catch2],
    include_directories : inc
  )
  test(t.get(0), exe, workdir : meson.project_source_root())
endforeach