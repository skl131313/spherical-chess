
#pragma once

#include <fstream>
#include <sstream>
#include <istream>
#include <limits>
#include <vector>

//! Contains simple misc support utility functions and classes. 
namespace Util
{

//! @brief Checks that @p value is within range [@p min, @p max).
//! @returns True if @p value is within range, false otherwise.
//! @remarks The argument @p min must be less than @p max.
//!          Otherwise will always return false, regardless of @p value.
template<typename T>
bool InRange(const T& value, const T& min, const T& max)
{
    return value >= min && value < max;
}

//! @brief Reads an entire file binary into memory.
//! @param [in] name The name of the file to read from.
//! @returns A container with binary data read from file.
//! @throws std::runtime_error When the file fails to open.
inline std::vector<char> ReadBinaryFile(const char* name)
{
    std::ifstream file(name, std::ios::binary);
    std::vector<char> output;

    if(!file) throw std::runtime_error("Failed to open file: " + std::string(name));

    file.seekg(0, std::ios::end);

    output.resize(file.tellg());

    file.seekg(0);
    file.read(output.data(), output.size());

    return output;
}

//! @brief Read an entire text file into memory as a string.
//! @param [in] name The name of the file to read from.
//! @returns A string with that contains text from the file.
//! @throws std::runtime_error When the file fails to open.
inline std::string ReadStringFile(const char* name)
{
    std::ifstream file(name);
    std::stringstream stream;

    if(!file) throw std::runtime_error("Failed to open file: " + std::string(name));

    stream << file.rdbuf();

    return stream.str();
}


}
