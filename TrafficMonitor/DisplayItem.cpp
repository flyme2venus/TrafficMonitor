#include "stdafx.h"
#include "DisplayItem.h"
#include "Common.h"
#include "TrafficMonitor.h"

CommonDisplayItem::CommonDisplayItem(DisplayItem item)
{
    is_plugin = false;
    item_type = item;
    plugin_item = nullptr;
}

CommonDisplayItem::CommonDisplayItem(IPluginItem* item)
{
    is_plugin = true;
    plugin_item = item;
}

bool CommonDisplayItem::operator<(const CommonDisplayItem& item) const
{
    if (is_plugin && !item.is_plugin)
        return false;
    else if (!is_plugin && item.is_plugin)
        return true;
    else if (!is_plugin)
        return item_type < item.item_type;
    else
        return theApp.m_plugins.GetItemIndex(plugin_item) < theApp.m_plugins.GetItemIndex(item.plugin_item);
}

bool CommonDisplayItem::operator==(const CommonDisplayItem& item) const
{
    if (is_plugin != item.is_plugin)
        return false;
    else if (!is_plugin)
        return item_type == item.item_type;
    else
        return plugin_item == item.plugin_item;
}

bool CommonDisplayItem::IsPlugin() const
{
    return is_plugin;
}

DisplayItem CommonDisplayItem::ItemType() const
{
    return item_type;
}

IPluginItem* CommonDisplayItem::PluginItem() const
{
    return plugin_item;
}

CString CommonDisplayItem::GetItemName() const
{
    if (is_plugin)
    {
        if (plugin_item != nullptr)
            return plugin_item->GetItemName();
        else
            return CString();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP: return CCommon::LoadText(IDS_UPLOAD);
        case TDI_DOWN: return CCommon::LoadText(IDS_DOWNLOAD);
        case TDI_TOTAL_SPEED: return CCommon::LoadText(IDS_TOTAL_NET_SPEED);
        case TDI_CPU: return CCommon::LoadText(IDS_CPU_USAGE);
        case TDI_MEMORY: return CCommon::LoadText(IDS_MEMORY_USAGE);
        case TDI_GPU_USAGE: return CCommon::LoadText(IDS_GPU_USAGE);
        case TDI_CPU_TEMP: return CCommon::LoadText(IDS_CPU_TEMPERATURE);
        case TDI_GPU_TEMP: return CCommon::LoadText(IDS_GPU_TEMPERATURE);
        case TDI_HDD_TEMP: return CCommon::LoadText(IDS_HDD_TEMPERATURE);
        case TDI_MAIN_BOARD_TEMP: return CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE);
        case TDI_HDD_USAGE: return CCommon::LoadText(IDS_HDD_USAGE);
        case TDI_CPU_FREQ: return CCommon::LoadText(IDS_CPU_FREQ);
        case TDI_TODAY_TRAFFIC: return CCommon::LoadText(IDS_TRAFFIC_USED);
        default:
            ASSERT(false);
            break;
        }
    }
    return CString();
}

std::wstring CommonDisplayItem::DefaultString(bool is_main_window) const
{
    std::wstring default_text;
    if (is_plugin)
    {
        default_text = plugin_item->GetItemLableText();
        default_text += L' ';
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP:
            if (is_main_window)
                default_text = CCommon::LoadText(IDS_UPLOAD_DISP, _T(": "));
            else
                default_text = _T("↑: ");
            break;
        case TDI_DOWN:
            if (is_main_window)
                default_text = CCommon::LoadText(IDS_DOWNLOAD_DISP, _T(": "));
            else
                default_text = _T("↓: ");
            break;
        case TDI_TOTAL_SPEED:
            default_text = _T("↑↓: ");
            break;
        case TDI_TODAY_TRAFFIC:
            default_text = CCommon::LoadText(IDS_TRAFFIC_USED, _T(": "));
            break;
        case TDI_CPU:
            default_text = _T("CPU: ");
            break;
        case TDI_CPU_TEMP:
            default_text = _T("CPU: ");
            break;
        case TDI_CPU_FREQ:
            default_text = CCommon::LoadText(IDS_CPU_FREQ, _T(": "));
            break;
        case TDI_MEMORY:
            default_text = CCommon::LoadText(IDS_MEMORY_DISP, _T(": "));
            break;
        case TDI_GPU_USAGE:
            default_text = CCommon::LoadText(IDS_GPU_DISP, _T(": "));
            break;
        case TDI_GPU_TEMP:
            default_text = CCommon::LoadText(IDS_GPU_DISP, _T(": "));
            break;
        case TDI_HDD_TEMP:
            default_text = CCommon::LoadText(IDS_HDD_DISP, _T(": "));
            break;
        case TDI_MAIN_BOARD_TEMP:
            default_text = CCommon::LoadText(IDS_MAINBOARD_DISP, _T(": "));
            break;
        case TDI_HDD_USAGE:
            default_text = CCommon::LoadText(IDS_HDD_DISP, _T(": "));
            break;
        default:
            ASSERT(false);
            break;
        }
    }

    return default_text;
}

const wchar_t* CommonDisplayItem::GetItemIniKeyName() const
{
    if (is_plugin)
    {
        return plugin_item->GetItemId();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP: return L"up_string";
        case TDI_DOWN: return L"down_string";
        case TDI_CPU: return L"cpu_string";
        case TDI_MEMORY: return L"memory_string";
        case TDI_GPU_USAGE: return L"gpu_string";
        case TDI_CPU_TEMP: return L"cpu_temp_string";
        case TDI_GPU_TEMP: return L"gpu_temp_string";
        case TDI_HDD_TEMP: return L"hdd_temp_string";
        case TDI_MAIN_BOARD_TEMP: return L"main_board_temp_string";
        case TDI_HDD_USAGE: return L"hdd_string";
        case TDI_TOTAL_SPEED: return L"total_speed_string";
        case TDI_CPU_FREQ: return L"cpu_freq_string";
        case TDI_TODAY_TRAFFIC: return L"today_traffic_string";
        }
        ASSERT(FALSE);
        return L"";
    }
}

ValueUnitText CommonDisplayItem::GetItemValueUnitText(bool is_main_window) const
{
    if (is_plugin)
    {
        //插件项目的文本完全由插件决定，不拆分单位
        ValueUnitText value_unit_text;
        value_unit_text.value = plugin_item->GetItemValueText();
        return value_unit_text;
    }
    else
    {
        const PublicSettingData* cfg_data{};
        if (is_main_window)
            cfg_data = &theApp.m_main_wnd_data;
        else
            cfg_data = &theApp.m_taskbar_data;
        ValueUnitText value_unit_text;
        switch (item_type)
        {
        //上传、下载、总网速
        case TDI_UP:
        case TDI_DOWN:
        case TDI_TOTAL_SPEED:
        {
            ValueUnitText in_speed = CCommon::DataSizeToValueUnit(theApp.m_in_speed, *cfg_data);
            ValueUnitText out_speed = CCommon::DataSizeToValueUnit(theApp.m_out_speed, *cfg_data);
            ValueUnitText total_speed = CCommon::DataSizeToValueUnit(theApp.m_in_speed + theApp.m_out_speed, *cfg_data);
            if (!cfg_data->hide_unit || cfg_data->speed_unit == SpeedUnit::AUTO)
            {
                in_speed.unit += _T("/s");
                out_speed.unit += _T("/s");
                total_speed.unit += _T("/s");
            }
            //交换上传和下载位置
            if (is_main_window && theApp.m_main_wnd_data.swap_up_down)
                std::swap(in_speed, out_speed);
            if (item_type == TDI_UP)
                value_unit_text = out_speed;
            else if (item_type == TDI_DOWN)
                value_unit_text = in_speed;
            else
                value_unit_text = total_speed;
        }
            break;
        //CPU利用率
        case TDI_CPU:
            value_unit_text = CCommon::UsageToValueUnit(theApp.m_cpu_usage, *cfg_data);
            break;
        //内存利用率
        case TDI_MEMORY:
            if (cfg_data->memory_display == MemoryDisplay::MEMORY_USED)
                value_unit_text = CCommon::DataSizeToValueUnit(static_cast<unsigned long long>(theApp.m_used_memory) * 1024, cfg_data->separate_value_unit_with_space);
            else if (cfg_data->memory_display == MemoryDisplay::MEMORY_AVAILABLE)
                value_unit_text = CCommon::DataSizeToValueUnit((static_cast<unsigned long long>(theApp.m_total_memory) - static_cast<unsigned long long>(theApp.m_used_memory)) * 1024, cfg_data->separate_value_unit_with_space);
            else
                value_unit_text = CCommon::UsageToValueUnit(theApp.m_memory_usage, *cfg_data);
            break;
        //显卡利用率
        case TDI_GPU_USAGE:
            value_unit_text = CCommon::UsageToValueUnit(theApp.m_gpu_usage, *cfg_data);
            break;
        //硬盘利用率
        case TDI_HDD_USAGE:
            value_unit_text = CCommon::UsageToValueUnit(theApp.m_hdd_usage, *cfg_data);
            break;
        //CPU温度
        case TDI_CPU_TEMP:
            value_unit_text = CCommon::TemperatureToValueUnit(theApp.m_cpu_temperature, *cfg_data);
            break;
        //显卡温度
        case TDI_GPU_TEMP:
            value_unit_text = CCommon::TemperatureToValueUnit(theApp.m_gpu_temperature, *cfg_data);
            break;
        //硬盘温度
        case TDI_HDD_TEMP:
            value_unit_text = CCommon::TemperatureToValueUnit(theApp.m_hdd_temperature, *cfg_data);
            break;
        //主板温度
        case TDI_MAIN_BOARD_TEMP:
            value_unit_text = CCommon::TemperatureToValueUnit(theApp.m_main_board_temperature, *cfg_data);
            break;
        //CPU频率
        case TDI_CPU_FREQ:
            value_unit_text = CCommon::FreqToValueUnit(theApp.m_cpu_freq, *cfg_data);
            break;
        //总流量
        case TDI_TODAY_TRAFFIC:
            value_unit_text = CCommon::KBytesToValueUnit((theApp.m_today_up_traffic + theApp.m_today_down_traffic) / 1024u);
            break;
        default:
            break;
        }
        return value_unit_text;
    }
}

ValueUnitText CommonDisplayItem::GetItemValueSampleUnitText(bool is_main_window) const
{
    if (is_plugin)
    {
        //插件项目的文本完全由插件决定，不拆分单位
        ValueUnitText value_unit_text;
        value_unit_text.value = plugin_item->GetItemValueSampleText();
        return value_unit_text;
    }
    //主窗口（用于绘制预览图）
    else if (is_main_window)
    {
        ValueUnitText value_unit_text;
        switch (item_type)
        {
        case TDI_UP:
            value_unit_text.value = _T("88.8");
            value_unit_text.unit = _T(" KB/s");
            break;
        case TDI_DOWN:
            value_unit_text.value = _T("88.9");
            value_unit_text.unit = _T(" KB/s");
            break;
        case TDI_TOTAL_SPEED:
            value_unit_text.value = _T("90");
            value_unit_text.unit = _T(" KB/s");
            break;
        case TDI_TODAY_TRAFFIC:
            value_unit_text.value = _T("100");
            value_unit_text.unit = _T(" MB");
            break;
        case TDI_CPU:
            value_unit_text.value = _T("50");
            value_unit_text.unit = _T(" %");
            break;
        case TDI_MEMORY:
            value_unit_text.value = _T("51");
            value_unit_text.unit = _T(" %");
            break;
        case TDI_CPU_TEMP: case TDI_GPU_TEMP: case TDI_HDD_TEMP: case TDI_MAIN_BOARD_TEMP:
            value_unit_text.value = _T("40");
            value_unit_text.unit = _T(" °C");
            break;
        case TDI_CPU_FREQ:
            value_unit_text.value = _T("1.0");
            value_unit_text.unit = _T(" GHz");
            break;
        default:
            value_unit_text.value = _T("99");
            break;
        }
        return value_unit_text;
    }
    //任务栏窗口（用于计算任务栏窗口宽度）
    else
    {
        ValueUnitText value_unit_text;
        switch (item_type)
        {
        //网速
        case TDI_UP:
        case TDI_DOWN:
        case TDI_TOTAL_SPEED:
        {
            wstring digits(theApp.m_taskbar_data.digits_number, L'8');      //根据数据位数生成指定个数的“8”
            bool hide_unit{ theApp.m_taskbar_data.hide_unit && theApp.m_taskbar_data.speed_unit != SpeedUnit::AUTO };
            if (theApp.m_taskbar_data.speed_short_mode)
            {
                if (hide_unit)
                    value_unit_text.value.Format(_T("%s."), digits.c_str());
                else
                {
                    value_unit_text.value.Format(_T("%s."), digits.c_str());
                    value_unit_text.unit = _T("M/s");
                }
            }
            else
            {
                if (hide_unit)
                    value_unit_text.value.Format(_T("%s.8"), digits.c_str());
                else
                {
                    value_unit_text.value.Format(_T("%s.8"), digits.c_str());
                    value_unit_text.unit = _T("MB/s");
                }
            }
            //注：原实现中空格是添加在整个字符串末尾的，这里同样把空格放在单位的末尾，保证测得的宽度不变
            if (!hide_unit && theApp.m_taskbar_data.separate_value_unit_with_space)
                value_unit_text.unit += _T(' ');
            if (theApp.m_taskbar_data.speed_short_mode && !theApp.m_taskbar_data.unit_byte && !theApp.m_taskbar_data.hide_unit)
                value_unit_text.unit += _T('b');
        }
            break;
        //占用率百分比
        case TDI_CPU:
        case TDI_MEMORY:
        case TDI_GPU_USAGE:
        case TDI_HDD_USAGE:
        {
            value_unit_text.value = _T("100");
            if (!theApp.m_taskbar_data.hide_percent)
            {
                if (theApp.m_taskbar_data.separate_value_unit_with_space)
                    value_unit_text.unit = _T(" %");
                else
                    value_unit_text.unit = _T("%");
            }
            //内存显示不为已使用百分比时
            if (item_type == TDI_MEMORY)
            {
                if (theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_USED || theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_AVAILABLE)
                {
                    //宽度为总内存的宽度
                    value_unit_text = CCommon::DataSizeToValueUnit(static_cast<unsigned long long>(theApp.m_total_memory) * 1024, theApp.m_taskbar_data.separate_value_unit_with_space);
                }
            }
        }
            break;
        //温度
        case TDI_CPU_TEMP:
        case TDI_GPU_TEMP:
        case TDI_HDD_TEMP:
        case TDI_MAIN_BOARD_TEMP:
        {
            value_unit_text.value = _T("99");
            if (theApp.m_taskbar_data.separate_value_unit_with_space)
                value_unit_text.unit = _T(" °C");
            else
                value_unit_text.unit = _T("°C");
        }
            break;
        //CPU频率
        case TDI_CPU_FREQ:
        {
            value_unit_text.value = _T("1.00");
            if (theApp.m_taskbar_data.separate_value_unit_with_space)
                value_unit_text.unit = _T(" GHz");
            else
                value_unit_text.unit = _T("GHz");
        }
            break;
        //流量
        case TDI_TODAY_TRAFFIC:
        {
            value_unit_text.value = _T("999.99");
            if (theApp.m_taskbar_data.separate_value_unit_with_space)
                value_unit_text.unit = _T(" MB");
            else
                value_unit_text.unit = _T("MB");
        }
            break;
        }
        return value_unit_text;
    }
}

bool CommonDisplayItem::IsDoubleLineExclusive() const
{
    if (is_plugin)
    {
        auto* plugin = theApp.m_plugins.GetPluginByItem(plugin_item);
        if (plugin != nullptr && plugin->GetAPIVersion() >= 8)
        {
            return plugin_item->IsDoubleLineExclusive() == 1;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
DisplayItemSet::DisplayItemSet(std::initializer_list<DisplayItem> items)
    : data(items)
{
}

void DisplayItemSet::Add(DisplayItem item)
{
    data.insert(item);
}

void DisplayItemSet::Remove(DisplayItem item)
{
    data.erase(item);
}

bool DisplayItemSet::Contains(DisplayItem item) const
{
    auto iter = data.find(item);
    return iter != data.end();
}

int DisplayItemSet::ToInt() const
{
    int value = 0;
    //将set中的枚举值转换成int的每个bit位
    for (const auto& item : data)
    {
        if (item <= 31)     //int左移不超过31位
            value |= (1 << item);
    }
    return value;
}

void DisplayItemSet::FromInt(int value)
{
    data.clear();
    //将int的每个bit位转换成set中的枚举值
    for (const auto& item : AllDisplayItems)
    {
        if (item <= 31 && (value & (1 << item)))
            data.insert(item);
    }
}

bool DisplayItemSet::IsEmpty() const
{
    return data.empty();
}
