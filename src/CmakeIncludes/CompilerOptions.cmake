if(CMAKE_COMPILER_IS_GNUCC)
  # Enable additional warnings.
  # add_definitions(-Weffc++)
  add_definitions(-Wall -Wextra -Wshadow -Winline -Winvalid-pch)
  add_definitions(-Wctor-dtor-privacy -Wold-style-cast -Woverloaded-virtual)

  # Turn off all pragma warnings.
  add_definitions(-Wno-pragmas -Wno-unknown-pragmas)

  add_definitions(-m32)
  set (CMAKE_EXE_LINKER_FLAGS "-m32")
  set (CMAKE_SHARED_LINKER_FLAGS "-m32")
  set (CMAKE_MODULE_LINKER_FLAGS "-m32")

  if(CMAKE_BUILD_TYPE STREQUAL ${Debug})
    # Debugging symbols
    add_definitions(-g3)
    add_definitions(-O0)
    add_definitions(-D _DEBUG)
    set(CMAKE_CXX_FLAGS "-march=native -ffast-math")

  elseif(CMAKE_BUILD_TYPE STREQUAL ${Release})
    #add_definitions(-pg)
    #add_definitions(-flto=2 -fwhole-program)
    #set(CMAKE_EXE_LINKER_FLAGS "-flto=2 -fwhole-program -fuse-linker-plugin")
    set(CMAKE_CXX_FLAGS "-march=native -O3 -ffast-math")

  elseif(CMAKE_BUILD_TYPE STREQUAL ${RelWithDebInfo})
    add_definitions(-g3)
    set(CMAKE_CXX_FLAGS "-march=native -O3 -ffast-math")

  elseif(CMAKE_BUILD_TYPE STREQUAL ${Profile})
    #add_definitions(-march=native)
    #add_definitions(-pg)
    #set(CMAKE_EXE_LINKER_FLAGS "-pg")
    add_definitions(-g)
    #add_definitions(--coverage)
    add_definitions(-march=native -O3 -ffast-math)
    #set(CMAKE_CXX_FLAGS "-march=native -O3 -ffast-math")
  endif()

endif()
