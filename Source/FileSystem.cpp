#include "FileSystem.h"
#include "Globals.h"
#include "ExternalLibraries/physfs-3.0.1/src/physfs.h"

bool FileSystem::Init()
{
	PHYSFS_init(".");
	PHYSFS_setWriteDir(".");
	PHYSFS_mount(".", "", 0);
	return true;
}

bool FileSystem::CleanUp()
{
	PHYSFS_deinit();
	return true;
}

bool FileSystem::Write(const std::string & path, const void * data, unsigned size) const
{	
	PHYSFS_File* file = PHYSFS_openWrite(path.c_str());
	if (file == nullptr)
	{
		LOG("Error writing in %s -> %s", path.c_str(), PHYSFS_getLastError());
		return false;
	}
	unsigned k = PHYSFS_writeBytes(file, data, size);
	if (k == size)
	{
		PHYSFS_close(file);
		return true;
	}
	LOG("Error writing in %s -> %s", path.c_str(), PHYSFS_getLastError());
	PHYSFS_close(file);
	return false;
}

bool FileSystem::Read(const std::string& path, void* data, unsigned size) const
{
	PHYSFS_File* file = PHYSFS_openRead(path.c_str());
	PHYSFS_ErrorCode errorCode = PHYSFS_getLastErrorCode();
	if (errorCode == PHYSFS_ERR_BAD_FILENAME) //possibly it's from outside the filesystem -> read as C
	{
		LOG("Reading outside filesystem.");
		FILE* file = nullptr;
		fopen_s(&file, path.c_str(), "rb");
		if (file == nullptr)
		{
			LOG("Impossible to read %s", path.c_str());
			return 0;
		}
		fread_s(data, size, 1, size, file);
		fclose(file);
		return true;
	}
	else
	{
		if (file == nullptr)
		{
			LOG("Error reading %s -> %s", path.c_str(), PHYSFS_getErrorByCode(errorCode));
			return false;
		}

		if (PHYSFS_readBytes(file, data, size) == size)
		{
			PHYSFS_close(file);
			return true;
		}
		PHYSFS_close(file);
		return false;
	}
}

bool FileSystem::Exists(const std::string & path) const
{
	return PHYSFS_exists(path.c_str()) != 0;
}

unsigned FileSystem::Size(const std::string & path) const
{
	PHYSFS_File* file = PHYSFS_openRead(path.c_str());
	PHYSFS_ErrorCode errorCode = PHYSFS_getLastErrorCode();
	if (errorCode == PHYSFS_ERR_BAD_FILENAME) //possibly it's from outside the filesystem -> read as C
	{
		LOG("Reading outside filesystem.");
		FILE* file = nullptr;
		fopen_s(&file, path.c_str(), "rb");
		if (file == nullptr)
		{
			LOG("Impossible to read %s", path.c_str());
			return 0;
		}
		fseek(file, 0L, SEEK_END);
		return ftell(file);
	}
	if (file == nullptr)
	{
		LOG("Error reading %s -> %s", path.c_str(), PHYSFS_getErrorByCode(errorCode));
		return 0;
	}
	return PHYSFS_fileLength(file);
}

bool FileSystem::CreateDir(const std::string & path) const
{
	return false;
}

bool FileSystem::IsDir(const std::string & path) const
{
	PHYSFS_Stat fStat;
	if (PHYSFS_stat(path.c_str(), &fStat) == 0)
		LOG("Error guessing if is a dir -> %s", PHYSFS_getLastError());
	return fStat.filetype == PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY;
}

bool FileSystem::Delete(const std::string & path) const
{
	return false;
}

void FileSystem::GetContentList(const std::string & path, std::vector<std::string>& files, std::vector<std::string>& dirs) const
{
	char** filesList = PHYSFS_enumerateFiles(path.c_str());

	char **i;

	for (i = filesList; *i != nullptr; i++)
	{
		std::string completePath = path + "/" + *i;
		if (IsDir(completePath.c_str()))
		{
			dirs.push_back(*i);
		}
		else
		{
			files.push_back(*i);
		}
	}

	PHYSFS_freeList(filesList);
}
