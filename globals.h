#ifndef GLOBALS_H
#define GLOBALS_H

//inline helper function
#include <type_traits>


template<typename T,typename U,typename V>
inline T QueryMarkItemInterface(V mark)
{
	return static_cast<T>(static_cast<U>(mark));
}

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
enum ItemTypes
{
	Slice = 1,
	Mark
};

namespace MarkProperty
{
	enum
	{
		Color,
		CategoryColor,
		Name,
		CategoryName,
		SliceIndex,
		SliceType,
		VisibleState,
		Length,
	};
}




#endif // GLOBALS_H
