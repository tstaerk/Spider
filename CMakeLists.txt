project( spider )
find_package( KDE4 REQUIRED )
include_directories( ${KDE4_INCLUDES} )
 
set( KDE4ProjectSources parser.cpp spider.cpp )
 
kde4_add_executable( spider ${KDE4ProjectSources} )
 
target_link_libraries( spider ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} tidy )
