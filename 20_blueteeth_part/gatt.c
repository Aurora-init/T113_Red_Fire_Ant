// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2015  Intel Corporation. All rights reserved.
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include "lib/bluetooth.h"
#include "lib/l2cap.h"
#include "lib/uuid.h"
#include "src/shared/mainloop.h"
#include "src/shared/util.h"
#include "src/shared/queue.h"
#include "src/shared/att.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-server.h"
#include "src/shared/gatt-client.h"
#include "peripheral/gatt.h"

#define ATT_CID 4

#define UUID_GAP 0x1800

struct gatt_conn {
	struct bt_att *att;
	struct bt_gatt_server *gatt;
	struct bt_gatt_client *client;
};

static int att_fd = -1;
static struct queue *conn_list = NULL;
static struct gatt_db *gatt_db = NULL;
static struct gatt_db *gatt_cache = NULL;

static uint8_t static_addr[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t dev_name[20];
static uint8_t dev_name_len = 0;

void gatt_set_static_address(uint8_t addr[6])
{
	memcpy(static_addr, addr, sizeof(static_addr));
}

void gatt_set_device_name(uint8_t name[20], uint8_t len)
{
	memcpy(dev_name, name, sizeof(dev_name));
	dev_name_len = len;
}

static void gatt_conn_destroy(void *data)
{
	struct gatt_conn *conn = data;

	bt_gatt_client_unref(conn->client);
	bt_gatt_server_unref(conn->gatt);
	bt_att_unref(conn->att);

	free(conn);
}

static void gatt_conn_disconnect(int err, void *user_data)
{
	struct gatt_conn *conn = user_data;

	printf("Device disconnected: %s\n", strerror(err));

	queue_remove(conn_list, conn);
	gatt_conn_destroy(conn);
}

static void client_ready_callback(bool success, uint8_t att_ecode,
							void *user_data)
{
	printf("GATT client discovery complete\n");
}

static void client_service_changed_callback(uint16_t start_handle,
						uint16_t end_handle,
						void *user_data)
{
	printf("GATT client service changed notification\n");
}

static struct gatt_conn *gatt_conn_new(int fd)
{
	struct gatt_conn *conn;
	uint16_t mtu = 0;

	conn = new0(struct gatt_conn, 1);
	if (!conn)
		return NULL;

	conn->att = bt_att_new(fd, false);
	if (!conn->att) {
		fprintf(stderr, "Failed to initialze ATT transport layer\n");
		free(conn);
		return NULL;
	}

	bt_att_set_close_on_unref(conn->att, true);
	bt_att_register_disconnect(conn->att, gatt_conn_disconnect, conn, NULL);

	//bt_att_set_security(conn->att, BT_SECURITY_MEDIUM);

	conn->gatt = bt_gatt_server_new(gatt_db, conn->att, mtu, 0);
	if (!conn->gatt) {
		fprintf(stderr, "Failed to create GATT server\n");
		bt_att_unref(conn->att);
		free(conn);
		return NULL;
	}

	conn->client = bt_gatt_client_new(gatt_cache, conn->att, mtu, 0);
	if (!conn->gatt) {
		fprintf(stderr, "Failed to create GATT client\n");
		bt_gatt_server_unref(conn->gatt);
		bt_att_unref(conn->att);
		free(conn);
		return NULL;
	}

	bt_gatt_client_ready_register(conn->client, client_ready_callback,
								conn, NULL);
	bt_gatt_client_set_service_changed(conn->client,
				client_service_changed_callback, conn, NULL);

	return conn;
}

static void att_conn_callback(int fd, uint32_t events, void *user_data)
{
	struct gatt_conn *conn;
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	int new_fd;

	if (events & (EPOLLERR | EPOLLHUP)) {
		mainloop_remove_fd(fd);
		return;
	}

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	new_fd = accept(att_fd, (struct sockaddr *) &addr, &addrlen);
	if (new_fd < 0) {
		fprintf(stderr, "Failed to accept new ATT connection: %m\n");
		return;
	}

	conn = gatt_conn_new(new_fd);
	if (!conn) {
		fprintf(stderr, "Failed to create GATT connection\n");
		close(new_fd);
		return;
	}

	if (!queue_push_tail(conn_list, conn)) {
		fprintf(stderr, "Failed to add GATT connection\n");
		gatt_conn_destroy(conn);
		close(new_fd);
	}

	printf("New device connected\n");
}

static void gap_device_name_read(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	uint8_t error;
	const uint8_t *value;
	size_t len;

	if (offset > dev_name_len) {
		error = BT_ATT_ERROR_INVALID_OFFSET;
		value = NULL;
		len = dev_name_len;
	} else {
		error = 0;
		len = dev_name_len - offset;
		value = len ? &dev_name[offset] : NULL;
	}

	gatt_db_attribute_read_result(attrib, id, error, value, len);
}

static void gap_device_data_read(struct gatt_db_attribute *attrib,
								 unsigned int id, uint16_t offset,
								 uint8_t opcode, struct bt_att *att,
								 void *user_data)
{
	printf("执行读取函数 gap_device_data_read\n");
	uint8_t error = 0;
	const uint8_t *value;
	//这里这个check_internet()是自定义的函数，如果联网则返回0，断网则返回1，我没实现，先注释掉，统一返回0
	int ret = 0;
	if (0 == ret)
	{
		printf("CONNECTED\n");
		value = ((const uint8_t *)"connect");
	}
	else
	{
		printf("DISCONNECTED\n");
		value = ((const uint8_t *)"disconnect");
	}

	size_t len = strlen((const char *)value);
	printf("len = %d\n", len);

	gatt_db_attribute_read_result(attrib, id, error, value, len);
}

//#define BUFFER_SIZE 256
//
//static char buffer[BUFFER_SIZE];
//static int buffer_len = 0;
//
//static void gap_user_write_service(struct gatt_db_attribute *attrib,
//                                   unsigned int id,
//                                   uint16_t offset,
//                                   const uint8_t *value,
//                                   size_t len,
//                                   uint8_t opcode,
//                                   struct bt_att *att,
//                                   void *user_data)
//{
//    if (len != 0)
//    {
//        // 将收到的数据存储到缓冲区中
//        if (buffer_len + len < BUFFER_SIZE) {
//            memcpy(buffer + buffer_len, value, len);
//            buffer_len += len;
//        } else {
//            printf("缓冲区已满，无法存储更多数据\n");
//        }
//        
//        // 解析缓冲区中的数据
//        while (buffer_len >= sizeof(int)) 
//		{
//            int msg_len = *(int*)buffer;
//            if (buffer_len >= msg_len + sizeof(int)) 
//			{
//                // 从缓冲区中截取消息，并将消息存储到 value 变量中
//                char msg[msg_len + 1];
//                memcpy(msg, buffer + sizeof(int), msg_len);
//                msg[msg_len] = '\0';
//                printf("收到长度：%d\n", msg_len);
//                printf("收到内容：%s\n", msg);
//                
//                // 将收到的WiFi名称和WiFi密码写入文件
//                FILE *fp = fopen("/root/wifi.txt", "w");
//                if (fp == NULL) 
//				{
//                    printf("无法打开文件\n");
//                    gatt_db_attribute_write_result(attrib, id, 1); // 写入失败
//                    return;
//                }
//                fprintf(fp, "%s\n", msg);
//                fclose(fp);
//                
//                // 从缓冲区中移除已经处理的数据
//                memmove(buffer, buffer + sizeof(int) + msg_len, buffer_len - sizeof(int) - msg_len);
//                buffer_len -= sizeof(int) + msg_len;
//
//                gatt_db_attribute_write_result(attrib, id, 0); // 写入成功
//            }
//			else
//			{
//                break;
//            }
//        }
//    }
//    else
//	{
//        printf("收到null\n");
//        gatt_db_attribute_write_result(attrib, id, 0);
//    }
//}



static void gap_user_write_service(struct gatt_db_attribute *attrib,
                                   							   unsigned int id,
                                   							   uint16_t offset,
                                   						  const uint8_t *value,
                                   									size_t len,
                                   								uint8_t opcode,
                                   							struct bt_att *att,
                                   							   void *user_data)
{
	char msg[len + 1];
	
    if (len != 0)
    {
    	memcpy(msg, value, len);
		msg[len] = '\0';
        printf("收到长度：%d\n", len);
        printf("收到内容: %s\n", msg);
        
        // 将收到的WiFi名称和WiFi密码写入文件
        FILE *fp = fopen("/root/wifi.txt", "w");
        if (fp == NULL) {
            printf("无法打开文件\n");
            gatt_db_attribute_write_result(attrib, id, 1); // 写入失败
            return;
        }
        fprintf(fp, "%s\n", msg);
        fclose(fp);

        gatt_db_attribute_write_result(attrib, id, 0); // 写入成功
        return;
    }
    else
        printf("收到null\n");

    gatt_db_attribute_write_result(attrib, id, 0);
}


static void populate_gap_service(struct gatt_db *db)
{
	struct gatt_db_attribute *service;
	bt_uuid_t uuid;

	bt_uuid16_create(&uuid, UUID_GAP);
	service = gatt_db_add_service(db, &uuid, true, 6);

	bt_uuid16_create(&uuid, GATT_CHARAC_DEVICE_NAME);
	gatt_db_service_add_characteristic(service, &uuid,
					BT_ATT_PERM_READ,
					BT_GATT_CHRC_PROP_READ,
					gap_device_name_read, NULL, NULL);

	gatt_db_service_set_active(service, true);
}

static void populate_my_service(struct gatt_db *db)
{
	printf("创建服务\n");
	struct gatt_db_attribute *service;
	bt_uuid_t uuid;

	//------------------------这里定义的uuid为 0xffff-------------------------------
	bt_uuid16_create(&uuid, 0xffff);
	service = gatt_db_add_service(db, &uuid, true, 18);
	
	//--------------------定义一个【读】的服务，uuid为0xaaaa-------------------------
	bt_uuid16_create(&uuid, 0xaaaa);
	gatt_db_service_add_characteristic(service, &uuid,
									   BT_ATT_PERM_READ,
									   BT_GATT_CHRC_PROP_READ,
									   gap_device_data_read, NULL, "qqq");
	//--------------------定义一个【写】的服务，uuid为0xbbbb-------------------------
	bt_uuid16_create(&uuid, 0xbbbb);
	gatt_db_service_add_characteristic(service, &uuid,
									   BT_ATT_PERM_WRITE,
									   BT_GATT_CHRC_PROP_WRITE,
									   NULL, gap_user_write_service, NULL);
	//--------------------------------------------------------------------------------
	gatt_db_service_set_active(service, true);
}


static void populate_devinfo_service(struct gatt_db *db)
{
	struct gatt_db_attribute *service;
	bt_uuid_t uuid;

	bt_uuid16_create(&uuid, 0x180a);
	service = gatt_db_add_service(db, &uuid, true, 17);

	gatt_db_service_set_active(service, true);
}

void gatt_server_start(void)
{
	struct sockaddr_l2 addr;
	char rec_data[1024];
	
	if (att_fd >= 0)
		return;

	att_fd = socket(PF_BLUETOOTH, SOCK_SEQPACKET | SOCK_CLOEXEC,
							BTPROTO_L2CAP);
	if (att_fd < 0) {
		fprintf(stderr, "Failed to create ATT server socket: %m\n");
		return;
	}

	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_cid = htobs(ATT_CID);
	memcpy(&addr.l2_bdaddr, static_addr, 6);
	addr.l2_bdaddr_type = BDADDR_LE_RANDOM;

	if (bind(att_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Failed to bind ATT server socket: %m\n");
		close(att_fd);
		att_fd = -1;
		return;
	}

	if (listen(att_fd, 1) < 0) {
		fprintf(stderr, "Failed to listen on ATT server socket: %m\n");
		close(att_fd);
		att_fd = -1;
		return;
	}

	gatt_db = gatt_db_new();
	if (!gatt_db) {
		close(att_fd);
		att_fd = -1;
		return;
	}

	// 清空
	memset(rec_data, 0, 1024);
	// 提供设备名称读取服务
	populate_gap_service(gatt_db);
	// 提供设备信息服务
	populate_devinfo_service(gatt_db);
	// 刚才创建的服务
	populate_my_service(gatt_db);
	gatt_cache = gatt_db_new();

	conn_list = queue_new();
	if (!conn_list) {
		gatt_db_unref(gatt_db);
		gatt_db = NULL;
		close(att_fd);
		att_fd = -1;
		return;
	}

	mainloop_add_fd(att_fd, EPOLLIN, att_conn_callback, NULL, NULL);
}

void gatt_server_stop(void)
{
	if (att_fd < 0)
		return;

	mainloop_remove_fd(att_fd);

	queue_destroy(conn_list, gatt_conn_destroy);

	gatt_db_unref(gatt_cache);
	gatt_cache = NULL;

	gatt_db_unref(gatt_db);
	gatt_db = NULL;

	close(att_fd);
	att_fd = -1;
}
