
#include "markdownelement.h"

MarkdownElement::MarkdownElement(MarkdownFile *parentFile, const char *typenName)
    : m_parentFile(parentFile)
    , m_isBlock(false)
    , m_typeName(typenName)
{
}

MarkdownElement::~MarkdownElement()
{
    // TODO CLEANUP AFTER STRUCUTRE IS FINAL
}

bool MarkdownElement::isBlock() { return m_isBlock; }

const char* MarkdownElement::typeName() { return m_typeName; }

MarkdownFile* MarkdownElement::parentFile() { return m_parentFile; }

bool MarkdownElement::endsWith(const std::string &line)
{
    return m_isBlock ? line.empty() : true;
}

void MarkdownElement::serializeLine(const LineData &line)
{
    auto file = m_parentFile;
    if(!file) return;

    int textStart = 0;
    for(auto inlineElement : line.m_inlineElements)
    {
        int textEnd = inlineElement->m_startIdx;
        int textLen = textEnd - textStart;
        file->serialize_write(line.m_lineText.substr(textStart, textLen));
        inlineElement->serialize();
        textStart = inlineElement->m_endIdx;
    }
    if(textStart < line.m_lineText.length() - 1)
    {
        int textEnd = line.m_lineText.length();
        int textLen = textEnd - textStart;
        file->serialize_write(line.m_lineText.substr(textStart, textLen));
    }
}

static std::vector<MarkdownInlineElement*> m_inlinePrototypes;

void MarkdownElement::parseLine(const std::string &line, bool isFirstLine, bool raw)
{   
    m_lineData.push_back(LineData(line));

    if(raw) return;
    
    // test setup once
    // TODO: MOVE TO PROPER SCOPE
    if(m_inlinePrototypes.empty())
    {
        m_inlinePrototypes.emplace_back(new BoldText(this));
        m_inlinePrototypes.emplace_back(new ItalicText(this));
        m_inlinePrototypes.emplace_back(new StrikethroughText(this));
        m_inlinePrototypes.emplace_back(new HighlightText(this));
        m_inlinePrototypes.emplace_back(new InlineCode(this));
        m_inlinePrototypes.emplace_back(new Emoji(this));
        m_inlinePrototypes.emplace_back(new Image(this));
        m_inlinePrototypes.emplace_back(new Hyperlink(this));
    }
    
    // parse inline elements

    int begin = 0;
    MarkdownInlineElement *inlineElement = nullptr;
    do
    {
        inlineElement = nullptr;
        MarkdownInlineElement::range_t elementRange;
    
        // find closest registered type
    
        for(auto registeredType : m_inlinePrototypes)
        {
            auto range = registeredType->findIn(line, begin);
            if(range.first != -1 && (!inlineElement || (range.first < elementRange.first)))
            {
                inlineElement = registeredType;
                elementRange = range;
            }
        }
    
        begin = elementRange.second;
    
        // if found
    
        if(inlineElement)
        {
            inlineElement = inlineElement->clone();
            inlineElement->parse(line, elementRange);
    
            m_lineData[m_lineData.size() - 1].m_inlineElements.push_back(inlineElement);
        }
    } while (inlineElement);
}