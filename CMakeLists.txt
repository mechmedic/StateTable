cmake_minimum_required(VERSION 3.0.0)
project(StateTable VERSION 0.1.0)

include(CTest)
enable_testing()

# CKim - Added from the cisst MultiThread Example.
# /home/chunwoo/ChunGit/CISST/cisst/cisstMultiTask/examples/tutorial/CMakeLists.txt

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  #include (${CISST_USE_FILE})

#  add_executable (mtsExTutorial
#                  main.cpp
#                  counter.h counter.cpp
#                  user.h user.cpp
#                  )
#  set_property (TARGET mtsExTutorial PROPERTY FOLDER "cisstMultiTask/examples")

#  add_executable(${PROJECT_NAME} main.cpp cmdQueue.h cmdQueue.cpp)
#  add_executable(${PROJECT_NAME} cmdQueueTest.cpp cmdQueue.h)
  add_executable(${PROJECT_NAME} ./src/StateArrayBase.h ./src/StateArray.h ./src/StateIndex.h 
  ./src/TimeServer.h ./src/TimeServer.cpp ./src/StateTable.h ./src/StateTable.cpp ./src/StateTableTest.cpp )
# add_executable(${PROJECT_NAME} StateTableTest.cpp StateArray.h StateIndex.h)
  #cisst_target_link_libraries (${PROJECT_NAME} ${REQUIRED_CISST_LIBRARIES})
else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)

#add_executable(StateTable main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
