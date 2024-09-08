#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <map>

namespace Xml
{
    enum class OBJECT_TYPE
    {
        STRING,
        NUMBER,
        BOOLEAN,
        NONE,
        ARRAY,
        MAP
    };

    class Object    
    {
    protected:
        OBJECT_TYPE type;
    public:
        inline static std::string array_name;
        virtual std::string toXmlString() = 0;
        virtual std::string toJsonString() = 0;
        virtual OBJECT_TYPE getType() { return this->type; }
    };
    class XmlString : public Object
    {
    public:
        std::string value;

    public:
        inline std::string toXmlString() override
        {
            return value;
        }
        inline std::string toJsonString() override
        {
            return "\"" + value + "\"";
        }
        XmlString(std::string value)
        {
            this->value = value;
            this->type = OBJECT_TYPE::STRING;
        }
    };
    class XmlNumber : public Object
    {
    public:
        double value;

    public:
        static std::string shortenDouble(double value, int precision = 6)
        {
            std::ostringstream out;
            out << std::fixed << std::setprecision(precision) << value;
            std::string str = out.str();
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.')
            {
                str.pop_back();
            }

            return str;
        }
        inline std::string toXmlString() override
        {
            return shortenDouble(this->value);
        }
        inline std::string toJsonString() override
        {
            return shortenDouble(this->value);
        }
        XmlNumber(double value)
        {
            this->value = value;
            this->type = OBJECT_TYPE::NUMBER;
        }
    };
    class XmlBoolean : public Object
    {
    public:
        bool value;

    public:
        inline std::string toXmlString() override
        {
            return this->value == true ? "true" : "false";
        }
        inline std::string toJsonString() override
        {
            return this->value == true ? "true" : "false";
        }
        XmlBoolean(bool value)
        {
            this->value = value;
            this->type = OBJECT_TYPE::BOOLEAN;
        }
    };
    class XmlNull : public Object
    {
    public:
        inline std::string toXmlString() override
        {
            return "";
        }
        inline std::string toJsonString() override
        {
            return "null";
        }
        XmlNull()
        {
            this->type = OBJECT_TYPE::NONE;
        }
    };
    class XmlArray : public Object
    {
    private:
        std::vector<Object *> values;

    public:
        inline std::string toXmlString() override
        {
            std::ostringstream oss;
            for (size_t i = 0; i < values.size(); ++i)
            {
                oss << "<" << Object::array_name << ">";
                oss << values[i]->toXmlString();
                if (i < values.size() - 1)
                {
                    oss << "\n";
                }
                oss << "</" << Object::array_name << ">";
                
            }
            return oss.str();
        }
        inline std::string toJsonString() override
        {
            std::ostringstream oss;
            oss << "[";

            for (size_t i = 0; i < values.size(); ++i)
            {
                oss << values[i]->toJsonString();
                if (i < values.size() - 1)
                {
                    oss << ",";
                }
            }
            oss << "]";
            return oss.str();
        }
        void AddElement(Object *obj)
        {
            values.push_back(obj);
        }
        XmlArray()
        {
            this->type = OBJECT_TYPE::ARRAY;
        }
    };
    class XmlMap : public Object
    {
    private:
        std::map<std::string, Object *> map;

    public:
        XmlMap() { this->type = OBJECT_TYPE::MAP; }
        inline std::string toXmlString() override
        {
            std::ostringstream oss;
            for (auto it = map.begin(); it != map.end(); ++it)
            {

                if (it->second->getType() == OBJECT_TYPE::ARRAY)
                {
                    Object::array_name = it->first;
                    oss << it->second->toXmlString();
                }
                else
                {
                    oss << "<" << it->first << ">" << it->second->toXmlString() << "</" << it->first << ">";
                }

                if (it != std::prev(map.end()))
                {
                    oss << "\n";
                }
            }
            return oss.str();
        }
        inline std::string toJsonString() override
        {
            std::ostringstream oss;
            oss << "{";

            for (auto it = map.begin(); it != map.end(); ++it)
            {
                oss << "\"" << it->first << "\":"
                    << it->second->toJsonString();
                if (it != std::prev(map.end()))
                {
                    oss << ",";
                }
            }

            oss << "}";
            return oss.str();
        }
        inline void AddElement(Object *key, Object *value)
        {
            XmlString *str_key = dynamic_cast<XmlString *>(key);
            map[str_key->value] = value;
        }
    };

} // namespace Xml
