build_lib(
  LIBNAME nhdp
  SOURCE_FILES
    helper/nhdp-helper.cc
    model/nhdp-info-base.cc
    model/nhdp-client.cc
  HEADER_FILES
    helper/nhdp-helper.h
    model/nhdp-info-base.h
    model/nhdp-client.h
  LIBRARIES_TO_LINK ${libinternet}
                    ${libnetwork}
                    ${libcore}
)
