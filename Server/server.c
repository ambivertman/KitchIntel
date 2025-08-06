#include<header.h>
#define MAX_SOCKET 100
#define MAX_LINE_LENGTH 20
void save_data(const char* buf);
void send_data(int net_fd);
void get_last_line(const char *filename, char *last_line);
void delete_data(void);
void get_config(char *ip, char *port);
int get_parameter(const char *key, char *value);

typedef struct {
    int fd;
    time_t last_time;
}msg;

int main(void) {
    char ip[20] = {0};
    char port[10] = {0};
    get_config(ip,port);
    printf("%s\n%s",ip,port);
    //创建socket对象
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //实现如果端口被time_wait占用也可以重新使用该端口
    int reuse = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    //初始化sockaddr结构体
    struct sockaddr_in socketAddr;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.s_addr = inet_addr(ip);
    socketAddr.sin_port = htons(atoi(port));

    //绑定端口IP
    bind(socket_fd, (struct sockaddr *)&socketAddr, sizeof(socketAddr));

    //监听端口, socket_fd, 队列长度
    listen(socket_fd, 10);

    //用于监听服务用户的socket
    msg list[MAX_SOCKET];
    memset(list, 0, sizeof(list));
    int size = 0; //当前socket数量

    //创建监听集合并初始化
    //base_set用于记录下一次循环需要监听的设备
    fd_set base_set;
    FD_ZERO(&base_set);
    FD_SET(socket_fd, &base_set);

    while (1) {
        //temp_set用于记录当前循环需要监听的设备
        int net_fd = 0;
        fd_set temp_set;
        memcpy(&temp_set, &base_set, sizeof(base_set));
        //调用select()对设备进行监听
        //轮询的数量/监听集合/
        //利用select检测是否超时

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        select(MAX_SOCKET, &temp_set, NULL, NULL, &tv);

        //判断是哪个设备就绪
        //如果是socket_fd说明有新链接进入
        if (FD_ISSET(socket_fd, &temp_set)) {
            net_fd = accept(socket_fd, NULL, NULL);
            FD_SET(net_fd, &base_set);
            list[size].fd = net_fd;
            time(&list[size].last_time);
            size++;
            printf("new tcp connection\n");
        }
        //遍历fds查看哪些连接可读,进行处理
        for (int i = 0;i < size;i++) {
            net_fd = list[i].fd;
            if (FD_ISSET(net_fd, &temp_set)) {
                char buf[1024] = { 0 };
                //接收客户端发来的消息
                ssize_t ret = recv(net_fd, buf, sizeof(buf), 0);

                //如果返回值为0,说明该客户端断开连接
                //需要关闭对于该客户端的监听
                if (ret == 0) {
                    FD_CLR(net_fd, &base_set);
                    close(net_fd);
                    //调整剩下的socket对象在fds中的位置
                    msg temp = list[size - 1];
                    list[i] = temp;
                    memset(&list[size - 1], 0, sizeof(msg));
                    //index=i处的fd被删除,被最后一个替换,所以
                    //i号需要再访问一次
                    size--;
                    i--;
                }

                printf("%s",buf);
                time(&list[i].last_time);
                //根据HTTP头部进行区分响应
                //如果是POST则进行存储
                if(strstr(buf,"POST")!=NULL){
                    save_data(buf);                    
                    char * HTTP_Header = "HTTP/1.0 200 OK \n\n";
                    send(net_fd,HTTP_Header,strlen(HTTP_Header),0);
                }
                //如果是GET则进行发送
                if(strstr(buf,"GET /1")!=NULL){
                    send_data(net_fd);
                }
                if(strstr(buf,"GET /2")!=NULL){
                    delete_data();
                }
            }
        }
        //检测超时
        for(int i = 0;i < size;i++) {
            time_t now_time;
            time(&now_time);
            if (now_time - list[i].last_time > 120) {
                net_fd = list[i].fd;
                FD_CLR(net_fd, &base_set);
                close(net_fd);
                //调整剩下的socket对象在fds中的位置
                msg temp = list[size - 1];
                list[i] = temp;
                memset(&list[size - 1], 0, sizeof(msg));
                //index=i处的fd被删除,被最后一个替换,所以
                //i号需要再访问一次
                size--;
                i--;
            }
        }
    }
    close(socket_fd);
    return 0;
}

void save_data(const char * buf){
    printf("writing data\n");
    //用于数据的本地化存储
    int Light_fd = open("./data/light.txt",O_RDWR|O_CREAT|O_APPEND,0666);
    int Tmp_fd = open("./data/temprature.txt",O_RDWR|O_CREAT|O_APPEND,0666);
    char newline = '\n';
    // --- 步骤一：提取正文部分 (与之前的回答相同) ---
    const char *delimiter = "\n\n";
    char *body_start = strstr(buf, delimiter);

    if (body_start == NULL) {
        printf("未找到正文内容。\n");
        return;
    }

    // 将指针移动到正文内容的起始位置
    body_start += strlen(delimiter);

    // 分割多行正文
    // 因为 strtok 会修改字符串，先创建一个副本
    char *body_copy = strdup(body_start);
    if (body_copy == NULL) {
        fprintf(stderr, "内存分配失败\n");
        return;
    }

    // 定义分隔符
    const char *line_delimiters = "\n";

    // 首次调用 strtok
    char *line = strtok(body_copy, line_delimiters);

    int line_number = 1;
    // 循环获取每一行
    while (line != NULL) {
        // 忽略空行
        int len = strlen(line); 
        if (len > 0) {
            if(strstr(line,"Light")!=NULL){
                write(Light_fd,line,strlen(line));
                write(Light_fd,&newline,sizeof(newline));
                close(Light_fd);
            }
            if(strstr(line,"Tmp")!=NULL){
                write(Tmp_fd,line,strlen(line));
                write(Tmp_fd,&newline,sizeof(newline));
                close(Tmp_fd);
            }


        }

        // 后续调用 strtok，第一个参数传入 NULL
        line = strtok(NULL, line_delimiters);
    }
    // 释放为副本分配的内存
    free(body_copy);
}


void send_data(int net_fd){
    char HTTP_buf[100] = {0};
    //用于数据的本地化存储
    char * ligth_file = "./data/light.txt";
    char * tmp_file = "./data/temprature.txt";

    char *HTTP_Header = "HTTP/1.0 200 OK\n\n";
    char light_lline[MAX_LINE_LENGTH] = {0};
    char tmp_lline[MAX_LINE_LENGTH] = {0};

    get_last_line(ligth_file,light_lline);
    get_last_line(tmp_file,tmp_lline);

    sprintf(HTTP_buf,"%s{\"light\":\"%s\",\"tmp\":\"%s\"}",HTTP_Header,light_lline,tmp_lline);
    send(net_fd,HTTP_buf,strlen(HTTP_buf),0);
    close(net_fd);

}
void  get_last_line(const char *filename, char * last_line) {
    FILE *file = fopen(filename, "r"); // 打开文件
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }
    char current_line[MAX_LINE_LENGTH] = {0}; // 存储当前读取的行
    while (fgets(current_line, sizeof(current_line), file) != NULL) {
        strncpy(last_line, current_line, strlen(current_line)-1); // 将当前行复制到last_line
    }
    fclose(file); // 关闭文件
}

void delete_data(void){
    int Light_fd = open("./data/light.txt",O_RDWR|O_CREAT|O_APPEND,0666);
    int Tmp_fd = open("./data/temprature.txt",O_RDWR|O_CREAT|O_APPEND,0666);

    ftruncate(Light_fd, 0);
    ftruncate(Tmp_fd, 0);

    close(Light_fd);
    close(Tmp_fd);
}
void get_config(char *ip, char *port){
    get_parameter("ip",ip);
    get_parameter("port",port);
    
}

int get_parameter(const char *key, char *value){
    FILE * file = fopen("./config/config.ini", "r");
    while(1){
        char line[100];
        bzero(line, sizeof(line));
        // 读一行数据
        char *res = fgets(line, sizeof(line), file);
        if(res == NULL){
            char buf[] = "没有要找的内容 \n";
            memcpy(value, buf, strlen(buf));
            return -1;
        }
        // 处理数据
        char *line_key = strtok(line, "=");
        if(strcmp(key, line_key) == 0){
            // 要找的内容
            char *line_value = strtok(NULL, "=");
            memcpy(value, line_value, strlen(line_value));
            return 0;
        }
    }
}
