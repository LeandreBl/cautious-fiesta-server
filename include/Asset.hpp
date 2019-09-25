#pragma once

#include <string>

namespace cf
{
class Asset
{
      public:
	Asset(uint64_t filesize, const std::string &filename);
	~Asset() noexcept = default;
	uint64_t getFileSize() const noexcept;
	const std::string &getFileName() const noexcept;
	uint32_t getChksum() const noexcept;

      private:
	uint64_t _filesize;
	uint32_t _chksum;
	std::string _filename;
};
} // namespace cf