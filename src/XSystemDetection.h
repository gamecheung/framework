#ifndef _X_SYSTEM_DETECTION_H
#define _X_SYSTEM_DETECTION_H

/**
The operating system, must be one of: (X_OS_x)

	DARWIN   - Any Darwin system
	MAC      - OS X and iOS
	OSX      - OS X
	IOS      - iOS
	MSDOS    - MS-DOS and Windows
	OS2      - OS/2
	OS2EMX   - XFree86 on OS/2 (not PM)
	WIN32    - Win32 (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
	WINCE    - WinCE (Windows CE 5.0)
	WINRT    - WinRT (Windows 8 Runtime)
	CYGWIN   - Cygwin
	SOLARIS  - Sun Solaris
	HPUX     - HP-UX
	ULTRIX   - DEC Ultrix
	LINUX    - Linux [has variants]
	FREEBSD  - FreeBSD [has variants]
	NETBSD   - NetBSD
	OPENBSD  - OpenBSD
	BSDI     - BSD/OS
	IRIX     - SGI Irix
	OSF      - HP Tru64 UNIX
	SCO      - SCO OpenServer 5
	UNIXWARE - UnixWare 7, Open UNIX 8
	AIX      - AIX
	HURD     - GNU Hurd
	DGUX     - DG/UX
	RELIANT  - Reliant UNIX
	DYNIX    - DYNIX/ptx
	QNX      - QNX [has variants]
	QNX6     - QNX RTP 6.1
	LYNX     - LynxOS
	BSD4     - Any BSD 4.4 system
	UNIX     - Any UNIX BSD/SYSV system
	ANDROID  - Android platform
	HAIKU    - Haiku

The following operating systems have variants:
	LINUX    - both X_OS_LINUX and X_OS_ANDROID are defined when building for Android
			 - only X_OS_LINUX is defined if building for other Linux systems
	QNX      - both X_OS_QNX and X_OS_BLACKBERRY are defined when building for Blackberry 10
			 - only X_OS_QNX is defined if building for other QNX targets
	FREEBSD  - X_OS_FREEBSD is defined only when building for FreeBSD with a BSD userland
			 - X_OS_FREEBSD_KERNEL is always defined on FreeBSD, even if the userland is from GNU
*/

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define X_OS_DARWIN
#  define X_OS_BSD4
#  ifdef __LP64__
#    define X_OS_DARWIN64
#  else
#    define X_OS_DARWIN32
#  endif
#elif defined(__ANDROID__) || defined(ANDROID)
#  define X_OS_ANDROID
#  define X_OS_LINUX
#elif defined(__CYGWIN__)
#  define X_OS_CYGWIN
#elif !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define X_OS_WIN32
#  define X_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINCE) || defined(_WIN32_WCE)
#    define X_OS_WINCE
#  elif defined(WINAPI_FAMILY)
#    if defined(WINAPI_FAMILY_PHONE_APP) && WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
#      define X_OS_WINPHONE
#      define X_OS_WINRT
#    elif WINAPI_FAMILY==WINAPI_FAMILY_PC_APP
#      define X_OS_WINRT
#    else
#      define X_OS_WIN32
#    endif
#  else
#    define X_OS_WIN32
#  endif
#elif defined(__sun) || defined(sun)
#  define X_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define X_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define X_OS_ULTRIX
#elif defined(sinix)
#  define X_OS_RELIANT
#elif defined(__native_client__)
#  define X_OS_NACL
#elif defined(__linux__) || defined(__linux)
#  define X_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#  ifndef __FreeBSD_kernel__
#    define X_OS_FREEBSD
#  endif
#  define X_OS_FREEBSD_KERNEL
#  define X_OS_BSD4
#elif defined(__NetBSD__)
#  define X_OS_NETBSD
#  define X_OS_BSD4
#elif defined(__OpenBSD__)
#  define X_OS_OPENBSD
#  define X_OS_BSD4
#elif defined(__bsdi__)
#  define X_OS_BSDI
#  define X_OS_BSD4
#elif defined(__sgi)
#  define X_OS_IRIX
#elif defined(__osf__)
#  define X_OS_OSF
#elif defined(_AIX)
#  define X_OS_AIX
#elif defined(__Lynx__)
#  define X_OS_LYNX
#elif defined(__GNU__)
#  define X_OS_HURD
#elif defined(__DGUX__)
#  define X_OS_DGUX
#elif defined(__QNXNTO__)
#  define X_OS_QNX
#elif defined(_SEQUENT_)
#  define X_OS_DYNIX
#elif defined(_SCO_DS) /* SCO OpenServer 5 + GCC */
#  define X_OS_SCO
#elif defined(__USLC__) /* all SCO platforms + UDK or OUDK */
#  define X_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define X_OS_UNIXWARE
#elif defined(__INTEGRITY)
#  define X_OS_INTEGRITY
#elif defined(VXWORKS) /* there is no "real" VxWorks define - this has to be set in the mkspec! */
#  define X_OS_VXWORKS
#elif defined(__HAIKU__)
#  define X_OS_HAIKU
#elif defined(__MAKEDEPEND__)
#else
#  error "Xd has not been ported to this OS - see http://www.qt-project.org/"
#endif


#if defined(X_OS_WIN32) || defined(X_OS_WIN64) || defined(X_OS_WINCE) || defined(X_OS_WINRT)
#  define X_OS_WIN
#endif

#if defined(X_OS_DARWIN)
#  define X_OS_MAC
#  if defined(X_OS_DARWIN64)
#     define X_OS_MAC64
#  elif defined(X_OS_DARWIN32)
#     define X_OS_MAC32
#  endif
#  include <TargetConditionals.h>
#  if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#     define X_OS_IOS
#  elif defined(TARGET_OS_MAC) && TARGET_OS_MAC
#     define X_OS_OSX
#     define X_OS_MACX // compatibility synonym
#  endif
#endif

#if defined(X_OS_WIN)
#  undef X_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define X_OS_UNIX
#endif

#ifdef X_OS_DARWIN
#  include <Availability.h>
#  include <AvailabilityMacros.h>
#
#  ifdef X_OS_OSX
#    if !defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6
#       undef __MAC_OS_X_VERSION_MIN_REQUIRED
#       define __MAC_OS_X_VERSION_MIN_REQUIRED __MAC_10_6
#    endif
#    if !defined(MAC_OS_X_VERSION_MIN_REQUIRED) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_6
#       undef MAC_OS_X_VERSION_MIN_REQUIRED
#       define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_6
#    endif
#  endif
#
#  // Numerical checks are preferred to named checks, but to be safe
#  // we define the missing version names in case Qt uses them.
#
#  if !defined(__MAC_10_7)
#       define __MAC_10_7 1070
#  endif
#  if !defined(__MAC_10_8)
#       define __MAC_10_8 1080
#  endif
#  if !defined(__MAC_10_9)
#       define __MAC_10_9 1090
#  endif
#  if !defined(__MAC_10_10)
#       define __MAC_10_10 101000
#  endif
#  if !defined(__MAC_10_11)
#       define __MAC_10_11 101100
#  endif
#  if !defined(MAC_OS_X_VERSION_10_7)
#       define MAC_OS_X_VERSION_10_7 1070
#  endif
#  if !defined(MAC_OS_X_VERSION_10_8)
#       define MAC_OS_X_VERSION_10_8 1080
#  endif
#  if !defined(MAC_OS_X_VERSION_10_9)
#       define MAC_OS_X_VERSION_10_9 1090
#  endif
#  if !defined(MAC_OS_X_VERSION_10_10)
#       define MAC_OS_X_VERSION_10_10 101000
#  endif
#  if !defined(MAC_OS_X_VERSION_10_11)
#       define MAC_OS_X_VERSION_10_11 101100
#  endif
#
#  if !defined(__IPHONE_4_3)
#       define __IPHONE_4_3 40300
#  endif
#  if !defined(__IPHONE_5_0)
#       define __IPHONE_5_0 50000
#  endif
#  if !defined(__IPHONE_5_1)
#       define __IPHONE_5_1 50100
#  endif
#  if !defined(__IPHONE_6_0)
#       define __IPHONE_6_0 60000
#  endif
#  if !defined(__IPHONE_6_1)
#       define __IPHONE_6_1 60100
#  endif
#  if !defined(__IPHONE_7_0)
#       define __IPHONE_7_0 70000
#  endif
#  if !defined(__IPHONE_7_1)
#       define __IPHONE_7_1 70100
#  endif
#  if !defined(__IPHONE_8_0)
#       define __IPHONE_8_0 80000
#  endif
#  if !defined(__IPHONE_8_1)
#       define __IPHONE_8_1 80100
#  endif
#  if !defined(__IPHONE_8_2)
#       define __IPHONE_8_2 80200
#  endif
#  if !defined(__IPHONE_8_3)
#       define __IPHONE_8_3 80300
#  endif
#  if !defined(__IPHONE_8_4)
#       define __IPHONE_8_4 80400
#  endif
#  if !defined(__IPHONE_9_0)
#       define __IPHONE_9_0 90000
#  endif
#endif

#ifdef __LSB_VERSION__
#  if __LSB_VERSION__ < 40
#    error "This version of the Linux Standard Base is unsupported"
#  endif
#ifndef XD_LINUXBASE
#  define XD_LINUXBASE
#endif
#endif


#endif //_X_SYSTEM_DETECTION_H
