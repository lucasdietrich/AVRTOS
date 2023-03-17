# In cmake : add all subdirectory
set(curdir ${CMAKE_CURRENT_LIST_DIR})

file(GLOB children RELATIVE ${curdir} ${curdir}/*)

foreach(child ${children})
	if(IS_DIRECTORY ${curdir}/${child} )
		# Allow to build only a single sample to not lost VS Code Intellisense with defines
		# in kernel code
		if (NOT DEFINED ENABLE_SINGLE_SAMPLE OR ENABLE_SINGLE_SAMPLE STREQUAL child)
			message(${curdir}/${child})
			add_subdirectory(${curdir}/${child})
		endif()
	endif()
endforeach()
