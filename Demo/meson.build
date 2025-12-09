# Demo executables

# Standard demos (no external dependencies)
basic_demos = [
  'HelloWorld',
  'Basic',
  'Constexpr',
]

foreach demo : basic_demos
  executable(demo.to_lower(),
    demo / 'main.cpp',
    dependencies: serdes_dep,
    install: false
  )
endforeach

# JsonArray demo (requires nlohmann/json)
# Try system package first, fallback to subproject if not found
nlohmann_json_dep = dependency('nlohmann_json',
  required: false,
  fallback: ['nlohmann_json', 'nlohmann_json_dep']
)

if nlohmann_json_dep.found()
  executable('jsonarray',
    'JsonArray/main.cpp',
    dependencies: [serdes_dep, nlohmann_json_dep],
    install: false
  )

  if nlohmann_json_dep.type_name() == 'internal'
    message('Using bundled nlohmann/json (downloaded via wrap)')
  else
    message('Using system nlohmann/json')
  endif
else
  warning('nlohmann/json not available, skipping JsonArray demo')
endif