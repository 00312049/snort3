
AM_DEFAULT_SOURCE_EXT = .cc

check_PROGRAMS = \
nhttp_uri_norm_test \
nhttp_normalizers_test \
nhttp_module_test

TESTS = $(check_PROGRAMS)

nhttp_uri_norm_test_CPPFLAGS = $(AM_CPPFLAGS) @CPPUTEST_CPPFLAGS@
nhttp_uri_norm_test_LDADD = \
../nhttp_uri_norm.o \
../nhttp_module.o \
../nhttp_test_manager.o \
../nhttp_test_input.o \
../nhttp_normalizers.o \
../nhttp_str_to_code.o \
../nhttp_field.o \
../nhttp_tables.o \
../../../framework/module.o \
@CPPUTEST_LDFLAGS@

nhttp_normalizers_test_CPPFLAGS = $(AM_CPPFLAGS) @CPPUTEST_CPPFLAGS@
nhttp_normalizers_test_LDADD = \
../nhttp_normalizers.o \
../nhttp_field.o \
@CPPUTEST_LDFLAGS@

nhttp_module_test_CPPFLAGS = $(AM_CPPFLAGS) @CPPUTEST_CPPFLAGS@
nhttp_module_test_LDADD = \
../nhttp_module.o \
../nhttp_tables.o \
../nhttp_normalizers.o \
../nhttp_uri_norm.o \
../nhttp_field.o \
../../../framework/module.o \
@CPPUTEST_LDFLAGS@

