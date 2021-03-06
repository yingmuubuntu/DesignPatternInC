
#include <stdio.h>
#include <apr_strings.h>
#include "FileText.h"

//属性、方法声明

struct FileText_Fld
{
    apr_pool_t *m_pPool;

    IFile m_file;

    char *m_pName;
};

//实现接口方法
static void Free(IFile **ppFile)
{
	FileText_Free(&(FileText *)(*ppFile)->pImplicit);
    *ppFile = NULL;
}

//实现接口方法
static void Add(IFile *pFile, IFile *pAdd)
{
    puts("文本文件不支持添加子文件操作。");
}

//实现接口方法
static void Remove(IFile *pFile, IFile *pRmv)
{
    puts("文本文件不支持移除子文件操作.");
}

//实现接口方法
static IFile *GetChild(IFile *pFile, int nIndex)
{
    puts("文本文件不支持获取子文件操作。");
    return NULL;
}

//实现接口方法
static void KillVirus(IFile *pFile)
{
	FileText *pInst = (FileText *)pFile->pImplicit;

    //模拟杀毒
    printf("对文本%s进行杀毒。\n", pInst->pFld->m_pName);
}

FileText * FileText_New(apr_pool_t * pSupPool, const char * const pName)
{
    apr_pool_t *pPool;
    apr_pool_create(&pPool, pSupPool);

	FileText *pInst = apr_palloc(pPool, sizeof(FileText));

    pInst->pFld = apr_palloc(pPool, sizeof(FileText_Fld));
    pInst->pFld->m_pPool = pPool;
    pInst->pFld->m_file.pImplicit = pInst;
    pInst->pFld->m_file.Free = Free;
    pInst->pFld->m_pName = apr_pstrdup(pInst->pFld->m_pPool, pName);

    pInst->pFld->m_file.Add = Add;
    pInst->pFld->m_file.Remove = Remove;
    pInst->pFld->m_file.GetChild = GetChild;
    pInst->pFld->m_file.KillVirus = KillVirus;

    return pInst;
}

IFile * FileText2IFile(FileText * pInst)
{
    return &(pInst->pFld->m_file);
}

void FileText_Free(FileText ** ppInst)
{
    apr_pool_destroy((*ppInst)->pFld->m_pPool);
    *ppInst = NULL;
}
