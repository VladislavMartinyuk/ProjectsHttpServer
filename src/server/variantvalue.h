#ifndef HTTPROUTER_VARIANTVALUE_H
#define HTTPROUTER_VARIANTVALUE_H

#include <cstdint>
#include <string>

class VariantValue
{
public:
    VariantValue() = default;
    explicit VariantValue(std::string_view value);
    explicit VariantValue(int value);
    explicit VariantValue(float value);
    explicit VariantValue(double value);

    std::string toString() const;
    std::string_view toStringView() const;
    int toInt() const;
    int64_t toInt64() const;
    float toFloat() const;
    double toDouble() const;

private:
    std::string_view value;
    std::string stringValue;
    int intValue{0};
    float floatValue{0};
    double doubleValue{0};
};

#endif //HTTPROUTER_VARIANTVALUE_H