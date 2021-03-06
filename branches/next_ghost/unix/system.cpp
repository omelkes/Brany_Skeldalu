/*
 *  This file is part of Skeldal project
 * 
 *  Skeldal is free software: you can redistribute 
 *  it and/or modify it under the terms of the GNU General Public 
 *  License as published by the Free Software Foundation, either 
 *  version 3 of the License, or (at your option) any later version.
 *
 *  OpenSkeldal is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Skeldal.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  --------------------
 *
 *  Project home: https://sourceforge.net/projects/skeldal/
 *  
 *  Last commit made by: $Id$
 */

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cassert>
#include <climits>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <inttypes.h>

#include "libs/system.h"
#include "game/globals.h"
#include "libs/memman.h"

char *strupr(char *str) {
	int i;
	for (i = 0; str[i]; i++) {
		str[i] = toupper(str[i]);
	}
	return str;
}

// FIXME: implement mouse support
void Mouse_MapWheel(char up, char down) {
//	assert(0);

}

// FIXME: Implement in GUI
void Sys_ErrorBox(const char *msg) {
	fprintf(stderr, "Error: %s", msg);
}

// FIXME: Implement in GUI
void Sys_WarnBox(const char *msg) {
	fprintf(stderr, "Warning: %s", msg);
}

// FIXME: Implement in GUI
void Sys_InfoBox(const char *msg) {
	fprintf(stderr, "Info: %s", msg);
}

void Sys_Shutdown(void) {
	Screen_Shutdown();
}

void Sys_SetEnv(const char *name, const char *value) {
	setenv(name, value, 1);
}

int Sys_LatestFile(const char *mask, int offset) {
	assert(0);
	return 0;
}

// FIXME: get rid of temps
void Sys_PurgeTemps(char all) {
	DIR *dir;
	struct dirent *ent;
	char *ptr, buf[PATH_MAX], *cat;

	strcpy(buf, Sys_FullPath(SR_TEMP, ""));
	cat = buf + strlen(buf);
	dir = opendir(buf);

	while ((ent = readdir(dir))) {
		ptr = strrchr(ent->d_name, '.');

		if (!ptr || strcasecmp(ptr, ".tmp")) {
			continue;
		}

		if ((ent->d_name[0] == '~') && !all) {
			continue;
		}

		strcpy(cat, ent->d_name);
		unlink(buf);
	}

	closedir(dir);
}

int pack_status_file(WriteStream&, const char*);

int Sys_PackStatus(WriteStream &stream) {
	DIR *dir;
	struct dirent *ent;
	char *ptr, buf[PATH_MAX], *cat;
	int ret;

	strcpy(buf, Sys_FullPath(SR_TEMP, ""));
	cat = buf + strlen(buf);
	dir = opendir(buf);

	while ((ent = readdir(dir))) {
		ptr = strrchr(ent->d_name, '.');

		if (!ptr || strcasecmp(ptr, ".tmp") || (ent->d_name[0] == '~')) {
			continue;
		}

		ret = pack_status_file(stream, ent->d_name);

		if (ret) {
			return ret;
		}
	}

	closedir(dir);

	memset(buf, 0, 12);
	stream.write(buf, 12);

	return 0;
}

int Sys_FileExists(const char *file) {
	return !access(file, F_OK);
}

// FIXME: rewrite
void ShareCPU(void) {
//	assert(0);
	Timer_Sleep(5);
}

int Task_Add(int stack, TaskerFunctionName func, ...) {
	assert(0);
	return 0;
}

void *Task_Sleep(void *data) {
	assert(0);
	return NULL;
}

char Task_IsMaster(void) {
	assert(0);
	return 0;
}

int Task_Count(void) {
	assert(0);
	return 0;
}

void Task_Term(int id) {
	assert(0);

}

char Task_IsRunning(int id) {
	assert(0);
	return 0;
}

char Task_QuitMsg() {
	assert(0);
	return 0;
}

void Task_Shutdown(int id) {
	assert(0);

}

// FIXME: implement game launcher
char OtevriUvodniOkno() {
	return 1;
}

char SelectAdventure() {
	return 0;
}

char *GetSelectedAdventure() {
	assert(0);
	return NULL;
}

