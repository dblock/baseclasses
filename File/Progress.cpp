/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  _____________________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>

#include "Progress.hpp"

CProgress::CProgress(int Segment, bool Verbose) {
	m_Segment = BASE_MAX(1, Segment);
	m_SegmentChar = '*';
	Init(Verbose);
}

void CProgress::Init(bool Verbose) {
	m_Current = m_Previous = m_Shown = 0;
	if (Verbose) { cout << '['; cout.flush(); }
}

void CProgress::Init(int Segment, bool Verbose) {
	m_Segment = BASE_MAX(1, Segment);
	Init(Verbose);
}

CProgress::~CProgress(void) {

}

bool CProgress::Show(int Current, int Total, bool Verbose) {
	if (Total < Current) return false;
	m_Current = (Current * 100) / BASE_MAX(1, Total);
	if (m_Current > m_Previous + m_Segment) {
		if (Verbose) {
			cout << m_SegmentChar; cout.flush();
			m_Shown++;
		}
		m_Previous = m_Current;
		return true;
	} else return false;
}

void CProgress::Finish(bool Verbose) {
	if (Verbose) {
		for (register int i = m_Shown; i < (100/m_Segment); i++) 
            cout << m_SegmentChar;
		cout << ']'; cout.flush();
	}
}

void CProgress::Error(bool Verbose) {
	if (Verbose) {
        cout << 'e';
		for (register int i = ++m_Shown; i < (100/m_Segment); i++) 
            cout << ' ';
		cout << ']'; cout.flush();
	}
}
