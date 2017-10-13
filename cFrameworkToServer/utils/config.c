#include"config.h"

dict * dict_config = NULL;

int loadConfig(char * configFilePath)
{
	FILE * confFile = NULL;
	char buff_line[512];
	dict_config = dictCreateEx();
	if (dict_config == NULL)
		return ERR_DICT_CREATE_FAIL;
	confFile = fopen(configFilePath, "r");
	if (confFile == NULL)
		return ERR_CONFIG_FILE_OPEN_FAIL;
	while (!feof(confFile))
	{
		memset(buff_line , 512 , 0);
		fgets(buff_line, 512, confFile);
		
		if (buff_line[0] == '#' || buff_line[0] == '\n' || buff_line[0] == '\r')
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
			if (rightArg[i] == '\n' || rightArg[i] == '\r')
				rightArg[i] ='\0';
			i--;
		}
		dictAdd(dict_config, Trim(leftArg) , Trim(rightArg));
	}
	fclose(confFile);
	return RETURN_OK;
}

char * getConfig(char * key)
{
	return dictFetchValue(dict_config , key);
}

int setConfig(char * key, char * value)
{
	return dictReplace(dict_config , key , value);
}

int addConfig(char * key, char * value)
{
	return dictAdd(dict_config, key, value);
}