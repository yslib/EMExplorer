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


	// For arithmetic.h



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
