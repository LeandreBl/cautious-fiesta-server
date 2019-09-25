#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

#include "Protocole.hpp"
#include "Asset.hpp"

namespace cf
{
class Serializer
{
      public:
	Serializer() noexcept;
	~Serializer() noexcept;
	void clear() noexcept;
	bool set(const std::string &str) noexcept;
	bool set(const TcpPacketHeader &header) noexcept;
	bool set(const Asset &asset) noexcept;
	template <typename T> bool set(const std::vector<T> &vec)
	{
		if (!nativeSet(vec.size()))
			return false;
		for (auto &&i : vec)
			if (!set(i))
				return false;
		return true;
	}
	template <typename T> bool set(const T &nativeObject) noexcept
	{
		return nativeSet(nativeObject);
	}
	bool get(const Asset &asset) noexcept;
	bool get(std::string &str) noexcept;
	bool get(void *dest, size_t len) noexcept;
	template <typename T> bool get(T &dest) noexcept
	{
		return get(&dest, sizeof(dest));
	}
	void *getNativeHandle() const noexcept;
	bool forceSize(size_t newSize) noexcept;
	template <typename T> bool forceSetFirst(const T &obj)
	{
		if (resizeForNewElement(sizeof(obj)) == false)
			return false;
		std::memmove(_data, _data + sizeof(obj), _size);
		std::memcpy(_data, &obj, sizeof(obj));
		_size += sizeof(obj);
		return true;
	}
	int reserve(uint64_t size) noexcept;
	size_t getSize() const noexcept;

      private:
	int8_t *_data;
	size_t _size;
	size_t _alloc_size;
	int resizeForNewElement(size_t newElementSize) noexcept;
	void shift(size_t from) noexcept;
	bool nativeSet(const void *data, size_t len) noexcept
	{
		if (resizeForNewElement(len) == -1)
			return false;
		std::memcpy(_data + _size, data, len);
		_size += len;
		return true;
	}
	template <typename T> bool nativeSet(const T &obj) noexcept
	{
		return nativeSet(&obj, sizeof(obj));
	}
};
} // namespace cf
