#include <stdio.h>
#include <conio.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

class FilePath{
private:
    std::string path_;
    typedef std::vector<std::string> StringVec;
    // first - disk, last - filename if is present
    StringVec names_;
    static const char kNameDelimChar;

public:
    enum NameType{kDir, kFile, kRoot };
    // Возвращает true, если в заданнои имени файла будут найдены недопустимые символы.
    bool InvalidCharsInName(const std::string& name, NameType type) const {
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

    bool Root (const std::string& name) const {
        if (name.at(name.size() - 1) == ':' ) return true;
        return false;
    }

    // Возвращает true, если путь пустой
    bool Empty() const {
        return path_.empty();
    }

    // Нормализация разделителей имен директорий - замена разных слешей на единообразный '\'
    void NormalizeNameDelimChar() {
        std::for_each(path_.begin(), path_.end(), [](char& c)-> void{
                      if (c == '/') c = kNameDelimChar;});
    }

    /* Удаление дупликатов разделителя имен директорий.
    Предусловия:
    1) Нормализованные разделители имен директорий в пути path_;
    */
    void RemoveNameDelimCharDups() {
        if (path_.empty())
            return;
        path_.erase(std::unique(path_.begin(), path_.end(), [](char l, char r){
            return l == kNameDelimChar && r == kNameDelimChar;}), path_.end());
    }

    /*
    Предусловия:
    1) Нормализованные разделители имен директорий в пути path_;
    2) Не должно быть два или более подряд идущих символа '\\'.
    */
    void Split() {
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
    void DebugPrint_SplitTest(){
        for (size_t i = 0; i < names_.size(); i++){
            std::cout << "[" << i << "]=" << names_[i] << std::endl;
        }
    }

    void Init() {
        NormalizeNameDelimChar();
        RemoveNameDelimCharDups();
        Split();
    }
    /* Проверка валидности задания пути.
    Путь считается невалидным при присутствии недопустимых символов в его составляющих.
    Пустой путь считается валидным.
    Предусловия:
    1) путь должен быть разбит на составляющие
    */
    bool Valid() const {
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

    /*
    Предусловия:
    1) Валидный путь
    */
    bool Absolute() const {
        if (!names_.empty() && Root(names_[0]))
            return true;
        return false;
    }
public:
    FilePath() { Init(); }
    FilePath(const char* path) : path_(path) { Init(); }
    void PrintPath(){
        std::cout << path_ << std::endl;
    }


};
const char FilePath::kNameDelimChar = '\\';

int main()
{
    const char* validStrs[] = {
    "",                                 // 0
    "C:\\user/smr\\documents",          // 1
    "\\user/smr\\documents",            // 2
    "C:\\user/smr\\documents\\",        // 3
    "user",                             // 4
    "..\\..\\user/smr\\documents",      // 5
    "..\\..\\"                          // 6
    };
    const char* invalidStrs[] = {
    "tr!nd",                                 // 0
    ":\\user/smr\\documents",          // 1
    "\\user/!smr\\:documents",            // 2
    "C:\\user/smr\\c:\\documents\\",        // 3
    "user/smr\\c:\\documents\\",        // 4
    "\\"
    };
    for (size_t i = 0; i < sizeof(validStrs) / sizeof(const char*); i++){
        FilePath path(validStrs[i]);
        path.PrintPath();
        //path.DebugPrint_SplitTest();
        std::cout << "valid: " << path.Valid() << std::endl;
        std::cout << "absolute: " << path.Absolute() << std::endl;
    }

    for (size_t i = 0; i < sizeof(invalidStrs) / sizeof(const char*); i++){
        FilePath path(invalidStrs[i]);
        path.PrintPath();
        //path.DebugPrint_SplitTest();
        std::cout << "valid: " << path.Valid() << std::endl;
        std::cout << "absolute: " << path.Absolute() << std::endl;
    }


    printf ("Press any key to quit...");
    getch();
}

