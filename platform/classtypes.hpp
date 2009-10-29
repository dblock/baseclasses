/*
    © Vestris Inc., Geneva, Switzerland
    http://www.vestris.com, 1994-1999 All Rights Reserved
    ______________________________________________

    written by Daniel Doubrovkine - dblock@vestris.com

*/

#ifndef BASE_CLASS_TYPES
#define BASE_CLASS_TYPES

#define protected_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field;

#define copy_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline void Set##_Field(_FieldType _Value) { m_##_Field = _Value; } \
     inline _FieldType Get##_Field(void) const { return m_##_Field; }

#define private_copy_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  private: \
     inline void Set##_Field(_FieldType _Value) { m_##_Field = _Value; } \
     inline _FieldType Get##_Field(void) const { return m_##_Field; }

#define readonly_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline _FieldType& Get##_Field(void) { return m_##_Field; } \
     inline const _FieldType& Get##_Field(void) const { return m_##_Field; }

#define bool_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline bool Set##_Field(_FieldType _Value) { \
        if (m_##_Field != _Value) { \
            m_##_Field = _Value; \
            return true; \
        } else return false; \
     } \
     inline _FieldType& Get##_Field(void) { return m_##_Field; } \
     inline const _FieldType& Get##_Field(void) const { return m_##_Field; }

#define bool_readonly_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline _FieldType& Get##_Field(void) { return m_##_Field; } \
     inline const _FieldType& Get##_Field(void) const { return m_##_Field; }

#define bool_writeonly_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline bool Set##_Field(_FieldType _Value) { \
        if (m_##_Field != _Value) { \
            m_##_Field = _Value; \
            return true; \
        } else return false; \
     }

#define mutable_private_property(_FieldType, _Field) \
   protected: \
     mutable _FieldType m_##_Field; \
     inline _FieldType& Get##_Field(void) { return m_##_Field; } \
     inline const _FieldType& Get##_Field(void) const { return m_##_Field; } \
     inline void Set##_Field(_FieldType _Value) const { m_##_Field = _Value; }

#define mutable_property(_FieldType, _Field) \
  protected: \
     mutable _FieldType m_##_Field; \
  public: \
     inline void Set##_Field(_FieldType _Value) { m_##_Field = _Value; } \
     inline _FieldType& Get##_Field(void) { return m_##_Field; } \
     inline const _FieldType& Get##_Field(void) const { return m_##_Field; }

#define property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline void Set##_Field(_FieldType _Value) { m_##_Field = _Value; } \
     inline _FieldType& Get##_Field(void) { return m_##_Field; } \
     inline const _FieldType& Get##_Field(void) const { return m_##_Field; }

#define writeonly_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline void Set##_Field(_FieldType _Value) { m_##_Field = _Value; }

#define private_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
     inline _FieldType& Get##_Field(void) { return m_##_Field; } \
     inline const _FieldType& Get##_Field(void) const { return m_##_Field; } \
     inline void Set##_Field(_FieldType _Value) { m_##_Field = _Value; }

#define readonly_copy_property(_FieldType, _Field) \
  protected: \
     _FieldType m_##_Field; \
  public: \
     inline _FieldType Get##_Field(void) const { return m_##_Field; }

#endif
