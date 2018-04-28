#ifndef Z3DFILEPATH_H
#define Z3DFILEPATH_H

#include <vector>
#include <string>

namespace z3D{

class FilePath{
private:
    typedef std::vector<std::string> StringVec;
    enum NameType{kDir, kFile, kRoot };

private:
    std::string path_;
    // first - disk, last - filename if is present
    StringVec names_;
    static const char kNameDelimChar;

    // Возвращает true, если в заданнои имени файла будут найдены недопустимые символы.
    bool InvalidCharsInName(const std::string& name, NameType type) const;

    bool Root (const std::string& name) const;

    // Нормализация разделителей имен директорий - замена разных слешей на единообразный '\'
    void NormalizeNameDelimChar();

    /* Удаление дупликатов разделителя имен директорий.
    Предусловия:
    1) Нормализованные разделители имен директорий в пути path_;
    */
    void RemoveNameDelimCharDups();

    /*
    Предусловия:
    1) Нормализованные разделители имен директорий в пути path_;
    2) Не должно быть два или более подряд идущих символа '\\'.
    */
    void Split();

    void Init();

public:
    FilePath();

    FilePath(const char* path);

    // Возвращает true, если путь пустой
    bool Empty() const;

    /* Проверка валидности задания пути.
    Путь считается невалидным при присутствии недопустимых символов в его составляющих.
    Пустой путь считается валидным.
    Предусловия:
    1) путь должен быть разбит на составляющие
    */
    bool Valid() const;

    /*
    Предусловия:
    1) Валидный путь
    */
    bool Absolute() const;

    // debug print
    void PrintPath();
    void DebugPrint_SplitTest();

};

} // end of z3D

#endif // Z3DFILEPATH_H

