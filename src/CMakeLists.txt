add_library(allreduce_base "")

target_sources(
        allreduce_base
        PRIVATE
        allreduce_base.cc
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/allreduce_base.h
)

target_include_directories(
        allreduce_base
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../dmlc-core/include
        ${CMAKE_CURRENT_LIST_DIR}/../include
)