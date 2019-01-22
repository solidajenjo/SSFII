#ifndef __FILESYSTEM_H_
#define __FILESYSTEM_H_

#include <string>
#include <vector>

class FileSystem 
{
public:

	bool Init();
	bool CleanUp();

	bool Write(const std::string &path, const void* data, unsigned size, bool userData = false) const; //writes data to path
	bool Read(const std::string &path, void* data, unsigned size) const; //reads from path and allocates in data. NOTE: The caller should be responsible to clean it

	bool Exists(const std::string &path) const;

	unsigned Size(const std::string &path) const;

	bool CreateDir(const std::string &path) const;

	bool IsDir(const std::string &path) const;

	bool Delete(const std::string &path) const;

	void GetContentList(const std::string & path, std::vector<std::string>& files, std::vector<std::string>& dirs) const;

	//members 

	std::string prefPath;

	char userDataMountPoint[20] = "User Data";
};


#endif