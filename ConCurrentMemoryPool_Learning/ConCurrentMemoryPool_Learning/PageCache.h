#pragma once

#include "Common.h"
#include "ObjectPool.h"
#include "PageMap.h"

class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_sInst;
	}

	// 获取从对象到span的映射
	Span* MapObjectToSpan(void* obj);

	// 释放空闲span回到Pagecache，并合并相邻的span
	void ReleaseSpanToPageCache(Span* span);

	// 获取一个K页的span
	Span* NewSpan(size_t k);

	std::mutex _pageMtx;
private:
	SpanList _spanLists[NPAGES];
	ObjectPool<Span> _spanPool;

	//std::unordered_map<PAGE_ID, Span*> _idSpanMap;
	//std::map<PAGE_ID, Span*> _idSpanMap;
	//TCMalloc_PageMap1<32 - PAGE_SHIFT> _idSpanMap;

	#ifdef _WIN64
	// 64位系统下由于页号上限庞大，需要使用三层基数树
		TCMalloc_PageMap3<64 - PAGE_SHIFT> _idSpanMap;
	#elif _WIN32
	// 32位系统下使用两层或一层都可以
		TCMalloc_PageMap2<32 - PAGE_SHIFT> _idSpanMap;
	#else 
	// Linux 环境同理处理...
	#endif
	PageCache()
	{
	}
	PageCache(const PageCache&) = delete;


	static PageCache _sInst;
};