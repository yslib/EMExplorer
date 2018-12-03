#ifndef GLOBALS_H
#define GLOBALS_H

//inline helper function
#include <type_traits>
#include <exception>
#include <QGraphicsItem>

#include <iostream>


//template<typename T,typename U,typename V>
//inline T QueryMarkItemInterface(V mark)
//{
//	return static_cast<T>(static_cast<U>(mark));
//}




template<typename T>
inline
void DELETEANDSETNULL(std::remove_reference<std::remove_pointer<T>> *& p)
{
	if (p != nullptr)
	{
		delete p;
		p = nullptr;
	}
}


enum class SliceType
{
	Top,		//Z
	Right,		//Y
	Front		//X
};
//Q_DECLARE_METATYPE(SliceType);

enum ItemTypes
{
	Slice = QGraphicsItem::UserType+1,
	StrokeMark
};


namespace ysl
{
	using Float = float;

	constexpr Float Pi = 3.14159265358979323846;

	constexpr Float LOWEST_Float_VALUE = (std::numeric_limits<Float>::lowest)();

	constexpr Float MAX_Float_VALUE = (std::numeric_limits<Float>::max)();		// For fucking min/max macro defined in windows.h

	inline
		void
		ysl_assert_x(const char * where, const char * what, const char * file, int line)noexcept
	{
		std::cerr << where << " " << what << " " << file << " " << line << std::endl;
		assert(false);
	}

#define YSL_ASSERT_X(cond,where,what) ((cond)?(static_cast<void>(0)):ysl_assert_x(where,what,__FILE__,__LINE__))

	// For arithmetic.h

	inline
	Float
	DegreesToRadians(Float degrees)
	{
		return degrees * Float(Pi / 180);
	}

	inline
	Float
	RadiansToDegrees(Float radians)
	{
		return radians * Float(180 / Pi);
	}

}

//namespace MarkProperty
//{
//	enum Property
//	{
//		Color,					//Mark Color
//		CategoryColor,			//color of category the mark belongs to
//		Name,					//Mark name
//		CategoryName,			//Name of category the mark belongs to
//		SliceIndex,				//Index of slice the mark belongs to
//		SliceType,				//Type of the slice the mark belongs to
//		VisibleState,			//Visibility of the mark
//		Length,					//Length of the mark
//		PropertyInfo,			//Info of the mark
//		Mesh					//Mesh of the mark
//	};
//}

#endif // GLOBALS_H
