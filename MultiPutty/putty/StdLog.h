#ifndef STDLOG_H
#define STDLOG_H

#ifdef _DEBUG
#define _STDLOG
#endif

void stdlog(const char* lpFormat, ...);
void Trace(const char* pszFormat, ...);

#endif
