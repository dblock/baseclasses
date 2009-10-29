/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  _____________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_PROGRESS_HPP
#define BASE_PROGRESS_HPP

#include <platform/include.hpp>
#include <Object/Object.hpp>

class CProgress : public CObject {
	property(int, Segment);
	property(char, SegmentChar);
	readonly_property(int, Current);
	readonly_property(int, Shown);
	readonly_property(int, Previous);
public:
	CProgress(int Segment = 10, bool Verbose = true);
	virtual ~CProgress(void);
	void Init(bool Verbose = true);
	void Init(int, bool);
	bool Show(int Current, int Total, bool Verbose = true);
    void Error(bool Verbose = true);
	void Finish(bool Verbose = true);
};

#endif
