CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(HephaestusEngine)

IF(NOT MSVC) # compile commands used for clang-format and clang-tidy
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ENDIF()

# exclude_from_all so we don't just have a ton of targets in IDE
ADD_SUBDIRECTORY(third_party/diamond_dogs EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(tests)
