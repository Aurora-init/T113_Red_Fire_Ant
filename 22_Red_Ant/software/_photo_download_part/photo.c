#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h> // 需要添加jansson库来解析JSON

struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*) userp;
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

int main(void)
{
    CURL* curl;
    CURLcode res;
    FILE* fp;
    char* api_key = "Mz0TPUdO4P5=B=PQbdv=dBXsQoY="; // OneNet平台的API key
    char* device_id = "1026301742"; // 设备ID
    char url[256];
    char* hex_data;

    // 初始化curl库
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // 构造HTTP请求的URL
        sprintf(url, "http://api.heclouds.com/devices/%s/datapoints?datastream_id=datastreams&limit=100&page=10&per_page=100", device_id);

        // 设置HTTP请求的头信息
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charsets: utf-8");
        char api_key_header[256];
        sprintf(api_key_header, "api-key: %s", api_key);
        headers = curl_slist_append(headers, api_key_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // 设置HTTP请求的URL和方法
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        // 设置HTTP请求的回调函数，将响应数据存储在字符串中
        struct MemoryStruct chunk;
        chunk.memory = malloc(2);
        chunk.size = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        // 执行HTTP请求并获取响应
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to download data: %s\n", curl_easy_strerror(res));
        } else {
            // 解析JSON响应以获取数据点数量
            json_error_t error;
            json_t* root = json_loads(chunk.memory, 0, &error);

			//printf("Received response: %s\n", chunk.memory); 
			
            json_t* data_points = json_object_get(root, "data");
			json_t* datastreams = json_object_get(data_points, "datastreams");

			json_t* datapoints = json_array_get(datastreams, 0);
			json_t* datapointss = json_object_get(datapoints, "datapoints");

			size_t num_data_points = json_array_size(datapointss);	//获取这个包含所有数据流的数组的长度								

			for (size_t i = 0; i < num_data_points; i++)
			{
				json_t* value = json_array_get(datapointss, i);
				json_t* values = json_object_get(value, "value");

				json_t* datapointsss = json_array_get(values, 0);
				json_t* datapointssss = json_object_get(datapointsss, "datapoints");
				json_t* valuess = json_array_get(datapointssss, 0);

				// 获取16进制字符串并转换为二进制数据
				hex_data = (char*)json_string_value(json_object_get(valuess, "value"));
				
				int hex_len = strlen(hex_data);
				unsigned char* binary_data = (unsigned char*) malloc(sizeof(unsigned char) * (hex_len / 2 + 1));
				for (int j = 0; j < hex_len; j += 2) {
					sscanf(&hex_data[j], "%02hhx", &binary_data[j / 2]);
				}
				binary_data[hex_len / 2] = '\0';

				// 将二进制数据写入文件
				char filename[64];
				sprintf(filename, "downloaded_photo_%zu.jpg", i);
				fp = fopen(filename, "wb");
				if (fp == NULL) {
					fprintf(stderr, "Failed to open file\n");
				} else {
					fwrite(binary_data, sizeof(unsigned char), hex_len / 2, fp);
					fclose(fp);
				}

				// 释放缓冲区
				free(binary_data);

			}
			// 释放JSON对象
	        json_decref(root);
        }

        // 释放资源
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        if (chunk.memory) {
            free(chunk.memory);
        }
    }
    curl_global_cleanup();

    return 0;
}
