#include "markdowninlineelement.h"
#include "markdownelement.h"
#include "settings.h"
#include "stdx.h"

bool MarkdownInlineElement::parse(const std::string &line, const range_t &range)
{
    if(!rangeValid(range, line)) return false;

    auto rangeStart = range.first + m_delimStart.length();
    auto rangeEnd = range.second - m_delimEnd.length();
    auto rangeLen = rangeEnd - rangeStart;
    m_textValue = line.substr(rangeStart, rangeLen);
    m_startIdx = range.first;
    m_endIdx = range.second;
    return true;
}

void BoldText::serialize()
{
    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    pf->serialize_write("<b>");
    pf->serialize_write(m_textValue);
    pf->serialize_write("</b>");
}

void ItalicText::serialize()
{
    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    pf->serialize_write("<i>");
    pf->serialize_write(m_textValue);
    pf->serialize_write("</i>");
}

void StrikethroughText::serialize()
{
    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    pf->serialize_write("<del>");
    pf->serialize_write(m_textValue);
    pf->serialize_write("</del>");
}

void HighlightText::serialize()
{
    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    pf->serialize_write("<span class=\"highlight\">");
    pf->serialize_write(m_textValue);
    pf->serialize_write("</span>");
}

void InlineCode::serialize()
{
    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    pf->serialize_write("<code class=\"inline-code\">");
    pf->serialize_write(m_textValue);
    pf->serialize_write("</code>");
}

#include <map>

static std::map<std::string, std::string> emojiMap;

void Emoji::serialize()
{
    if(emojiMap.empty())
    {
        emojiMap["smile"] = "&#128512;";
    }

    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    if (auto search = emojiMap.find(m_textValue); search != emojiMap.end())
    {
        pf->serialize_write(search->second);
    }
    else
    {
        pf->serialize_write(m_delimStart);
        pf->serialize_write(m_textValue);
        pf->serialize_write(m_delimEnd);
    }
}

void Hyperlink::serialize()
{
    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    pf->serialize_write("<a href=\"" + m_url + "\">");
    pf->serialize_write(m_textValue.empty() ? m_url : m_textValue);
    pf->serialize_write("</a>");
}


bool Image::parse(const std::string &line, const range_t &range)
{
    bool parsed = Hyperlink::parse(line, range);
    if(parsed)
    {
        bool isRelativeLink = !std::starts_with(m_url, "https://") && !std::starts_with(m_url, "http://");
        if(isRelativeLink)
        {
            // relative link (to resource on same site)
            auto index = m_url.find_last_of("/");
            if(index != std::string::npos)
            {
                if(index > m_url.length() - 2)
                {
                    m_url = "parsing error";
                    return false;
                }
                std::string path = "";
                path += m_url.substr(0, index) + "/";
                if(!std::starts_with(m_url, "/"))
                {
                    path += Settings::attachmentsDir + "/";
                }
                path += m_url.substr(index + 1, m_url.length() - index - 1);
                m_url = path;
            }
            else
            {
                std::string path = "";
                path += Settings::attachmentsDir + "/" + m_url;
                m_url = path;
            }
        }
    }
    return parsed;
}

void Image::serialize()
{
    if(!m_parentElement) return;
    if(!m_parentElement->parentFile()) return;

    auto pf = m_parentElement->parentFile();

    pf->serialize_write("<img class=\"large\" src=\"" + m_url + "\" alt=\"" + m_textValue + " (Image not found)" + "\"></img>");
}