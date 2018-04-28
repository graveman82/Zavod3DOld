#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "z3DLib/z3DFilePath.h"

namespace z3D{

bool FilePath::InvalidCharsInName(const std::string& name, NameType type) const {
    if (name.empty()) return false;
    if (type == kRoot) {
        if (std::isalpha(name.at(0) == 0)) return true;
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
    if (path_.empty())
        return;
    path_.erase(std::unique(path_.begin(), path_.end(), [](char l, char r){
        return l == kNameDelimChar && r == kNameDelimChar;}), path_.end());
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

const char FilePath::kNameDelimChar = '\\';


} // end of z3D
