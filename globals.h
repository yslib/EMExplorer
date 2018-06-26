#ifndef GLOBALS_H
#define GLOBALS_H

//inline helper function
#include <type_traits>
#include <exception>

#include <QGraphicsItem>


//template<typename T,typename U,typename V>
//inline T QueryMarkItemInterface(V mark)
//{
//	return static_cast<T>(static_cast<U>(mark));
//}

Q_DECLARE_METATYPE(QScopedPointer<QGraphicsItem>); 


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

class ResourceException:public std::exception
{
public:
	ResourceException():exception("Allocation for resources faildd.",1){}
};
class FileOpenException:public std::exception
{
public:
	FileOpenException() :exception("Opening file failed", 2){}
};





#endif // GLOBALS_H
