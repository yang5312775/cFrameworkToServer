#include"config.h"

dict * dict_config = NULL;

int configerInit(char * configFilePath)
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
		//key与value均由dict之外malloc
		dictAdd(dict_config, Trim(leftArg) , Trim(rightArg));
	}
	fclose(confFile);
	return RETURN_OK;
}

void configerUnInit(void)
{
	if(dict_config != NULL)
		dictRelease(dict_config);
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

int print_config(void)
{
	dictEntry * entry = NULL;
	dictIterator * iter = NULL;
	iter = dictGetIterator(dict_config);
	entry = 1;
	log_print(L_INFO, "--------------------start print config table-------------------------\n");
	while (entry != NULL)
	{
		entry = dictNext(iter);
		if (entry != NULL)
			log_print(L_INFO , "[%s]:[%s]\n" , entry->key , entry->v.val);
	}
	log_print(L_INFO, "---------------------end print config table--------------------------\n");
	return 0;
}