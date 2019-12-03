cmake_minimum_required(VERSION 2.8)
include_directories ("${PROJECT_SOURCE_DIR}/lib/" "${PROJECT_SOURCE_DIR}/src/")

add_library(iotf_network_tls_wrapper iotf_network_tls_wrapper.c)
add_library(iotf_utils iotf_utils.c)
add_library(iotfclient iotfclient.c)
add_library(iotfdeviceclient SHARED deviceclient.c)
add_library(iotfgatewayclient SHARED gatewayclient.c)
add_library(iotfdevicemgmt SHARED devicemanagementclient.c)

TARGET_LINK_LIBRARIES (iotf_network_tls_wrapper mbedcrypto mbedx509 mbedtls )
TARGET_LINK_LIBRARIES (iotfdeviceclient iotfclient iotf_utils iotf_network_tls_wrapper mqttlib)
TARGET_LINK_LIBRARIES (iotfgatewayclient iotfclient iotf_utils iotf_network_tls_wrapper mqttlib)
TARGET_LINK_LIBRARIES (iotfdevicemgmt iotfclient iotfdeviceclient cJSON iotf_utils iotf_network_tls_wrapper mqttlib)
