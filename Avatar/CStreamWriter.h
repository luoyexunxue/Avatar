//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSTREAMWRITER_H_
#define _CSTREAMWRITER_H_
#include "CLog.h"
#include <cstring>

/**
* @brief 流写入类
*/
class CStreamWriter {
public:
	/**
	* 默认构造函数
	*/
	CStreamWriter(unsigned char* buffer, size_t size) {
		m_pBuffer = buffer;
		m_pCurrent = buffer;
		m_iBufferSize = size;
		m_iCurrentPos = 0;
	}

	/**
	* 设置流指针位置
	*/
	CStreamWriter& SetPosition(size_t pos) {
		m_pCurrent = m_pBuffer + pos;
		m_iCurrentPos = pos;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamWriter SetPosition() out of range");
		}
		return *this;
	}

	/**
	* 将流指针跳过指定字节数
	*/
	CStreamWriter& Skip(size_t nByte) {
		m_pCurrent += nByte;
		m_iCurrentPos += nByte;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamWriter Skip() out of range");
		}
		return *this;
	}

	/**
	* 获取当前位置
	*/
	size_t GetPosition() {
		return m_iCurrentPos;
	}

	/**
	* 剩余字节数
	*/
	size_t Available() {
		return m_iBufferSize - m_iCurrentPos;
	}

	/**
	* 获取当前指针
	*/
	unsigned char* GetPointer() {
		return m_pCurrent;
	}

	/**
	* 设置值
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
	* 写入数据
	*/
	size_t Write(unsigned char* data, size_t size) {
		size_t writeSize = m_iCurrentPos + size > m_iBufferSize ? m_iBufferSize - m_iCurrentPos : size;
		memcpy(m_pCurrent, data, writeSize);
		m_pCurrent += writeSize;
		m_iCurrentPos += writeSize;
		return writeSize;
	}

	/**
	* 流操作符
	*/
	template <typename T>
	CStreamWriter& operator << (T& value) {
		SetValue<T>(value);
		return *this;
	}

private:
	unsigned char* m_pBuffer;
	unsigned char* m_pCurrent;
	size_t m_iBufferSize;
	size_t m_iCurrentPos;
};

#endif