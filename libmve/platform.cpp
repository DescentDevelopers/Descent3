#if _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <MMSystem.h>

	unsigned int platform_timeGetTime(void)
	{
		return timeGetTime();
	}

#else
	#include <unistd.h>
	#include <sys/time.h>

	unsigned int platform_timeGetTime(void)
	{
		struct timeval t;
		gettimeofday(&t,NULL);

		return (t.tv_sec*1000 + t.tv_usec/1000);
	}

#endif
