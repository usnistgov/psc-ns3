build_lib_example(
  NAME nhdp-example
  SOURCE_FILES nhdp-example.cc
  LIBRARIES_TO_LINK
    ${libcore}
    ${libinternet}
    ${libmobility}
    ${libnhdp}
    ${libwifi}
)
