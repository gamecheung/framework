#ifndef _X_COMPILER_DETECTION_H
#define _X_COMPILER_DETECTION_H

/**
The compiler, must be of of: (X_CC_x)
	MSVC	- Microsoft Visual C/C++, Inter C++ for Windows
	BOR		- Borland/Turbo C++
	GNU		- GNU C++
	RVCT	- ARM Realview Compiler Suite
	INTEL	- Intel C++ for Linux, Inter C++ for Windows
	CLANG	- C++ front-end for the LLVM compiler
	
*/

#if defined(_MSC_VER)
#	define X_CC_MSVC	(_MSC_VER)

//#elif defined(__BORLANDC__) || defined(__TURBOC__)
//#	define X_CC_BOR
//
#elif defined(__ARMCC__) || defined(__CC_ARM)
#	define X_CC_RVCT
//
//#elif defined(__GNUC__)
//#	define X_CC_GNU
//
//#elif defined(__INTEL_COMPILER)
//#	define X_CC_INTEL	(__INTEL_COMPILER)

#else
#	error "Xd has not been tested with this compiler."
#endif // 

#if defined(X_CC_MSVC) && !defined(X_CC_INTEL)
#  if defined(__cplusplus)
#    if _MSC_VER >= 1400 // VC2005(VC8)
#      define X_COMPILER_VARIADIC_MACROS
#       ifndef __cplusplus_cli
		/* 2005 supports the override and final contextual keywords, in
		the same positions as the C++11 variants, but 'final' is
		called 'sealed' instead:
		http://msdn.microsoft.com/en-us/library/0w2w91tf%28v=vs.80%29.aspx
		The behavior is slightly different in C++/CLI, which requires the
		"virtual" keyword to be present too, so don't define for that.
		So don't define X_COMPILER_EXPLICIT_OVERRIDES (since it's not
		the same as the C++11 version), but define the X_DECL_* flags
		accordingly: */
#       define X_DECL_OVERRIDE	override
#       define X_DECL_FINAL	sealed
#      endif //__cplusplus_cli
#     endif // VS2005
#    if _MSC_VER >= 1600 // VS2010(VC10)
#		define X_COMPILER_NULLPTR
#    endif // VS2010
#    if _MSC_VER >= 1700 // VS2012(VC11)
#      undef X_DECL_OVERRIDE	// undo 2005/2008 settings...
#      undef X_DECL_FINAL		// undo 2005/2008 settings...
#      define X_COMPILER_EXPLICIT_OVERRIDES	// ...and use std C++11 now
#    endif // VS2012
#    if _MSC_VER >= 1800 // VS2013(VC12)
#		define X_COMPILER_DEFAULT_MEMBERS
#		define X_COMPILER_DELETE_MEMBERS
#		define X_COMPILER_DELEGATING_CONSTRUCTORS
#		define X_COMPILER_EXPLICIT_CONVERSIONS
#		define X_COMPILER_NONSTATIC_MEMBER_INIT
#    endif // end VS2013
#    if _MSC_FULL_VER >= 180030324 // VC 12 SP 2 RC
#    endif /* VC 12 SP 2 RC */
#    if _MSC_VER >= 1900 // VS2015(VC14)
#    endif // VS2015
#  endif // __cplusplus
#endif //X_CC_MSVC


// C++11 keywords and expressions

#ifdef X_COMPILER_EXPLICIT_OVERRIDES
#  define X_DECL_OVERRIDE override
#  define X_DECL_FINAL final
#else
#  ifndef X_DECL_OVERRIDE
#    define X_DECL_OVERRIDE
#  endif
#  ifndef X_DECL_FINAL
#    define X_DECL_FINAL
#  endif
#endif

#ifdef X_COMPILER_NULLPTR
#	define X_NULLPTR	nullptr
#else
#	define X_NULLPTR	NULL
#endif

#ifdef X_COMPILER_DELETE_MEMBERS
#	define X_DECL_EQ_DELETE = delete
#else
#	define X_DECL_EQ_DELETE
#endif


#endif //_X_COMPILER_DETECTION_H

