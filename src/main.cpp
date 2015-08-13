/*
   +--------------------------------------------------------------------+
   | ..........................:: HLMaster ::.......................... |
   +--------------------------------------------------------------------+
   | Copyright (C) 2001 Silvan Minghetti                                |
   |                                                                    |
   | This program is free software; you can redistribute it and/or      |
   | modify it under the terms of the GNU General Public License        |
   | as published by the Free Software Foundation; either version 2     |
   | of the License, or (at your option) any later version.             |
   |                                                                    |
   | This program is distributed in the hope that it will be useful,    |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of     |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      |
   | GNU General Public License for more details.                       |
   |                                                                    |
   | You should have received a copy of the GNU General Public License  |
   | along with this program; if not, write to the Free Software        |
   | Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA          |
   | 02111-1307, USA.                                                   |
   |                                                                    |
   +--------------------------------------------------------------------+
   | Created.......: Thursday, 10. May 2001                             |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Half-Life master-server deamon.                    |
   +--------------------------------------------------------------------+
*/

// my includes
// ======================================================================
#include "PString.h"
#include "PStringList.h"
#include "PLogfile.h"
#include "PArgumentList.h"
#include "PUDPSocket.h"
#include "CInetAddress.h"

#include "HLMaster.h"
#include "HLServerList.h"
#include "HLServerFilterList.h"


// general includes
// ======================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pfc++/system.h>

// begin windows specific
// ======================================================================
#if defined PFC_SYSTEM_TYPE_WINDOWS
#include <time.h>
#include <winsock2.h>

WSADATA wsaData;


// begin linux specific
// ======================================================================
#elif defined PFC_SYSTEM_TYPE_UNIX
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>


// end linux specific
// ======================================================================
#endif


#define VERSION_MAJOR	0
#define VERSION_MINOR	9
#define VERSION_PATCH	3

#define DEFAULT_SERVERNAME	"HLMaster 0.9.3"



// function prototypes
// ======================================================================
void print_usage();
void print_version();


// begin unix specific
// ======================================================================
#ifdef PFC_SYSTEM_TYPE_UNIX

int  daemon_start();

bool pidfile_create(const char *pszFilename);
bool pidfile_remove();

bool sig_install_handlers();
bool sig_restore_handlers();

void sig_block_all_init();
bool sig_block_all_install();
bool sig_block_all_restore();

void sig_block_reload_init();
bool sig_block_reload_install();
bool sig_block_reload_restore();

sigset_t g_sig_blockmask_all;
sigset_t g_sig_blockmask_reload;
sigset_t g_sig_blockmask_original;

static void sig_handle_TERM(int nSignal);
static void sig_handle_HUP(int nSignal);


PString *g_pPidFile  = NULL;


// end unix specific
// ======================================================================
#endif




// global variables
// ======================================================================
PLogfile *g_pInfoLog   = NULL;
PLogfile *g_pAccessLog = NULL;

HLMaster *g_pMaster    = NULL;

bool g_bIsDaemon  = false;
bool g_bIsPrivate = false;

volatile bool g_bIsRunning     = false;
volatile bool g_bRequestReload = false;




// main
// ======================================================================

int main(int argc, char *argv[])
{
	PArgumentList *pArgs = new PArgumentList();
	pArgs->AddOption('a', "ip-address", true, "0.0.0.0");
	pArgs->AddOption('p', "port", true, "27010");
	pArgs->AddOption('h', "help");
	pArgs->AddOption('V', "version");
	pArgs->AddOption('n', "log-level", true, "4");
	pArgs->AddOption('m', "max-servers", true, "200");
	pArgs->AddOption('f', "server-filter", true);
	pArgs->AddOption('l', "server-list", true);
	pArgs->AddOption('s', "master-name", true);
	pArgs->AddOption('c', "closed", false);
	pArgs->AddOption('I', "info-log", true, "info.log");
	pArgs->AddOption('A', "access-log", true, "access.log");

	#if defined PFC_SYSTEM_TYPE_UNIX
	pArgs->AddOption('P', "pid-file", true);
	pArgs->AddOption('d', "no-daemon");
	pArgs->AddOption('u', "user", true);
	pArgs->AddOption('g', "group", true);
	#endif

	pArgs->ParseArguments((const char**) &argv[1], (argc - 1));


	// check for --help
	if(pArgs->IsEnabled('h') == true)
	{
		print_usage();
		return 0;
	}

	// check for --version
	if(pArgs->IsEnabled('V') == true)
	{
		print_version();
		return 0;
	}

	// check for unrecognzied options
	if(pArgs->GetNumUnknownArguments() > 0)
	{
		printf("Usage: hlmaster [OPTION]...\n");

		printf("\n");
		printf("Unrecognized option(s):\n");

		PStringList *pStrings = pArgs->GetUnknownArgumentsList();
		for(pStrings->PickFirst(); pStrings->IsCurrentValid() == true; pStrings->PickNext())
			printf("  %s\n", pStrings->GetCurrent());

		printf("\n");
		printf("Try 'hlmaster --help' for more options.\n");

		return 1;
	}



#ifdef PFC_SYSTEM_TYPE_WINDOWS
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		fprintf(stderr, "Error: Required version 2 of Winsock not available\n");
		return 1;
	}

#elif defined PFC_SYSTEM_TYPE_UNIX
	// check for group switching
	if(pArgs->IsEnabled('g') == true)
	{
		struct group *grp_entry;

		grp_entry = getgrnam(pArgs->GetValue('g'));
		if(grp_entry != NULL)
		{
			if(setgid(grp_entry->gr_gid) != 0)
			{
				fprintf(stderr, "Error: Cannot run as group \"%s\". Permission denied.\n", pArgs->GetValue('g'));
				return 1;
			}

			if(getgid() != grp_entry->gr_gid)
			{
				fprintf(stderr, "Error: Cannot run as group \"%s\". Setgid failed.\n", pArgs->GetValue('g'));
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "Error: Cannot run as group \"%s\". Group does not exist.\n", pArgs->GetValue('g'));
			return 1;
		}
	}

	// check for user switching
	if(pArgs->IsEnabled('u') == true)
	{
		struct passwd *pwd_entry;

		pwd_entry = getpwnam(pArgs->GetValue('u'));
		if(pwd_entry != NULL)
		{
			if(setuid(pwd_entry->pw_uid) != 0)
			{
				fprintf(stderr, "Error: Cannot run as user \"%s\". Permission denied.\n", pArgs->GetValue('u'));
				return 1;
			}

			if(getuid() != pwd_entry->pw_uid)
			{
				fprintf(stderr, "Error: Cannot run as user \"%s\". Setuid failed.\n", pArgs->GetValue('u'));
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "Error: Cannot run as user \"%s\". User does not exist.\n", pArgs->GetValue('u'));
			return 1;
		}
	}

	// check for daemon mode
	if(pArgs->IsEnabled('d') == true)
		g_bIsDaemon = false;
	else
		g_bIsDaemon = true;

	if(g_bIsDaemon == true)
	{
		int nDaemonStatus = 0;
		const char *pChar;

		// check filenames for absolute paths
		if(pArgs->IsEnabled('I') == true)
		{
			pChar = pArgs->GetValue('I');

			if(pChar[0] != '/')
			{
				fprintf(stderr, "Error: Path to info-log does not look absolute <%s>\n", pChar);
				fprintf(stderr, "Absolute paths are required in daemon mode (eg. /var/log/hlmaster/info.log)\n");
				return 1;
			}
		}
		if(pArgs->IsEnabled('A') == true)
		{
			pChar = pArgs->GetValue('A');

			if(pChar[0] != '/')
			{
				fprintf(stderr, "Error: Path to access-log does not look absolute <%s>\n", pChar);
				fprintf(stderr, "Absolute paths are required in daemon mode (eg. /var/log/hlmaster/access.log)\n");
				return 1;
			}
		}
		if(pArgs->IsEnabled('P') == true)
		{
			pChar = pArgs->GetValue('P');

			if(pChar[0] != '/')
			{
				fprintf(stderr, "Error: Path to pid-file does not look absolute <%s>\n", pChar);
				fprintf(stderr, "Absolute paths are required in daemon mode (eg. /var/run/hlmaster.pid)\n");
				return 1;
			}
		}


		// enter daemon mode
		nDaemonStatus = daemon_start();
		if(nDaemonStatus != 0)
		{
			if(nDaemonStatus == 1)
			{
				fprintf(stderr, "Error: Could not fork to daemon.\n");
				return 1;
			}
			else if(nDaemonStatus == 2)
			{
				// parent, exit
				return 0;
			}
		}
		// now we are in the child process
	}

	if(pArgs->IsEnabled('P') == true)
	{
		pidfile_create(pArgs->GetValue('P'));
	}

	if(sig_install_handlers() != true)
	{
		if(g_bIsDaemon != true)
			fprintf(stderr, "Error: Could not install signal handlers.\n");

		return 1;
	}
	sig_block_all_init();
	sig_block_reload_init();

#else
#error Unknown operating system. Only WIN32 / LINUX are recognized.
#endif	// PFC_SYSTEM_TYPE_UNIX



	// check for info_log
	if(pArgs->IsEnabled('I') == true)
	{
		try
		{
			g_pInfoLog = new PLogfile(pArgs->GetValue('I'), atoi(pArgs->GetValue('n')));
		}
		catch(PFileException ex)
		{
			if(g_bIsDaemon != true)
				fprintf(stderr, "Error: Could not open logfile <%s>\n", pArgs->GetValue('I'));

			ex.GetValue();
			return 1;
		}

		g_pInfoLog->WriteLine("Log started", PLogfile::c_LEVEL_INFO);
	}


	// check for access_log
	if(pArgs->IsEnabled('A') == true)
	{

		#if defined PFC_SYSTEM_TYPE_WINDOWS
		if(g_pInfoLog != NULL && (PString::EqualsIgnoreCase(g_pInfoLog->GetFilename(), pArgs->GetValue('A')) == true))
		#elif defined PFC_SYSTEM_TYPE_UNIX
		if(g_pInfoLog != NULL && (PString::Equals(g_pInfoLog->GetFilename(), pArgs->GetValue('A')) == true))
		#endif
		{
			// access && info log are equal
			g_pAccessLog = g_pInfoLog;
		}
		else
		{
			try
			{
				g_pAccessLog = new PLogfile(pArgs->GetValue('A'), PLogfile::c_LEVEL_INFO);
			}
			catch(PFileException ex)
			{
				if(g_bIsDaemon != true)
					fprintf(stderr, "Error: Could not open logfile <%s>\n", pArgs->GetValue('A'));

				ex.GetValue();
				return 1;
			}
		}
	}



	try
	{
		g_pMaster = new HLMaster(pArgs->GetValue('a'), atoi(pArgs->GetValue('p')), atoi(pArgs->GetValue('m')), g_pInfoLog, g_pAccessLog);
	}
	catch(PSocketException ex)
	{
		if(g_bIsDaemon != true)
			fprintf(stderr, "Error: Could not bind to network interface/port <%s:%s>\n", pArgs->GetValue('a'), pArgs->GetValue('p'));

		if(g_pInfoLog != NULL && g_pInfoLog->Begin(PLogfile::c_LEVEL_CRITICAL) == true)
		{
			g_pInfoLog->Append("Could not bind to network interface/port <");
			g_pInfoLog->Append(pArgs->GetValue('a'));
			g_pInfoLog->Append(':');
			g_pInfoLog->Append(pArgs->GetValue('p'));
			g_pInfoLog->Append('>');

			g_pInfoLog->End();
		}

		ex.GetValue();
		return 1;
	}

	// set servername
	if(pArgs->IsEnabled('s') == true)
	{
		pfc::CStringBuffer *pNameBuffer = new pfc::CStringBuffer(64);

		pNameBuffer->Append(DEFAULT_SERVERNAME);
		pNameBuffer->Append(" - ");
		pNameBuffer->Append(pArgs->GetValue('s'));

		g_pMaster->SetServerName(pNameBuffer->GetCharPointer());
		delete pNameBuffer;
	}
	else
		g_pMaster->SetServerName(DEFAULT_SERVERNAME);



	// check for closed mode
	g_bIsPrivate = pArgs->IsEnabled('c');
	if(g_bIsPrivate == true)
	{
		// yep, private
		g_pMaster->EnableHostFilter();
		HLServerFilterList *pFilter = g_pMaster->GetHostFilter();

		pFilter->SetOrderDeny();
		pFilter->SetDefaultDeny();

		if(g_pInfoLog != NULL)
			g_pInfoLog->WriteLine("Closed mode enabled", PLogfile::c_LEVEL_INFO);
	}
	else if(pArgs->IsEnabled('f') == true)
	{
		// load banlist
		g_pMaster->LoadHostFilter(pArgs->GetValue('f'));
	}


	// load hostlist
	if(pArgs->IsEnabled('l') == true)
	{
		g_pMaster->LoadHosts(pArgs->GetValue('l'));
	}
	else if(g_bIsPrivate == true)
	{
		// cannot be in private mode without
		// having a host-list. we would have nothing to do!
		if(g_bIsDaemon != true)
			fprintf(stderr, "Error: Cannot enable closed mode without loading a list of gameservers (--server-list)\n");

		if(g_pInfoLog != NULL)
			g_pInfoLog->WriteLine("Cannot enable closed mode without loading a list of gameservers (--server-list)", PLogfile::c_LEVEL_ERROR);

		return 1;
	}



	// we do not need the argument list anymore
	delete pArgs;
	pArgs = NULL;


	// run the server
	g_bIsRunning = true;
	while(true)
	{
		if(g_pMaster->WaitForInput() == true)
		{
			if(g_pMaster->ReadInput() != true)
				continue;

			// check for reload
			#if defined PFC_SYSTEM_TYPE_UNIX
			sig_block_reload_install();
			#endif

			if(g_bRequestReload == true)
			{
				if(g_bIsDaemon != true)
					printf("Configuration reload requested by signal.\n");

				if(g_pInfoLog != NULL)
					g_pInfoLog->WriteLine("Configuration reload requested by signal.", PLogfile::c_LEVEL_INFO);

				g_pMaster->ReloadHostFilter();
				g_pMaster->ReloadHosts();

				g_bRequestReload = false;
			}

			// process request
			g_pMaster->ProcessInput();

			#if defined PFC_SYSTEM_TYPE_UNIX
			sig_block_reload_restore();
			#endif
		}
	}
	g_bIsRunning = false;



	// destroy master-server
	delete g_pMaster;


	// close logfiles
	if(g_pAccessLog != NULL)
	{
		if(g_pAccessLog != g_pInfoLog)
			delete g_pAccessLog;

		g_pAccessLog = NULL;
	}
	if(g_pInfoLog != NULL)
	{
		delete g_pInfoLog;
		g_pInfoLog = NULL;
	}

	// cleanup
	#if defined PFC_SYSTEM_TYPE_WINDOWS
	WSACleanup();

	#elif defined PFC_SYSTEM_TYPE_UNIX
	pidfile_remove();

	sig_restore_handlers();
	#endif

	// success
	return 0;
}




// Begin unix specific
// ======================================================================
#ifdef PFC_SYSTEM_TYPE_UNIX

int daemon_start()
{
	pid_t pidDaemon = 0;

	// fork and exit parent
	pidDaemon = fork();

	if(pidDaemon < 0)
	{
		// fork error
		return 1;
	}
	else if(pidDaemon == 0)
	{
		// child process
		// do nothing
	}
	else
	{
		// parent
		// exit so we get a new daddy (PID 1 -> init)
		return 2;
	}
	
	
	// get session leader
	setsid();

	// change working dir to root
	chdir("/");

	// unset umask
	umask(0);


	// close all open filehandles
	close(fileno(stdin));
	close(fileno(stdout));
	close(fileno(stderr));

	// success
	return 0;
}


// ======================================================================

bool pidfile_create(const char *pszFilename)
{
	// copy filename

	if(g_pPidFile == NULL)
		g_pPidFile = new PString(PFC_MAX_PATH, pszFilename);
	else
	{
		// pidfile allready exists
		return false;
	}


	mode_t mode_prev;
	// TODO: errorhandling

	FILE * fpPidfile = NULL;

	// set umask (-rw-r--r--)
	mode_prev = umask(S_IWGRP | S_IWOTH);

	fpPidfile = fopen(g_pPidFile->GetValue(), "w");

	// reset umask
	umask(mode_prev);

	if(fpPidfile != NULL)
	{
		// write our process id to file
		fprintf(fpPidfile, "%d\n", getpid());

		fclose(fpPidfile);
	}
	else
	{
		// error
		return 1;
	}

	// success
	return 0;
}


// ======================================================================

bool pidfile_remove()
{
	// TODO: errorhandling
	if(g_pPidFile == NULL)
		return false;

	if(unlink(g_pPidFile->GetValue()) == 0)
	{
		// success
		delete g_pPidFile;
		g_pPidFile = NULL;

		return 0;
	}
	else
	{
		// error
		return 1;
	}
}

// ======================================================================

bool sig_install_handlers()
{
	struct sigaction new_action;

	new_action.sa_handler = sig_handle_HUP;
	new_action.sa_flags   = 0;
	new_action.sa_flags  |= SA_RESTART;

	sigemptyset(&new_action.sa_mask);
	sigaddset(&new_action.sa_mask, SIGHUP);
	sigaddset(&new_action.sa_mask, SIGUSR1);
	sigaddset(&new_action.sa_mask, SIGUSR2);

	if(sigaction(SIGHUP, &new_action, NULL) < 0)
		return false;
	if(sigaction(SIGUSR1, &new_action, NULL) < 0)
		return false;
	if(sigaction(SIGUSR2, &new_action, NULL) < 0)
		return false;


	new_action.sa_handler = sig_handle_TERM;
	new_action.sa_flags   = 0;

	sigemptyset(&new_action.sa_mask);
	sigaddset(&new_action.sa_mask, SIGTERM);
	sigaddset(&new_action.sa_mask, SIGQUIT);
	sigaddset(&new_action.sa_mask, SIGINT);

	if(sigaction(SIGTERM, &new_action, NULL) < 0)
		return false;
	if(sigaction(SIGQUIT, &new_action, NULL) < 0)
		return false;
	if(sigaction(SIGINT, &new_action, NULL) < 0)
		return false;

	return true;
}


// ======================================================================

bool sig_restore_handlers()
{
	if(signal(SIGHUP, SIG_DFL) == SIG_ERR)
		return false;
	if(signal(SIGUSR1, SIG_DFL) == SIG_ERR)
		return false;
	if(signal(SIGUSR2, SIG_DFL) == SIG_ERR)
		return false;
	if(signal(SIGTERM, SIG_DFL) == SIG_ERR)
		return false;
	if(signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		return false;
	if(signal(SIGINT, SIG_DFL) == SIG_ERR)
		return false;

	return true;
}


// ======================================================================

void sig_block_all_init()
{
	// initialize signal mask
	sigemptyset(&g_sig_blockmask_all);
	sigemptyset(&g_sig_blockmask_original);

	sigaddset(&g_sig_blockmask_all, SIGTERM);
	sigaddset(&g_sig_blockmask_all, SIGQUIT);
	sigaddset(&g_sig_blockmask_all, SIGINT);

	sigaddset(&g_sig_blockmask_all, SIGHUP);
	sigaddset(&g_sig_blockmask_all, SIGUSR1);
	sigaddset(&g_sig_blockmask_all, SIGUSR2);
}


// ======================================================================

void sig_block_reload_init()
{
	// initialize signal mask
	sigemptyset(&g_sig_blockmask_reload);
	sigemptyset(&g_sig_blockmask_original);

	sigaddset(&g_sig_blockmask_reload, SIGHUP);
	sigaddset(&g_sig_blockmask_reload, SIGUSR1);
	sigaddset(&g_sig_blockmask_reload, SIGUSR2);
}


// ======================================================================

inline bool sig_block_all_install()
{
	sigemptyset(&g_sig_blockmask_original);
	
	// install signal block mask
	if(sigprocmask(SIG_BLOCK, &g_sig_blockmask_all, &g_sig_blockmask_original) < 0)
	{
		// error
		return false;
	}

	// success
	return true;
}


// ======================================================================

inline bool sig_block_reload_install()
{
	sigemptyset(&g_sig_blockmask_original);
	
	// install signal block mask
	if(sigprocmask(SIG_BLOCK, &g_sig_blockmask_reload, &g_sig_blockmask_original) < 0)
	{
		// error
		return false;
	}

	// success
	return true;
}


// ======================================================================

inline bool sig_block_all_restore()
{
	if(sigprocmask(SIG_SETMASK, &g_sig_blockmask_original, NULL) < 0)
		return false;
	else
		return true;
}


// ======================================================================

inline bool sig_block_reload_restore()
{
	if(sigprocmask(SIG_SETMASK, &g_sig_blockmask_original, NULL) < 0)
		return false;
	else
		return true;
}


// ======================================================================

static void sig_handle_HUP(int nSignal)
{
	if(g_bIsRunning != true)
		return;

	if(nSignal == SIGHUP || nSignal == SIGUSR1)
	{
		// request reloading of banlist, hostlist
		g_bRequestReload = true;
	}

	if(nSignal == SIGHUP || nSignal == SIGUSR2)
	{
		// reload logfiles
		if(g_pInfoLog != NULL)
			g_pInfoLog->ReOpen();

		if(g_pAccessLog != NULL)
			g_pAccessLog->ReOpen();
	}
}


// ======================================================================

static void sig_handle_TERM(int nSignal)
{
	if(g_bIsRunning == true && g_pInfoLog != NULL)
	{
		if(nSignal == SIGTERM)
			g_pInfoLog->WriteLine("Received termination signal (SIGTERM).", PLogfile::c_LEVEL_INFO);
		else if(nSignal == SIGQUIT)
			g_pInfoLog->WriteLine("Received termination signal (SIGQUIT).", PLogfile::c_LEVEL_INFO);
		else if(nSignal == SIGINT)
			g_pInfoLog->WriteLine("Received termination signal (SIGINT).", PLogfile::c_LEVEL_INFO);
		else
			g_pInfoLog->WriteLine("Received termination signal (unknown).", PLogfile::c_LEVEL_INFO);
	}

	if(g_bIsRunning == true)
	{
		// destroy master-server
		delete g_pMaster;

		// close logfiles
		if(g_pAccessLog != NULL)
		{
			if(g_pAccessLog != g_pInfoLog)
				delete g_pAccessLog;

			g_pAccessLog = NULL;
		}
		if(g_pInfoLog != NULL)
		{
			delete g_pInfoLog;
			g_pInfoLog = NULL;
		}

		// remove pidfile
		pidfile_remove();
	}

	exit(128 + nSignal);
}


// end linux specific
// ======================================================================
#endif



// ======================================================================

void print_usage()
{
	printf("HLMaster %d.%d.%d - A Half-Life master-server daemon\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
	printf("Usage: hlmaster [OPTION]...\n");
	printf("\n");

	printf("Startup:\n");
	printf("  -V, --version              display the version of hlmaster and exit.\n");
	printf("  -h, --help                 display this help.\n");
	printf("  -n, --log-level=LEVEL      set log level. (0-5, default 4)\n");
#if defined PFC_SYSTEM_TYPE_UNIX
	printf("  -d, --no-daemon            do not go to background after startup.\n");
	printf("  -u, --user=USER            run hlmaster as USER.\n");
	printf("  -g, --group=GROUP          run hlmaster as GROUP.\n");
#endif
	printf("\n");

	printf("Settings:\n");
	printf("  -a, --ip-address=ADDRESS   set ip address to listen on. (default 0.0.0.0)\n");
	printf("  -p, --port=PORT            set port to listen on. (default 27010)\n");
	printf("  -s, --master-name=NAME     set the master-server name.\n");
	printf("  -m, --max-servers=NUM      set maximum NUM of game-servers. (default 200)\n");
	printf("  -f, --server-filter=FILE   load server filter-list from FILE.\n");
	printf("  -l, --server-list=FILE     load game-servers from FILE.\n");
	printf("  -c, --closed               do not accept any heartbeats. (default off)\n");
	printf("\n");

	printf("Logging:\n");
#if defined PFC_SYSTEM_TYPE_UNIX
	printf("  -P, --pid-file=FILE        write process id (PID) to FILE.\n");
#endif
	printf("  -I, --info-log=FILE        log general information to FILE.\n");
	printf("  -A, --access-log=FILE      log access information to FILE.\n");

	printf("\n");
	printf("Mail bug-reports and suggestions to <bullet@users.sourceforge.net>.\n");
	printf("Check http://hlmaster.sourceforge.net for newest version.\n");
}


// ======================================================================

void print_version()
{
	printf("HLMaster version %d.%d.%d --- Copyright (c) 2001 Silvan Minghetti\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
	printf("\n");
	printf("This program is distributed in the hope that it will be useful,\n");
	printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	printf("GNU General Public License for more details.\n");
	printf("\n");
	printf("Written by Silvan Minghetti <bullet@users.sourceforge.net>.\n");
	printf("Check http://hlmaster.sourceforge.net for newest version.\n");
}

