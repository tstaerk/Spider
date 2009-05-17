project( spider )

find_package( KDE4 REQUIRED )

FIND_PATH(LIBTIDY_INCLUDE_DIR tidy.h)
if( NOT LIBTIDY_INCLUDE_DIR )
    find_path(LIBTIDY_INCLUDE_DIR tidy.h PATH_SUFFIXES tidy)
    set(LIBTIDY_INCLUDE_DIR ${LIBTIDY_INCLUDE_DIR}/tidy CACHE PATH "Tidy include dir")
endif( NOT LIBTIDY_INCLUDE_DIR )

include_directories( ${KDE4_INCLUDES} ${LIBTIDY_INCLUDE_DIR} )
 
set( KDE4ProjectSources parser.cpp spider.cpp )
 
kde4_add_executable( spider ${KDE4ProjectSources} )
 
target_link_libraries( spider ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} tidy )
