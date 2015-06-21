project(neuralnet)

# Source directories
source_dirs(private public)

# Include directories
include_directories(public)

# Output targets
add_executable(neuralnet ${SRCS})

# Dependencies
# Boost
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED)

if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(neuralnet ${Boost_LIBRARIES})
endif()