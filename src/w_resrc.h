#pragma once


struct Resource
{
	char* buf{ nullptr };
	int size{ 0 };
	char name[4]{};
	int id{ 0 };
	struct Resource* next{ nullptr };
};


typedef char** Handle;

Handle GetResource(char* name, int id);
void GetIndString(char* str, int id, int num);
