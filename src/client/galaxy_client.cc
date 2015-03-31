// Copyright (c) 2015, Galaxy Authors. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: yanshiguang02@baidu.com

#include "sdk/galaxy.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>

void help() {
    fprintf(stderr, "./galaxy_client master_addr command(list/add/kill) args\n");
    fprintf(stderr, "./galaxy_client master_addr add task_raw cmd_line replicate_count\n");
    fprintf(stderr, "./galaxy_client master_addr list task_id\n");
    fprintf(stderr, "./galaxy_client master_addr kill task_id\n");
    return;
}

enum Command {
    LIST = 0,
    LISTJOB,
    LISTNODE,
    ADD,
    KILLTASK,
    KILLJOB
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        help();
        return -1;
    }
    int COMMAND = 0;
    if (strcmp(argv[2], "add") == 0) {
        COMMAND = ADD;
        if (argc < 6) {
            help();
            return -1;
        }
    } else if (strcmp(argv[2], "list") == 0) {
        COMMAND = LIST;
    } else if (strcmp(argv[2], "listjob") == 0) {
        COMMAND = LISTJOB;
    } else if (strcmp(argv[2], "listnode") == 0) {
        COMMAND = LISTNODE;
    } else if (strcmp(argv[2], "kill") == 0) {
        COMMAND = KILLTASK;
        if (argc < 4) {
            help();
            return -1;
        }    
    } else if (strcmp(argv[2], "killjob") == 0) {
        COMMAND = KILLJOB;
        if (argc < 4) {
            help();
            return -1;
        }
    } else {
        help();
        return -1;
    }

    if (COMMAND == ADD) {
        std::string task_raw;
        if (!boost::starts_with(argv[3], "ftp://")) {
            FILE* fp = fopen(argv[3], "r");
            if (fp == NULL) {
                fprintf(stderr, "Open %s for read fail\n", argv[3]);
                return -2;
            }
            char buf[1024];
            int len = 0;
            while ((len = fread(buf, 1, 1024, fp)) > 0) {
                task_raw.append(buf, len);
            }
            fclose(fp);
            printf("Task binary len %lu\n", task_raw.size());
        }
        else {
            task_raw = argv[3];
        }
        galaxy::Galaxy* galaxy = galaxy::Galaxy::ConnectGalaxy(argv[1]);
        galaxy::JobDescription job;
        galaxy::PackageDescription pkg;
        pkg.source = task_raw;
        job.pkg = pkg;
        job.cmd_line = argv[4];
        job.replicate_count = atoi(argv[5]);
        job.job_name = argv[3];
        galaxy->NewJob(job);
    } else if (COMMAND == LIST) {
        int64_t job_id = -1;
        if (argc == 4) {
            job_id = atoi(argv[3]);
        }

        galaxy::Galaxy* galaxy = galaxy::Galaxy::ConnectGalaxy(argv[1]);
        galaxy->ListTask(job_id,NULL);
    } else if (COMMAND == LISTNODE) {
        galaxy::Galaxy* galaxy = galaxy::Galaxy::ConnectGalaxy(argv[1]);
        std::vector<galaxy::NodeDescription> nodes;
        galaxy->ListNode(&nodes);
        std::vector<galaxy::NodeDescription>::iterator it = nodes.begin();
        fprintf(stdout, "================================\n");
        for(; it != nodes.end(); ++it){
            fprintf(stdout, "%ld\t%s\tTASK:%d\tCPU:%d\tMEM:%dGB\n",
                    it->node_id, it->addr.c_str(),
                    it->task_num, it->cpu_share, it->mem_share);
        }
    } else if (COMMAND == LISTJOB) {
        galaxy::Galaxy* galaxy = galaxy::Galaxy::ConnectGalaxy(argv[1]);
        std::vector<galaxy::JobInstanceDescription> jobs;
        galaxy->ListJob(&jobs);
        std::vector<galaxy::JobInstanceDescription>::iterator it = jobs.begin();
        fprintf(stdout, "================================\n");
        for(;it != jobs.end();++it){
            fprintf(stdout, "%ld\t%s\t%d\t%d\n",
                    it->job_id, it->job_name.c_str(),
                    it->running_task_num, it->replicate_count);
        }
    } else if (COMMAND == KILLTASK) {
        int64_t task_id = atoi(argv[3]);
        galaxy::Galaxy* galaxy = galaxy::Galaxy::ConnectGalaxy(argv[1]);
        galaxy->KillTask(task_id);
    } else if (COMMAND == KILLJOB) {
        int64_t job_id = atoi(argv[3]);
        galaxy::Galaxy* galaxy = galaxy::Galaxy::ConnectGalaxy(argv[1]);
        galaxy->TerminateJob(job_id);
    }
    return 0;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
