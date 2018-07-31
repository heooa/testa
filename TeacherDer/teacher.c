#include "teacher.h"
#include "itcast_asn1_der.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	typedef struct _Teacher
	{
		char name[64];
		int age;
		char *p;
		long plen;
	}Teacher;
*/
int encodeTeacher(Teacher * p, char ** outData, int * outlen)
{
	ITCAST_ANYBUF* ptemp = NULL;
	ITCAST_ANYBUF* phead = NULL;
	ITCAST_ANYBUF* pnext = NULL;
	// 1. 编码name, char* -> ITCAST_ANYBUF, 但是没有编码
	DER_ITCAST_String_To_AnyBuf(&ptemp, p->name, strlen(p->name)+1);
	// 1.1 编码name, phead是链表的第一个数据节点
	DER_ItAsn1_WritePrintableString(ptemp, &phead);
	DER_ITCAST_FreeQueue(ptemp);

	// 2. 编码age, 编码完成之后, 作为第二个链表节点, phead->next
	DER_ItAsn1_WriteInteger(p->age, &phead->next);

	// 3. 编码p
	pnext = phead->next;
	EncodeChar(p->p, strlen(p->p)+1, &pnext->next);

	// 4. 编码plen
	// next指针后移
	pnext = pnext->next;
	DER_ItAsn1_WriteInteger(p->plen, &pnext->next);

	// 5. 对整个链表编码, 得到一ITCAST_ANYBUF
	// ptemp 中存储了整个链表的数据
	DER_ItAsn1_WriteSequence(phead, &ptemp);

	// 6. 传出参数赋值操作
	*outData = ptemp->pData;
	*outlen = ptemp->dataLen;

	// 7. 释放链表
	DER_ITCAST_FreeQueue(phead);

	return 0;
}

/*
	typedef struct _Teacher
	{
		char name[64];
		int age;
		char *p;
		long plen;
	}Teacher;
*/
int decodeTeacher(char * inData, int inLen, Teacher ** p)
{
	ITCAST_ANYBUF* ptemp = NULL;
	ITCAST_ANYBUF* phead = NULL;
	ITCAST_ANYBUF* pnext = NULL;
	// 申请一块内存存储TEacher结构体数据
	Teacher* teacher = (Teacher*)malloc(sizeof(Teacher));
	memset(teacher, 0, sizeof(Teacher));

	// inData -> 序列化之后的字符串, 反序列化
	// inData->ITCAST_ANYBUF
	// ptemp -> 序列化之后的链表中的所有数据
	DER_ITCAST_String_To_AnyBuf(&ptemp, inData, inLen);
	// ptemp还原为一个链表
	// phead中存储的是name序列化之后的数据
	DER_ItAsn1_ReadSequence(ptemp, &phead);
	DER_ITCAST_FreeQueue(ptemp);
	// 还原name的值
	// ptemp存储的解码之后的数据
	DER_ItAsn1_ReadPrintableString(phead, &ptemp);
	// 取出name的原始值
	strcpy(teacher->name, ptemp->pData);
	DER_ITCAST_FreeQueue(ptemp);

	// 2. 解析age, pnext指向的节点类型ITCAST_ANYBUF
	pnext = phead->next;
	DER_ItAsn1_ReadInteger(pnext, &teacher->age);

	// 3. 解析p
	pnext = pnext->next;
	DER_ItAsn1_ReadPrintableString(pnext, &ptemp);
	teacher->p = (char*)malloc(ptemp->dataLen);
	memset(teacher->p, 0, ptemp->dataLen);
	strcpy(teacher->p, ptemp->pData);
	DER_ITCAST_FreeQueue(ptemp);

	// 4. plen
	pnext = pnext->next;
	DER_ItAsn1_ReadInteger(pnext, &teacher->plen);

	// 5. 传出参数赋值
	*p = teacher;

	// 6. 释放链表
	DER_ITCAST_FreeQueue(phead);

	return 0;
}

void freeTeacher(Teacher ** p)
{
	if ((*p) != NULL)
	{
		if ((*p)->p != NULL)
		{
			free((*p)->p);
		}
		free(*p);
	}
}
