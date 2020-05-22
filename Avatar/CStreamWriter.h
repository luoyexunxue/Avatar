//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSTREAMWRITER_H_
#define _CSTREAMWRITER_H_
#include "CLog.h"
#include <cstring>

/**
* @brief ��д����
*/
class CStreamWriter {
public:
	/**
	* Ĭ�Ϲ��캯��
	*/
	CStreamWriter(unsigned char* buffer, int size) {
		m_pBuffer = buffer;
		m_pCurrent = buffer;
		m_iBufferSize = size;
		m_iCurrentPos = 0;
	}

	/**
	* ������ָ��λ��
	*/
	CStreamWriter& SetPosition(int pos) {
		m_pCurrent = m_pBuffer + pos;
		m_iCurrentPos = pos;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamWriter SetPosition() out of range");
		}
		return *this;
	}

	/**
	* ����ָ������ָ���ֽ���
	*/
	CStreamWriter& Skip(int nByte) {
		m_pCurrent += nByte;
		m_iCurrentPos += nByte;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamWriter Skip() out of range");
		}
		return *this;
	}

	/**
	* ��ȡ��ǰλ��
	*/
	int GetPosition() {
		return m_iCurrentPos;
	}

	/**
	* ʣ���ֽ���
	*/
	int Available() {
		return m_iBufferSize - m_iCurrentPos;
	}

	/**
	* ��ȡ��ǰָ��
	*/
	unsigned char* GetPointer() {
		return m_pCurrent;
	}

	/**
	* ����ֵ
	*/
	template <typename T>
	void SetValue(T& value) {
		if (m_iCurrentPos + static_cast<int>(sizeof(T)) > m_iBufferSize) {
			CLog::Error("CStreamWriter SetValue() out of range");
		}
		memcpy(m_pCurrent, &value, sizeof(T));
		m_pCurrent += sizeof(T);
		m_iCurrentPos += sizeof(T);
	}

	/**
	* д������
	*/
	int Write(unsigned char* data, int size) {
		int writeSize = m_iCurrentPos + size > m_iBufferSize? m_iBufferSize - m_iCurrentPos: size;
		memcpy(m_pCurrent, data, writeSize);
		m_pCurrent += writeSize;
		m_iCurrentPos += writeSize;
		return writeSize;
	}

	/**
	* ��������
	*/
	template <typename T>
	CStreamWriter& operator << (T& value) {
		SetValue<T>(value);
		return *this;
	}

private:
	unsigned char* m_pBuffer;
	unsigned char* m_pCurrent;
	int m_iBufferSize;
	int m_iCurrentPos;
};

#endif