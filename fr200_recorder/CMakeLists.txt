cmake_minimum_required(VERSION 2.8)

project(fr200_recorder)

# SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(OpenCV 3.0 REQUIRED)

#find_package(PCL 1.7 REQUIRED)

# message("==============================")
# message("OpenCV address " ${OpenCV_DIR}) 
# message("==============================")

include_directories(
  ${OpenCV_INCLUDE_DIRS}
)



# include_directories("/home/jin/Q/He/plane_04_13_2017/src")

find_package(catkin REQUIRED COMPONENTS
  #pcl
  roscpp
  std_msgs
  message_filters
  cv_bridge
  image_transport
  tf
)

catkin_package(
  CATKIN_DEPENDS roscpp std_msgs tf
  DEPENDS std_msgs opencv 
)

SET( CMAKE_CXX_COMPILER "g++")
SET( CMAKE_BUILD_TYPE Release )
SET( CMAKE_CXX_FLAGS "-O3 -std=c++11" )


include_directories(
  ${catkin_INCLUDE_DIRS}
)

# add_executable(saveRGBD src/saveRGBD.cpp)
# target_link_libraries(saveRGBD ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})

# add_executable(saveRGBD_orb src/saveRGBD_orb.cpp)
# target_link_libraries(saveRGBD_orb ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})

add_executable(save_data src/save_data.cpp)
target_link_libraries(save_data ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})

# add_executable(tmp src/tmp.cpp)
# target_link_libraries(tmp ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})

# add_executable(opencv_test src/writeImage.cpp)
# target_link_libraries(opencv_test ${OpenCV_LIBRARIES})

# add_executable(saveRGBD_orb_reg src/saveRGBD_orb_reg.cpp)
# target_link_libraries(saveRGBD_orb ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
