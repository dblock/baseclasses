/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Serge Huber - huber@xo3.com
*/


#ifndef BASE_ORB_HPP
#define BASE_ORB_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>
#include <String/String.hpp>
#include <HashTable/HashTable.hpp>
#include <Mutex/Mutex.hpp>

class COrb : public CObject {
	private_property(CHashTable<CObject *>, ObjectTable);
public:
	// Object management procedures
	bool AddObject(const CString&, CObject*);
	bool RemoveObject(const CString&);
	CObject *FindObject(const CString&) const;
	// Object repository procedures
	COrb(void);
	virtual ~COrb(void);
	// later will probably be added routines to make multiple
	// ORB's interface with each other, especially remote ones.
};

#endif
