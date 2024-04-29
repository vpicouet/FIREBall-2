#ifndef NC_PLATFORM_H
#define NC_PLATFORM_H

// Set platform

#if defined(_WIN64) && !defined(WIN64)
	#define WIN64
#endif

#if defined(_WIN32) && !defined(_WIN64) && !defined(WIN32)
	#define WIN32
#endif

#ifdef WIN32
	#define WINDOWS
#endif

#ifdef WIN64
	#define WINDOWS
#endif

// Set platform
#if defined(__APPLE__)
	#include "TargetConditionals.h"
	#if TARGET_OS_MAC
		#ifndef MACOS
			#define MACOS
			#if defined(LINUX)
				#undef LINUX
			#endif
		#endif
	#endif
	#include <AvailabilityMacros.h>
	// NOTE: Defines (amongst others)
	//       MAC_OS_X_VERSION_MAX_ALLOWED with format MMmmpp
	//       MAC_OS_X_VERSION_MIN_REQUIRED with format MMmmpp
	//       MAC_OS_X_VERSION_10_11 (as appropriate) with format MMmm00
#elif defined(__linux__)
	#ifndef LINUX
	#define LINUX
		#if defined(MACOS)
			#undef MACOS
		#endif
	#endif
//#elif defined(__unix__) // NOTE: More general case if necessary
#endif

#if defined(LINUX) || defined(MACOS)
	#ifndef UNIX
		#define UNIX
	#endif
#endif

#if defined(WINDOWS) && defined(UNIX)
	#error "Too many platforms defined"
#elif defined(LINUX) && defined(MACOS)
	#error "Too many platforms defined"
#elif !defined(LINUX) && !defined(UNIX) && !defined(WINDOWS)
	#error "No platform defined"
#endif

// Set compiler version
#if defined(LINUX) // Expect GCC
	#define GCC_VERSION ( 	__GNUC__ * 10000 \
							+ __GNUC_MINOR__ * 100 \
							+ __GNUC_PATCHLEVEL__)
#endif

#if defined(MACOS) // Expect CLANG (LLVM)
	#define CLANG_VERSION ( 	__clang_major__ * 10000 \
								+ __clang_minor__ * 100 \
								+ __clang_patchlevel__)
	// NOTE: Internal __clang_version__ is also available but is a string with non-trivial structure.
	
#endif



// Set build
#if defined(_DEBUG)
	#ifndef DEBUG
		#define DEBUG
	#endif
#endif

#if defined(NDEBUG)
	#ifndef RELEASE
		#define RELEASE
	#endif
#endif

#if defined(DEBUG) && defined(RELEASE)
	#error "Too many build flags defined"
#endif

#if defined(RELEASE) && !defined(NDEBUG)
	#define NDEBUG
#endif

#endif	// NC_PLATFORM_H
