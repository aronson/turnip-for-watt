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
  'source/main.cpp',
  'source/physics.cpp',
  'source/scene.cpp',
]

# All of the include directories used by your project.
includes = [
  'include',
]

grit = find_program('grit')

# Dependencies. gba-meson provides a large number of GBA development libraries.
# Check them out and add the ones you want to use to this list.
dependencies = [
  dependency('minrt'),
  dependency('libseven'),
  dependency('libutil'),
  dependency('libsavgba'),
  dependency('libtonc'),
  dependency('agbabi'),
  dependency('posprintf'),
  dependency('maxmod'),
]

jimmy = files('jimmy.bmp')

sprite_bin = custom_target(
  'Generate sprites',
  input : jimmy,
  output : ['jimmy.h', 'jimmy.s'],
  command : [
    grit, '@INPUT@', '-gB8', '-Mh2', '-Mw2', '-pS'
  ],
  build_by_default : true)

# The main executable for your game. We give it a '.elf' file extension,
# just to make the file more easy to differentiate from a raw ROM.
elf = executable(
  name,
  sources + sprite_bin,
  include_directories: includes,
  link_args: '-specs=nosys.specs',
  dependencies: dependencies,
  name_suffix: 'elf')

# Get the 'makerom' program from sdk-seven.
makerom = find_program('makerom')

# Build a raw GBA ROM file from your ELF file.
rom = custom_target(
  name + '-rom',
  input: elf,
  output: name + '.gba',
  command: [makerom, '@INPUT@', '@OUTPUT@'],
  build_by_default: true)
