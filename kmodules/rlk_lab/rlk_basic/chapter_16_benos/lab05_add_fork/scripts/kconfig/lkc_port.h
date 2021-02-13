#ifndef __WIN32_PORTABLE_H
#define __WIN32_PORTABLE_H

#ifndef WIN32
#define DIRDELIM	"/"
#define DIRDELIMC	'/'
#else
#define DIRDELIM	"\\"
#define DIRDELIMC	'\\'
#endif

#ifdef WIN32

#include <windows.h>
#define inline

#define bool int
#define true 1
#define false !true

#define vsnprintf	_vsnprintf
#define random		rand
#define srandom		srand
#define mkdir(x, y)	_mkdir(x)
#define S_ISDIR(x)	_S_IFDIR&&x
#define S_ISREG(x)	_S_IFREG&&x
#define S_IRWXU		(_S_IREAD | _S_IWRITE | _S_IEXEC)
#define PATH_MAX	_MAX_PATH
#define __attribute__(x)

#define _UTSNAME_LENGTH 65
#define _UTSNAME_NODENAME_LENGTH _UTSNAME_LENGTH
#define _UTSNAME_DOMAIN_LENGTH _UTSNAME_LENGTH

struct utsname {
	char sysname[_UTSNAME_LENGTH];
	char nodename[_UTSNAME_NODENAME_LENGTH];
	char release[_UTSNAME_LENGTH];
	char version[_UTSNAME_LENGTH];
	char machine[_UTSNAME_LENGTH];
	char domainname[_UTSNAME_DOMAIN_LENGTH];
};

static inline int uname(struct utsname * uts)
{
	enum { WinNT, Win95, Win98, WinUnknown };
	OSVERSIONINFO osver;
	SYSTEM_INFO sysinfo;
	DWORD sLength;
	DWORD os = WinUnknown;

	memset (uts, 0, sizeof (*uts));

	osver.dwOSVersionInfoSize = sizeof (osver);
	GetVersionEx (&osver);
	GetSystemInfo (&sysinfo);

	switch (osver.dwPlatformId) {
	case VER_PLATFORM_WIN32_NT: /* NT, Windows 2000 or Windows XP */
		if (osver.dwMajorVersion == 4)
			strcpy (uts->sysname, "Windows NT4x"); /* NT4x */
		else if (osver.dwMajorVersion <= 3)
			strcpy (uts->sysname, "Windows NT3x"); /* NT3x */
		else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion < 1)
			strcpy (uts->sysname, "Windows 2000"); /* 2k */
		else if (osver.dwMajorVersion >= 5)
			strcpy (uts->sysname, "Windows XP");   /* XP */
		os = WinNT;
		break;
	case VER_PLATFORM_WIN32_WINDOWS: /* Win95, Win98 or WinME */
		if ((osver.dwMajorVersion > 4) ||
		    ((osver.dwMajorVersion == 4) && (osver.dwMinorVersion > 0))) {
			if (osver.dwMinorVersion >= 90)
				strcpy (uts->sysname, "Windows ME"); /* ME */
			else
				strcpy (uts->sysname, "Windows 98"); /* 98 */
			os = Win98;
		} else {
			strcpy (uts->sysname, "Windows 95"); /* 95 */
			os = Win95;
		}
		break;
	case VER_PLATFORM_WIN32s: /* Windows 3.x */
		strcpy (uts->sysname, "Windows");
		break;
	}

	sprintf (uts->version, "%ld.%02ld",
		osver.dwMajorVersion, osver.dwMinorVersion);

	if (osver.szCSDVersion[0] != '\0' &&
	    (strlen (osver.szCSDVersion) + strlen (uts->version) + 1) <
	    sizeof (uts->version)) {
		strcat (uts->version, " ");
		strcat (uts->version, osver.szCSDVersion);
	}

	sprintf (uts->release, "build %ld", osver.dwBuildNumber & 0xFFFF);

	switch (sysinfo.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_PPC:
		strcpy (uts->machine, "ppc");
		break;
	case PROCESSOR_ARCHITECTURE_ALPHA:
		strcpy (uts->machine, "alpha");
		break;
	case PROCESSOR_ARCHITECTURE_MIPS:
		strcpy (uts->machine, "mips");
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		switch (os) {
		case Win95:
		case Win98:
			switch (sysinfo.dwProcessorType) {
			case PROCESSOR_INTEL_386:
			case PROCESSOR_INTEL_486:
			case PROCESSOR_INTEL_PENTIUM:
				sprintf (uts->machine, "i%ld", sysinfo.dwProcessorType);
				break;
			default:
				strcpy (uts->machine, "i386");
				break;
			}
			break;
			case WinNT:
				sprintf (uts->machine, "i%d86", sysinfo.wProcessorLevel);
				break;
			default:
				strcpy (uts->machine, "unknown");
				break;
		}
		break;
	default:
		strcpy (uts->machine, "unknown");
		break;
	}

	sLength = sizeof (uts->nodename) - 1;
	GetComputerName (uts->nodename, &sLength);
	return 0;
}

static inline int setenv(const char *name, char *value, int unused)
{
	char *buf = malloc(strlen(name) + strlen(value) + 2);

	sprintf(buf, "%s=%s", name, value);
	return putenv(buf);
}

static inline char *basename(const char *path)
{
	return strrchr(path, DIRDELIMC) + 1;
}
#endif

#endif /* __WIN32_PORTABLE_H */
