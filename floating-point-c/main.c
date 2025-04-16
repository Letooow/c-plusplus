#include "number_names.h"
#include "return_codes.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t getError(char *expect, char *actual)
{
	fprintf(stderr, "ILLEGAL ARGUMENT ERROR: expected %s, actual: %s", expect, actual);
	return ERROR_ARGUMENTS_INVALID;
}

typedef struct FloatingPointNumber
{
	uint8_t sing;
	int16_t exponent;
	uint64_t mantis;
} FloatingPointNumber;

typedef struct ModMantis
{
	bool firstUnit;
	bool wasUnits;
} ModMantis;

int8_t checkType(FloatingPointNumber *number, char format)
{
	int16_t checkExp = (format == 'f') ? SINGLE_FLOATING_POINT_EXP : HALF_FLOATING_POINT_EXP + 1;
	uint32_t checkMantis = (format == 'f') ? SINGLE_FLOATING_POINT_BIT : HALF_FLOATING_POINT_BIT;
	if (number->exponent == checkExp && number->mantis == 0)
	{
		return ZERO;
	}
	if (number->exponent != -checkExp + 1)
	{
		return NUMBER;
	}
	if (number->mantis > checkMantis)
	{
		return NAN;
	}
	return INFINITY;
}

void shiftMantiss(uint64_t *fraction, ModMantis *mod)
{
	if (mod->firstUnit == true && mod->wasUnits != true)
	{
		mod->wasUnits = true;
		mod->firstUnit = false;
	}

	if ((*fraction & 0x1) == 1)
	{
		mod->firstUnit = true;
	}
	else
	{
		mod->firstUnit = false;
	}

	(*fraction) >>= 1;
}

void toNormalize(FloatingPointNumber *number, ModMantis *ostShift, char format)
{
	int16_t exp = number->exponent;
	uint64_t fraction = number->mantis;
	int16_t compareExp = (format == 'f') ? SINGLE_FLOATING_POINT_EXP : HALF_FLOATING_POINT_EXP;
	uint8_t shiftFraction = (format == 'f') ? SINGLE_FLOATING_POINT_SHIFT : HALF_FLOATING_POINT_SHIFT;
	if (number->mantis == 0)
		return;
	if (exp == compareExp)
		exp++;

	if (fraction >> shiftFraction == 0)
	{
		while (fraction >> shiftFraction != 1)
		{
			fraction <<= 1;
			exp--;
		}
	}
	else
	{
		while (fraction >> shiftFraction != 1)
		{
			shiftMantiss(&fraction, ostShift);
			exp++;
		}
	}
	number->mantis = fraction;
	number->exponent = exp;
}

void printZero(char format, bool sign)
{
	if (sign)
		printf("-");
	if (format == 'f')
	{
		printf("0x0.000000p+0");
	}
	else
	{
		printf("0x0.000p+0");
	}
}

void printInf(bool sign)
{
	if (sign)
		printf("-");
	printf("inf");
}

bool parseNumber(FloatingPointNumber *result, char *number, char format)
{
	if (format == 'f')
	{
		uint32_t temp;
		sscanf(number, "%x", &temp);
		(*result).sing = temp >> 31;
		(*result).exponent = ((temp >> SINGLE_FLOATING_POINT_SHIFT) & 0xFF) + SINGLE_FLOATING_POINT_EXP;
		(*result).mantis = temp & 0x7FFFFF;
	}
	else if (format == 'h')
	{
		uint16_t temp;
		sscanf(number, "%hi", &temp);
		(*result).sing = temp >> 15;
		(*result).exponent = ((temp >> HALF_FLOATING_POINT_SHIFT) & 0x1F) + (HALF_FLOATING_POINT_EXP + 1);
		(*result).mantis = temp & 0x3ff;
	}
	else
	{
		return ERROR_ARGUMENTS_INVALID;
	}

	ModMantis tempo;
	if (format == 'f')
	{
		if (result->exponent == SINGLE_FLOATING_POINT_EXP && result->mantis > 0)
			toNormalize(result, &tempo, format);
		else if (result->mantis > 0)
			result->mantis += SINGLE_FLOATING_POINT_BIT;
	}
	else
	{
		if (result->exponent == HALF_FLOATING_POINT_EXP && result->mantis > 0)
			toNormalize(result, &tempo, format);
		else if (result->mantis > 0)
			result->mantis += HALF_FLOATING_POINT_BIT;
	}

	return SUCCESS;
}

void printRes(FloatingPointNumber *res, char format)
{
	if (res->sing == 1)
		printf("-");

	if (format == 'f')
		printf("0x1.%.6llxp%+i", (res->mantis & SINGLE_FLOATING_POINT_MANTIS) << 1, res->exponent);
	else
		printf("0x1.%.3llxp%+i", (res->mantis & HALF_FLOATING_POINT_MANTIS) << 2, res->exponent);
}

void shiftAdd(FloatingPointNumber *number, uint32_t exp, ModMantis *mod)
{
	while (number->exponent != exp)
	{
		shiftMantiss(&number->mantis, mod);
		number->exponent++;
	}
}

uint8_t checkNaN(FloatingPointNumber *left, FloatingPointNumber *right, char format)
{
	if ((checkType(left, format)) == NAN || checkType(right, format) == NAN)
		return NAN;
	return SUCCESS;
}

FloatingPointNumber add(FloatingPointNumber *left, FloatingPointNumber *right, ModMantis *mod)
{
	FloatingPointNumber result;
	if (left->exponent > right->exponent)
		shiftAdd(right, left->exponent, mod);
	else
		shiftAdd(left, right->exponent, mod);

	result.exponent = left->exponent;
	if ((left->sing ^ right->sing) == 0)
	{
		result.mantis = left->mantis + right->mantis;
		result.exponent = left->exponent;
		result.sing = left->sing;
	}
	else
	{
		if (left->mantis >= right->mantis)
		{
			result.sing = left->sing;
			result.mantis = left->mantis - right->mantis;
		}
		else
		{
			result.sing = right->sing;
			result.mantis = right->mantis - left->mantis;
		}
	}
	return result;
}

FloatingPointNumber multiply(FloatingPointNumber *numberLeft, FloatingPointNumber *numberRight, char format, ModMantis *mod)
{
	FloatingPointNumber result;
	result.exponent = numberLeft->exponent + numberRight->exponent;
	result.mantis = (numberLeft->mantis * numberRight->mantis);
	if (format == 'f')
	{
		for (int i = 0; i < SINGLE_FLOATING_POINT_SHIFT; i++)
			shiftMantiss(&result.mantis, mod);
	}
	else
	{
		for (int i = 0; i < HALF_FLOATING_POINT_SHIFT; i++)
			shiftMantiss(&result.mantis, mod);
	}
	result.sing = (numberLeft->sing ^ numberRight->sing) & 0x1;
	return result;
}

FloatingPointNumber divide(FloatingPointNumber *left, FloatingPointNumber *right)
{
	FloatingPointNumber result;
	result.sing = (left->sing ^ right->sing) & 0x1;
	result.exponent = left->exponent - right->exponent;
	result.mantis = (left->mantis << SINGLE_FLOATING_POINT_SHIFT) / right->mantis;
	return result;
}

int main(int argc, char *argv[])
{
	if (argc <= 3)
	{
		fprintf(stderr, "Missing arguments");
		return ERROR_ARGUMENTS_INVALID;
	}
	else if (argc <= 5)
	{
		if (strlen(argv[1]) > 1)
			return getError("f or h", argv[1]);
		if (strlen(argv[2]) > 1 || argv[2][0] > '3' || argv[2][0] < '0')
			return getError("0 - 3", argv[2]);

		FloatingPointNumber number;

		if (parseNumber(&number, argv[3], *argv[1]))
			return getError("f or h", argv[1]);
		if (checkType(&number, *argv[1]) != NUMBER)
		{
			switch (checkType(&number, *argv[1]))
			{
			case ZERO:
				if (argv[1][0] == 'f')
					printf("0x0.%.6llxp+0", number.mantis);
				else
					printf("0x0.%.3llup+0", number.mantis);
				break;
			case INFINITY:
				printInf(number.sing);
				break;
			case NAN:
				printf("nan");
			default:
				break;
			}
			return SUCCESS;
		}
		printRes(&number, *argv[1]);
	}
	else if (argc <= 7)
	{
		if (strlen(argv[1]) > 1)
			return getError("f or h", argv[1]);
		FloatingPointNumber leftNumber;
		FloatingPointNumber rightNumber;

		if ((parseNumber(&leftNumber, argv[3], *argv[1]) != SUCCESS) || (parseNumber(&rightNumber, argv[5], *argv[1]) != SUCCESS))
			return getError("f or h", argv[1]);

		FloatingPointNumber result;

		if (checkNaN(&leftNumber, &rightNumber, *argv[1]))
		{
			printf("nan");
			return SUCCESS;
		}

		// operation
		ModMantis mod;

		char operation = argv[4][0];
		int8_t leftCheck = checkType(&leftNumber, *argv[1]);
		int8_t rightCheck = checkType(&rightNumber, *argv[1]);
		uint8_t resSign = leftNumber.sing ^ rightNumber.sing;

		switch (operation)
		{
		case '*':
			if ((rightCheck == ZERO && leftCheck == INFINITY) || (leftCheck == ZERO && rightCheck == INFINITY))
			{
				printf("nan");
				return SUCCESS;
			}

			if (leftCheck == ZERO || rightCheck == ZERO)
			{
				printZero(*argv[1], leftNumber.sing ^ rightNumber.sing);
				return SUCCESS;
			}
			if (leftCheck == INFINITY || rightCheck == INFINITY)
			{
				printInf(resSign);
				return SUCCESS;
			}

			result = multiply(&leftNumber, &rightNumber, *argv[1], &mod);
			break;
		case '-':
			rightNumber.sing = ~(rightNumber.sing) & 0x1;
		case '+':
			if (leftCheck == ZERO && rightCheck == ZERO)
			{
				printZero(*argv[1], false);
				return SUCCESS;
			}
			if (leftNumber.sing != rightNumber.sing && leftCheck == INFINITY && rightCheck == INFINITY)
			{
				printf("nan");
				return SUCCESS;
			}
			if (leftCheck == INFINITY)
			{
				printInf(leftNumber.sing);
				return SUCCESS;
			}
			if (rightCheck == INFINITY)
			{
				printInf(rightNumber.sing);
				return SUCCESS;
			}

			result = add(&leftNumber, &rightNumber, &mod);
			break;
		case '/':
			if (leftCheck != ZERO && rightCheck == ZERO)
			{
				printInf(resSign);
				return SUCCESS;
			}
			else if (leftCheck != INFINITY && rightCheck == INFINITY)
			{
				printZero(argv[1][0], resSign);
				return SUCCESS;
			}
			if (leftCheck == rightCheck)
			{
				if (leftCheck == ZERO || leftCheck == INFINITY)
				{
					printf("nan");
					return SUCCESS;
				}
			}
			if (leftCheck == INFINITY)
			{
				printInf(resSign);
				return SUCCESS;
			}
			if (leftCheck == ZERO)
			{
				printZero(*argv[1], resSign);
				return SUCCESS;
			}

			result = divide(&leftNumber, &rightNumber);
			break;
		default:
			return getError("operations", argv[4]);
		}

		toNormalize(&result, &mod, argv[1][0]);

		// rounding

		switch (argv[2][0])
		{
		case '0':
			printRes(&result, *argv[1]);
			break;
		case '1':
			if (mod.firstUnit)
			{
				if (mod.wasUnits)
					result.mantis += 1;
				else
					result.mantis += ((result.mantis & 0x1) == 0) ? 1 : 0;
			}

			printRes(&result, *argv[1]);
			break;
		case '2':

			if (mod.firstUnit || mod.wasUnits)
				result.mantis += (result.sing == 0) ? 1 : 0;
			printRes(&result, *argv[1]);
			break;
		case '3':

			if (mod.firstUnit || mod.wasUnits)
				result.mantis += (result.sing == 1) ? 1 : 0;

			printRes(&result, *argv[1]);
			break;
		}
	}
	else
	{
		fprintf(stderr, "Incorrect count OF arguments");
		return ERROR_ARGUMENTS_INVALID;
	}
	return SUCCESS;
}
