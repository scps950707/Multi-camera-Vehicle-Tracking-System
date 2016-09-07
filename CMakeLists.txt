cmake_minimum_required(VERSION 2.8)
project( finalProject )
find_package( OpenCV REQUIRED )
find_package( Boost COMPONENTS iostreams system filesystem REQUIRED )

add_executable( gmm gmm.cpp bsgmm.cpp rect.cpp )
target_link_libraries( gmm ${OpenCV_LIBS} )

add_executable( roadmap roadmap.cpp bsgmm.cpp rect.cpp ptrans.cpp )
target_link_libraries( roadmap ${OpenCV_LIBS} )

add_executable( testptrans testptrans.cpp ptrans.cpp )
target_link_libraries( testptrans ${OpenCV_LIBS} )

add_executable( project project.cpp bsgmm.cpp rect.cpp ptrans.cpp avi.cpp Ctracker.cpp Kalman.cpp HungarianAlg.cpp )
target_link_libraries( project ${OpenCV_LIBS} ${Boost_LIBRARIES} )

add_executable( resizer resizer.cpp avi.cpp )
target_link_libraries( resizer ${OpenCV_LIBS} )

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
