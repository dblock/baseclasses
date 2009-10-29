/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_TEMPLATES_LIST_HPP
#define BASE_TEMPLATES_LIST_HPP

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4660 4700 )
#endif

#include <platform/include.hpp>

#include <List/List.cpp>

template class CList<CObject *>;
template class CListElement<CObject *>;
template ostream& operator<<(ostream&, CList<CObject *> const &);
 
#include <Internet/Cookie.hpp>
template class CList<CCookie>;
template class CListElement<CCookie>;
template ostream& operator<<(ostream&, CList<CCookie> const &);

#include <Internet/HtmlTag.hpp>
template class CList<CHtmlTag>;
template class CListElement<CHtmlTag>;
template ostream& operator<<(ostream&, CList<CHtmlTag> const &);
 
#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
