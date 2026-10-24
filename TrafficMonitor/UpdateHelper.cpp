#include "stdafx.h"
#include "UpdateHelper.h"
#include "SimpleXML.h"
#include "Common.h"


CUpdateHelper::CUpdateHelper()
{
}


CUpdateHelper::~CUpdateHelper()
{
}

void CUpdateHelper::SetUpdateSource(UpdateSource update_source)
{
    m_update_source = update_source;
}

bool CUpdateHelper::CheckForUpdate()
{
    wstring version_info;
    //注：本仓库是上游 TrafficMonitor 的个人定制版，更新信息统一从本仓库获取，
    //避免把用户引导到上游版本（上游版本不包含本仓库的定制功能）。
    //“更新源”选项仍然保留（GitHub/Gitee），但两者都读取同一份更新信息：
    //本仓库没有 Gitee 镜像，清单中的 GitHub 和 Gitee 两节内容相同。
    //
    //国内网络通常无法访问 raw.githubusercontent.com，因此优先使用 jsDelivr 的
    //GitHub 镜像，读取失败时再回退到 raw.githubusercontent.com。
    //清单中的下载链接仍然只允许 GitHub/Gitee 域名（见 ParseUpdateInfo 中的校验）。
    static const wchar_t* UPDATE_INFO_URLS[] = {
        L"https://cdn.jsdelivr.net/gh/flyme2venus/TrafficMonitor@master/version_utf8.info",
        L"https://raw.githubusercontent.com/flyme2venus/TrafficMonitor/master/version_utf8.info",
    };

    bool succeed{ false };
    for (const wchar_t* url : UPDATE_INFO_URLS)
    {
        if (CCommon::GetURL(url, version_info, true))     //获取版本信息
        {
            succeed = true;
            break;
        }
    }
    if (!succeed)
        return false;

    m_row_data = true;

    ParseUpdateInfo(version_info);

    return true;
}

void CUpdateHelper::ParseUpdateInfo(wstring version_info)
{
    CSimpleXML version_xml;
    version_xml.LoadXMLContentDirect(version_info);

    m_version = version_xml.GetNode(L"version");

    // Validate version string: must only contain digits, dots, and alphanumeric chars.
    // An empty or malformed version string indicates a tampered/invalid manifest.
    if (m_version.empty())
        return;
    for (wchar_t ch : m_version)
    {
        if (!iswalnum(ch) && ch != L'.' && ch != L'-')
        {
            m_version.clear();
            return;
        }
    }

    wstring str_source_tag = (m_update_source == UpdateSource::GitHubSource ? L"GitHub" : L"Gitee");
    wstring str_link_tag, str_link_tag_x64, str_link_tag_arm64ec;
#ifdef WITHOUT_TEMPERATURE
    str_link_tag = L"link_without_temperature";
    str_link_tag_x64 = L"link_without_temperature_x64";
    str_link_tag_arm64ec = L"link_without_temperature_arm64ec";
#else
    str_link_tag = L"link";
    str_link_tag_x64 = L"link_x64";
    str_link_tag_arm64ec = L"link_arm64ec";
#endif
    m_link64 = version_xml.GetNode(str_link_tag_x64.c_str(), str_source_tag.c_str());
    m_link = version_xml.GetNode(str_link_tag.c_str(), str_source_tag.c_str());
    m_link_arm64ec = version_xml.GetNode(str_link_tag_arm64ec.c_str(), str_source_tag.c_str());

    // Validate all download URLs: must use HTTPS and originate from a trusted domain.
    // This prevents a network-level attacker from injecting plain-HTTP or off-domain
    // download links via a tampered update manifest (MITM / supply-chain protection).
    auto IsValidDownloadUrl = [](const std::wstring& url) -> bool {
        if (url.empty())
            return true;
        // Enforce HTTPS scheme
        if (url.compare(0, 8, L"https://") != 0)
            return false;
        // Enforce trusted hosting domains (GitHub or Gitee)
        const std::wstring host = url.substr(8);
        return (host.compare(0, 11, L"github.com/") == 0 ||
                host.compare(0, 10, L"gitee.com/") == 0);
    };

    if (!IsValidDownloadUrl(m_link) || !IsValidDownloadUrl(m_link64) || !IsValidDownloadUrl(m_link_arm64ec))
    {
        // Clear all update data to prevent MITM-injected URLs from being acted upon
        m_version.clear();
        m_link.clear();
        m_link64.clear();
        m_link_arm64ec.clear();
        return;
    }

    CString contents_zh_cn = version_xml.GetNode(L"contents_zh_cn", L"update_contents").c_str();
    CString contents_en = version_xml.GetNode(L"contents_en", L"update_contents").c_str();
    CString contents_zh_tw = version_xml.GetNode(L"contents_zh_tw", L"update_contents").c_str();
    contents_zh_cn.Replace(L"\\n", L"\r\n");
    contents_en.Replace(L"\\n", L"\r\n");
    contents_zh_tw.Replace(L"\\n", L"\r\n");
    m_contents_zh_cn = contents_zh_cn;
    m_contents_en = contents_en;
    m_contents_zh_tw = contents_zh_tw;
}

const std::wstring& CUpdateHelper::GetVersion() const
{
    return m_version;
}

const std::wstring& CUpdateHelper::GetLink() const
{
    return m_link;
}

const std::wstring& CUpdateHelper::GetLink64() const
{
    return m_link64;
}

const std::wstring& CUpdateHelper::GetLinkArm64ec() const
{
    return m_link_arm64ec;
}

const std::wstring& CUpdateHelper::GetContentsEn() const
{
    return m_contents_en;
}

const std::wstring& CUpdateHelper::GetContentsZhCn() const
{
    return m_contents_zh_cn;
}

const std::wstring& CUpdateHelper::GetContentsZhTw() const
{
    return m_contents_zh_tw;
}

bool CUpdateHelper::IsRowData()
{
    return m_row_data;
}
