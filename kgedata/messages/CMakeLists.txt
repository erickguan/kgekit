set(KGEKIT_PROTOS triple_index.proto)
set(KGEKIT_SRCS)
set(KGEKIT_HDRS)


# Code Generation
foreach(proto_file ${KGEKIT_PROTOS})
  get_filename_component(proto_file_abs ${proto_file} ABSOLUTE)
  get_filename_component(basename ${proto_file} NAME_WE)
  set(generated_files ${basename}.pb.cc ${basename}.pb.h)

  list(APPEND KGEKIT_SRCS ${basename}.pb.cc)
  list(APPEND KGEKIT_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${basename}.pb.h)

  add_custom_command(
      OUTPUT ${generated_files}
      COMMAND protobuf::protoc --cpp_out ${CMAKE_CURRENT_BINARY_DIR} --python_out ${CMAKE_CURRENT_LIST_DIR}/../../kgekit -I ${CMAKE_CURRENT_SOURCE_DIR} ${proto_file_abs}
      COMMENT "Generating ${generated_files} from ${proto_file}"
      VERBATIM
  )
endforeach()

add_library(kgedata_structs ${KGEKIT_SRCS})
target_link_libraries(kgedata_structs protobuf::libprotobuf)
target_include_directories(kgedata_structs PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
