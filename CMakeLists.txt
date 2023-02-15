# CMakeList.txt: Unchanged 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 设置vcpkg的cmake工具链路径
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_TRIPLET "x64-windows")

project ("Unchanged")

# 包含的库
find_package(assimp CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
find_package(Bullet CONFIG REQUIRED)

include_directories(./include)              #头文件位置
aux_source_directory(./src DIR_TOOT_SRCS)
aux_source_directory(./src/Model DIR_TOOT_SRCS)
aux_source_directory(./src/Game DIR_TOOT_SRCS)
aux_source_directory(./src/Event DIR_TOOT_SRCS)

file(COPY "shader/" DESTINATION "shader/")	# 复制shader文件到执行文件夹
file(COPY "world_model/" DESTINATION "world_model/")	

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME} ${DIR_TOOT_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BULLET_LIBRARIES})

# TODO: 如有需要，请添加测试并安装目标。
