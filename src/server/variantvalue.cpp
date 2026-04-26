#include "variantvalue.h"

VariantValue::VariantValue(std::string_view value)
    : stringValue(value)
{
}

VariantValue::VariantValue(int value)
    : intValue(value)
{
}

VariantValue::VariantValue(float value)
    : floatValue(value)
{
}

VariantValue::VariantValue(double value)
    : doubleValue(value)
{
}

std::string VariantValue::toString() const
{
    return stringValue;
}

int VariantValue::toInt() const
{
    try {
        int result = intValue;
        if (result == 0) {
            result = std::stoi(stringValue);
        }
        return result;
    } catch (const std::exception &) {
        return 0;
    }
}

int64_t VariantValue::toInt64() const
{
    try {
        int64_t result = std::stoll(stringValue);
        return result;
    } catch (const std::exception &) {
        return 0;
    }
}

float VariantValue::toFloat() const
{
    try {
        float result = floatValue;
        if (result == 0.0f) {
            result = std::stof(stringValue);
        }
        return result;
    } catch (const std::exception &) {
        return 0.0f;
    }
}

double VariantValue::toDouble() const
{
    try {
        double result = doubleValue;
        if (result == 0.0) {
            result = std::stod(stringValue);
        }
        return result;
    } catch (const std::exception &) {
        return 0.0;
    }
}