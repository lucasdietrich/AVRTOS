# In cmake : add all subdirectory
set(examples_dir ${CMAKE_CURRENT_LIST_DIR})

file(GLOB samples RELATIVE ${examples_dir} ${examples_dir}/*)

foreach(example ${samples})
	if(IS_DIRECTORY ${examples_dir}/${example} )
		# Allow to build only a single sample to not lost VS Code Intellisense with defines
		# in kernel code
		if (NOT DEFINED ENABLE_SINGLE_SAMPLE OR ENABLE_SINGLE_SAMPLE STREQUAL example)
			if(EXISTS "${examples_dir}/${example}/CMakeLists.txt")
				message(${examples_dir}/${example})
				add_subdirectory(${examples_dir}/${example})
			endif()
		endif()
	endif()
endforeach()
