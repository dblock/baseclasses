/*

  © Vestris Inc., Geneva, Switzerland
  http://www.vestris.com, 1994-1999 All Rights Reserved
  ______________________________________________

  written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_XML_TREE_HPP
#define BASE_XML_TREE_HPP

#include <platform/include.hpp>
#include <String/String.hpp>
#include <Tree/Tree.hpp>

typedef enum { xmlnUnknown, xmlnOpen, xmlnClose, xmlnData } CXmlType;

class CXmlNode : public CObject {
  property(CXmlType, Type);                             // type of the XML node
  property(CString,  Data);                             // data of above type
  friend class CXmlTree;
public:
  CXmlNode(void);
  CXmlNode(const CXmlNode& XmlNode);
  virtual ~CXmlNode(void);
  void GetXml(CString& ssResult) const;
  inline virtual CXmlNode& operator=(const CXmlNode& XmlNode) {
    m_Type = XmlNode.m_Type;
    m_Data = XmlNode.m_Data;
    return * this;
  }
  inline virtual CXmlNode& operator=(const CString& String) {
    m_Type = xmlnUnknown;
    m_Data = String;
    return * this;
  }
  inline bool operator<(const CXmlNode& Other) const { return m_Type < Other.m_Type; }
  inline bool operator>(const CXmlNode& Other) const { return m_Type > Other.m_Type; }
  inline bool operator<=(const CXmlNode& Other) const { return m_Type <= Other.m_Type; }
  inline bool operator>=(const CXmlNode& Other) const { return m_Type >= Other.m_Type; }
  inline bool operator!=(const CXmlNode& Other) const { return m_Type != Other.m_Type; }
  inline bool operator==(const CXmlNode& Other) const { return m_Data == Other.m_Data; }
  // is tag self-terminating (with a / at the end)
  inline bool GetSelfTerm(void) { return (m_Data.GetLength() && (m_Data[m_Data.GetLength() - 1] == '/')); }
  CString GetQuotedData(void) const;
};

class CXmlTree : public CTree< CXmlNode >
{
private:
  bool ReadTag(const CString& String, long& CurPos, CXmlNode& Tag);
  bool ReadString(const CString& String, long& CurPos, CString& ssResult);
  bool Parse(const CString& String, CTreeElement< CXmlNode > * Node);
public:
  CXmlTree(void);
  virtual ~CXmlTree(void);
  bool GetXml(CString& ssResult, CTreeElement< CXmlNode > * Node) const;
  bool GetWml(CString& ssResult, CTreeElement< CXmlNode > * Node) const;
  bool SetXml(const CString& String);
  CTreeElement< CXmlNode > * XmlFind(const CVector< CString >& EltArray, bool bCreate = false) const;  
  CTreeElement< CXmlNode > * XmlFind(const CString& String) const;  
  bool SetValue(const CString& Path, const CString& Value);
  CString GetValueSS(const CString& Path) const;
  bool GetValue(const CString& Path,  CString& Value) const;
  bool GetXml(CString& ssResult) const;
  bool GetWml(CString& ssResult) const;
  CString GetXmlSS(void) const; 
};


 
#endif
