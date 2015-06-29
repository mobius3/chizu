find_package(SDL2 REQUIRED) # Need SDL2
find_package(SDL2_image REQUIRED) # Need SDL2_image

# Need at least C99
set(CMAKE_C_STANDARD 99)

# Flags on MSVC and GNU
if(MSVC)
  if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fvisibility=hidden")
endif()

option(CHIZU_EXECUTABLE "If the chizu executable/tool should be built." ON)

set(CHIZU_SOURCES
    chizu.c
    czmap.c
    czsurface.c
    stb_image_write.h
)

set(CHIZU_PRIVATE_HEADERS
    chizu.h
    czmap.h
    czsurface.h
    czrect.h
    czsize.h
    czpoint.h
)

set(CHIZU_PUBLIC_HEADERS
    chizu.h
)

# Add the chizu library
add_library(chizu ${CHIZU_SOURCES} ${CHIZU_PRIVATE_HEADERS})
target_include_directories(chizu PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(chizu PUBLIC ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})
set_target_properties(chizu PROPERTIES DEFINE_SYMBOL CHIZU_EXPORTS ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Install rules for lib, header and executable
install(TARGETS chizu
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if (CHIZU_EXECUTABLE)
    # Add the chizu executable
    add_executable(chizu_e main.c)
    target_link_libraries(chizu_e PUBLIC chizu)
    set_target_properties(chizu_e PROPERTIES OUTPUT_NAME "chizu" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

    install(TARGETS chizu_e
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()

install(FILES ${CHIZU_PUBLIC_HEADERS} DESTINATION include/chizu)
