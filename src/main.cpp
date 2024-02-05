#include <iostream>
#include "main.h"

using namespace std;

USER_DATA_t user_data;

int main()
{
    initialize_data();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int32_t droop_rout = user_data.tempco_mantissa * (user_data.temperature - 25);
    uint32_t left_shift_one = SHIFT_EXPONENT((uint32_t)1, -user_data.tempco_exponent);
    droop_rout = left_shift_one + droop_rout;
    uint32_t rout_mantissa = ((user_data.r25_mantissa * droop_rout) >> 4);
    int32_t rout_exponent = (user_data.r25_exponent + user_data.tempco_exponent + 4);
    uint16_t L11_rout = (uint16_t)((TWOS_COMPLEMENT(5, rout_exponent) << 11) | (rout_mantissa & 0x7ff));

    cout << "L11_rout:\t" << hex << L11_rout << endl;

    uint32_t turnsratio_temp = SHIFT_EXPONENT((uint32_t)4, -user_data.turnsratio_exponent) + (user_data.turnsratio_mantissa * 2); // normalized it to same format as turnsratio
    int32_t IOUT_temp_mantissa = SHIFT_EXPONENT(user_data.vin_mantissa, -(user_data.turnsratio_exponent -user_data.vin_exponent)) / (turnsratio_temp >> 5);
    int32_t IOUT_temp_exp = -5;
    uint16_t L11_IOUT_temp = (uint16_t)((TWOS_COMPLEMENT(5, IOUT_temp_exp) << 11) | (IOUT_temp_mantissa & 0x7ff));

    cout << "L11_IOUT_temp:\t" << hex << L11_IOUT_temp << endl;

    int32_t Iout_init_mantissa;
    int32_t Iout_init_exp;

    if (-user_data.vout_exponent > -IOUT_temp_exp)
    {
        Iout_init_mantissa = SHIFT_EXPONENT(IOUT_temp_mantissa, -(user_data.vout_exponent - IOUT_temp_exp)) - user_data.vout_mantissa;
        Iout_init_exp = user_data.vout_exponent;
    }
    else
    {
        Iout_init_mantissa = IOUT_temp_mantissa - SHIFT_EXPONENT(user_data.vout_mantissa, -(IOUT_temp_exp - user_data.vout_exponent));
        Iout_init_exp = IOUT_temp_exp;
    }

    uint16_t L11_IOUT_init = (uint16_t)((TWOS_COMPLEMENT(5, Iout_init_exp) << 11) | (Iout_init_mantissa & 0x7ff));

    cout << "L11_IOUT_init:\t" << hex << L11_IOUT_init << endl;

    int32_t Iout_mantissa;
    int32_t Iout_exp;

    if (-rout_exponent > -Iout_init_exp)
    {
        Iout_mantissa = (Iout_init_mantissa << 6) / SHIFT_EXPONENT(rout_mantissa, -(rout_exponent - Iout_init_exp));
    }
    else
    {
        Iout_mantissa = (Iout_init_mantissa << 6) / SHIFT_EXPONENT(rout_mantissa, -(Iout_init_exp - rout_exponent));
    }

    cout << hex << (uint16_t)((TWOS_COMPLEMENT(5, -6) << 11) | (Iout_mantissa & 0x7ff)) << endl;

    Iout_exp = -10;

    int32_t Iout_mantissa_A = Iout_mantissa * 1000;

    Iout_mantissa_A = Iout_mantissa_A >> 4;

    int32_t Iout_exp_A = Iout_exp + 4;

    uint16_t L11_IOUT = (uint16_t)((TWOS_COMPLEMENT(5, Iout_exp_A) << 11) | (Iout_mantissa_A & 0x7ff));

    cout << "L11_IOUT:\t" << hex << L11_IOUT << endl;

    int32_t Iout_normalized;
    int32_t Iin_exponent;

    // uint32_t normalized_turnsratio = SHIFT_EXPONENT(Iout_mantissa, -(turnsratio_exponent - Iout_exp - 6));

    Iout_normalized = SHIFT_EXPONENT(Iout_mantissa_A, (Iout_exp_A));
    uint32_t turnsratio_normmalized = SHIFT_EXPONENT(turnsratio_temp, (user_data.turnsratio_exponent));

    // if (-turnsratio_exponent > -Iout_exp) {
    // Iin_mantissa = SHIFT_EXPONENT(Iout_mantissa, -(turnsratio_exponent - Iout_exp - 6)) / turnsratio_temp;
    // }
    // else {
    // Iin_mantissa = SHIFT_EXPONENT(Iout_mantissa,  -(Iout_exp - turnsratio_exponent - 6)) / turnsratio_temp;
    // }

    // Iin_exponent = -6;
    //
    // int32_t Iin_mantissa_A = Iin_mantissa * 1000;
    //
    // Iin_mantissa_A = Iin_mantissa_A >> 8;
    //
    // int32_t Iin_exponent_A = Iin_exponent + 8;
    //
    // uint16_t L11_IIN =  (uint16_t)((TWOS_COMPLEMENT(5, Iin_exponent_A) << 11) | (Iin_mantissa_A & 0x7ff));

    int32_t Iin;

    if ((int16_t)Iout_mantissa_A > 1023)
    {
        Iin = 0;
    }
    else
    {
        Iin = Iout_normalized / (turnsratio_normmalized);

        // Iin = SHIFT_EXPONENT(Iout_mantissa_A, -(turnsratio_exponent - Iout_exp_A)) / (turnsratio_normmalized >> 5);
    }

    return 0;
}

/////////////////////////////////////QFORMAT API///////////////////////////////////////////////////////////////////

__inline int32_t LINEAR11_TO_MANTISSA(u_int16_t packed_linear_format_number)
{
    int32_t mant_bits = packed_linear_format_number & 0x07ff;
    int32_t mant_signbit = mant_bits & 0x0400;
    return ((int32_t)(mant_signbit ? (mant_bits | 0xfffff800) : mant_bits));
}

__inline int32_t LINEAR11_TO_EXPONENT(u_int32_t packed_linear_format_number)
{
    int32_t exp_bits = (packed_linear_format_number >> 11) & 0x1f;
    int32_t exp_signbit = exp_bits & 0x0010;
    return ((int32_t)(exp_signbit ? (exp_bits | 0xffffffe0) : exp_bits));
}

__inline u_int32_t SHIFT_EXPONENT(u_int32_t value, int32_t exponent)
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

u_int8_t TWOS_COMPLEMENT(uint8_t numbits, int32_t exp)
{
    return ((u_int8_t)exp & ((1 << numbits) - 1));
}



void initialize_data()
{
    cout << "LINEAR11 tempco value: \t";
    cin >> user_data.get_tempco;
    cout << endl;

    cout << "LINEAR11 temperature value: \t";
    cin >> user_data.get_temp;
    cout << endl;

    cout << "LINEAR11 R25 value: \t";
    cin >> user_data.get_R25;
    cout << endl;

    cout << "LINEAR11 turnsratio value: \t";
    cin >> user_data.get_turnsratio;
    cout << endl;

    cout << "LINEAR11 vin value: \t";
    cin >> user_data.get_vin;
    cout << endl;

    cout << "LINEAR11 vout value: \t";
    cin >> user_data.get_vout;
    cout << endl;

    user_data.tempco_mantissa = LINEAR11_TO_MANTISSA(user_data.get_tempco);
    user_data.tempco_exponent = LINEAR11_TO_EXPONENT(user_data.get_tempco);

    user_data.temp_mantissa = LINEAR11_TO_MANTISSA(user_data.get_temp);
    user_data.temp_exponent = LINEAR11_TO_EXPONENT(user_data.get_temp);
    user_data.temperature = SHIFT_EXPONENT(user_data.temp_mantissa, user_data.temp_exponent);

    user_data.r25_mantissa = LINEAR11_TO_MANTISSA(user_data.get_R25);
    user_data.r25_exponent = LINEAR11_TO_EXPONENT(user_data.get_R25);

    user_data.turnsratio_mantissa = LINEAR11_TO_MANTISSA(user_data.get_turnsratio);
    user_data.turnsratio_exponent = LINEAR11_TO_EXPONENT(user_data.get_turnsratio);

    user_data.vin_mantissa = LINEAR11_TO_MANTISSA(user_data.get_vin);
    user_data.vin_exponent = LINEAR11_TO_EXPONENT(user_data.get_vin);

    user_data.vout_mantissa = LINEAR11_TO_MANTISSA(user_data.get_vout);
    user_data.vout_exponent = LINEAR11_TO_EXPONENT(user_data.get_vout);
}