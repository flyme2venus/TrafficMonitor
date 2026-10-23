#pragma once

//数值和单位分开存放的文本。value + unit 与拆分前完全一致
//定义在此独立头文件中，以便 DisplayItem.h 和 CommonData.h 都能使用（两者之间存在相互包含）
struct ValueUnitText
{
    CString value;      //数值部分
    CString unit;       //单位部分（如“ KB/s”、“%”、“°C”，可以为空）

    CString ToString() const { return value + unit; }
    bool HasUnit() const { return !unit.IsEmpty(); }
};
