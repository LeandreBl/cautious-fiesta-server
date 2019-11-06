#include <sstream>
#include <cstring>

#include "Trace.hpp"

int trace(const char string[]) noexcept
{
	return fputs(string, stderr);
}

int trace(bool isOk) noexcept
{
	const char *color;
	auto t = time(NULL);
	auto tp = localtime(&t);
	char *asc = asctime(tp);
	int ret;

	if (isOk == true)
		color = "[ " BOLDGREEN "OK" RESET " ] ";
	else
		color = "[ " BOLDRED "KO" RESET " ] ";
	asc[strlen(asc) - 1] = 0;
	ret = fputs(color, stderr);
	return ret + fprintf(stderr, "[%s%s%s]: ", BOLDYELLOW, asc, RESET);
}

int trace(bool isOk, const char string[]) noexcept
{
	int ret = trace(isOk);
	return ret + trace(string);
}