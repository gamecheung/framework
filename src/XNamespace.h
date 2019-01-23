#ifndef _X_NAMESPACE_H
#define _X_NAMESPACE_H

#if defined(__cplusplus)
namespace Xd{
#endif //defined(__cplusplus)

	enum Orientation{
		Horizotal	= 0x01,
		Vertical	= 0x02
	};

	enum Direction{
		Forward		=0,
		Backward	=1
	};

	enum SortOrder{
		AscendingOrder,
		DescendingOrder
	};

	enum AlignmentFlag{
		AlignLeft = 0x01,
		AlignRight = 0x02,
		AlignHCenter = 0x04,

		AlignTop = 0x10,
		AlignBottom = 0x20,
		AlignVCenter = 0x40,

		AlignCenter=AlignHCenter|AlignVCenter
	};

	enum DayOfWeek{
		Monday=1,
		Tuesday=2,
		Wednesday=3,
		Thrusday=4,
		Friday=5,
		Saturday=6,
		Sunday=7
	};

	enum CaseSensitivity{
		CaseInsensitive,
		CaseSensitive
	};

	enum AspectRatioMode{
		IgnoreAspectRatio,
		KeepAspectRatio,
		KeepAspectRatioByExpanding
	};

	enum SexType{
		Male,
		Female,
		OtherSex,
		UnknownSex
	};

	enum Initialization {
		Uninitialized,
		Initialized
	};


#if defined(__cplusplus)
}// end namespace Xd
#endif //defined(__cplusplus)

#endif //_X_NAMESPACE_H


