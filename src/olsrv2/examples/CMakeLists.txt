build_lib_example(
  NAME olsrv2-hna
  SOURCE_FILES olsrv2-hna.cc
  LIBRARIES_TO_LINK
    ${libcore}
    ${libmobility}
    ${libwifi}
    ${libcsma}
    ${libolsr}
)

build_lib_example(
  NAME simple-point-to-point-olsrv2
  SOURCE_FILES simple-point-to-point-olsrv2.cc
  LIBRARIES_TO_LINK
    ${libpoint-to-point}
    ${libinternet}
    ${libolsr}
    ${libapplications}
    ${libwifi}
)
