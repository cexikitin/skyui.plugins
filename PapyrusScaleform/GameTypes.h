#pragma once

#include "skse/Utilities.h"

// core library types (lists, strings, vectors)
// preferably only the ones that bethesda created (non-netimmerse)

class BSIntrusiveRefCounted
{
public:
	volatile UInt32	m_refCount;	// 00
};

// 04
template <typename T>
class BSTSmartPointer
{
public:
	// refcounted
	T	* ptr;
};

class SimpleLock
{
private:
	volatile UInt32	threadID;
	UInt32			lockCount;
};

// refcounted threadsafe string storage
// use StringCache::Ref to access everything, other internals are for documentation only
class StringCache
{
public:
	// BSFixedString?
	struct Ref
	{
		const char	* data;

		MEMBER_FN_PREFIX(Ref);
		DEFINE_MEMBER_FN(ctor, Ref *, 0x00A49570, const char * buf);
		DEFINE_MEMBER_FN(Set, Ref *, 0x00A495C0, const char * buf);
		DEFINE_MEMBER_FN(Release, void, 0x00A49560);

		Ref() :data(NULL) { }
		Ref(const char * buf);

		bool operator==(const Ref& lhs) const { return data == lhs.data; }
		bool operator<(const Ref& lhs) const { return data < lhs.data; }
	};

	struct Lock
	{
		SimpleLock	lock;
	};

	StringCache();
	~StringCache();

	static StringCache *	GetSingleton(void);

	Lock *	GetLock(UInt32 crc16);

private:
	struct Entry
	{
		Entry	* next;		// 00
		union
		{
			struct  
			{
				UInt16	refCount;	// invalid if 0x8000 is set
				UInt16	hash;
			};
			UInt32	refCountAndHash;
		} state;	// 04
		UInt32	length;		// 08
		// data follows
	};

	Entry	* table[0x10000];	// crc16
	Lock	locks[0x20];		// crc16 & 0x1F
	UInt8	unk;
};

typedef StringCache::Ref	BSFixedString;

// 08
class BSString
{
public:
	const char *	Get(void);

private:
	char	* m_data;	// 00
	UInt16	m_dataLen;	// 04
	UInt16	m_bufLen;	// 06
};

template <class T>
class tArray
{
public:
	struct Array {
		T* entries;
		UInt32 unk4;
	};

	Array arr;
	UInt32 count;
	
	bool GetNthItem(UInt32 index, T& pT)
	{
		if (index < count) {
			pT = arr.entries[index];
			return true;
		}
		return false;
	}

	UInt32 GetItemIndex(T pFind)
	{
		for (UInt32 n = 0; n < count; n++) {
			T& pT = arr.entries[n];
			if (pT == pFind)
				return n;
		}
		return -1;
	}
};

typedef tArray<UInt32> UnkArray;
typedef tArray<TESForm*> UnkFormArray;

enum {
	eListCount = -3,
	eListEnd = -2,
	eListInvalid = -1,		
};

template <class Item>
class tList
{
	typedef Item tItem;
	struct _Node {
		
		tItem*	item;
		_Node*	next;

		tItem* Item() const { return item; }
		_Node* Next() const { return next; }

		// become the next entry and return my item
		tItem* RemoveMe() {
			tItem* pRemoved = item;
			_Node* pNext = next;
			if (pNext) {
				item = pNext->item;
				next = pNext->next;
				FormHeap_Free(pNext);
			} else {
				item = NULL;
				next = NULL;
			}
			return pRemoved;
		}
	};

	_Node m_listHead;


private:

	template <class Op>
	UInt32 FreeNodes(_Node* node, Op &compareOp) const
	{
		static UInt32 nodeCount = 0;
		static UInt32 numFreed = 0;
		static _Node* lastNode = NULL;
		static bool bRemovedNext = false;

		if (node->Next())
		{
			nodeCount++;
			FreeNodes(node->Next(), compareOp);
			nodeCount--;
		}

		if (compareOp.Accept(node->Item()))
		{
			if (nodeCount)
				node->Delete();
			else
				node->DeleteHead(lastNode);
			numFreed++;
			bRemovedNext = true;
		}
		else
		{
			if (bRemovedNext)
				node->SetNext(lastNode);
			bRemovedNext = false;
			lastNode = node;
		}

		if (!nodeCount)	//reset vars after recursing back to head
		{
			numFreed = 0;
			lastNode = NULL;
			bRemovedNext = false;
		}

		return numFreed;
	}


	struct NodePos {
		NodePos(): node(NULL), index(eListInvalid) {}

		_Node*	node;
		SInt32	index;
	};


	NodePos GetNthNode(SInt32 index) const {
		NodePos pos;
		SInt32 n = 0;
		_Node* pCur = Head();

		while (pCur && pCur->Item()) {
			if (n == index) break;
			if (eListEnd == index && !pCur->Next()) break;
			pCur = pCur->Next();
			++n;
		}

		pos.node = pCur;
		pos.index = n;

		return pos;
	}

public:

	_Node* Head() const { return const_cast<_Node*>(&m_listHead); }

	class Iterator
	{
		_Node*	m_cur;
	public:
		Iterator() : m_cur(NULL) {}
		Iterator(_Node* node) : m_cur(node) { }
		Iterator operator++()	{ if (!End()) m_cur = m_cur->Next(); return *this;}
		bool End()	{	return m_cur == NULL;	}
		const Item* operator->() { return (m_cur) ? m_cur->Item() : NULL; }
		const Item* operator*() { return (m_cur) ? m_cur->Item() : NULL; }
		const Iterator& operator=(const Iterator& rhs) {
			m_cur = rhs.m_cur;
			return *this;
		}
		Item* Get() { return (m_cur) ? m_cur->Item() : NULL; }
	};
	
	const Iterator Begin() const { return Iterator(Head()); }


	UInt32 Count() const {
		NodePos pos = GetNthNode(eListCount);
		return (pos.index > 0) ? pos.index : 0;
	};

	Item* GetNthItem(SInt32 n) const {
		NodePos pos = GetNthNode(n);
		return (pos.index == n && pos.node) ? pos.node->Item() : NULL;
	}

	Item* GetLastItem() const {
		NodePos pos = GetNthNode(eListEnd);
		return pos.node->Item();
	}

	SInt32 AddAt(Item* item, SInt32 index) {
		if (!m_listHead.item) {
			m_listHead.item = item;
			return 0;
		}

		NodePos pos = GetNthNode(index);
		_Node* pTargetNode = pos.node;
		_Node* newNode = (_Node*)FormHeap_Allocate(sizeof(newNode));
		if (newNode && pTargetNode) {
			if (index == eListEnd) {
				pTargetNode->next = newNode;
				newNode->item = item;
				newNode->next = NULL;
			} else {
				newNode->item = pTargetNode->item;
				newNode->next = pTargetNode->next;
				pTargetNode->item = item;
				pTargetNode->next = newNode;
			}
			return pos.index;
		}
		return eListInvalid;
	}

	template <class Op>
	void Visit(Op& op, _Node* prev = NULL) const {
		const _Node* pCur = (prev) ? prev->next : Head();
		bool bContinue = true;
		while (pCur && bContinue) {
			bContinue = op.Accept(pCur->Item());
			if (bContinue) {
				pCur = pCur->next;
			}
		}
	}

	template <class Op>
	Item* Find(Op& op) const
	{
		const _Node* pCur = Head(); 

		bool bFound = false;
		while (pCur && !bFound)
		{
			if (!pCur->Item())
				pCur = pCur->Next();
			else
			{
				bFound = op.Accept(pCur->Item());
				if (!bFound)
					pCur = pCur->Next();
			}
		}
		return (bFound && pCur) ? pCur->Item() : NULL;
	}

	template <class Op>
	Iterator Find(Op& op, Iterator prev) const
	{
		Iterator curIt = (prev.End()) ? Begin() : ++prev;
		bool bFound = false;
		
		while(!curIt.End() && !bFound) {
			const tItem * pCur = *curIt;
			if (pCur) {
				bFound = op.Accept(pCur);
			}
			if (!bFound) {
				++curIt;
			}	
		}
		return curIt;
	}

	const _Node* FindString(char* str, Iterator prev) const
	{
		return Find(StringFinder_CI(str), prev);
	}

	template <class Op>
	UInt32 CountIf(Op& op) const
	{
		UInt32 count = 0;
		const _Node* pCur = Head();
		while (pCur)
		{
			if (pCur->Item() && op.Accept(pCur->Item()))
				count++;
			pCur = pCur->Next();
		}
		return count;
	}

	class AcceptAll {
	public:
		bool Accept(Item* item) {
			return true;
		}
	};

	void RemoveAll() const
	{
		FreeNodes(const_cast<_Node*>(Head()), AcceptAll());
	}

	Item* RemoveNth(SInt32 n) 
	{
		Item* pRemoved = NULL;
		if (n == 0) {
			pRemoved =  m_listHead.RemoveMe();
		} else if (n > 0) {
			NodePos nodePos = GetNthNode(n);
			if (nodePos.node && nodePos.index == n) {
				pRemoved = nodePos.node->RemoveMe();
			}
		}
		return pRemoved;
	};

	Item* ReplaceNth(SInt32 n, tItem* item) 
	{
		Item* pReplaced = NULL;
		NodePos nodePos = GetNthNode(n);
		if (nodePos.node && nodePos.index == n) {
			pReplaced = nodePos.node->item;
			nodePos.node->item = item;
		}
		return pReplaced;
	}

	template <class Op>
	UInt32 RemoveIf(Op& op)
	{
		return FreeNodes(const_cast<_Node*>(Head()), op);
	}

	template <class Op>
	SInt32 GetIndexOf(Op& op)
	{
		SInt32 idx = 0;
		const _Node* pCur = Head();
		while (pCur && pCur->Item() && !op.Accept(pCur->Item()))
		{
			idx++;
			pCur = pCur->Next();
		}

		if (pCur && pCur->Item())
			return idx;
		else
			return -1;
	}

};

STATIC_ASSERT(sizeof(tList<void *>) == 0x8);


typedef void (__cdecl * _CRC32_Calc4)(UInt32 * out, UInt32 data);
extern const _CRC32_Calc4 CRC32_Calc4;

typedef void (__cdecl * _CRC32_Calc8)(UInt32 * out, UInt64 data);
extern const _CRC32_Calc8 CRC32_Calc8;


// 01C
template <typename Item>
class tHashSet
{
	struct _Entry
	{
		Item	item;
		_Entry	* next;

		bool		IsFree() const	{ return next == NULL; }
		void		Free()			{ next = NULL; }
	};

	static _Entry sentinel;

	UInt32		unk_000;		// 000
	UInt32		m_size;			// 004
	UInt32		m_freeCount;	// 008
	UInt32		m_freeOffset;	// 00C
	_Entry		* m_eolPtr;		// 010
	UInt32		unk_014;		// 014
	_Entry		* m_entries;	// 018


	_Entry * GetEntry(UInt32 hash) const
	{
		return (_Entry*) (((UInt32) m_entries) + sizeof(_Entry) * (hash & (m_size - 1)));
	}

	_Entry * NextFreeEntry(void)
	{
		_Entry * result = NULL;

		if (m_freeCount == 0)
			return NULL;

		do
		{
			m_freeOffset = (m_size - 1) & (m_freeOffset - 1);
			_Entry * entry = (_Entry*) (((UInt32) m_entries) + sizeof(_Entry) * m_freeOffset);

			if (entry->IsFree())
				result = entry;
		}
		while (!result);
		m_freeCount--;

		return result;
	}

	bool Insert(Item * item)
	{
		if (! m_entries)
			return false;

		_Entry * targetEntry = GetEntry(item->GetHash());
		_Entry * p = NULL;

		// Case 1: Target entry is free
		if (!targetEntry->next)
		{
			targetEntry->item = *item;
			targetEntry->next = m_eolPtr;
			--m_freeCount;

			return true;
		}

		// -- Target entry is already in use

		// Case 2: Item already included
		p = targetEntry;
		do
		{
			if (p->item.Equals(item))
				return true;
			p = p->next;
		}
		while (p != m_eolPtr);

		// -- Either hash collision or bucket overlap

		_Entry * freeEntry = NextFreeEntry();
		// No more space?
		if (!freeEntry)
			return false;

		p = GetEntry(targetEntry->item.GetHash());

		// Case 3a: Hash collision - insert new entry between target entry and successor
        if (targetEntry == p)
        {
			freeEntry->item = *item;
			freeEntry->next = targetEntry->next;
			targetEntry->next = freeEntry;

			return true;
        }
		// Case 3b: Bucket overlap
		while (p->next != targetEntry)
			p = p->next;

        memcpy_s(freeEntry, sizeof(_Entry), targetEntry, sizeof(_Entry));
        p->next = freeEntry;
		targetEntry->item = *item;
		targetEntry->next = m_eolPtr;

		return true;
	}

	bool CopyEntry(_Entry * sourceEntry)
	{
		if (! m_entries)
			return false;

		_Entry * targetEntry = GetEntry(sourceEntry->item.GetHash());

		// Case 1: Target location is unused
		if (!targetEntry->next)
		{
			targetEntry->item = sourceEntry->item;
			targetEntry->next = m_eolPtr;
			--m_freeCount;

			return true;
		}

		// Target location is in use. Either hash collision or bucket overlap.

		_Entry * freeEntry = NextFreeEntry();
		_Entry * p = GetEntry(targetEntry->item.GetHash());

		// Case 2a: Hash collision - insert new entry between target entry and successor
		if (targetEntry == p)
		{
			freeEntry->item = sourceEntry->item;
			freeEntry->next = targetEntry->next;
			targetEntry->next = freeEntry;

			return true;
		}

		// Case 2b: Bucket overlap - forward until hash collision is found, then insert
		while (p->next != targetEntry)
			p = p->next;

		// Source entry takes position of target entry - not completely understood yet
		memcpy_s(freeEntry, sizeof(_Entry), targetEntry, sizeof(_Entry));
		p->next = freeEntry;
		targetEntry->item = sourceEntry->item;
		targetEntry->next = m_eolPtr;

		return true;
	}

	void Grow(void)
	{
		UInt32 oldSize = m_size;
		UInt32 newSize = oldSize ? 2*oldSize : 8;

		_Entry * oldEntries = m_entries;
		_Entry * newEntries = (_Entry*)FormHeap_Allocate(newSize * sizeof(_Entry));
		
		m_entries = newEntries;
		m_size = m_freeCount = m_freeOffset = newSize;

		// Initialize new table data (clear next pointers)
		if (newEntries)
		{
			_Entry * p = newEntries;
			for (UInt32 i = 0; i < newSize; i++, p++)
				p->next = NULL;
		}

		// Copy old entries, free old table data
		if (oldEntries)
		{
			_Entry * p = oldEntries;
			for (UInt32 i = 0; i < oldSize; i++, p++)
				if (p->next)
					CopyEntry(p);
			FormHeap_Free(oldEntries);
		}
	}

public:

	tHashSet() : m_size(0), m_freeCount(0), m_freeOffset(0), m_entries(NULL), m_eolPtr(&sentinel) { }

	UInt32	Size() const		{ return m_size; }
	UInt32	FreeCount() const	{ return m_freeCount; }
	UInt32	FillCount() const	{ return m_size - m_freeCount; }

	class Iterator
	{
		_Entry * m_cur;
		_Entry * m_end;

		void Forward(bool inc)
		{
			if (IsDone())
				return;

			if (inc)
				m_cur++;

			while (!IsDone() && m_cur->IsFree())
				m_cur++;
		}

	public:

		Iterator(_Entry * start, _Entry * end) : m_cur(start), m_end(end)
		{
			if (!m_cur)
			{
				m_end = NULL;
				return;
			}
				
			Forward(false);
		}

		Iterator operator++()		{ Forward(true); return *this; }
		const Item * operator->()	{ return m_cur ? const_cast<const Item*>(&m_cur->item) : NULL; }
		const Item * operator*()	{ return m_cur ? const_cast<const Item*>(&m_cur->item) : NULL; }
		Item * Get()				{ return m_cur ? &m_cur->item : NULL; }
		bool IsDone()				{ return m_cur >= m_end; }
	};
	
	const Iterator Entries() const
	{
		return Iterator(m_entries, (_Entry*) (((UInt32) m_entries) + sizeof(_Entry) * m_size));
	}

	template <typename KeyType>
	Item * Find(KeyType key)
	{
		if (!m_entries)
			return NULL;

		_Entry * entry = GetEntry(Item::CalcHash(key));
		if (! entry->next)
			return NULL;

		while (! entry->item.Matches(key))
		{
			entry = entry->next;
			if (entry == m_eolPtr)
				return NULL;
		}

		return &entry->item;
	}

	void Add(Item * item)
	{
		while (!Insert(item))
			Grow();
	}

	template <typename KeyType>
	bool Remove(KeyType key)
	{
		if ( !m_entries)
			return false;

		_Entry * entry = GetEntry(Item::CalcHash(key));
		if (! entry->next)
			return NULL;

		_Entry * prevEntry = NULL;
		while (! entry->item.Matches(key))
		{
			prevEntry = entry;
			entry = entry->next;
			if (entry == m_eolPtr)
				return false;
		}

		// Remove tail?
		_Entry * nextEntry = entry->next;
		if ( nextEntry == m_eolPtr )
		{
			if (prevEntry)
				prevEntry->next = m_eolPtr;
			entry->next = 0;
		}
		else
		{
			entry->next = NULL;
			memcpy_s(entry, sizeof(_Entry), nextEntry, sizeof(_Entry));
			nextEntry->next = NULL;
		}

		++m_freeCount;
		return true;
	}

	void Clear()
	{
		if (m_entries)
		{
			_Entry * p = m_entries;
			for (UInt32 i = 0; i < m_size; i++, p++)
				p->next = NULL;
		}
		else
		{
			m_size = 0;
		}
		m_freeCount = m_freeOffset = m_size;
	}
};
STATIC_ASSERT(sizeof(tHashSet<void*>) == 0x1C);

template <typename Item>
typename tHashSet<Item>::_Entry tHashSet<Item>::sentinel = tHashSet<Item>::_Entry();



class HandleTableItem
{
public:
	UInt64		handle;		// 000

	HandleTableItem() : handle(0) { }
	HandleTableItem(UInt64 a_handle) : handle(a_handle) { }

	static UInt32 CalcHash(UInt64 a_handle)
	{
		UInt32 hash;
		CRC32_Calc8(&hash, (UInt64) a_handle);
		return hash;
	}

	UInt32 GetHash() const
	{
		UInt32 hash;
		CRC32_Calc8(&hash, handle);
		return hash;
	}

	bool Equals(HandleTableItem * item) const
	{
		return item->handle == handle;
	}

	bool Matches(UInt64 a_handle) const
	{
		return handle == a_handle;
	}
};

typedef tHashSet<HandleTableItem> HandleTable;