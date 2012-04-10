macro(SetUpLTR LTR_Source_Path)
	# let use pthread on unix
	IF (UNIX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -pthread")
	ENDIF (UNIX)

	########################################################################################
	# lets SETup boost 
	INCLUDE (${LTR_Source_Path}/cmake/boost.cmake)

	##########################################################################################
	# lets SETup ltr
	SET(LTR_Install_Path ${LTR_Source_Path}/lib)

	INCLUDE_DIRECTORIES(${LTR_Source_Path})
	INCLUDE_DIRECTORIES(${LTR_Source_Path}/contrib)
	LINK_DIRECTORIES(${LTR_Install_Path})

	FIND_LIBRARY(LTR_Library ltr HINTS ${LTR_Install_Path})

	# lets SETup Utilities
	FIND_LIBRARY(util_timer_library util_timer HINTS ${LTR_Install_Path})
	FIND_LIBRARY(util_logger_library util_logger HINTS ${LTR_Install_Path})
	SET(UTILITY util_logger util_timer)

	# lets SETup Puppy
	FIND_LIBRARY(puppy_library puppy HINTS ${LTR_Install_Path})

	MESSAGE("LTR_Library           : ${LTR_Library}")
	MESSAGE("LTR_Source_Path       : ${LTR_Source_Path}")
	
	SET(LTR_libs ltr puppy ${UTILITY} ${Boost_LIBRARIES})
endmacro()
