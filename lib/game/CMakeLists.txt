add_library(game
	"src/editorpart.h"
	"src/editorpart.cpp"
)

target_include_directories(game
  INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  PRIVATE
  boiler)
target_link_libraries(game PRIVATE boiler)
