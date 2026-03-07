#pragma once

//#include "markdownelement.h"
#include "markdownfile.h"

#include <iostream>

class MarkdownElement;

class MarkdownInlineElement
{
public:
    typedef std::pair<int,int> range_t;

    const char *m_typeName;
    MarkdownElement *m_parentElement;
    std::string m_textValue;
    std::string m_delimStart, m_delimEnd;
    int m_startIdx;
    int m_endIdx;

    MarkdownInlineElement(MarkdownElement *parentElement, const char *typeName)
        : m_parentElement(parentElement)
        , m_typeName(typeName)
        , m_startIdx(-1)
        , m_endIdx(-1)
    {
    }

    static bool rangeValid(const range_t &range, const std::string &str)
    {
        return range.first >= 0 && range.second > range.first && range.second <= str.length();
    }

    // should return the index of the starting position of the inline element
    // if start and end have been found and parsing can start
    // -1 if not found or invalid
    // can be overriden
    virtual range_t findIn(const std::string &line, int from)
    {
        auto start = line.find(m_delimStart, from);
        if(start != std::string::npos)
        {
            auto end = line.find(m_delimEnd, start + m_delimStart.length());
            if(end != std::string::npos)
            {
                end += m_delimEnd.length();
                return std::make_pair<int, int>(start, end);
            }
        }
        return std::make_pair<int, int>(-1, -1);
    }

    virtual bool parse(const std::string &line, const range_t &range);

    virtual void serialize() = 0;
    virtual MarkdownInlineElement* clone() = 0;
};

// DERIVED CLASSES

class BoldText : public MarkdownInlineElement
{
public:
    BoldText(MarkdownElement *parentElement)
        : MarkdownInlineElement(parentElement, "BoldText")
    {
        m_delimStart = "**";
        m_delimEnd = "**";
    }

    //virtual range_t findIn(const std::string &line) = super

    //virtual bool parse(const std::string &line, const range_t &range) = super

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new BoldText(m_parentElement); };
};

class ItalicText : public MarkdownInlineElement
{
public:
    ItalicText(MarkdownElement *parentElement)
        : MarkdownInlineElement(parentElement, "ItalicText")
    {
        m_delimStart = "*";
        m_delimEnd = "*";
    }

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new ItalicText(m_parentElement); };
};

class StrikethroughText : public MarkdownInlineElement
{
public:
    StrikethroughText(MarkdownElement *parentElement)
        : MarkdownInlineElement(parentElement, "StrikethroughText")
    {
        m_delimStart = "~~";
        m_delimEnd = "~~";
    }

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new StrikethroughText(m_parentElement); };
};

class HighlightText : public MarkdownInlineElement
{
public:
    HighlightText(MarkdownElement *parentElement)
        : MarkdownInlineElement(parentElement, "HighlightText")
    {
        m_delimStart = "==";
        m_delimEnd = "==";
    }

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new HighlightText(m_parentElement); };
};

class InlineCode : public MarkdownInlineElement
{
public:
    InlineCode(MarkdownElement *parentElement)
        : MarkdownInlineElement(parentElement, "InlineCode")
    {
        m_delimStart = "`";
        m_delimEnd = "`";
    }

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new InlineCode(m_parentElement); };
};

class Emoji : public MarkdownInlineElement
{
public:
    Emoji(MarkdownElement *parentElement)
        : MarkdownInlineElement(parentElement, "Emoji")
    {
        m_delimStart = ":";
        m_delimEnd = ":";
    }

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new Emoji(m_parentElement); };
};

class Hyperlink : public MarkdownInlineElement
{
protected:
    std::string m_delimCenter;
    int m_centerIdx;
    std::string m_url;

public:
    Hyperlink(MarkdownElement *parentElement)
        : MarkdownInlineElement(parentElement, "Hyperlink")
        , m_delimCenter("](")
        , m_url()
    {
        m_delimStart = "[";
        m_delimEnd = ")";
    }

    virtual range_t findIn(const std::string &line, int from) override
    {
        // like baseclass but nested inbetween start and end also search the center delim
        auto start = line.find(m_delimStart, from);
        if(start != std::string::npos)
        {
            auto center = line.find(m_delimCenter, start + m_delimStart.length());
            if(center != std::string::npos)
            {
                auto end = line.find(m_delimEnd, center + m_delimCenter.length());
                if(end != std::string::npos)
                {
                    end += m_delimEnd.length();
                    return std::make_pair<int, int>(start, end);
                }
            }
        }
        return std::make_pair<int, int>(-1, -1);
    }

    virtual bool parse(const std::string &line, const range_t &range) override
    {
        if(!rangeValid(range, line)) return false;

        m_startIdx = range.first;
        m_endIdx = range.second;
        m_centerIdx = line.find(m_delimCenter, m_startIdx + m_delimStart.length());

        if(m_centerIdx == std::string::npos || m_centerIdx > m_endIdx)
        {
            // parsing error
            return false;
        }

        auto range1Start = range.first + m_delimStart.length();
        auto range1End = m_centerIdx;
        auto range1Len = range1End - range1Start;

        auto range2Start = range1End + m_delimCenter.length();
        auto range2End = range.second - m_delimEnd.length();
        auto range2Len = range2End - range2Start;

        m_url = line.substr(range2Start, range2Len);
        m_textValue = line.substr(range1Start, range1Len);

        std::cout << "-- hyperlink parsed: url=" << m_url << ", title=" << m_textValue << std::endl; 

        return true;
    }

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new Hyperlink(m_parentElement); };
};

class Image : public Hyperlink
{
public:
    Image(MarkdownElement *parentElement)
        : Hyperlink(parentElement)
    {
        m_typeName = "Image";
        m_delimStart = "![";
    }

    virtual void serialize() override;
    virtual MarkdownInlineElement* clone() override { return new Image(m_parentElement); };
};