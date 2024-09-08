#include "Tokenizer.hpp"
#include <iostream>
#include <algorithm>

std::string toLower(const std::string &str)
{
    std::string lowerStr;
    lowerStr.resize(str.size());
    std::transform(str.begin(), str.end(), lowerStr.begin(), [](unsigned char c)
                   { return std::tolower(c); });
    return lowerStr;
}
bool isNumber(const std::string &str)
{
    // Check for empty string
    if (str.empty())
        return false;

    // Iterator to keep track of the current character
    auto it = str.begin();

    // Check for a leading sign
    if (*it == '-' || *it == '+')
        ++it;

    // Check if there are any digits
    bool hasDigits = std::any_of(it, str.end(), ::isdigit);

    // Check for digits and only one decimal point
    bool hasDecimalPoint = false;
    for (; it != str.end(); ++it)
    {
        if (*it == '.')
        {
            // Ensure only one decimal point
            if (hasDecimalPoint)
                return false;
            hasDecimalPoint = true;
        }
        else if (!std::isdigit(static_cast<unsigned char>(*it)))
        {
            return false;
        }
    }

    // Return true if the string contains digits and follows number formatting rules
    return hasDigits;
}
bool isBooleanTrue(const std::string &str)
{
    return toLower(str) == "true";
}
bool isBooleanFalse(const std::string &str)
{
    return toLower(str) == "false";
}
bool isNull(const std::string &str)
{
    return toLower(str) == "null";
}

bool isSymbol(char current_char)
{
    return current_char == '%' || current_char == '$' ||
           current_char == '#' || current_char == '+' ||
           current_char == '!' || current_char == '&' ||
           current_char == '-' || current_char == '_' || 
           current_char == ',' || current_char == '.' || 
           current_char == '\''|| current_char == ';' ||
           current_char == ':' || current_char == '\n';
}

std::vector<TokenJson> Tokenizer::TokenizeJson(std::string &jsonString)
{
    unsigned int current = 0;
    std::vector<TokenJson> tokens;
    char current_char;

    while (current < jsonString.size())
    {
        current_char = jsonString[current];

        if (current_char == '{')
        {
            tokens.push_back(TokenJson(TOKEN_TYPE::BRACE_OPEN, current_char));
            current++;
            continue;
        }

        if (current_char == '}')
        {
            tokens.push_back(TokenJson(TOKEN_TYPE::BRACE_CLOSE, current_char));
            current++;
            continue;
        }

        if (current_char == '[')
        {
            tokens.push_back(TokenJson(TOKEN_TYPE::BRACKET_OPEN, current_char));
            current++;
            continue;
        }
        if (current_char == ']')
        {
            tokens.push_back(TokenJson(TOKEN_TYPE::BRACKET_CLOSE, current_char));
            current++;
            continue;
        }

        if (current_char == ':')
        {
            tokens.push_back(TokenJson(TOKEN_TYPE::COLON, current_char));
            current++;
            continue;
        }

        if (current_char == ',')
        {
            tokens.push_back(TokenJson(TOKEN_TYPE::COMMA, current_char));
            current++;
            continue;
        }

        if (current_char == '"')
        {
            std::string value = "";
            current_char = jsonString[++current];
            while (current_char != '"')
            {
                value += current_char;
                current_char = jsonString[++current];
            }
            current++;
            tokens.push_back(TokenJson(TOKEN_TYPE::STRING, value));
            continue;
        }

        if (std::isalnum(current_char))
        {
            std::string value = "";
            while (std::isalnum(current_char) || current_char == '.')
            {
                value += current_char;
                current_char = jsonString[++current];
            }
            if (isNumber(value))
                tokens.push_back(TokenJson(TOKEN_TYPE::NUMBER, value));
            else if (isBooleanTrue(value))
                tokens.push_back(TokenJson(TOKEN_TYPE::TRUE, value));
            else if (isBooleanFalse(value))
                tokens.push_back(TokenJson(TOKEN_TYPE::FALSE, value));
            else if (isNull(value))
                tokens.push_back(TokenJson(TOKEN_TYPE::NONE, value));
            else
                throw std::runtime_error("Unexpected value: " + value);
            continue;
        }
        current++;
    }

    return tokens;
}
std::vector<TokenXml> Tokenizer::TokenizeXml(std::string &XmlString)
{
    unsigned int current = 0;
    std::vector<TokenXml> tokens;
    char current_char;
    bool tagname_complete = true;
    std::string attrib_info ="";

    while (current < XmlString.size())
    {
        current_char = XmlString[current];

        if (current_char == '<')
        {
            std::string tagname = "";
            std::string attrib_info = "";
            if (XmlString[current + 1] == '/')
            {
                current++;
                current_char = XmlString[++current];
                while (current_char != '>')
                {
                    tagname += current_char;
                    current_char = XmlString[++current];
                }

                tokens.push_back(TokenXml(TOKEN_TYPE::TAG_CLOSE, tagname));
                current++;
                continue;
            }
        }
        if (current_char == '<')
        {
            std::string tagname = "";
            current_char = XmlString[++current];
            while (current_char != '>')
            {
                if (current_char == ' ')
                    tagname_complete = false;

                if(tagname_complete)
                    tagname += current_char;
                else
                    attrib_info += current_char;
                current_char = XmlString[++current];
            }
            tagname_complete = true;
            current++;
            TokenXml token = TokenXml(TOKEN_TYPE::TAG_OPEN, tagname);
            std::string key, value;
            bool isKey = true, inQuotes = false;

            for (char ch : attrib_info)
            {
                if (ch == '=')
                {
                    isKey = false;
                }
                else if (ch == '\"')
                {
                    inQuotes = !inQuotes;
                    if (!inQuotes)
                    {
                        token.addAttribute(key, value);
                        key.clear();
                        value.clear();
                        isKey = true;
                    }
                }
                else if (ch == ' ' && !inQuotes)
                {
                    // Skip spaces outside of quotes
                }
                else
                {
                    if (isKey)
                    {
                        key += ch;
                    }
                    else
                    {
                        value += ch;
                    }
                }
            }
            tokens.push_back(token);

            continue;
        }

        if (current_char == '"')
        {
            std::string value = "";
            current_char = XmlString[++current];
            while (current_char != '"')
            {
                value += current_char;
                current_char = XmlString[++current];
            }
            current++;
            tokens.push_back(TokenXml(TOKEN_TYPE::STRING, value));
            continue;
        }

        if (std::isalnum(current_char) || isSymbol(current_char))
        {
            std::string value = "";
            while (std::isalnum(current_char) || isSymbol(current_char) || std::isspace(current_char))
            {
                value += current_char;
                current_char = XmlString[++current];
            }
            if (isNumber(value))
                tokens.push_back(TokenXml(TOKEN_TYPE::NUMBER, value));
            else if (isBooleanTrue(value))
                tokens.push_back(TokenXml(TOKEN_TYPE::TRUE, value));
            else if (isBooleanFalse(value))
                tokens.push_back(TokenXml(TOKEN_TYPE::FALSE, value));
            else if (isNull(value))
                tokens.push_back(TokenXml(TOKEN_TYPE::NONE, value));
            else
                tokens.push_back(TokenXml(TOKEN_TYPE::STRING, value));
            continue;
        }
        current++;
    }
    return tokens;
};
