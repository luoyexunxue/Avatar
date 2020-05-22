//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSTREAMREADER_H_
#define _CSTREAMREADER_H_
#include "CLog.h"
#include <cstring>

/**
* @brief ����ȡ��
*/
class CStreamReader {
public:
	/**
	* Ĭ�Ϲ��캯��
	*/
	CStreamReader(unsigned char* buffer, int size) {
		m_pBuffer = buffer;
		m_pCurrent = buffer;
		m_iBufferSize = size;
		m_iCurrentPos = 0;
	}

	/**
	* ������ָ��λ��
	*/
	CStreamReader& SetPosition(int pos) {
		m_pCurrent = m_pBuffer + pos;
		m_iCurrentPos = pos;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamReader SetPosition() out of range");
		}
		return *this;
	}

	/**
	* ����ָ������ָ���ֽ���
	*/
	CStreamReader& Skip(int nByte) {
		m_pCurrent += nByte;
		m_iCurrentPos += nByte;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamReader Skip() out of range");
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
	* ��ȡֵ
	*/
	template <typename T>
	T GetValue() {
		T value;
		if (m_iCurrentPos + static_cast<int>(sizeof(T)) > m_iBufferSize) {
			CLog::Error("CStreamReader GetValue() out of range");
		}
		memcpy(&value, m_pCurrent, sizeof(T));
		m_pCurrent += sizeof(T);
		m_iCurrentPos += sizeof(T);
		return value;
	}

	/**
	* ��ȡ����
	*/
	int Read(unsigned char* data, int size) {
		int readSize = m_iCurrentPos + size > m_iBufferSize? m_iBufferSize - m_iCurrentPos: size;
		memcpy(data, m_pCurrent, readSize);
		m_pCurrent += readSize;
		m_iCurrentPos += readSize;
		return readSize;
	}

	/**
	* ��������
	*/
	template <typename T>
	CStreamReader& operator >> (T& value) {
		value = GetValue<T>();
		return *this;
	}

private:
	unsigned char* m_pBuffer;
	unsigned char* m_pCurrent;
	int m_iBufferSize;
	int m_iCurrentPos;
};

#endif