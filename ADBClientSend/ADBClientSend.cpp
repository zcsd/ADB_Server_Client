#include <winsock2.h>    
#include <stdio.h>    
#include<iostream>
#include<conio.h>    

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
       
#pragma  comment(lib,"ws2_32.lib")    
#define SERVER_IP "192.168.0.4"
#define SERVER_PORT 9000

SOCKET initSocket();
int sendFrame(SOCKET, cv::Mat);
void receiver(SOCKET);

SOCKET initSocket(){
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	
	SOCKET sclient= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	return sclient;
}

int sendFrame(SOCKET socket_id, cv::Mat srcFrame) {

	int imgSize = 640 * 480 * 3;
	int bytes = 0;
	//cv::imwrite("FRA.jpg", frame);
	//printf("width is %d, height is %d\n", frame.cols, frame.rows);
	//printf("image size: %d\n",frame.total()*frame.elemSize());
	//printf("channels:%d\n", frame.channels());
	srcFrame = (srcFrame.reshape(0, 1));
	//char* buf = (char*)malloc(imgSize);
	//memcpy(buf, frame.data, imgSize);
	const char* data0 = (const  char *)srcFrame.data;
	if ((bytes = send(socket_id, data0, imgSize, 0)) < 0) {
		printf("Send Fail\n");
		return 0;
	}
	//data0 = NULL;
	//free(buf);
	//const char * sendData = "1111111 from Client!\n";
	//send(sclient, sendData, strlen(sendData), 0);
	//printf("aaa is %s\n", data0);
	if (bytes != imgSize) {
		printf("Connection Closed\n");
		closesocket(socket_id);
		return 0;
	}
	//printf("Sent Finished!\n");
	return 1;
}

void receiver(SOCKET socket_id) {
	char recData[255];
	int ret = recv(socket_id, recData, 255, 0);
	if (ret > 0)
	{
		recData[ret] = 0x00;
		printf(recData);
	}
}

int main(int argc, char* argv[])
{
	SOCKET socket_id = initSocket();

	int input;
	printf("Please input your choice:");
	scanf("%d", &input);

	cv::VideoCapture capWebcam(0);           
	capWebcam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capWebcam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//capWebcam.set(CV_CAP_PROP_FPS, 2);
	if (capWebcam.isOpened() == false) {                               
		printf("error: capWebcam not accessed successfully\n");      
		_getch();                                                           
		return(0);                                                          
	}

	cv::Mat frame;   
	cv::namedWindow("frameWnd", CV_WINDOW_NORMAL);
	char charCheckForEscKey = 0;
	int count=0;
	while (charCheckForEscKey != 27 && capWebcam.isOpened()) {
		bool blnFrameReadSuccessfully = capWebcam.read(frame);            

		if (!blnFrameReadSuccessfully || frame.empty()) {                 
			printf("error: frame not read from webcam\n");                 
			break;                                                             
		}
  
		cv::imshow("frameWnd", frame);

		count = count + 1;
		if (count % 3 == 0) {
			int sendFlag = sendFrame(socket_id, frame);
			if (sendFlag != 1) {
				printf("Send Failure.\n");
				return 0;
			}
		}
		charCheckForEscKey = cv::waitKey(100);
	}

	printf("Ready to receive\n");

	receiver(socket_id);

	closesocket(socket_id);
	WSACleanup();
	return 0;
}