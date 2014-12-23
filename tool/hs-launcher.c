/*
 * Copyright (c) 2014, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <errno.h>
#include <aul/aul.h>
#include <bundle.h>

int main(int argc, char *argv[])
{
    bundle *b  = NULL;
    int retval = -1;

    if (argc != 6) {
        fprintf(stderr,
                "Incorrect amount of parameters.\n"
                "Usage example: %s appid user running_apps_path default_apps_path flag_file_path\n", argv[0]);
        return EINVAL;
    }

    b = bundle_create();
    if (!b) {
        fputs("Failed to create bundle\n", stderr);
        return ENOMEM;
    }

    fprintf(stdout,
            "Adding the following parameters to the homescreen launch "
            "bundle: %s , %s , %s , %s\n",
            argv[2], argv[3], argv[4], argv[5]);

    bundle_add(b, "HS_PARAM_U",   argv[2]);
    bundle_add(b, "HS_PARAM_D",   argv[3]);
    bundle_add(b, "HS_PARAM_DD",  argv[4]);
    bundle_add(b, "HS_PARAM_FLG", argv[5]);

    fprintf(stdout,
            "Launching appid %s with the created bundle\n", argv[1]);

    retval = aul_launch_app(argv[1], b);

    bundle_free(b);

    if (retval < 0) {
        fprintf(stderr,
                "Launching homescreen with appid %s failed with errcode %d\n",
                argv[1], retval);
        return 1;
    } else {
        fprintf(stdout,
                "Launching homescreen with appid %s succeeded with PID %d\n",
                argv[1], retval);
        return 0;
    }
}
