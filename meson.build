# Build settings for TPhrase
# Copyright © 2024 OOTA, Masato
# This is published under CC0 1.0.
# For more information, see CC0 1.0 Universal (CC0 1.0) at <https://creativecommons.org/publicdomain/zero/1.0/legalcode>.

project('translatable_phrase_generator', 'cpp',
    default_options: [
        'cpp_std=c++11',
        'buildtype=release',
        'warning_level=3',
        'strip=true',
    ],
    version: '1.0.0',
    license: 'GPL-3.0-or-later',
    # license_files: ['LICENSE'],
)
so_version = '1'

incdirs = ['include']

srcs = [
    'src/CharFeeder.cpp',
    'src/DataGsubs.cpp',
    'src/DataOptions.cpp',
    'src/DataPhrase.cpp',
    'src/DataProductionRule.cpp',
    'src/DataSyntax.cpp',
    'src/DataText.cpp',
    'src/Generator.cpp',
    'src/Syntax.cpp',
    'src/parse.cpp',
    'src/random.cpp',
    'src/trunc_syntax.cpp',
]

incfile = [
    'include/tphrase/Generator.h',
    'include/tphrase/error_utils.h',
]
incfile_common = [
    'include/tphrase/common/InputIterator.h',
    'include/tphrase/common/TempRefHolder.h',
    'include/tphrase/common/ext_context.h',
    'include/tphrase/common/gsub_func.h',
    'include/tphrase/common/random_func.h',
]


lib = library(
    'tphrase',
    srcs,
    version : meson.project_version(),
    soversion : so_version,
    include_directories : incdirs,
    install: true,
)

install_headers(
    incfile,
    subdir: 'tphrase',
)
install_headers(
    incfile_common,
    subdir: 'tphrase/common',
)

test_args = ['-fsanitize=address', '-fsanitize=undefined']
compiler = meson.get_compiler('cpp')
test_args = compiler.get_supported_arguments(test_args)

test_lib = static_library(
    'tphrase_test',
    srcs,
    build_by_default: false,
    # version : meson.project_version(),
    # soversion : so_version,
    include_directories : incdirs,
    install: false,
    cpp_args: test_args,
    link_args: test_args,
    override_options: [
        'buildtype=debugoptimized',
        'strip=false',
        'cpp_debugstl=true',
    ],
)

subdir('test')
