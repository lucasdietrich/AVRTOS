# In cmake : add all subdirectory
set(curdir ${CMAKE_CURRENT_LIST_DIR})

file(GLOB children RELATIVE ${curdir} ${curdir}/*)

foreach(child ${children})
	if(IS_DIRECTORY ${curdir}/${child})
		message(${curdir}/${child})
		add_subdirectory(${curdir}/${child})
	endif()
endforeach()
