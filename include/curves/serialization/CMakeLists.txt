SET(${PROJECT_NAME}_SERIALIZATION_HEADERS
  archive.hpp
  eigen-matrix.hpp
  fwd.hpp
  )

INSTALL(FILES
  ${${PROJECT_NAME}_SERIALIZATION_HEADERS}
  DESTINATION include/curves/serialization
  )