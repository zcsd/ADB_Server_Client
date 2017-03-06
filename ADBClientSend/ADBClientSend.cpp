#include <WINSOCK2.H>    
#include <STDIO.H>    
#include "windows.h"
#include <iostream>
#include <conio.h>    

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
       
#pragma  comment(lib,"ws2_32.lib")    
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000

int main(int argc, char* argv[])
{
	cv::Mat frame;
	frame = cv::imread("123.jpg");

	if (frame.empty()) {
		printf("Load Image Error\n");
		return 0;
	}

	int imgSize = frame.total()*frame.elemSize();
	int bytes = 0;

	printf("width is %d, height is %d.\n", frame.cols, frame.rows);
	printf("Image size is %d.\n", imgSize);

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		return 0;
	}

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(SERVER_PORT);
	serAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	int res = connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr));
	if (res == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		return 0;
	}
	printf("connect res is %d\n", res);
	
	int input;
	printf("Please input your choice:");
	scanf("%d", &input);

	frame = (frame.reshape(0, 1));

	const char* data0 = (const char *)frame.data;
	if ((bytes = send(sclient, data0, imgSize, 0)) < 0) {
		printf("Send Fail\n");
		return 0;
	}

	if (bytes != imgSize) {
		printf("Connection Closed\n");
		closesocket(sclient);
		return 0;
	}
	printf("Sent Alraeady\n");
/*
	bool isContinue = true;
	while (isContinue) {
		int choice;
		printf("Please Input your Choice: ");
		scanf("%d", &choice);
		if (choice == 0) {
			isContinue = false;
		}
		else if (choice == 1) {
			const char * sendData = "1111111 from Client!\n";
			send(sclient, sendData, strlen(sendData), 0);
		}
		else if (choice == 2) {
			const char * sendData = "2222222 from Client!\n";
			send(sclient, sendData, strlen(sendData), 0);
		}
	}
*/
	char recData[255];
	int ret = recv(sclient, recData, 255, 0);
	if (ret > 0)
	{
		recData[ret] = 0x00;
		printf(recData);
	}
	closesocket(sclient);
	WSACleanup();
	return 0;
}