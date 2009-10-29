/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#include <baseclasses.hpp>
#include  "HtmlTag.hpp"

CHtmlTag::CHtmlTag(void) {

}

CHtmlTag::CHtmlTag(const CString& Name) {
	m_Name = Name;
}

CHtmlTag::~CHtmlTag(void) {

}

CHtmlTag::CHtmlTag(const CHtmlTag& HtmlTag) {
	operator=(HtmlTag);
}

CHtmlTag& CHtmlTag::operator=(const CHtmlTag& HtmlTag){
	m_Name = HtmlTag.m_Name;
	m_Parameters = HtmlTag.m_Parameters;
	m_Free = HtmlTag.m_Free;
	return * this;
}

CString CHtmlTag::GetBuffer(void) const {
	CString Result(m_Free);
	if (m_Name.GetLength()) {
		Result = ('<' + m_Name);
		for (register int i=0;i<(int)m_Parameters.GetSize();i++) {
			Result.Append(' ' + m_Parameters.GetNameAt(i));
			if (m_Parameters.GetValueAt(i).GetLength())
				Result.Append("=\"" + m_Parameters.GetValueAt(i) + '\"');
		}
		Result.Append('>');
	}
	return Result;
}

ostream& CHtmlTag::operator<<(ostream& Stream) const {
	if (m_Free.GetLength()) Stream << m_Free;
	if (m_Name.GetLength()) {
		Stream << '<' << m_Name;
		for (register int i=0;i<(int)m_Parameters.GetSize();i++) {
			Stream << ' ' << m_Parameters.GetNameAt(i);
			if (m_Parameters.GetValueAt(i).GetLength())
				Stream << "=\"" << m_Parameters.GetValueAt(i) << '\"';
		}
		Stream << '>';
	}
	return Stream;
}

istream& CHtmlTag::operator>>(istream& Stream) {
	assert(0);
	return Stream;
}
