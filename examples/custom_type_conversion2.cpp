
/* custom_type_conversion2.cpp
 *
 * Author: mem99
 * Created On: 21 Oct 2022
 */

#include <inicpp.h>
#include <iostream>

// the conversion functor must live in the "ini" namespace
namespace ini
{
    template<typename T, typename T2>
    struct Convert<std::map<T, T2>>
    {
        void decode(const std::string &value, std::map<T, T2> &result, const std::map<T, T2> &default_value)
        {
            result = default_value;

            std::size_t startPos = 0, endPos = 0;
            while (endPos != std::string::npos)
            {
                if (endPos)
                    startPos = endPos + 2;
                endPos = value.find(":{", startPos);

                if (endPos == std::string::npos)
                    break;

                std::string tmp = value.substr(startPos, endPos - startPos);
                T key;
                Convert<T> conv;
                conv.decode(tmp, key, T());

                startPos = endPos + 2;
                endPos = value.find("},", startPos);

                tmp = value.substr(startPos, endPos - startPos);
                T2 output;
                Convert<T2> conv2;
                auto elem = default_value.find(key);
                conv2.decode(tmp, output, elem == default_value.end() ? T2() : elem->second);
                result[key] = output;
            }
        }

        void encode(const std::map<T, T2> &value, std::string &result)
        {
            std::string encoded;
            std::stringstream ss;
            for (const auto &o : value)
            {
                Convert<T> conv;
                conv.encode(o.first, encoded);
                ss << encoded << ':';
                Convert<T2> conv2;
                conv2.encode(o.second, encoded);
                ss << '{' << encoded << '}' << ',';
            }
            result = ss.str();
        }
    };
}

int main()
{
    {
        // create some ini content
        std::string content = "[Foo]\nintMap=ac:{6},ab:{7},ay:{8},";

        // decode the ini contents
        ini::IniFile inputIni;
        inputIni.decode(content);

        // print the results
        std::cout << "Parsed ini file" << std::endl;
        std::cout << "===============" << std::endl;

        // parse the int map
        std::map<std::string, int> intMap = { {"ab",7}, {"ac",6}, {"ay",8} };
        intMap = inputIni["Foo"]["intMap"].as<std::map<std::string, int>>(intMap);
        std::cout << "int map:" << std::endl;
        for(const auto &object : intMap)
            std::cout << "  " << object.first << " " << object.second << std::endl;

        std::cout << std::endl;
    }

    {
        // create some misformatted ini content
        std::string content = "[Foo]\nintMap=ac:{18,,},ab:5472,ay{33}";

        // decode the ini contents
        ini::IniFile inputIni;
        inputIni.decode(content);

        // print the results
        std::cout << "Parsed ini file" << std::endl;
        std::cout << "===============" << std::endl;

        // parse the int map
        std::map<std::string, int> intMap = { {"ab",7}, {"ac",5}, {"ay",9} };
        intMap = inputIni["Foo"]["intMap"].as<std::map<std::string, int>>(intMap);
        std::cout << "int map 2:" << std::endl;
        for(const auto &object : intMap)
            std::cout << "  " << object.first << " " << object.second << std::endl;

        std::cout << std::endl;
    }

    // create another ini file for encoding
    ini::IniFile outputIni;
    outputIni["Bar"]["floatList"] =std::map<const char *, float>{ {"float", 1.0f}, {"another", 9.0f}, {"zero", 0.0f}};
    outputIni["Bar"]["boolList"] =std::map<int, bool>{ {1,true}, {5,false}, {2,false}, {9,true}};

    std::cout << "Encoded ini file" << std::endl;
    std::cout << "================" << std::endl;
    std::cout << outputIni.encode() << std::endl;

    return 0;
}
