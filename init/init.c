#include <stdio.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <unistd.h>

#define MOUNT_POINT "/root"

static void print_meminfo() {
    printf("\n--- System Memory Info (/proc/meminfo) ---\n");
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp))
            printf("%s", line);

        fclose(fp);
        printf("------------------------------------------\n\n");
    }
}


int main()
{
    printf("Mounting rootfs\n");
    mount("proc", "/proc", "proc", 0, NULL);
    // noatime: reads must not dirty inodes, every SD write risks the SDIO hang
    while (mount("/dev/mmcblk0p1", MOUNT_POINT, "ext2", MS_NOATIME, 0));
    print_meminfo();
    char *argv[] = { "sh", NULL };
    char *envp[] = {
        "PATH=" MOUNT_POINT "/bin",
        "HOME=" MOUNT_POINT,
        "TSLIB_CALIBFILE=" MOUNT_POINT "/etc/pointercal",
        "TSLIB_CONFFILE=" MOUNT_POINT "/etc/ts.conf",
        "TSLIB_TSDEVICE=/dev/input/event0",
        "TSLIB_PLUGINDIR=/usr/lib/ts",
        "TSLIB_CONSOLEDEVICE=none",
        NULL
    };
    execve(MOUNT_POINT "/bin/sh", argv, envp);
}
