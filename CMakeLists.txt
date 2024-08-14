# CMakeList.txt : CMake project for space, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/CompilerOptions.cmake")

if(MSVC)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "/MDd /Ob0 /Od /RTC1 /utf-8")
set(CMAKE_CXX_FLAGS_INIT "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /utf-8")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "/MD /O1 /Ob1 /DNDEBUG /utf-8")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "/MD /O2 /Ob2 /DNDEBUG /utf-8")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MD /O2 /Ob1 /DNDEBUG /utf-8")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("space")

# Add source to this project's executable.
set(SRCS	"src/Main.cpp"
			"src/engine/Engine.cpp"
			"src/engine/Render.cpp"
			"src/engine/Localization.cpp")
add_executable (space ${SRCS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET space PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
