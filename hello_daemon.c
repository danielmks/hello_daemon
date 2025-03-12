/*
 * 파일명: hello_daemon.c
 * 설명: 데몬화하여 매 시간 "hello"와 현재 시간을 로그 파일에 기록하는 간단한 서비스 프로그램.
 *       로그 파일명은 "daemon_service_log_YYYYMMDD.log" 형식으로 생성되며,
 *       지정한 보관 기간(예제에서는 7일) 이후에는 자동으로 삭제됩니다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#define PROCESS_NAME "daemon_service"    // 프로세스명
#define LOG_RETENTION_DAYS 7               // 로그 보관 기간 (예: 7일)

// 데몬화 함수: 프로세스를 백그라운드 데몬으로 전환
void daemonize() {
    pid_t pid, sid;
    
    // 첫 번째 fork
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS); // 부모 프로세스 종료

    // 새로운 세션 생성
    sid = setsid();
    if (sid < 0)
        exit(EXIT_FAILURE);

    // 두 번째 fork
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // 파일 모드 마스크 초기화
    umask(0);

    // 작업 디렉토리 변경 (루트 디렉토리)
    if (chdir("/") < 0)
        exit(EXIT_FAILURE);

    // 표준 입출력 닫기
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

// 현재 날짜를 이용하여 로그 파일명을 생성하고 "hello"와 시간 기록
void write_log() {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char filename[256];

    // 로그 파일명: "프로세스명_log_YYYYMMDD.log"
    snprintf(filename, sizeof(filename), "%s_log_%04d%02d%02d.log",
             PROCESS_NAME, tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);

    // 로그 파일 열기 (append 모드)
    FILE *fp = fopen(filename, "a");
    if(fp == NULL) {
        return;
    }

    // 현재 시각 문자열 (HH:MM:SS 형식)
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);

    // 로그 메시지 기록
    fprintf(fp, "hello - %s\n", timeStr);
    fclose(fp);
}

// 지정된 보관 기간보다 오래된 로그 파일 삭제 함수
void delete_old_logs() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if(d) {
        while((dir = readdir(d)) != NULL) {
            // 파일명이 "_log_"와 ".log"를 포함하면 로그 파일로 간주
            if (strstr(dir->d_name, "_log_") != NULL && strstr(dir->d_name, ".log") != NULL) {
                struct stat file_stat;
                if (stat(dir->d_name, &file_stat) == 0) {
                    // 파일 수정 시간과 현재 시간의 차이를 초 단위로 계산
                    time_t now = time(NULL);
                    double diff = difftime(now, file_stat.st_mtime);
                    // 보관 기간 초과 시 파일 삭제
                    if(diff > LOG_RETENTION_DAYS * 24 * 3600) {
                        remove(dir->d_name);
                    }
                }
            }
        }
        closedir(d);
    }
}

int main() {
    // 데몬화: 백그라운드에서 실행되도록 전환
    daemonize();

    // 무한 루프: 매 시간마다 로그 기록 및 오래된 로그 삭제
    while(1) {
        write_log();
        delete_old_logs();
        sleep(3600);  // 3600초(1시간) 대기
    }

    return 0;
}
