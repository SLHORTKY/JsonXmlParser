#include <iostream>
#include "Parser.hpp"
#include "Tokenizer.hpp"
#include "Json.hpp"

Json::Object *Parser::ParseJsonValue()
{
    TokenJson curToken = currentTokenJson();

    switch (curToken.type)
    {
    case TOKEN_TYPE::STRING:
        return new Json::JsonString(curToken.value);
        break;
    case TOKEN_TYPE::NUMBER:
        return new Json::JsonNumber(stod(curToken.value));
        break;
    case TOKEN_TYPE::TRUE:
        return new Json::JsonBoolean(true);
        break;
    case TOKEN_TYPE::FALSE:
        return new Json::JsonBoolean(false);
        break;
    case TOKEN_TYPE::NONE:
        return new Json::JsonNull();
        break;
    case TOKEN_TYPE::BRACE_OPEN:
        return ParseJsonObject();
        break;
    case TOKEN_TYPE::BRACKET_OPEN:
        return ParseJsonArray();
        break;
    }
    throw std::runtime_error("unexpected token");
}
Json::Object *Parser::ParseJsonObject()
{
    TokenJson token = nextTokenJson();
    Json::JsonMap *jsonMap = new Json::JsonMap();

    while (token.type != TOKEN_TYPE::BRACE_CLOSE)
    {
        Json::Object *key = ParseJsonValue();
        token = nextTokenJson();
        if (token.type != TOKEN_TYPE::COLON)
            throw std::runtime_error("Expected : in key-value pair");
        token = nextTokenJson();
        Json::Object *value = ParseJsonValue();
        jsonMap->AddElement(key, value);

        token = nextTokenJson();
        if (token.type == TOKEN_TYPE::COMMA)
            token = nextTokenJson();
    }
    return jsonMap;
}
Json::Object *Parser::ParseJsonArray()
{
    TokenJson token = nextTokenJson();
    Json::JsonArray *jsonArray = new Json::JsonArray();

    while (token.type != TOKEN_TYPE::BRACKET_CLOSE)
    {
        Json::Object *value = ParseJsonValue();
        jsonArray->AddElement(value);

        token = nextTokenJson();
        if (token.type == TOKEN_TYPE::COMMA)
            token = nextTokenJson();
    }
    return jsonArray;
}

Xml::Object *Parser::ParseXmlValue()
{
    TokenXml curToken = currentTokenXml();

    switch (curToken.type)
    {
    case TOKEN_TYPE::STRING:
        return new Xml::XmlString(curToken.value);
        break;
    case TOKEN_TYPE::NUMBER:
        return new Xml::XmlNumber(stod(curToken.value));
        break;
    case TOKEN_TYPE::TRUE:
        return new Xml::XmlBoolean(true);
        break;
    case TOKEN_TYPE::FALSE:
        return new Xml::XmlBoolean(false);
        break;
    case TOKEN_TYPE::NONE:
        return new Xml::XmlNull();
        break;
    case TOKEN_TYPE::TAG_OPEN:
        
        if(current == 0){
            return ParseXmlObject();
        }

        TokenXml rootToken = XmlTokens[current - 1];
        rootToken.type = TOKEN_TYPE::TAG_CLOSE;

        TokenXml token = XmlTokens[current];
        unsigned short index = 1;
        while (!XmlTokens[index + current].isEqual(rootToken))
        {
            if (token.isEqual(XmlTokens[index + current]))
            {
                return ParseXmlArray();
            }
            index++;
        }
        return ParseXmlObject();
        break;
    }
    throw std::runtime_error("unexpected token");
}
Xml::Object *Parser::ParseXmlObject()
{
    TokenXml token = currentTokenXml();
    Xml::XmlMap *map = new Xml::XmlMap();
    Xml::XmlString *key;
    Xml::Object *value;

    if (XmlTokens[current + 1].type == TOKEN_TYPE::TAG_OPEN)
    {
        key = new Xml::XmlString(token.value);
        token = nextTokenXml();
        value = ParseXmlValue();
        map->AddElement(key,value);
        delete key;
    }
    else if (XmlTokens[current + 1].type != TOKEN_TYPE::TAG_OPEN &&
             XmlTokens[current + 1].type != TOKEN_TYPE::TAG_CLOSE)
    {
        TokenXml rootToken = XmlTokens[current - 1];
        rootToken.type = TOKEN_TYPE::TAG_CLOSE;
        while (!XmlTokens[current].isEqual(rootToken))
        {
            key = new Xml::XmlString(token.value);
            token = nextTokenXml();
            value = ParseXmlValue();
            token = nextTokenXml();
            token = nextTokenXml();
            map->AddElement(key,value);
        }
        token = nextTokenXml();
    }
    return map;
}
Xml::Object *Parser::ParseXmlArray()
{
    Xml::XmlArray *arr = new Xml::XmlArray();
    Xml::XmlMap * map = new Xml::XmlMap();

    TokenXml rootToken = XmlTokens[current-1];
    rootToken.type = TOKEN_TYPE::TAG_CLOSE;

    Xml::XmlString* key = new Xml::XmlString(currentTokenXml().value);

    while (!rootToken.isEqual(XmlTokens[current]))
    {
        current++;
        Xml::Object *value = ParseXmlValue();
        arr->AddElement(value);
    }
    map->AddElement(key, arr);
    return map;
}

Xml::Object *Parser::ParseXml(std::string &XmlString)
{
    this->current = 0;
    Tokenizer tk;
    this->XmlTokens = tk.TokenizeXml(XmlString);

    return ParseXmlValue();
}
std::string Parser::UnParseXml(Xml::Object &object)
{
    return object.toXmlString();
}
Json::Object *Parser::ParseJson(std::string &jsonString)
{
    this->current = 0;
    Tokenizer tk;
    this->JsonTokens = tk.TokenizeJson(jsonString);
    return ParseJsonValue();
}
std::string Parser::UnParseJson(Json::Object &object)
{
    return object.toJsonString();
}

std::string Parser::JsonToXml(std::string &jsonString)
{
    Json::Object *obj = this->ParseJson(jsonString);
    return obj->toXmlString();
}
std::string Parser::XmlToJson(std::string &XmlString)
{
    Xml::Object *obj = this->ParseXml(XmlString);
    return obj->toJsonString();
}
