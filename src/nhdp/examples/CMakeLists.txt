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

build_lib_example(
  NAME manet-routing
  SOURCE_FILES manet-routing.cc
  LIBRARIES_TO_LINK
    ${libwifi}
    ${libstats}
    ${libnhdp}
    ${libolsr}
    ${libolsrv2}
    ${libinternet}
    ${libapplications}
    ${libflow-monitor}
)

