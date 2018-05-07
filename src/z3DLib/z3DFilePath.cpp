#include <stdio.h>
#include <io.h>
#include <iostream>
#include <algorithm>
#include "z3DLib/z3DFilePath.h"

namespace z3D{

bool FilePath::InvalidCharsInName(const std::string& name, NameType type) const {
    if (name.empty()) return false;
    if (type == kRoot) {
        if (std::isalpha(name.at(0)) == 0) return true;
        if ((name.length() > 1) && (name.at(1) != ':')) return true;
        return false;
    }
    const char* invalidChars = "\\/:*?\"<>|@!+%\r\n";

    if (type == kFile){
        if (name.at(name.size() - 1) == '.' ||
            name.at(name.size() - 1) == ' ') return true;
    }

    return name.find_first_of(invalidChars) != path_.npos;
}

bool FilePath::Root (const std::string& name) const {
    if (name.at(name.size() - 1) == ':' ) return true;
    return false;
}

bool FilePath::Empty() const {
    return path_.empty();
}

void FilePath::NormalizeNameDelimChar() {
    std::for_each(path_.begin(), path_.end(), [](char& c)-> void{
                  if (c == '/') c = kNameDelimChar;});
}

void FilePath::RemoveNameDelimCharDups() {
#ifdef Z3D_CPP11
    if (path_.empty())
        return;
    path_.erase(std::unique(path_.begin(), path_.end(), [](char l, char r){
        return l == kNameDelimChar && r == kNameDelimChar;}), path_.end());
#else
    std::string::size_type pos = path_.find_first_of(kDirSplitChar, 0);
    while (pos != path_.npos) {
        ++pos;
        while (pos < path_.size()) {
            if (path_.at(pos) == kDirSplitChar) {
                path_.erase(pos, 1);
            }
            else {
                pos = path_.find_first_of(kDirSplitChar, pos);
                break;
            }
        }
    }
#endif
}

void FilePath::Split() {
    if (path_.empty())
        return;

    names_.clear();
    std::string::size_type pos = path_.find_first_not_of(kNameDelimChar, 0);
    while (pos != path_.npos) {
        std::string::size_type delimPos = path_.find_first_of(kNameDelimChar, pos);
        if (pos != delimPos) {
            names_.push_back(path_.substr(pos, delimPos - pos));
        }
        pos = path_.find_first_not_of(kNameDelimChar, delimPos);
    }

}

void FilePath::DebugPrint_SplitTest(){
    for (size_t i = 0; i < names_.size(); i++){
        std::cout << "[" << i << "]=" << names_[i] << std::endl;
    }
}

void FilePath::Init() {
    NormalizeNameDelimChar();
    RemoveNameDelimCharDups();
    Split();
}

bool FilePath::Valid() const {
    if (path_.empty())
        return true;

    // non-empty path validation
    if (names_.empty())
        return false;

    size_t startOfPathWithoutRoot = 0;
    if (Root(names_[0])){
        if (names_[0].length() != 2) return false;
        if (InvalidCharsInName(names_[0], kRoot)) return false;
        startOfPathWithoutRoot = 1;
    }
    for (size_t i = startOfPathWithoutRoot; i < names_.size(); i++){
        if (InvalidCharsInName(names_[i], kDir))
            return false;
    }
    return true;
}

bool FilePath::Absolute() const {
    if (!names_.empty() && Root(names_[0]))
        return true;
    return false;
}

FilePath::FilePath() { Init(); }

FilePath::FilePath(const char* path) : path_(path) { Init(); }

void FilePath::PrintPath(){
    std::cout << path_ << std::endl;
}

bool FilePath::FindPathEntity(long* size, long* attrib) const {
    _finddata_t fd;
    long fh = _findfirst( path_.c_str(), &fd );
    long flags = 0;
    long filesize = 0;
    if ( fh != -1 ) {
        if (fd.attrib & _A_RDONLY)
            flags |= kReadOnly;

        if (fd.attrib & _A_HIDDEN)
            flags |= kHidden;

        if (fd.attrib & _A_SUBDIR)
            flags |= kDirectory;
        else
            filesize = fd.size;
        _findclose( fh );

        if (attrib) *attrib = flags;
        if (size) *size = filesize;
    }

    return fh != -1;
}

bool FilePath::Exists() const {
    return FindPathEntity();
}

bool FilePath::IsDirectory() const {
    long attrib = 0;
    if (!FindPathEntity(0, &attrib)) return false;
    return attrib & kDirectory;
}

bool FilePath::IsFile() const {
    long attrib = 0;
    if (!FindPathEntity(0, &attrib)) return false;
    return (attrib & kDirectory) == 0;
}

long FilePath::FileLength() const {
    long attrib = 0;
    long size = 0;
    if (!FindPathEntity(&size, &attrib)) return -1;
    if ((attrib & kDirectory)!= 0) return -1;
    return size;
}

const char FilePath::kNameDelimChar = '\\';


} // end of z3D
