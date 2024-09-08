#pragma once
#include <string>
#include <vector>
#include "Json.hpp"

enum class TOKEN_TYPE
{
    BRACE_OPEN,
    BRACE_CLOSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    STRING,
    NUMBER,
    COMMA,
    COLON,
    TRUE,
    FALSE,
    NONE, // same as null
    TAG_OPEN,
    TAG_CLOSE,
};

typedef struct TokenJson
{
    TOKEN_TYPE type;
    std::string value;

    TokenJson(TOKEN_TYPE type, std::string value)
    {
        this->type = type;
        this->value = value;
    }
    TokenJson(TOKEN_TYPE type, char value)
    {
        this->type = type;
        this->value = value;
    }
} TokenJson;

typedef struct TokenXml
{
    TOKEN_TYPE type;
    std::string value;
    std::map<std::string , std::string> attributes;

    inline void addAttribute(std::string first, std::string second){
        attributes[first] = second;
    }

    TokenXml(TOKEN_TYPE type, std::string value)
    {
        this->type = type;
        this->value = value;
    }
    TokenXml(TOKEN_TYPE type, char value)
    {
        this->type = type;
        this->value = value;
    }

    bool isEqual(TokenXml token){
        return token.type == this->type && token.value == this->value;
    }
} TokenXml;

class Tokenizer
{
public:
    std::vector<TokenJson> TokenizeJson(std::string &jsonString);
    std::vector<TokenXml> TokenizeXml(std::string &XmlString);
};