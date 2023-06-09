#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

int main(int argc, char** argv)
{
    CURL* curl;
    CURLcode res;
    FILE* fp;
    long filesize;
    char* buffer;
    char* api_key = "Mz0TPUdO4P5=B=PQbdv=dBXsQoY="; // OneNet平台的API key
    char* device_id = "1026301742"; // 设备ID
    char url[256];

    // 打开待上传的jpg文件
    fp = fopen("test_photo.jpg", "rb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }

    // 获取文件大小并分配缓冲区
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    rewind(fp);
    buffer = (char*) malloc(sizeof(char) * filesize);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(fp);
        return 1;
    }

    // 读取文件数据到缓冲区
    if (fread(buffer, 1, filesize, fp) != filesize) {
        fprintf(stderr, "Failed to read file\n");
        fclose(fp);
        free(buffer);
        return 1;
    }

    // 关闭文件
    fclose(fp);

    // 将图片数据转换为二进制数据
    char* binary_data = (char*) malloc(sizeof(char) * filesize * 2 + 1);
    for (int i = 0; i < filesize; i++) {
        sprintf(&binary_data[i * 2], "%02x", (unsigned char) buffer[i]);
    }

    // 构造JSON数据
    char* json_template = "{\"datastreams\":[{\"id\":\"photo\",\"datapoints\":[{\"value\":\"%s\"}]}]}";
    char* json_data = (char*) malloc(sizeof(char) * (strlen(json_template) + strlen(binary_data) + 1));
    sprintf(json_data, json_template, binary_data);

    // 初始化curl库
    curl = curl_easy_init();
    if (curl) {
        // 构造HTTP请求的URL
        sprintf(url, "http://api.heclouds.com/devices/%s/datapoints?type=3", device_id);

        // 设置HTTP请求的头信息和数据
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charsets: utf-8");
        char api_key_header[256];
        sprintf(api_key_header, "api-key: %s", api_key);
        headers = curl_slist_append(headers, api_key_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(json_data));

        // 设置HTTP请求的URL和方法
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // 执行HTTP请求并获取响应
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to upload data: %s\n", curl_easy_strerror(res));
        }

        // 释放资源
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // 释放缓冲区
    free(binary_data);
    free(json_data);

    return 0;
}
