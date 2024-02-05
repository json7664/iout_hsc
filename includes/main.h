#include <iostream>


__inline int32_t LINEAR11_TO_MANTISSA(u_int16_t packed_linear_format_number);
__inline int32_t LINEAR11_TO_EXPONENT(u_int32_t packed_linear_format_number);
__inline u_int32_t SHIFT_EXPONENT(u_int32_t value, int32_t exponent);
u_int8_t TWOS_COMPLEMENT(uint8_t numbits, int32_t exp);
void initialize_data();

// global declaration
typedef struct main
{
    u_int32_t get_tempco;
    u_int32_t get_turnsratio;
    u_int32_t get_temp;
    u_int32_t get_R25; 
    u_int32_t get_vin; 
    u_int32_t get_vout;

    int32_t tempco_mantissa;
    int32_t tempco_exponent;
    int32_t temp_mantissa;
    int32_t temp_exponent;
    int32_t temperature;
    int32_t r25_mantissa;
    int32_t r25_exponent;
    int32_t turnsratio_mantissa;
    int32_t turnsratio_exponent;
    int32_t vin_mantissa;
    int32_t vin_exponent;
    int32_t vout_mantissa;
    int32_t vout_exponent;

    
} USER_DATA_t;

extern USER_DATA_t user_data;