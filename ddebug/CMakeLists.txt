set(CPPS
	$<$<PLATFORM_ID:Darwin,Linux>:
		lnxdebug.cpp
		lnxmono.cpp
	>
	$<$<PLATFORM_ID:Windows>:
		windebug.cpp
		winmono.cpp
	>
)

add_library(ddebug STATIC ${CPPS})

export(TARGETS ddebug NAMESPACE D3:: FILE ddebugConfig.cmake)
