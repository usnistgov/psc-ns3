build_lib(
  LIBNAME olsrv2
  SOURCE_FILES
    helper/olsrv2-helper.cc
    model/olsrv2-header.cc
    model/olsrv2-routing-protocol.cc
    model/olsrv2-state.cc
  HEADER_FILES
    helper/olsrv2-helper.h
    model/olsrv2-header.h
    model/olsrv2-repositories.h
    model/olsrv2-routing-protocol.h
    model/olsrv2-state.h
  LIBRARIES_TO_LINK ${libinternet}
  TEST_SOURCES
    test/olsrv2-regression-test-suite.cc
    test/olsrv2-bug780-test.cc
    test/olsrv2-hello-regression-test.cc
    test/olsrv2-header-test-suite.cc
    test/olsrv2-routing-protocol-test-suite.cc
    test/olsrv2-tc-regression-test.cc
)
