FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} )
find_package(Qt4 REQUIRED)
FIND_PACKAGE( ZLIB REQUIRED)

set(wings-thumbnailer-kde_PART_SRCS wingscreator.cpp)
set(wings_test_SRCS wingscreator.cpp main.cpp)
kde4_add_plugin(wings-thumbnailer-kde ${wings-thumbnailer-kde_PART_SRCS})

target_link_libraries(wings-thumbnailer-kde ${KDE4_KIO_LIBS} ${ZLIB_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
add_executable(test_wings_thumbnailer ${wings_test_SRCS})
target_link_libraries(test_wings_thumbnailer ${KDE4_KIO_LIBS} ${ZLIB_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS wings-thumbnailer-kde DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES wings-thumbnailer-kde.desktop DESTINATION ${SERVICES_INSTALL_DIR})
