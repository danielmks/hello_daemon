# hello_daemon

- gcc를 사용하여 컴파일
```bash
gcc -o hello_daemon hello_daemon.c
```
- 실행 파일을 적절한 디렉토리(예: `/usr/local/bin/`)로 이동시킵니다.
- 명령어로 권한을 확인한 후, 실행 권한이 없으면
```bash
sudo chmod +x /usr/local/bin/hello_daemon
```

- 서비스 파일을 /etc/systemd/system/hello_daemon.service에 저장한 후, 아래 명령어로 데몬을 등록하고 실행할 수 있습니다.
```bash
sudo systemctl daemon-reload
sudo systemctl enable hello_daemon
sudo systemctl start hello_daemon
```
- log 파일 디렉터리 권한 지정
```bash
sudo mkdir -p /var/log/daemon_service
sudo chown nobody:nobody /var/log/daemon_service
sudo chmod 755 /var/log/daemon_service
```
