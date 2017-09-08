//OHRRPGCE COMMON - Generic Unix versions of OS-specific routines
//Please read LICENSE.txt for GNU GPL License details and disclaimer of liability

#ifndef OS_H
#define OS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "common.h"

#ifdef _WIN32

typedef int IPCChannel;  //dummy types
#define NULL_CHANNEL 0
typedef void *ProcessHandle;

#else

struct PipeState;
typedef struct PipeState PipeState;
typedef PipeState *IPCChannel;
#define NULL_CHANNEL NULL
struct ProcessInfo {
        boolint waitable;
        FILE *file;
        int pid;
};
typedef struct ProcessInfo *ProcessHandle;

#endif

void init_runtime();

int memory_usage();
FBSTRING *memory_usage_string();

int copy_file_replacing(const char *source, const char *destination);

typedef enum {
	fileTypeNonexistent, // Doesn't exist
	fileTypeFile,        // Regular file or a symlink to one
	fileTypeDirectory,   // Directory (or mount point) or a symlink to one
	fileTypeOther,       // A device, fifo, or other special file type
	fileTypeError,       // Something unreadable (including broken symlinks)
} FileTypeEnum;

FileTypeEnum get_file_type(FBSTRING *fname);

//Advisory locking (actually mandatory on Windows)
int lock_file_for_write(FILE *fh, int timeout_ms);
int lock_file_for_read(FILE *fh, int timeout_ms);
void unlock_file(FILE *fh);
int test_locked(const char *filename, int writable);


//FBSTRING *channel_pick_name(const char *id, const char *tempdir, const char *rpg);
int channel_open_client(IPCChannel *result, FBSTRING *name);
int channel_open_server(IPCChannel *result, FBSTRING *name);
void channel_close(IPCChannel *channelp);
int channel_wait_for_client_connection(IPCChannel *channel, int timeout_ms);
int channel_write(IPCChannel *channel, const char *buf, int buflen);
int channel_write_string(IPCChannel *channel, FBSTRING *input);
int channel_input_line(IPCChannel *channel, FBSTRING *output);

ProcessHandle open_process (FBSTRING *program, FBSTRING *args, boolint waitable, boolint graphical);
ProcessHandle open_piped_process (FBSTRING *program, FBSTRING *args, IPCChannel *iopipe);
// run_process_and_get_output is Unix only
int run_process_and_get_output(FBSTRING *program, FBSTRING *args, FBSTRING *output);
ProcessHandle open_console_process (FBSTRING *program, FBSTRING *args);
boolint process_running (ProcessHandle process, int *exitcode);
void kill_process (ProcessHandle process);
void cleanup_process (ProcessHandle *processp);
int get_process_id();

void os_get_screen_size(int *wide, int *high);

#ifdef __cplusplus
}
#endif

#endif