cmake_minimum_required(VERSION 2.8)
project( finalProject )
find_package( OpenCV REQUIRED )

add_executable( gmm gmm.cpp bsgmm.cpp rect.cpp ptrans.cpp )
target_link_libraries( gmm ${OpenCV_LIBS} )

add_executable( roadmap roadmap.cpp bsgmm.cpp rect.cpp ptrans.cpp )
target_link_libraries( roadmap ${OpenCV_LIBS} )

add_executable( testptrans testptrans.cpp ptrans.cpp )
target_link_libraries( testptrans ${OpenCV_LIBS} )

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
