add_executable(DartZeroGravityDemo ZeroGravityDemo.cpp)
target_link_libraries(DartZeroGravityDemo ${DartSimLink})

add_executable(DartAnymalDemo AnymalDemo.cpp)
target_link_libraries(DartAnymalDemo ${DartSimLink})

add_executable(DartAnymalEnergyDemo AnymalEnergyDemo.cpp)
target_link_libraries(DartAnymalEnergyDemo ${DartSimLink})

add_executable(DartAtlasDemo AtlasDemo.cpp)
target_link_libraries(DartAtlasDemo ${DartSimLink})

add_executable(DartRigidBodyDemo RigidBodyDemo.cpp)
target_link_libraries(DartRigidBodyDemo ${DartSimLink})

add_executable(DartBuildingDemo BuildingDemo.cpp)
target_link_libraries(DartBuildingDemo ${DartSimLink})

add_executable(DartStackDemo StackDemo.cpp)
target_link_libraries(DartStackDemo ${DartSimLink})