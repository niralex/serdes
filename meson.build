project('serdes', 'cpp',
  version: '1.0.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
  ]
)

# Header-only library
serdes_inc = include_directories('Include')

serdes_dep = declare_dependency(
  include_directories: serdes_inc
)

# Build demos
subdir('Demo')