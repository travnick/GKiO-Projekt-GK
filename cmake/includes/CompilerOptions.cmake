if(MSVC) #is winfdows compiler
 #sets warning level for VS to 4
 add_definitions(-W4)
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUCC)
  # Enable additional warnings.
  add_definitions(-Wall)
#   add_definitions(-Weffc++)
  add_definitions(-Wall -Wextra -Wshadow -Winline -Winvalid-pch)
  add_definitions(-Wctor-dtor-privacy -Wold-style-cast -Woverloaded-virtual)
  
#   Turn off all pragma warnings.
  add_definitions(-Wno-pragmas -Wno-unknown-pragmas)
 
  # define _DEBUG for Debug profile.
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     Debugging symbols
   add_definitions(-g3)
#    add_definitions(-pg)
    set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
   add_definitions(-O0)
   add_definitions(-D _DEBUG)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#    add_definitions(-O3)
#    add_definitions(-march=native)
#    add_definitions(-pg)
    set(CMAKE_CXX_FLAGS "-march=native -O3 -ffast-math")  
  endif()
  
endif()
