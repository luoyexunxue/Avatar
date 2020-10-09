//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSTREAMREADER_H_
#define _CSTREAMREADER_H_
#include "CLog.h"
#include <cstring>

/**
* @brief 流读取类
*/
class CStreamReader {
public:
	/**
	* 默认构造函数
	*/
	CStreamReader(unsigned char* buffer, size_t size) {
		m_pBuffer = buffer;
		m_pCurrent = buffer;
		m_iBufferSize = size;
		m_iCurrentPos = 0;
	}

	/**
	* 设置流指针位置
	*/
	CStreamReader& SetPosition(size_t pos) {
		m_pCurrent = m_pBuffer + pos;
		m_iCurrentPos = pos;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamReader SetPosition() out of range");
		}
		return *this;
	}

	/**
	* 将流指针跳过指定字节数
	*/
	CStreamReader& Skip(size_t nByte) {
		m_pCurrent += nByte;
		m_iCurrentPos += nByte;
		if (m_iCurrentPos > m_iBufferSize) {
			CLog::Error("CStreamReader Skip() out of range");
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
	* 获取值
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
	* 读取数据
	*/
	size_t Read(unsigned char* data, size_t size) {
		size_t readSize = m_iCurrentPos + size > m_iBufferSize ? m_iBufferSize - m_iCurrentPos : size;
		memcpy(data, m_pCurrent, readSize);
		m_pCurrent += readSize;
		m_iCurrentPos += readSize;
		return readSize;
	}

	/**
	* 流操作符
	*/
	template <typename T>
	CStreamReader& operator >> (T& value) {
		value = GetValue<T>();
		return *this;
	}

private:
	unsigned char* m_pBuffer;
	unsigned char* m_pCurrent;
	size_t m_iBufferSize;
	size_t m_iCurrentPos;
};

#endif