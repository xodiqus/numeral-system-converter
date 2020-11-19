
#include "numeral_system_converter.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

int nsc_converti(nsc_number_t num, int base)
{
	int result = 0;

	for (unsigned i = 0; i < num.length; ++i)
	{
		unsigned power = num.length - (i + 1U);
		result += (int)pow(base, power) * num.buf[i];
	}	

	return result * (num.sign ? -1 : 1);
}

double nsc_convertd(nsc_number_t num, double base)
{
	double result = 0;

	for (unsigned i = 0; i < num.length; ++i)
	{
		int power = (int)num.point_pos - (int)(i + 1U);
		result += pow(base, power) * num.buf[i];
	}	

	return result * (num.sign ? -1.0 : 1.0);
}

_Dcomplex nsc_convertdc(nsc_number_t num, _Dcomplex base)
{
	_Dcomplex result = {._Val = {0, 0}};

	for (unsigned i = 0; i < num.length; ++i)
	{
		int power = (int)num.point_pos - (int)(i + 1U);
		
		_Dcomplex temp = _Cmulcr(cpow(base,_Cbuild(power, 0)), 
								 num.buf[i]);			 
		result._Val[0] += temp._Val[0];
		result._Val[1] += temp._Val[1];
	}	

	if (num.sign)
		return _Cmulcr(result, -1.0);

	return result;
}

static bool nsc_pasre_char(unsigned x, char* result)
{
	if (0 <= x && x <= 9)
	{
		*result = (char)x + '0';
		return true;
	}
	
	if (0xA <= x && x <= 0xA + (int)'Z')
	{
		*result = (char)x + 'A';
		return true;
	}
	
	return false;
}

static bool nsc_pasre_digit(char x, unsigned* result)
{	
	if (result == NULL)
		return false;

	if ('A' <= x && x <= 'Z')
	{
		*result = x - 'A' + 10;
		return true;
	}
	
	if ('0' <= x && x <= '9')
	{
		*result = x - '0';
		return true;
	}
	
	return false;
}

bool nsc_try_converti (const char* str, int base, unsigned digits_count, int* result)
{
	if (nsc_check(str, digits_count))
	{
		const size_t len = strlen(str);
		nsc_number_t num = {.buf = calloc(len, sizeof(*num.buf))};

		if(nsc_parse(str, &num))
		{
			*result = nsc_converti(num, base);

			free(num.buf);
			return true;
		}
		free(num.buf);
	}

	return false;
}

bool nsc_try_convertd (const char* str, double base, unsigned digits_count, double* result)
{
	if (nsc_check(str, digits_count))
	{
		const size_t len = strlen(str);
		nsc_number_t num = {.buf = calloc(len, sizeof(*num.buf))};

		if (nsc_parse(str, &num))
		{
			*result = nsc_convertd(num, base);

			free(num.buf);
			return true;
		}
		free(num.buf);
	}

	return false;
}

bool nsc_try_convertdc(const char* str, _Dcomplex base, unsigned digits_count, _Dcomplex* result)
{
	if (nsc_check(str, digits_count))
	{
		const size_t len = strlen(str);
		nsc_number_t num = {.buf = calloc(len, sizeof(*num.buf))};

		if (nsc_parse(str, &num))
		{
			*result = nsc_convertdc(num, base);

			free(num.buf);
			return true;
		}
		free(num.buf);
	}

	return false;
}

bool nsc_check(const char* str, unsigned digits_count)
{
	unsigned i = 0;
	switch(str[i])
	{
		case '+':  
		case '-': i++; break;
	}

	bool foundPoint = false;
	for (unsigned j = 0; str[i] != 0; ++i)
	{
		unsigned val;
		if (!foundPoint && str[i] == '.')
		{
			foundPoint = true;
		} 
		else if (!nsc_pasre_digit(str[i], &val) || val >= digits_count)
		{
			return false;
		}
	}
	return true;
}

bool nsc_parse(const char* str, nsc_number_t* number)
{	
	unsigned i = 0;

	if (str[i] != 0)
		switch(str[i])
		{
			case '+': number->sign = false; i++; break;
			case '-': number->sign =  true; i++; break;
		}
	
	unsigned j = 0;
	for (; str[i] != 0; ++i)
	{
		unsigned digit;
		if (str[i] == '.')
		{
			number->point_pos = j;
		}
		else if (nsc_pasre_digit(str[i], &digit))
		{
			number->buf[j++] = digit;
		}
		else return false;
	}

	number->length = j;
	return true;
}