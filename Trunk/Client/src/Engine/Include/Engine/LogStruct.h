#ifndef _LOG_STRUCT_H_
#define _LOG_STRUCT_H_


#define LOG_INFO_MAX_LENGTH     0xffff      //��־��Ϣ����󳤶�


enum LogType                                //��־����
{
	LT_NONE						= 0,		//
	LT_INFO                     = 1,        //һ����Ϣ
	LT_DEBUG                    = 2,        //������Ϣ
	LT_COMMAND                  = 3,        //������Ϣ
	LT_WARN                     = 4,        //������Ϣ
	LT_ERROR                    = 5,        //������Ϣ
	LT_FATAL                    = 6,        //������Ϣ
};



struct LogInfo                              //��־��Ϣ  
{
	LogType type;                           //��־����
	char    message[LOG_INFO_MAX_LENGTH];   //�һ��LOG��ϢLOG_INFO_MAX_LENGTH�ֽ�
};

#endif //_LOG_STRUCT_H_