#include"config.h"

Config serverConfig;

int loadConfig(char * configFilePath)
{
	FILE * confFile = NULL;
	char buff_line[512];
	memset(&serverConfig, 0, sizeof(Config));
	Config * conf = &serverConfig;
	confFile = fopen(configFilePath, "r");
	if (confFile == NULL)
		return ERR_CONFIG_FILE_OPEN_FAIL;
	while (!feof(confFile))
	{
		memset(buff_line , 512 , 0);
		fgets(buff_line, 512, confFile);
		if (buff_line[0] == '#')
			continue;
		char * pTemp = NULL;
		pTemp =(char * )strstr(buff_line , "=");
		if (pTemp == NULL)
			continue;
		*pTemp = '\0';
		char * leftArg = buff_line;
		char * rightArg = ++pTemp;

		int i = strlen(rightArg);
		while (i > 0)
		{
			if (rightArg[i] == '\n' || rightArg[i] == '\r' || rightArg[i] == ' ')
				rightArg[i] ='\0';
			i--;
		}
	
		if (!strcmp("LogPath", leftArg))
		{
			strcpy(conf->logPath , rightArg);
		}
		else if (!strcmp("ServerPort", leftArg))
		{
			strcpy(conf->serverPort, rightArg);
		}
		else if (!strcmp("MysqlIP", leftArg))
		{
			strcpy(conf->mysqlIP, rightArg);
		}
		else if (!strcmp("MysqlPort", leftArg))
		{
			strcpy(conf->mysqlPort, rightArg);
		}
		else if (!strcmp("MysqlAccount", leftArg))
		{
			strcpy(conf->mysqlAccount, rightArg);
		}
		else if (!strcmp("MysqlPassword", leftArg))
		{
			strcpy(conf->mysqlPassword, rightArg);
		}
		else if (!strcmp("MysqlConnectionTimeout", leftArg))
		{
			conf->mysqlConnectionTimeout = atoi(rightArg);
		}
		else if (!strcmp("DatabaseName", leftArg))
		{
			strcpy(conf->databaseName, rightArg);
		}
		else if (!strcmp("authPushPoolCount", leftArg))
		{
			conf->authPushPoolCount = atoi(rightArg);
		}
		else if (!strcmp("authPushPoolCapacity", leftArg))
		{
			char * ppp = strchr(rightArg , ']');
			*ppp = '\0';
			rightArg++;
			char* delim = ",";
			char* p = strtok(rightArg, delim);
			int i = 0;
			while (p != NULL) {
				conf->authPushPoolCapacity[i++] = atoi(p);
				p = strtok(NULL, delim);
			}
		}
		else if (!strcmp("authPushPoolRateX", leftArg))
		{
			char * ppp = strchr(rightArg, ']');
			*ppp = '\0';
			rightArg++;
			char* delim = ",";
			char* p = strtok(rightArg, delim);
			int i = 0;
			while (p != NULL) {
				conf->authPushPoolRateX[i++] = atoi(p);
				p = strtok(NULL, delim);
			}
		}
		else if (!strcmp("multicastIP", leftArg))
		{
			strcpy(conf->multicastIP, rightArg);
		}
		else if (!strcmp("multicastPORT", leftArg))
		{
			strcpy(conf->multicastPORT, rightArg);
		}
		else if (!strcmp("authPushPoolBandwidth", leftArg))
		{
			conf->authPushPoolBandwidth = atoi(rightArg);
		}
		else
		{
			;
		}

	}
	fclose(confFile);
	return RETURN_OK;

}