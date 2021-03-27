/////*
//// * @File  demo_socket.c
//// * @Brief socket(TCP) communication with block IO example
//// *
//// * @Author: Neucrack
//// * @Date: 2018-06-12 18:04:08
//// * @Last Modified by: Neucrack
//// * @Last Modified time: 2018-06-12 18:05:00
//// */
//
//
//
//
//
//
//
//#include <string.h>
//#include <stdio.h>
//#include <api_os.h>
//#include <api_event.h>
//#include <api_socket.h>
//#include <api_network.h>
//
//#include <api_gps.h>
//#include <api_hal_uart.h>
//#include <api_debug.h>
//#include "buffer.h"
//#include "gps_parse.h"
//#include "math.h"
//#include "gps.h"
//
//
///*******************************************************************/
///////////////////////////socket configuration////////////////////////
//// (online tcp debug tool: http://tt.ai-thinker.com:8000/ttcloud)
//#define SERVER_IP   "api.thingspeak.com"
//#define SERVER_PORT 80
//#define SERVER_PATH "/update?api_key=SD5OBD49N5H4O8RY&field1="
///*******************************************************************/
//
//
//#define MAIN_TASK_STACK_SIZE    (2048 * 2)
//#define MAIN_TASK_PRIORITY      0
//#define MAIN_TASK_NAME          "Socket Test Task"
//
//#define TEST_TASK_STACK_SIZE    (2048 * 2)
//#define TEST_TASK_PRIORITY      1
//#define TEST_TASK_NAME          "Test Task"
//
//
//#define GPS_TASK_STACK_SIZE    (2048 * 2)
//#define GPS_TASK_PRIORITY      2
//#define GPS_TASK_NAME          "gps Task"
//
//
//
//static HANDLE socketTaskHandle = NULL;
//static HANDLE testTaskHandle = NULL;
//static HANDLE semStart = NULL;
//
//
//static HANDLE gpsTaskHandle = NULL;
//bool gps_flag = false;
//bool isGpsOn = true;
//
//float latitude_gps=0.0000;
//float longitude_gps=0.0000;
//
//bool done_init_flag=false;
//
//void EventDispatch(API_Event_t* pEvent)
//{
//	switch(pEvent->id)
//	{
//	case API_EVENT_ID_NO_SIMCARD:
//		Trace(10,"DEBUG !!NO SIM CARD%d!!!!",pEvent->param1);
//		break;
//
//	case API_EVENT_ID_NETWORK_REGISTER_SEARCHING:
//		Trace(2,"DEBUG  network register searching");
//		break;
//
//	case API_EVENT_ID_NETWORK_REGISTER_DENIED:
//		Trace(2,"DEBUG  network register denied");
//		break;
//
//	case API_EVENT_ID_NETWORK_REGISTER_NO:
//		Trace(2,"DEBUG  network register no");
//		break;
//
//	case API_EVENT_ID_NETWORK_REGISTERED_HOME:
//	case API_EVENT_ID_NETWORK_REGISTERED_ROAMING:
//	{
//		uint8_t status;
//		Trace(2,"DEBUG  network register success");
//		bool ret = Network_GetAttachStatus(&status);
//		if(!ret)
//			Trace(1,"DEBUG  get attach staus fail");
//		Trace(1,"DEBUG  attach status:%d",status);
//		if(status == 0)
//		{
//			ret = Network_StartAttach();
//			if(!ret)
//			{
//				Trace(1,"DEBUG  network attach fail");
//			}
//		}
//		else
//		{
//			Network_PDP_Context_t context = {
//					.apn        ="etisalat",
//					.userName   = ""    ,
//					.userPasswd = ""
//			};
//			Network_StartActive(context);
//		}
//
//		Trace(1,"DEBUG  gprs register complete");
//		gps_flag = true;
//		break;
//	}
//	case API_EVENT_ID_NETWORK_ATTACHED:
//		Trace(2,"DEBUG  network attach success");
//		Network_PDP_Context_t context = {
//				.apn        ="etisalat",
//				.userName   = ""    ,
//				.userPasswd = ""
//		};
//		Network_StartActive(context);
//		break;
//
//	case API_EVENT_ID_NETWORK_ACTIVATED:
//		Trace(2,"DEBUG  network activate success");
//		OS_ReleaseSemaphore(semStart);
//		break;
//
//	case API_EVENT_ID_GPS_UART_RECEIVED:
//		// Trace(1,"received GPS data,length:%d, data:%s,flag:%d",pEvent->param1,pEvent->pParam1,flag);
//		GPS_Update(pEvent->pParam1,pEvent->param1);
//		break;
//	case API_EVENT_ID_UART_RECEIVED:
//		if(pEvent->param1 == UART1)
//		{
//			uint8_t data[pEvent->param2+1];
//			data[pEvent->param2] = 0;
//			memcpy(data,pEvent->pParam1,pEvent->param2);
//			Trace(1,"DEBUG     uart received data,length:%d,data:%s",pEvent->param2,data);
//			if(strcmp(data,"close") == 0)
//			{
//				Trace(1,"DEBUG  close gps");
//				GPS_Close();
//				isGpsOn = false;
//			}
//			else if(strcmp(data,"open") == 0)
//			{
//				Trace(1,"DEBUG  open gps");
//				GPS_Open(NULL);
//				isGpsOn = true;
//			}
//		}
//		break;
//
//
//
//	default:
//		break;
//	}
//}
//
//
//void gps_testTask(void)
//{
//	Trace(1,"debug gps_testTask ");
//
//	GPS_Info_t* gpsInfo = Gps_GetInfo();
//	uint8_t buffer[300];
//
//
//	if(done_init_flag==false)
//	{
//
//	//wait for gprs register complete
//	//The process of GPRS registration network may cause the power supply voltage of GPS to drop,
//	//which resulting in GPS restart.
//	while(!gps_flag)
//	{
//		Trace(1,"DEBUG  wait for gprs regiter complete gps");
//		OS_Sleep(2000);
//	}
//
//	//open GPS hardware(UART2 open either)
//	GPS_Init();
//	GPS_Open(NULL);
//
//	//wait for gps start up, or gps will not response command
//	while(gpsInfo->rmc.latitude.value == 0)
//		OS_Sleep(1000);
//
//
//	// set gps nmea output interval
//	for(uint8_t i = 0;i<5;++i)
//	{
//		bool ret = GPS_SetOutputInterval(10000);
//		Trace(1,"DEBUG  set gps ret:%d",ret);
//		if(ret)
//			break;
//		OS_Sleep(1000);
//	}
//
//	// if(!GPS_ClearInfoInFlash())
//	//     Trace(1,"erase gps fail");
//
//	// if(!GPS_SetQzssOutput(false))
//	//     Trace(1,"enable qzss nmea output fail");
//
//	// if(!GPS_SetSearchMode(true,false,true,false))
//	//     Trace(1,"set search mode fail");
//
//	// if(!GPS_SetSBASEnable(true))
//	//     Trace(1,"enable sbas fail");
//
//	if(!GPS_GetVersion(buffer,150))
//		Trace(1,"DEBUG  get gps firmware version fail");
//	else
//		Trace(1,"DEBUG  gps firmware version:%s",buffer);
//
//	// if(!GPS_SetFixMode(GPS_FIX_MODE_LOW_SPEED))
//	// Trace(1,"set fix mode fail");
//
//	if(!GPS_SetOutputInterval(1000))
//		Trace(1,"DEBUG  set nmea output interval fail");
//
//	Trace(1,"DEBUG  init ok");
//
//	done_init_flag=true;
//
//}
//
//		if(isGpsOn)
//		{
//			//show fix info
//			uint8_t isFixed = gpsInfo->gsa[0].fix_type > gpsInfo->gsa[1].fix_type ?gpsInfo->gsa[0].fix_type:gpsInfo->gsa[1].fix_type;
//			char* isFixedStr;
//			if(isFixed == 2)
//				isFixedStr = "2D fix";
//			else if(isFixed == 3)
//			{
//				if(gpsInfo->gga.fix_quality == 1)
//					isFixedStr = "3D fix";
//				else if(gpsInfo->gga.fix_quality == 2)
//					isFixedStr = "3D/DGPS fix";
//			}
//			else
//				isFixedStr = "no fix";
//
//			//convert unit ddmm.mmmm to degree(°)
//			int temp = (int)(gpsInfo->rmc.latitude.value/gpsInfo->rmc.latitude.scale/100);
//			double latitude = temp+(double)(gpsInfo->rmc.latitude.value - temp*gpsInfo->rmc.latitude.scale*100)/gpsInfo->rmc.latitude.scale/60.0;
//			temp = (int)(gpsInfo->rmc.longitude.value/gpsInfo->rmc.longitude.scale/100);
//			double longitude = temp+(double)(gpsInfo->rmc.longitude.value - temp*gpsInfo->rmc.longitude.scale*100)/gpsInfo->rmc.longitude.scale/60.0;
//
//
//			//you can copy ` latitude,longitude ` to http://www.gpsspg.com/maps.htm check location on map
//
//			snprintf(buffer,sizeof(buffer),"GPS fix mode:%d, BDS fix mode:%d, fix quality:%d, satellites tracked:%d, gps sates total:%d, is fixed:%s, coordinate:WGS84, Latitude:%f, Longitude:%f, unit:degree,altitude:%f",gpsInfo->gsa[0].fix_type, gpsInfo->gsa[1].fix_type,
//					gpsInfo->gga.fix_quality,gpsInfo->gga.satellites_tracked, gpsInfo->gsv[0].total_sats, isFixedStr, latitude,longitude,gpsInfo->gga.altitude);
//
//
//
//			 latitude_gps=latitude;
//			 longitude_gps=longitude;
//
//
//			//show in tracer
//			Trace(2,buffer);
//			//send to UART1
//			UART_Write(UART1,buffer,strlen(buffer));
//			UART_Write(UART1,"\r\n\r\n",4);
//		}
//
////		OS_Sleep(5000);
//
//}
//
//
//
////http get with no header
//int Http_Get(const char* domain, int port,const char* path, char* retBuffer, int* bufferLen)
//{
//	Trace(1,"debug Http_Get ");
//
//	bool flag = false;
//	uint16_t recvLen = 0;
//	uint8_t ip[16];
//	char buffer2[1024];
//	int Buffer2Len = sizeof(buffer2);
//
//	int retBufferLen = *bufferLen;
//	//connect server
//	memset(ip,0,sizeof(ip));
//	if(DNS_GetHostByName2(domain,ip) != 0)
//	{
//		Trace(1,"DEBUG  get ip error");
//		return -1;
//	}
//	Trace(1,"DEBUG  get ip success:%s -> %s",domain,ip);
//	char* servInetAddr = ip;
//	//    snprintf(retBuffer,retBufferLen,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",path,domain);
//
//		gps_testTask();
//
//		//for test only
////		latitude_gps=29.9940961;
////		longitude_gps=31.1477996;
//
////	snprintf(retBuffer,retBufferLen,"GET %s%d\r\n HTTP/1.1\r\nHost: %s\r\n\r\n",path,latitude_gps,domain);
//
//	snprintf(buffer2,Buffer2Len,"GET /update?api_key=SD5OBD49N5H4O8RY&field6=%f&field5=%f\r\n HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n",latitude_gps,longitude_gps);
//	char* pData = buffer2;
////	char* pData         = "GET /update?api_key=SD5OBD49N5H4O8RY&field1=90/nHTTP/1.1\nHost: api.thingspeak.com";
//
//
//
//
//	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if(fd < 0){
//		Trace(1,"DEBUG  socket fail");
//		return -1;
//	}
//	Trace(1,"DEBUG  fd:%d",fd);
//
//	struct sockaddr_in sockaddr;
//	memset(&sockaddr,0,sizeof(sockaddr));
//	sockaddr.sin_family = AF_INET;
//	sockaddr.sin_port = htons(port);
//	inet_pton(AF_INET,servInetAddr,&sockaddr.sin_addr);
//
//	int ret = connect(fd, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in));
//	if(ret < 0){
//		Trace(1,"DEBUG  socket connect fail");
//		close(fd);
//		return -1;
//	}
//	Trace(1,"DEBUG  socket connect success");
//	Trace(1,"DEBUG  send request:%s",pData);
//	ret = send(fd, pData, strlen(pData), 0);
//	if(ret < 0){
//		Trace(1,"DEBUG  socket send fail");
//		close(fd);
//		return -1;
//	}
//	Trace(1,"DEBUG  socket send success");
//
//
//
//	struct fd_set fds;
//	struct timeval timeout={12,0};
//	FD_ZERO(&fds);
//	FD_SET(fd,&fds);
//	while(!flag)
//	{
//		ret = select(fd+1,&fds,NULL,NULL,&timeout);
//		switch(ret)
//		{
//		case -1:
//			Trace(1,"DEBUG  select error");
//			flag = true;
//			break;
//		case 0:
//			Trace(1,"DEBUG  select timeout");
//			flag = true;
//			break;
//		default:
//			if(FD_ISSET(fd,&fds))
//			{
//				Trace(1,"DEBUG  select return:%d",ret);
//				memset(retBuffer+recvLen,0,retBufferLen-recvLen);
//				ret = recv(fd,retBuffer+recvLen,retBufferLen-recvLen,0);
//				Trace(1,"DEBUG  ret:%d",ret);
//				recvLen += ret;
//				if(ret < 0)
//				{
//					Trace(1,"DEBUG  recv error");
//					flag = true;
//					break;
//				}
//				else if(ret == 0)
//				{
//					Trace(1,"DEBUG  ret == 0");
//					flag = true;
//					break;
//				}
//				else if(ret < 1352)
//				{
//					Trace(1,"DEBUG  recv len:%d,data:%s",recvLen,retBuffer);
//					*bufferLen = recvLen;
//					close(fd);
//					return recvLen;
//				}
//
//			}
//			break;
//		}
//	}
//
//
//
//	close(fd);
//	return -1;
//
//}
//
//
//void Socket_BIO_Test()
//{
//
//
//
//		Trace(1,"debug Socket_BIO_Test ");
//
//	char buffer[2048];
//	int len = sizeof(buffer);
//	//wait for gprs network connection ok
//	semStart = OS_CreateSemaphore(0);
//	OS_WaitForSemaphore(semStart,OS_TIME_OUT_WAIT_FOREVER);
//	OS_DeleteSemaphore(semStart);
//
//
//
//
//	while(1)
//		{
//
//		Http_Get(SERVER_IP,SERVER_PORT,SERVER_PATH,buffer,&len);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//		OS_Sleep(5000);
//	}
//
//	//perform http get
//	if(Http_Get(SERVER_IP,SERVER_PORT,SERVER_PATH,buffer,&len) < 0)
//	{
//		Trace(1,"DEBUG  http get fail");
//	}
//	else
//	{
//		//show response message though tracer(pay attention:tracer can not show all the word if the body too long)
//		Trace(1,"DEBUG  http get success,ret:%s",buffer);
//		char* index0 = strstr(buffer,"\r\n\r\n");
//		char temp = index0[4];
//		index0[4] = '\0';
//		Trace(1,"DEBUG  http response header:%s",buffer);
//		index0[4] = temp;
//		Trace(1,"DEBUG  http response body:%s",index0+4);
//	}
//
//
//
//
//}
//
//void test_MainTask(void* param)
//{
//	API_Event_t* event=NULL;
//
//	Socket_BIO_Test();
//	Trace(1,"debug test_MainTask ");
//
//	while(1)
//	{
//		if(OS_WaitEvent(socketTaskHandle, (void**)&event, OS_TIME_OUT_WAIT_FOREVER))
//		{
//			EventDispatch(event);
//			OS_Free(event->pParam1);
//			OS_Free(event->pParam2);
//			OS_Free(event);
//		}
//	}
//}
//
//
//void socket_MainTask(void *pData)
//{
//	API_Event_t* event=NULL;
//
//	testTaskHandle = OS_CreateTask(test_MainTask,
//			NULL, NULL, TEST_TASK_STACK_SIZE, TEST_TASK_PRIORITY, 0, 0, TEST_TASK_NAME);
//
//	//open UART1 to print NMEA infomation
//	UART_Config_t config = {
//			.baudRate = UART_BAUD_RATE_115200,
//			.dataBits = UART_DATA_BITS_8,
//			.stopBits = UART_STOP_BITS_1,
//			.parity   = UART_PARITY_NONE,
//			.rxCallback = NULL,
//			.useEvent   = true
//	};
//	UART_Init(UART1,config);
//
//	while(1)
//	{
//		if(OS_WaitEvent(socketTaskHandle, (void**)&event, OS_TIME_OUT_WAIT_FOREVER))
//		{
//			EventDispatch(event);
//			OS_Free(event->pParam1);
//			OS_Free(event->pParam2);
//			OS_Free(event);
//		}
//
//	}
//}
//
//void socket_Main()
//{
//	socketTaskHandle = OS_CreateTask(socket_MainTask,
//			NULL, NULL, MAIN_TASK_STACK_SIZE, MAIN_TASK_PRIORITY, 0, 0, MAIN_TASK_NAME);
//	OS_SetUserMainHandle(&socketTaskHandle);
//}









#include <string.h>
#include <stdio.h>
#include <api_os.h>
#include <api_event.h>
#include <api_socket.h>
#include <api_network.h>

#include <api_gps.h>
#include <api_hal_uart.h>
#include <api_debug.h>
#include "buffer.h"
#include "gps_parse.h"
#include "math.h"
#include "gps.h"


#include "stdbool.h"
#include "api_call.h"
#include "api_audio.h"


#define DIAL_NUMBER "01115948824"


/*******************************************************************/
/////////////////////////socket configuration////////////////////////
// (online tcp debug tool: http://tt.ai-thinker.com:8000/ttcloud)
#define SERVER_IP   "api.thingspeak.com"
#define SERVER_PORT 80
#define SERVER_PATH "/update?api_key=SD5OBD49N5H4O8RY&field1="
/*******************************************************************/


#define MAIN_TASK_STACK_SIZE    (2048 * 2)
#define MAIN_TASK_PRIORITY      0
#define MAIN_TASK_NAME          "Socket Test Task"

#define TEST_TASK_STACK_SIZE    (2048 * 2)
#define TEST_TASK_PRIORITY      1
#define TEST_TASK_NAME          "Test Task"


#define GPS_TASK_STACK_SIZE    (2048 * 2)
#define GPS_TASK_PRIORITY      2
#define GPS_TASK_NAME          "gps Task"

#define SECOND_TASK_STACK_SIZE    (2048 * 2)
#define SECOND_TASK_PRIORITY      3
#define SECOND_TASK_NAME          "Second Test Task"

static HANDLE socketTaskHandle = NULL;
static HANDLE testTaskHandle = NULL;
static HANDLE semStart = NULL;
static HANDLE secondTaskHandle = NULL;


static HANDLE gpsTaskHandle = NULL;
bool gps_flag = false;
bool isGpsOn = true;

float latitude_gps=0.0000;
float longitude_gps=0.0000;

bool done_init_flag=false;

uint8_t call_flag = 0;
bool isDialSuccess = false;


void EventDispatch(API_Event_t* pEvent)
{
	switch(pEvent->id)
	{
	case API_EVENT_ID_NO_SIMCARD:
		Trace(10,"DEBUG !!NO SIM CARD%d!!!!",pEvent->param1);
		break;

	case API_EVENT_ID_NETWORK_REGISTER_SEARCHING:
		Trace(2,"DEBUG  network register searching");
		break;

	case API_EVENT_ID_NETWORK_REGISTER_DENIED:
		Trace(2,"DEBUG  network register denied");
		break;

	case API_EVENT_ID_NETWORK_REGISTER_NO:
		Trace(2,"DEBUG  network register no");
		break;

	case API_EVENT_ID_NETWORK_REGISTERED_HOME:
	case API_EVENT_ID_NETWORK_REGISTERED_ROAMING:
	{
		uint8_t status;
		Trace(2,"DEBUG  network register success");
		bool ret = Network_GetAttachStatus(&status);
		if(!ret)
			Trace(1,"DEBUG  get attach staus fail");
		Trace(1,"DEBUG  attach status:%d",status);
		if(status == 0)
		{
			ret = Network_StartAttach();
			if(!ret)
			{
				Trace(1,"DEBUG  network attach fail");
			}
		}
		else
		{
			Network_PDP_Context_t context = {
					.apn        ="etisalat",
					.userName   = ""    ,
					.userPasswd = ""
			};
			Network_StartActive(context);
		}

		Trace(1,"DEBUG  gprs register complete");
		gps_flag = true;
		call_flag=1;
		break;
	}
	case API_EVENT_ID_NETWORK_ATTACHED:
		Trace(2,"DEBUG  network attach success");
		Network_PDP_Context_t context = {
				.apn        ="etisalat",
				.userName   = ""    ,
				.userPasswd = ""
		};
		Network_StartActive(context);
		break;

	case API_EVENT_ID_NETWORK_ACTIVATED:
		Trace(2,"DEBUG  network activate success");
		OS_ReleaseSemaphore(semStart);
		break;








	case API_EVENT_ID_CALL_DIAL://param1: isSuccess, param2:error code(CALL_Error_t)
		Trace(1,"Is dial success:%d, error code:%d",pEvent->param1,pEvent->param2);
		if(pEvent->param1)
			isDialSuccess = true;
		break;
	case API_EVENT_ID_CALL_HANGUP:  //param1: is remote release call, param2:error code(CALL_Error_t)
		Trace(1,"Hang up,is remote hang up:%d, error code:%d",pEvent->param1,pEvent->param2);
		break;
	case API_EVENT_ID_CALL_INCOMING:   //param1: number type, pParam1:number
		Trace(1,"Receive a call, number:%s, number type:%d",pEvent->pParam1,pEvent->param1);
		OS_Sleep(5000);
		if(!CALL_Answer())
			Trace(1,"answer fail");
		break;
	case API_EVENT_ID_CALL_ANSWER  :
		Trace(1,"answer success");
		break;
	case API_EVENT_ID_CALL_DTMF    :  //param1: key
		Trace(1,"received DTMF tone:%c",pEvent->param1);
		break;








	case API_EVENT_ID_GPS_UART_RECEIVED:
		// Trace(1,"received GPS data,length:%d, data:%s,flag:%d",pEvent->param1,pEvent->pParam1,flag);
		GPS_Update(pEvent->pParam1,pEvent->param1);
		break;
	case API_EVENT_ID_UART_RECEIVED:
		if(pEvent->param1 == UART1)
		{
			uint8_t data[pEvent->param2+1];
			data[pEvent->param2] = 0;
			memcpy(data,pEvent->pParam1,pEvent->param2);
			Trace(1,"DEBUG     uart received data,length:%d,data:%s",pEvent->param2,data);
			if(strcmp(data,"close") == 0)
			{
				Trace(1,"DEBUG  close gps");
				GPS_Close();
				isGpsOn = false;
			}
			else if(strcmp(data,"open") == 0)
			{
				Trace(1,"DEBUG  open gps");
				GPS_Open(NULL);
				isGpsOn = true;
			}
		}
		break;



	default:
		break;
	}
}


void gps_testTask(void)
{
	Trace(1,"debug gps_testTask ");

	GPS_Info_t* gpsInfo = Gps_GetInfo();
	uint8_t buffer[300];


	if(done_init_flag==false)
	{

		//wait for gprs register complete
		//The process of GPRS registration network may cause the power supply voltage of GPS to drop,
		//which resulting in GPS restart.
		while(!gps_flag)
		{
			Trace(1,"DEBUG  wait for gprs regiter complete gps");
			OS_Sleep(2000);
		}

		//open GPS hardware(UART2 open either)
		GPS_Init();
		GPS_Open(NULL);

		//wait for gps start up, or gps will not response command
		while(gpsInfo->rmc.latitude.value == 0)
			OS_Sleep(1000);


		// set gps nmea output interval
		for(uint8_t i = 0;i<5;++i)
		{
			bool ret = GPS_SetOutputInterval(10000);
			Trace(1,"DEBUG  set gps ret:%d",ret);
			if(ret)
				break;
			OS_Sleep(1000);
		}

		// if(!GPS_ClearInfoInFlash())
		//     Trace(1,"erase gps fail");

		// if(!GPS_SetQzssOutput(false))
		//     Trace(1,"enable qzss nmea output fail");

		// if(!GPS_SetSearchMode(true,false,true,false))
		//     Trace(1,"set search mode fail");

		// if(!GPS_SetSBASEnable(true))
		//     Trace(1,"enable sbas fail");

		if(!GPS_GetVersion(buffer,150))
			Trace(1,"DEBUG  get gps firmware version fail");
		else
			Trace(1,"DEBUG  gps firmware version:%s",buffer);

		// if(!GPS_SetFixMode(GPS_FIX_MODE_LOW_SPEED))
		// Trace(1,"set fix mode fail");

		if(!GPS_SetOutputInterval(1000))
			Trace(1,"DEBUG  set nmea output interval fail");

		Trace(1,"DEBUG  init ok");

		done_init_flag=true;

	}

	if(isGpsOn)
	{
		//show fix info
		uint8_t isFixed = gpsInfo->gsa[0].fix_type > gpsInfo->gsa[1].fix_type ?gpsInfo->gsa[0].fix_type:gpsInfo->gsa[1].fix_type;
		char* isFixedStr;
		if(isFixed == 2)
			isFixedStr = "2D fix";
		else if(isFixed == 3)
		{
			if(gpsInfo->gga.fix_quality == 1)
				isFixedStr = "3D fix";
			else if(gpsInfo->gga.fix_quality == 2)
				isFixedStr = "3D/DGPS fix";
		}
		else
			isFixedStr = "no fix";

		//convert unit ddmm.mmmm to degree(Â°)
		int temp = (int)(gpsInfo->rmc.latitude.value/gpsInfo->rmc.latitude.scale/100);
		double latitude = temp+(double)(gpsInfo->rmc.latitude.value - temp*gpsInfo->rmc.latitude.scale*100)/gpsInfo->rmc.latitude.scale/60.0;
		temp = (int)(gpsInfo->rmc.longitude.value/gpsInfo->rmc.longitude.scale/100);
		double longitude = temp+(double)(gpsInfo->rmc.longitude.value - temp*gpsInfo->rmc.longitude.scale*100)/gpsInfo->rmc.longitude.scale/60.0;


		//you can copy ` latitude,longitude ` to http://www.gpsspg.com/maps.htm check location on map

		snprintf(buffer,sizeof(buffer),"GPS fix mode:%d, BDS fix mode:%d, fix quality:%d, satellites tracked:%d, gps sates total:%d, is fixed:%s, coordinate:WGS84, Latitude:%f, Longitude:%f, unit:degree,altitude:%f",gpsInfo->gsa[0].fix_type, gpsInfo->gsa[1].fix_type,
				gpsInfo->gga.fix_quality,gpsInfo->gga.satellites_tracked, gpsInfo->gsv[0].total_sats, isFixedStr, latitude,longitude,gpsInfo->gga.altitude);



		latitude_gps=latitude;
		longitude_gps=longitude;


		//show in tracer
		Trace(2,buffer);
		//send to UART1
		UART_Write(UART1,buffer,strlen(buffer));
		UART_Write(UART1,"\r\n\r\n",4);
	}

	//		OS_Sleep(5000);

}



//http get with no header
int Http_Get(const char* domain, int port,const char* path, char* retBuffer, int* bufferLen)
{
	Trace(1,"debug Http_Get ");

	bool flag = false;
	uint16_t recvLen = 0;
	uint8_t ip[16];
	char buffer2[1024];
	int Buffer2Len = sizeof(buffer2);

	int retBufferLen = *bufferLen;
	//connect server
	memset(ip,0,sizeof(ip));
	if(DNS_GetHostByName2(domain,ip) != 0)
	{
		Trace(1,"DEBUG  get ip error");
		return -1;
	}
	Trace(1,"DEBUG  get ip success:%s -> %s",domain,ip);
	char* servInetAddr = ip;
	//    snprintf(retBuffer,retBufferLen,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",path,domain);

	gps_testTask();

	//for test only
	//		latitude_gps=29.9940961;
	//		longitude_gps=31.1477996;

	//	snprintf(retBuffer,retBufferLen,"GET %s%d\r\n HTTP/1.1\r\nHost: %s\r\n\r\n",path,latitude_gps,domain);

	snprintf(buffer2,Buffer2Len,"GET /update?api_key=SD5OBD49N5H4O8RY&field6=%f&field5=%f\r\n HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n",latitude_gps,longitude_gps);
	char* pData = buffer2;
	//	char* pData         = "GET /update?api_key=SD5OBD49N5H4O8RY&field1=90/nHTTP/1.1\nHost: api.thingspeak.com";




	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd < 0){
		Trace(1,"DEBUG  socket fail");
		return -1;
	}
	Trace(1,"DEBUG  fd:%d",fd);

	struct sockaddr_in sockaddr;
	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	inet_pton(AF_INET,servInetAddr,&sockaddr.sin_addr);

	int ret = connect(fd, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in));
	if(ret < 0){
		Trace(1,"DEBUG  socket connect fail");
		close(fd);
		return -1;
	}
	Trace(1,"DEBUG  socket connect success");
	Trace(1,"DEBUG  send request:%s",pData);
	ret = send(fd, pData, strlen(pData), 0);
	if(ret < 0){
		Trace(1,"DEBUG  socket send fail");
		close(fd);
		return -1;
	}
	Trace(1,"DEBUG  socket send success");



	struct fd_set fds;
	struct timeval timeout={12,0};
	FD_ZERO(&fds);
	FD_SET(fd,&fds);
	while(!flag)
	{
		ret = select(fd+1,&fds,NULL,NULL,&timeout);
		switch(ret)
		{
		case -1:
			Trace(1,"DEBUG  select error");
			flag = true;
			break;
		case 0:
			Trace(1,"DEBUG  select timeout");
			flag = true;
			break;
		default:
			if(FD_ISSET(fd,&fds))
			{
				Trace(1,"DEBUG  select return:%d",ret);
				memset(retBuffer+recvLen,0,retBufferLen-recvLen);
				ret = recv(fd,retBuffer+recvLen,retBufferLen-recvLen,0);
				Trace(1,"DEBUG  ret:%d",ret);
				recvLen += ret;
				if(ret < 0)
				{
					Trace(1,"DEBUG  recv error");
					flag = true;
					break;
				}
				else if(ret == 0)
				{
					Trace(1,"DEBUG  ret == 0");
					flag = true;
					break;
				}
				else if(ret < 1352)
				{
					Trace(1,"DEBUG  recv len:%d,data:%s",recvLen,retBuffer);
					*bufferLen = recvLen;
					close(fd);
					return recvLen;
				}

			}
			break;
		}
	}



	close(fd);
	return -1;

}


void Socket_BIO_Test()
{



	Trace(1,"debug Socket_BIO_Test ");

	char buffer[2048];
	int len = sizeof(buffer);
	//wait for gprs network connection ok
	semStart = OS_CreateSemaphore(0);
	OS_WaitForSemaphore(semStart,OS_TIME_OUT_WAIT_FOREVER);
	OS_DeleteSemaphore(semStart);




	while(1)
	{

		Http_Get(SERVER_IP,SERVER_PORT,SERVER_PATH,buffer,&len);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
		OS_Sleep(5000);
	}

	//perform http get
	if(Http_Get(SERVER_IP,SERVER_PORT,SERVER_PATH,buffer,&len) < 0)
	{
		Trace(1,"DEBUG  http get fail");
	}
	else
	{
		//show response message though tracer(pay attention:tracer can not show all the word if the body too long)
		Trace(1,"DEBUG  http get success,ret:%s",buffer);
		char* index0 = strstr(buffer,"\r\n\r\n");
		char temp = index0[4];
		index0[4] = '\0';
		Trace(1,"DEBUG  http response header:%s",buffer);
		index0[4] = temp;
		Trace(1,"DEBUG  http response body:%s",index0+4);
	}




}

void test_MainTask(void* param)
{
	API_Event_t* event=NULL;

	Socket_BIO_Test();
	Trace(1,"debug test_MainTask ");

	while(1)
	{
		if(OS_WaitEvent(socketTaskHandle, (void**)&event, OS_TIME_OUT_WAIT_FOREVER))
		{
			EventDispatch(event);
			OS_Free(event->pParam1);
			OS_Free(event->pParam2);
			OS_Free(event);
		}
	}
}
void CallTest()
{
	//initialize
	Trace(1,"start Call test, initialize audio first");
	AUDIO_MicOpen();
	AUDIO_SpeakerOpen();
	AUDIO_MicSetMute(false);
	AUDIO_SpeakerSetVolume(15);

	//make a call
	Trace(1,"start make a call");
//	CALL_Dial(DIAL_NUMBER);

	if(!CALL_Dial(DIAL_NUMBER))
	{
		Trace(1,"make a call failed");
		return;
	}
	while(!isDialSuccess)
		OS_Sleep(100);


	CALL_Status_t* callStatus = NULL;
	uint8_t count;
	bool ret = CALL_Status(&callStatus,&count);
	uint8_t i;
	if(ret)
	{
		for(i=0;i<count;++i)
		{
			Trace(1,"index:%d,direction:%d,status:%d,mode:%d,mpty:%d,number:%s,number type:%d",
					callStatus[i].index,callStatus[i].direction,callStatus[i].status,callStatus[i].mode,
					callStatus[i].multiparty,callStatus[i].number,callStatus[i].numberType);
		}
		OS_Free(callStatus);
	}


	uint8_t dtmf = '0';
	for(int i=0;i<10;++i,++dtmf)
	{
		Trace(1,"make a DTMF:%c",dtmf);
		if(!CALL_DTMF(dtmf,CALL_DTMF_GAIN_m3dB,5,15,true))
		{
			Trace(1,"DTMF fail");
			break;
		}
		OS_Sleep(3000);
	}
	Trace(1,"sleep start");
	OS_Sleep(10000);
	Trace(1,"sleep end");
	if(!CALL_HangUp())
		Trace(1,"hang up fail");
}

void SecondTask(void *pData)
{

	  while(1)
	    {
	        if(call_flag == 1)
	        {
	            CallTest();
	            call_flag = 2;
	        }
	        OS_Sleep(100);

	    }

}


void socket_MainTask(void *pData)
{
	API_Event_t* event=NULL;

    call_flag = 0;
    isDialSuccess = false;

    secondTaskHandle = OS_CreateTask(SecondTask,
        NULL, NULL, SECOND_TASK_STACK_SIZE, SECOND_TASK_PRIORITY, 0, 0, SECOND_TASK_NAME);

	testTaskHandle = OS_CreateTask(test_MainTask,
			NULL, NULL, TEST_TASK_STACK_SIZE, TEST_TASK_PRIORITY, 0, 0, TEST_TASK_NAME);

	//open UART1 to print NMEA infomation
	UART_Config_t config = {
			.baudRate = UART_BAUD_RATE_115200,
			.dataBits = UART_DATA_BITS_8,
			.stopBits = UART_STOP_BITS_1,
			.parity   = UART_PARITY_NONE,
			.rxCallback = NULL,
			.useEvent   = true
	};
	UART_Init(UART1,config);

	while(1)
	{
		if(OS_WaitEvent(socketTaskHandle, (void**)&event, OS_TIME_OUT_WAIT_FOREVER))
		{
			EventDispatch(event);
			OS_Free(event->pParam1);
			OS_Free(event->pParam2);
			OS_Free(event);
		}

	}
}

void socket_Main()
{
	socketTaskHandle = OS_CreateTask(socket_MainTask,
			NULL, NULL, MAIN_TASK_STACK_SIZE, MAIN_TASK_PRIORITY, 0, 0, MAIN_TASK_NAME);
	OS_SetUserMainHandle(&socketTaskHandle);
}


