//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CJSONPARSER_H_
#define _CJSONPARSER_H_
#include "AvatarConfig.h"
#include <string>
#include <map>
#include <vector>
using std::string;
using std::map;
using std::vector;

/**
* @brief JSON ������
*/
class AVATAR_EXPORT CJsonParser {
public:
	//! Ĭ�Ϲ��캯��
	CJsonParser();
	//! ָ���ַ�������
	CJsonParser(const char* content);
	//! ʹ��ָ�����ȵ��ַ�������
	CJsonParser(const char* content, int length);

	//! ����ָ���ַ���
	bool Parse(const char* content, int length);
	//! �ж��Ƿ����ָ��ֵ
	bool IsContain(const char* name) const;
	//! ��ȡ����ڵ������С
	int GetCount() const;

	//! ��ȡ�ڵ���ָ�����Ƶ�ֵ
	CJsonParser& GetValue(const char* name);
	//! ��ȡ�ڵ���ָ�����Ƶ�ֵ
	CJsonParser& GetValue(const string& name);
	//! ��ȡ����ڵ���ָ����ֵ
	CJsonParser& GetValue(int index);
	//! ͬ GetValue(const char* name)
	CJsonParser& operator [] (const char* name);
	//! ͬ GetValue(const string& name)
	CJsonParser& operator [] (const string& name);
	//! ͬ GetValue(int index)
	CJsonParser& operator [] (int index);

public:
	//! �Ƿ�Ϊ�սڵ�
	bool IsEmpty() const { return m_sValue.type == SValue::NIL; }
	//! �Ƿ�Ϊ�������ͽڵ�
	bool IsBool() const { return m_sValue.type == SValue::BOOL; }
	//! �Ƿ�Ϊ�������ͽڵ�
	bool IsInteger() const { return m_sValue.type == SValue::INTEGER; }
	//! �Ƿ�Ϊ�������ͽڵ�
	bool IsFloat() const { return m_sValue.type == SValue::FLOAT; }
	//! �Ƿ�Ϊ�ַ������ͽڵ�
	bool IsString() const { return m_sValue.type == SValue::STRING; }
	//! �Ƿ�Ϊ�������ͽڵ�
	bool IsObject() const { return m_sValue.type == SValue::OBJECT; }
	//! �Ƿ�Ϊ�������ͽڵ�
	bool IsArray() const { return m_sValue.type == SValue::ARRAY; }

	//! ת��Ϊ��������
	bool ToBool() const;
	//! ת��Ϊ��������
	int ToInt() const;
	//! ת��Ϊ��������
	float ToFloat() const;
	//! ת��Ϊ�ַ�������
	string ToString() const;

private:
	//! JSON ֵ������
	typedef struct _SValue {
		enum { NIL, BOOL, INTEGER, FLOAT, STRING, OBJECT, ARRAY } type;
		int length;
		union {
			bool bValue;
			int iValue;
			float fValue;
			const char* pValue;
		};
	} SValue;

	//! ��������ֵ
	int ParseObject(const char* data, int length, CJsonParser& parser);
	//! ��������ֵ
	int ParseArray(const char* data, int length, CJsonParser& parser);
	//! ������ͨ�ַ���
	int ParseString(const char* data, int length, CJsonParser& parser);
	//! ������ֵ
	int ParseNull(const char* data, int length, CJsonParser& parser);
	//! ���������ַ���
	int ParseBool(const char* data, int length, CJsonParser& parser);
	//! ���������ַ���
	int ParseNumber(const char* data, int length, CJsonParser& parser);

private:
	//! JSON ֵ
	SValue m_sValue;
	//! JSON �ڲ���Ԫ��
	map<string, CJsonParser> m_mapValues;
	//! JSON �ڲ�����
	vector<CJsonParser> m_vecValues;
};

#endif