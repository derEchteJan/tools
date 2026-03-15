#pragma once

#include "markdownfile.h"
#include "markdowninlineelement.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class MarkdownElement
{
protected:
    typedef struct LineData 
    {
        std::string m_lineText;
        std::vector<MarkdownInlineElement*> m_inlineElements;
        
        LineData(const std::string &lineText)
        {
            m_lineText = lineText;
        }
    } LineData;

    const char *m_typeName;     // must be set by baseclass, for debug/logging purposes
    bool m_isBlock;             // can be set to true in derived classes
    MarkdownFile *m_parentFile; // ref to parent file

    std::vector<LineData> m_lineData;

public:
    MarkdownElement(MarkdownFile *parentFile, const char *typenName);

    virtual ~MarkdownElement();

    bool isBlock();
    const char* typeName();
    MarkdownFile* parentFile();

    virtual void serialize() = 0;
    virtual void serializeLine(const LineData &line);
    virtual MarkdownElement* clone() = 0;

    // must be implemented by subclass
    // returns true if given line matches the start of this element
    virtual bool startsWith(const std::string &line) = 0;
    // can be implemented by subclass
    // returns true if the line was accepted and parsing this element should continue
    // false indicates this element is done
    // baseclass default parses until empty line
    virtual bool endsWith(const std::string &line);

    // can be implemented by subclass
    // parses given line with, raw=true means also parse inline elements automatically within
    // subclasses overriding ususally want to call the baseclass method
    virtual void parseLine(const std::string &line, bool isFirstLine = false, bool raw = false);
};

// DERIVED CLASSES

/// TODO: lists and blockquote, tables, template links/includes, emojis (inline)

class Paragraph : public MarkdownElement
{
public:
    Paragraph(MarkdownFile *parentFile)
        : MarkdownElement(parentFile, "Paragraph")
    {
        m_isBlock = true;
    };

    virtual void serialize() override
    {
        if(!m_parentFile) return;
        m_parentFile->serialize_write("<p>");
        for(int li = 0; li < m_lineData.size(); li++)
        {
            auto line = m_lineData[li];
            // todo: call base class to write lines instead
            // baseclass then handles inline elements automatically
            //m_parentFile->serialize_write(line.m_lineText);
            MarkdownElement::serializeLine(line);
            if(li != m_lineData.size() - 1)
                m_parentFile->serialize_writeln("<br>");
        }
        m_parentFile->serialize_writeln("</p>");
    }

    virtual MarkdownElement* clone() override
    {
        return new Paragraph(m_parentFile);
    }

    virtual bool startsWith(const std::string &line) override
    {
        // paragraph special case, is the default
        return true;
    }

    virtual void parseLine(const std::string &line, bool isFirstLine, bool /*raw*/) override
    {
        MarkdownElement::parseLine(line, isFirstLine, /*raw:*/ false);
    }
};

class Headline : public MarkdownElement
{
    int m_size;
    std::string m_delim;
    std::string m_fragmentUrl;

public:
    Headline(MarkdownFile *parentFile, int size)
        : MarkdownElement(parentFile, "Headline")
        , m_size(size)
        , m_delim()
    {
        if(m_size < 1) m_size = 1;
        if(m_size > 4) m_size = 4;

        for(int n = 0; n < m_size; n++)
            m_delim.append("#");
    }

    virtual void serialize() override
    {
        if(!m_parentFile) return;
        if(m_lineData.empty()) return;

        m_parentFile->serialize_write("<h" + std::to_string(m_size) + " id=\"" + m_fragmentUrl + "\">");
        m_parentFile->serialize_write("<a class=\"hlink\" href=\"#" + m_fragmentUrl + "\"># </a>");
        m_parentFile->serialize_write(m_lineData[0].m_lineText);
        m_parentFile->serialize_writeln("</h" + std::to_string(m_size) + ">");
    }

    virtual MarkdownElement* clone() override
    {
        return new Headline(m_parentFile, m_size);
    }

    virtual bool startsWith(const std::string &line) override
    {
        // headlines start with #
        return line.find(m_delim) == 0;
    }

    virtual void parseLine(const std::string &line, bool isFirstLine, bool /*raw*/) override
    {
        const char *excluedChars = " ";
        auto startPos = line.find_first_not_of(excluedChars, m_size);
        std::string textContent;
        if(startPos < line.length())
            textContent = line.substr(startPos);
        else
            textContent = line.substr(m_size);
        
        m_fragmentUrl = "hl-" + textContent;
        std::replace(m_fragmentUrl.begin(), m_fragmentUrl.end(), ' ', '-');
        
        MarkdownElement::parseLine(textContent, isFirstLine, /*raw:*/ true);
    }
};

class CodeBlock : public MarkdownElement
{
public:
    CodeBlock(MarkdownFile *parentFile)
        : MarkdownElement(parentFile, "CodeBlock")
    {
        m_isBlock = true;
    }

    virtual void serialize() override
    {
        if(!m_parentFile) return;
        m_parentFile->serialize_writeln("<div class=\"code\">");
        m_parentFile->serialize_writeln("<pre>", 1);
        for(auto line : m_lineData)
        {
            m_parentFile->serialize_writeln(line.m_lineText, -1); // code blocks dont use serializeLine(), no inline elements are required
        }
        m_parentFile->serialize_writeln("</pre>", 1);
        m_parentFile->serialize_writeln("</div>");
    }

    virtual MarkdownElement* clone() override
    {
        return new CodeBlock(m_parentFile);
    }

    virtual bool startsWith(const std::string &line) override
    {
        // code blocks start with ```
        return line.find("```") == 0;
    }

    virtual bool endsWith(const std::string &line) override
    {
        // code blocks end with ``` as well
        return startsWith(line);
    }

    virtual void parseLine(const std::string &line, bool isFirstLine, bool /*raw*/) override
    {
        if(!isFirstLine)
            MarkdownElement::parseLine(line, isFirstLine, /*raw:*/ true);
    }
};

class Blockquote : public MarkdownElement
{
public:
    Blockquote(MarkdownFile *parentFile)
        : MarkdownElement(parentFile, "Blockquote")
    {
        m_isBlock = true;
    }

    virtual void serialize() override
    {
        if(!m_parentFile) return;
        m_parentFile->serialize_writeln("<p class=\"quote\">");
        for(auto line : m_lineData)
        {
            m_parentFile->serialize_write(line.m_lineText); // blockquotes dont use super::serializeLine(), no inline elements will be parsed
            m_parentFile->serialize_writeln("<br>");
        }
        m_parentFile->serialize_writeln("</p>");
    }

    virtual MarkdownElement* clone() override
    {
        return new Blockquote(m_parentFile);
    }

    virtual bool startsWith(const std::string &line) override
    {
        return !line.empty() && line.find(">") == 0;
    }

    virtual bool endsWith(const std::string &line) override
    {
        return !startsWith(line);
    }

    virtual void parseLine(const std::string &line, bool isFirstLine, bool raw) override
    {
        std::string truncatedLine;
        if(line.find("> ") == 0)
        {
            truncatedLine = line.substr(2);
        }
        else if(line.find(">") == 0)
        {
            truncatedLine = line.substr(1);
        }
        else
        {
            truncatedLine = line;
        }
        MarkdownElement::parseLine(truncatedLine, isFirstLine, true);
    }
};

class Table : public MarkdownElement
{
    std::vector<std::vector<size_t>> m_indices;
    int m_columns;
    bool m_hasHeaders;

public:
    Table(MarkdownFile *parentFile)
        : MarkdownElement(parentFile, "Table")
        , m_columns(1)
        , m_hasHeaders(false)
    {
        m_isBlock = true;

        m_hasHeaders = true; // TEST
    }

    virtual void serialize() override
    {
        if(!m_parentFile) return;
        m_parentFile->serialize_writeln("<table class=\"maintable\">");
        bool isFirstRow = true;
        for(auto &rowIndices : m_indices)
        {
            bool isHeader = isFirstRow && m_hasHeaders;
            m_parentFile->serialize_writeln("<tr>", 1);
            for(int col = 0; col < m_columns; col++)
            {
                if(col < rowIndices.size())
                {
                    m_parentFile->serialize_write(isHeader ? "<th>" : "<td>", 2);
                    auto lineDataIdx = rowIndices[col];
                    MarkdownElement::serializeLine(m_lineData[lineDataIdx]);
                    m_parentFile->serialize_writeln(isHeader ? "</th>" : "</td>", 2);
                }
                else
                {
                    m_parentFile->serialize_writeln(isHeader ? "<th></th>" : "<td></td>", 2);
                }
            }
            m_parentFile->serialize_writeln("</tr>", 1);
            isFirstRow = false;
        }
        m_parentFile->serialize_writeln("</table>");
    }

    virtual MarkdownElement* clone() override
    {
        return new Table(m_parentFile);
    }

    virtual bool startsWith(const std::string &line) override
    {
        return !line.empty() && line.find("|") == 0;
    }

    virtual bool endsWith(const std::string &line) override
    {
        return !startsWith(line);
    }

    virtual void parseLine(const std::string &line, bool isFirstLine, bool raw) override
    {
        m_indices.resize(m_indices.size() + 1);
        auto &rowIndices = m_indices[m_indices.size() - 1];

        int column = 0;

        int cellBegin = 1;
        for(int cellEnd = cellBegin; cellEnd < line.length(); cellEnd++)
        {
            if(line.at(cellEnd) == '|')
            {
                auto cellLen = cellEnd - cellBegin;
                std::string cellContent;
                if(cellLen > 0)
                {
                    cellContent = line.substr(cellBegin, cellLen);
                }
                MarkdownElement::parseLine(cellContent, isFirstLine, false);

                int lineDataIdx = m_lineData.size() - 1;
                rowIndices.push_back(lineDataIdx);

                column++;
                cellEnd++;
                cellBegin = cellEnd;

                if(m_columns < column) m_columns = column;
            }
        }

        auto done = 0; (void)done;
    }
};