/**
 * dmcrypt-get-device.c
 * (c) 2005 Canonical Ltd.
 * Author: Martin Pitt <martin.pitt@ubuntu.com>
 * License: GNU General Public License (http://www.gnu.org/copyleft/gpl.html)
 *
 * Given a dmcrypt device name, this program queries the underlying physical
 * device, prints "major:minor" to the standard output and exits with 0. If an
 * error occurs or the device is not a dmcrypt mapped one, nothing is printed
 * and the program exits with 1.
 *
 * Opening /dev/mapper/control requires root privileges, therefore this
 * program needs to be installed setuid root. Root privileges are dropped
 * immediately after querying the information from the device mapper. The
 * parsing is done with normal user privileges afterwards.
 */

#include <libdevmapper.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int
main (int argc, char** argv)
{
    struct dm_task *dmt = NULL;
    struct dm_info dmi;
    char *target_type = NULL, *params = NULL;
    uint64_t start, length;
    void *next = NULL;
    struct stat st;
    unsigned major, minor;

    if (argc != 2) {
        fputs ("Usage: dmcrypt-get-device <device>\n", stderr);
        return 1;
    }

    /* Check that argument has the form /dev/mapper/<name> */
    const char* devmapdir = "/dev/mapper/";
    const int devmapdirlen = strlen (devmapdir);
    if (strncmp (argv[1], devmapdir, devmapdirlen))
        return 1;
    if (strchr (argv[1] + devmapdirlen, '/'))
        return 1;

    /* Check that argument is a block device */
    if (stat (argv[1], &st) || !S_ISBLK(st.st_mode))
        return 1;

    /* Request device info */
    if (!(dmt = dm_task_create(DM_DEVICE_TABLE)))
        return 1;
    if (!dm_task_set_name(dmt, argv[1]))
        return 1;
    if (!dm_task_run(dmt))
        return 1;

    /* Drop all privileges */
    setgid(getgid());
    setuid(getuid());

    if (!dm_task_get_info(dmt, &dmi))
        return 1;

    /* Get underlying physical device */
    next = dm_get_next_target(dmt, next, &start, &length,
            &target_type, &params);

    /* verify validity and that it is a dmcrypt device */
    if (!target_type || strcmp(target_type, "crypt")  || next)
        return 1;

    /* params has the format: cipher key offset major:minor <unknown> */
    length = sscanf(params, "%*s %*s %*s %u:%u %*s", &major, &minor);
    if (length != 2)
        return 1;

    /* Success */
    printf ("%u:%u\n", major, minor);
    
    return 0;
}
