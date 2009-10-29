/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_ITERATOR_HPP
#define BASE_ITERATOR_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>

class CIterator : public CObject {	
	copy_property(void *, Current);
public:
	CIterator(void);
	virtual ~CIterator(void);
	CIterator(const CIterator&);
	CIterator& operator=(const CIterator&);	
};

#endif
