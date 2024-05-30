# Change these to suit your project, if you want.
project(
  'turnip-for-watt', 'c', 'cpp',
  version: '0.10.0',
  license: 'Zlib',
  meson_version: '>=1.3.0',
  default_options: ['buildtype=debugoptimized', 'warning_level=2', 'c_std=c99', 'cpp_std=c++20'])

# Set up recommended compiler and linker flags.
subdir('meson/toolchain')

# The name of your project.
name = 'turnip'

# All of the source files used by your project.
sources = [
  'source/actor.cpp',
  'source/graphics.cpp',
  'source/main.cpp',
  'source/physics.cpp',
  'source/scene.cpp',
  'source/terrain.cpp',
  'source/vram_sections.cpp',
]

# All of the include directories used by your project.
includes = [
  'include',
]

cmake = import('cmake')
realnumb_opts = cmake.subproject_options()
realnumb_opts.add_cmake_defines({'CMAKE_EXE_LINKER_FLAGS': '--specs=nosys.specs'})
realnumb = cmake.subproject('realnumb', options : realnumb_opts)
realnumb_dep = realnumb.dependency('realnumb')

grit = find_program('grit')

# Dependencies. gba-meson provides a large number of GBA development libraries.
# Check them out and add the ones you want to use to this list.
dependencies = [
  dependency('minrt'),
  dependency('libseven'),
  dependency('libutil'),
  dependency('libtonc'),
  dependency('gbfs'),
  dependency('libsavgba'),
  dependency('agbabi'),
  dependency('posprintf'),
  dependency('maxmod'),
  realnumb_dep,
]

jimmy = files('jimmy.bmp')
dorothy = files('dorothy.bmp')
candyland = files('CandyLand.png')

generated_files = []

sprite_bin = custom_target(
  'Generate sprites',
  input : jimmy,
  output : ['jimmy.h', 'jimmy.s', 'jimmyShared.h', 'jimmyShared.s'],
  command : [
    grit, '@INPUT@', '-gB4', '-Mh2', '-Mw2', '-pS', '-SjimmyShared'
  ],
  build_by_default : true)
generated_files += sprite_bin

dorothy_bin = custom_target(
  'Generate Dorothy sprite',
  input : dorothy,
  output : ['dorothy.h', 'dorothy.s', 'dorothyShared.h', 'dorothyShared.s'],
  command : [
    grit, '@INPUT@', '-gB4', '-Mh2', '-Mw2', '-pS', '-SdorothyShared'
  ],
  build_by_default : true)
generated_files += dorothy_bin

candyland_bin = custom_target(
  'Generate level tileset',
  input : candyland,
  output : ['CandyLand.h', 'CandyLand.s'],
  command : [
    grit, '@INPUT@', '-gB4', '-Mh1', '-Mw1',
  ],
  build_by_default : true)
generated_files += candyland_bin

# Required to invoke python scripts in a Windows-native build environment
pymod = import('python')
py3 = pymod.find_installation('python3', required: true)

# Get build-machine native tools
gba_tools = subproject('gba-tools')
gbfs = gba_tools.get_variable('gbfs_executable')
gbafix = find_program('gbafix-py', required : true)
makerom = find_program('makerom', required : true)
makebase = files('tools/makeBase.py')
subproject('mmutil')
mmutil = find_program('mmutil')

# References to python helper scripts
buildSoundbank = files('tools/buildSoundbank.py')
gbabuild = files('tools/gbaBuild.py')

soundbank_files = files('oh_my_god.mod')
soundbank_bin = 'soundbank.bin'
soundbank_header = 'soundbank.h'
effect_locations_file = 'effects.txt'

soundbank_target = custom_target(
  'soundbank',
  output : [soundbank_bin, soundbank_header, effect_locations_file, 'audio.gbfs'],
  command : [
    py3, buildSoundbank, 'default', mmutil, gbfs, soundbank_files,
  ],
  depend_files : soundbank_files,
)
generated_files += soundbank_target

# The main executable for your game. We give it a '.elf' file extension,
# just to make the file more easy to differentiate from a raw ROM.
elf = executable(
  name,
  sources + generated_files,
  include_directories: includes,
  link_args: '-specs=nosys.specs',
  dependencies: dependencies,
  name_suffix: 'elf')

makerom = find_program('makerom')
padbin = find_program('padbin')

# The base GBA ROM with no audio
rom_base = custom_target(
  name + '-base',
  input : elf,
  output : name + '-base.gba',
  command : [py3, makebase, makerom, '@INPUT@', '@OUTPUT@', gbafix])

# The final GBA ROM with audio
rom = custom_target(
  'final ROM with audio',
  output : name + '.gba',
  input : rom_base,
  command : [py3, gbabuild, padbin, '256', '@INPUT@', 'audio.gbfs', '0x100000', '@OUTPUT@', gbafix],
  build_by_default : true)
