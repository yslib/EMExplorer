#ifndef GLOBALS_H
#define GLOBALS_H

inline 
QGraphicsItem * QueryMarkItemInterface(AbstractMarkItem * mark)
{
	return static_cast<QGraphicsItem*>(static_cast<PolyMarkItem*>(mark));
}

inline 
AbstractMarkItem * QueryMarkItemInterface(QGraphicsItem* mark)
{
	return static_cast<AbstractMarkItem*>(static_cast<PolyMarkItem*>(mark));
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


#endif // GLOBALS_H
