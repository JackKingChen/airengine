#ifndef _LOG_STRUCT_H_
#define _LOG_STRUCT_H_


#define LOG_INFO_MAX_LENGTH     0xffff      //日志信息的最大长度


enum LogType                                //日志类型
{
	LT_NONE						= 0,		//
	LT_INFO                     = 1,        //一般信息
	LT_DEBUG                    = 2,        //调试信息
	LT_COMMAND                  = 3,        //命令信息
	LT_WARN                     = 4,        //警告信息
	LT_ERROR                    = 5,        //错误信息
	LT_FATAL                    = 6,        //致命信息
};



struct LogInfo                              //日志信息  
{
	LogType type;                           //日志类型
	char    message[LOG_INFO_MAX_LENGTH];   //最长一个LOG消息LOG_INFO_MAX_LENGTH字节
};

#endif //_LOG_STRUCT_H_