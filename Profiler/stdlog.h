#pragma once

#define _STDLOG

extern"C" {
	void stdlog(const char* f, int l, const char* lpFormat, ...);
}
#ifndef _STDLOG
#define Log(fmt, ...)
#else
#define Log(fmt, ...) stdlog(__func__, __LINE__, fmt, ##__VA_ARGS__)
#endif