cmake_minimum_required(VERSION 2.8)
project( finalProject )
find_package( OpenCV REQUIRED )

add_executable( gmm gmm.cpp bsgmm.cpp rect.cpp avi.cpp )
target_link_libraries( gmm ${OpenCV_LIBS} )

add_executable( roadmap roadmap.cpp bsgmm.cpp rect.cpp ptrans.cpp )
target_link_libraries( roadmap ${OpenCV_LIBS} )

add_executable( testptrans testptrans.cpp ptrans.cpp )
target_link_libraries( testptrans ${OpenCV_LIBS} )

add_executable( project project.cpp bsgmm.cpp rect.cpp ptrans.cpp avi.cpp tracker.cpp multiTracker.cpp kalman.cpp hungarianAlg.cpp )
target_link_libraries( project ${OpenCV_LIBS} )

add_executable( resizer resizer.cpp avi.cpp )
target_link_libraries( resizer ${OpenCV_LIBS} )

add_executable( gmmTracking gmmTracking.cpp kalman.cpp tracker.cpp multiTracker.cpp hungarianAlg.cpp bsgmm.cpp rect.cpp avi.cpp )
target_link_libraries( gmmTracking ${OpenCV_LIBS} )

add_executable( testHungarian testHungarian.cpp hungarianAlg.cpp )
target_link_libraries( testHungarian ${OpenCV_LIBS} )

set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
