#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

const std::string path = "E:\\Coding\\C-C++\\CodeBlocks Project\\LocalFileTransfer\\config.ini";

std::vector<std::string> pathList;
std::vector<std::string> destinationList;

DWORD flag;
bool NoOverwrite;
std::string inputFile, destinationFile;

BOOL isDirectoryExists(LPCSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void createParentDirectory(std::string path)
{
    size_t found = path.find_last_of('\\');
    std::string parentDirectory = path.substr(0, found);

    if (!isDirectoryExists(parentDirectory.c_str()))
    {
        createParentDirectory(parentDirectory);
    }

    if (!CreateDirectory(path.c_str(), NULL))
    {
        std::cerr << "Failed to create parent directory. Returned error: " << GetLastError() << std::endl;
    }
    else
    {
        std::clog << path << " not exists, created a new one" << std::endl;
    }
}

bool Initialize()
{
    char buffer[120];
    // NoOverwrite
    if (!GetPrivateProfileString("Init", "NoOverwrite", "true", buffer, sizeof(buffer), path.c_str()))
    {
        std::clog << "NoOverwrite section stopped." << std::endl;
        flag = GetLastError();
        return false;
    }

    if (strcmp(buffer, "TRUE") == 0 || strcmp(buffer, "true") == 0 || strcmp(buffer, "True") == 0)
        NoOverwrite = true;
    else if (strcmp(buffer, "FALSE") == 0 || strcmp(buffer, "false") == 0 || strcmp(buffer, "False") == 0)
        NoOverwrite = false;
    else    // default
    {
        std::clog << "NoOverwrite parameter not defined. Default: TRUE" << std::endl;
        NoOverwrite = true;
    }

    // Input
    if (!GetPrivateProfileString("Init", "Input", "input.txt", buffer, sizeof(buffer), path.c_str()))
    {
        std::clog << "Input section stopped." << std::endl;
        flag = GetLastError();
        return false;
    }

    inputFile = buffer;

    // Input
    if (!GetPrivateProfileString("Init", "Destination", "destination.txt", buffer, sizeof(buffer), path.c_str()))
    {
        std::clog << "Destination section stopped." << std::endl;
        flag = GetLastError();
        return false;
    }

    destinationFile = buffer;

    std::clog << "Successfully initialized!" << std::endl;
    return true;
}

bool readInput()
{
    std::ifstream file(inputFile);

    if (file.fail())
    {
        flag = 1;
        return false;   // fail bit
    }

    if (file.peek() == EOF)
    {
        flag = 2;
        return false;   // EOF
    }

    std::string path;
    size_t i = 0;

    while (!file.eof())
    {
        getline(file, path);
        pathList.push_back(path);
        i++;

        std::clog << "Pushed input #" << i << ": \"" << path << "\" to queue." << std::endl;
    }

    flag = 0;
    return true;
}

BOOL readDestination()
{
    std::ifstream file(destinationFile);

    if (file.fail())
    {
        flag = 1;
        return false;   // fail bit
    }

    if (file.peek() == EOF)
    {
        flag = 2;
        return false;   // EOF
    }

    std::string destination;
    size_t i = 0;

    while (!file.eof())
    {
        getline(file, destination);
        destinationList.push_back(destination);
        i++;

        std::clog << "Pushed destination #" << i << ": \"" << destination << "\" to queue." << std::endl;
    }

    flag = 0;
    return true;
}

bool transfer()
{
    for (size_t i = 0; i < pathList.size(); i++)
    {
        std::string path = pathList.at(i);
        std::string destination = destinationList.at(i);

        // check if directory exists
        size_t found = destination.find_last_of('\\');
        std::string parentDirectory = destination.substr(0, found);

        if (!isDirectoryExists(parentDirectory.c_str()))
        {
            createParentDirectory(parentDirectory);
        }

        // copy process
        if (!CopyFile(path.c_str(), destination.c_str(), NoOverwrite))
        {
            flag = GetLastError();
            return false;
        }

        std::clog << "Copied #" << ++i << ": \"" << path << "\" to \"" << destination << "\"." << std::endl;
    }

    std::clog << "All files are successfully transfered!" << std::endl;

    flag = 0;
    return true;
}

int main()
{
    if (!Initialize())
    {
        std::cerr << "Failed to initialize! Returned error: " << flag << std::endl;
        system("pause");
        return 0;
    }

    if (!(flag = readInput()))
    {
        std::cerr << "Failed to read input stream! Returned error: " << flag << std::endl;
        system("pause");
        return 0;
    }

    if (!(flag = readDestination()))
    {
        std::cerr << "Failed to read destination stream! Returned error: " << flag << std::endl;
        system("pause");
        return 0;
    }

    if (pathList.size() != destinationList.size())
    {
        std::cerr << "The number of lines in each file doesn't match!" << std::endl;
        system("pause");
        return 0;
    }

    if (!transfer())
    {
        std::cerr << "Failed to transfer files! Returned error: " << flag << std::endl;
    }

    system("pause");
    return 0;
}
