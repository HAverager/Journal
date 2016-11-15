#______________________________________________________________________________
#                                                                         FILES
set(headers ../include/utils.h  
		../include/hparser.h  
		../include/journal.h  
		../include/parser.h)
set(sources utils.C  
			hparser.C  
			journal.C  
			parser.C )


#______________________________________________________________________________
#                                                               ROOT DICTIONARY
SET(ROOTDICT lib${PROJECT_NAME}dict)
ROOT_GENERATE_DICTIONARY(${ROOTDICT} ${headers} )

#______________________________________________________________________________
#                                                                    GITVERSION
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in"
#               "${CMAKE_CURRENT_BINARY_DIR}/version.cpp" @ONLY)
#list(APPEND sources "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")


#______________________________________________________________________________
#                                                                       LIBRARY
add_library(${PROJECT_NAME} SHARED ${sources} ${ROOTDICT})
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES} )


#______________________________________________________________________________
#                                                                       INSTALL
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}LibraryDepends
        LIBRARY DESTINATION lib COMPONENT libs
        RUNTIME DESTINATION bin COMPONENT bin)
install(FILES ${headers}
        DESTINATION include/${PROJECT_NAME} COMPONENT dev)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${ROOTDICT}_rdict.pcm
        DESTINATION lib COMPONENT libs)
