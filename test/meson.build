# Build settings for the unit test of TPhrase
# Copyright © 2024 OOTA, Masato
# This is published under CC0 1.0.
# For more information, see CC0 1.0 Universal (CC0 1.0) at <https://creativecommons.org/publicdomain/zero/1.0/legalcode>.

test_srcs = [
    'UnitTest.cpp',
    'test_class_Generator.cpp',
    'test_class_InputIterator.cpp',
    'test_class_Syntax.cpp',
    'test_class_TempRefHolder.cpp',
    'test_error_utils.cpp',
    'test_generate.cpp',
    'test_main.cpp',
    'test_parse.cpp',
    'unit_test_utility.cpp',
]

test_exe = executable(
    'test_tphrase',
    test_srcs,
    build_by_default: false,
    include_directories : ['../include'],
    link_with: test_lib,
    cpp_args: test_args,
    link_args: test_args,
    override_options: [
        'buildtype=debugoptimized',
        'strip=false',
        'cpp_debugstl=true',
    ],
)
test('Unit Test', test_exe)
