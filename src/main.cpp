#include <iostream>

using namespace std;

__inline int32_t LINEAR11_TO_MANTISSA(u_int16_t packed_linear_format_number);
__inline int32_t LINEAR11_TO_EXPONENT(u_int32_t packed_linear_format_number);
__inline u_int32_t SHIFT_EXPONENT_UNSIGNED(u_int32_t value, int32_t exponent);

int main()
{

    u_int32_t transformer_scale_liner11 = 0xb3ff; // real value 0.999
    u_int32_t transformer_turns_ratio = (u_int32_t)LINEAR11_TO_MANTISSA(transformer_scale_liner11);
    int32_t transformer_scale_exponent = LINEAR11_TO_EXPONENT(transformer_scale_liner11);

    transformer_turns_ratio = SHIFT_EXPONENT_UNSIGNED(transformer_turns_ratio, (transformer_scale_exponent + 12));

    u_int32_t pid_ff_i82_div_trans_scale_loop = 0xFFF;
    if (transformer_turns_ratio > 0)
    {
        pid_ff_i82_div_trans_scale_loop = (1 << 19) / transformer_turns_ratio;
        if (pid_ff_i82_div_trans_scale_loop > 0xFFF)
            pid_ff_i82_div_trans_scale_loop = 0xFFF;
    }

    pid_ff_i82_div_trans_scale_loop *= 82;

    cout << "transformer_scale_liner11\t"       << transformer_scale_liner11        << endl;
    cout << "transformer_turns_ratio\t\t"       << transformer_turns_ratio          << endl;
    cout << "transformer_scale_exponent\t"      << transformer_scale_exponent       << endl;
    cout << "pid_ff_i82_div_trans_scale_loop\t" << pid_ff_i82_div_trans_scale_loop  << endl;

    int pid = pid_ff_i82_div_trans_scale_loop >> -transformer_scale_exponent;
    cout << "\n\npid\t" << pid << endl;

    return 0;
}

__inline int32_t LINEAR11_TO_MANTISSA(u_int16_t packed_linear_format_number)
{
    int32_t mant_bits = packed_linear_format_number & 0x07ff;
    int32_t mant_signbit = mant_bits & 0x0400;
    return ((int32_t)(mant_signbit ? (mant_bits | 0xfffff800) : mant_bits));
}

/// @brief 
/// @param packed_linear_format_number 
/// @return 
__inline int32_t LINEAR11_TO_EXPONENT(u_int32_t packed_linear_format_number)
{
    int32_t exp_bits = (packed_linear_format_number >> 11) & 0x1f;
    int32_t exp_signbit = exp_bits & 0x0010;
    return ((int32_t)(exp_signbit ? (exp_bits | 0xffffffe0) : exp_bits));
}

/// @brief 
/// @param value 
/// @param exponent 
/// @return 
__inline u_int32_t SHIFT_EXPONENT_UNSIGNED(u_int32_t value, int32_t exponent)
{
    if (exponent <= 0)
    {
        exponent -= exponent;
        value = value >> exponent;
    }
    else
    {
        value = value << exponent;
    }
    return value;
}