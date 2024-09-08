#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <map>

namespace Json
{
    enum class OBJECT_TYPE
    {
        NUMERIC,
        STRING,
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
    class JsonNumber : public Object
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
        inline std::string toXmlString() override { return shortenDouble(this->value); }
        inline std::string toJsonString() override { return shortenDouble(this->value); }
        inline JsonNumber(double value)
        {
            this->value = value;
            this->type = OBJECT_TYPE::NUMERIC;
        }
    };
    class JsonString : public Object
    {
    public:
        std::string value;

    public:
        inline std::string toXmlString() override { return this->value; }
        inline std::string toJsonString() override { return "\"" + this->value + "\""; }
        inline JsonString(std::string value)
        {
            this->value = value;
            this->type = OBJECT_TYPE::STRING;
        }
    };
    class JsonBoolean : public Object
    {
    public:
        bool value;

    public:
        inline std::string toXmlString() override { return this->value == true ? "true" : "false"; }
        inline std::string toJsonString() override { return this->value == true ? "true" : "false"; }
        inline JsonBoolean(bool value)
        {
            this->value = value;
            this->type = OBJECT_TYPE::BOOLEAN;
        }
    };
    class JsonNull : public Object
    {
    public:
        inline std::string toXmlString() override { return "null"; }
        inline std::string toJsonString() override { return "null"; }
        inline JsonNull() { this->type = OBJECT_TYPE::NONE; }
    };
    class JsonArray : public Object
    {
    public:
        std::vector<Json::Object *> values;

    private:
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

    public:
        inline Object *operator[](int index) { return values[index]; }
        inline JsonArray() { this->type = OBJECT_TYPE::ARRAY; }
        inline void AddElement(Object *obj) { values.push_back(obj); }
    };
    class JsonMap : public Object
    {
    public:
        std::map<std::string, Object *> map;

    private:
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

    public:
        inline Object *operator[](std::string key) { return map[key]; }
        inline JsonMap() { this->type = OBJECT_TYPE::MAP; }
        inline void AddElement(Object *key, Object *value)
        {
            JsonString *str_key = dynamic_cast<JsonString *>(key);
            map[str_key->value] = value;
        }
    };

} // namespace Json
