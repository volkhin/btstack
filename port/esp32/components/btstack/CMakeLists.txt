set(COMPONENT_ADD_INCLUDEDIRS "3rd-party/bluedroid/decoder/include"
                              "3rd-party/bluedroid/encoder/include"
                              "3rd-party/hxcmod-player"
                              "3rd-party/hxcmod-player/mods"
                              "../lwip/lwip/src/include"
                              "3rd-party/lwip/dhcp-server"
                              "3rd-party/md5"
                              "3rd-party/yxml"
                              "src/classic"
                              "src/ble/gatt-service"
                              "src/ble"
                              "src/classic"
                              "src"
                              "platform/embedded"
                              "platform/freertos"
                              "platform/lwip"
                              "include")

set(COMPONENT_PRIV_INCLUDEDIRS "3rd-party/micro-ecc")

set(COMPONENT_SRCDIRS "3rd-party/bluedroid/decoder/srce"
                      "3rd-party/bluedroid/encoder/srce"
                      "3rd-party/hxcmod-player"
                      "3rd-party/hxcmod-player/mods"
                      "../lwip/lwip/src/apps/httpd"
                      "3rd-party/lwip/dhcp-server"
                      "3rd-party/micro-ecc"
                      "3rd-party/md5"
                      "src/ble/gatt-service"
                      "src/ble"
                      "src/classic"
                      "src/"
                      "platform/freertos"
                      "platform/lwip"
                      ".")

set(COMPONENT_REQUIRES "nvs_flash" "bt")
register_component()
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)
