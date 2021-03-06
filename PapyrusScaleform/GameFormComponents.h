#pragma once

// everything deriving from BaseFormComponent but not TESForm goes here

#include "skse/GameTypes.h"

class TESObjectSTAT;
class BGSSoundDescriptorForm;
class BGSKeyword;
class TESForm;
class SpellItem;
class TESShout;
class IngredientItem;
class TESRace;
class EnchantmentItem;

//// root

// 04
class BaseFormComponent
{
public:
	BaseFormComponent();
	virtual ~BaseFormComponent();

	virtual void	Init(void);
	virtual void	ReleaseRefs(void);
	virtual void	CopyFromBase(BaseFormComponent * rhs);

	//	void	** _vtbl;	// 00
};

//// members of other components

// 08
class TESTexture : public BaseFormComponent
{
public:
	virtual UInt32			Unk_04(void);
	virtual void			GetNormalMapName(BSString * out);	// might have const char * retn type
	virtual const char *	GetSearchDir(void);

	StringCache::Ref	str;	// 04
};

// 08
class TESIcon : public TESTexture
{

};

// 08
class TESTexture1024 : public TESTexture
{
	
};

//// leaf nodes

// 08
class BGSAttackDataForm : public BaseFormComponent
{
public:
	BSTSmartPointer	<void>	unk04;	// 04
};

// 10
class BGSBipedObjectForm : public BaseFormComponent
{
public:
	// applicable to DefaultRace
	enum
	{
		kPart_Head =		1 << 0,
		kPart_Hair =		1 << 1,
		kPart_Body =		1 << 2,
		kPart_Hands =		1 << 3,
		kPart_Forearms =	1 << 4,
		kPart_Amulet =		1 << 5,
		kPart_Ring =		1 << 6,
		kPart_Feet =		1 << 7,
		kPart_Calves =		1 << 8,
		kPart_Shield =		1 << 9,
		kPart_Unnamed10 =	1 << 10,
		kPart_LongHair =	1 << 11,
		kPart_Circlet =		1 << 12,
		kPart_Ears =		1 << 13,
		kPart_Unnamed14 =	1 << 14,
		kPart_Unnamed15 =	1 << 15,
		kPart_Unnamed16 =	1 << 16,
		kPart_Unnamed17 =	1 << 17,
		kPart_Unnamed18 =	1 << 18,
		kPart_Unnamed19 =	1 << 19,
		kPart_Unnamed20 =	1 << 20,
		kPart_Unnamed21 =	1 << 21,
		kPart_Unnamed22 =	1 << 22,
		kPart_Unnamed23 =	1 << 23,
		kPart_Unnamed24 =	1 << 24,
		kPart_Unnamed25 =	1 << 25,
		kPart_Unnamed26 =	1 << 26,
		kPart_Unnamed27 =	1 << 27,
		kPart_Unnamed28 =	1 << 28,
		kPart_Unnamed29 =	1 << 29,
		kPart_Unnamed30 =	1 << 30,
		kPart_FX01 =		1 << 31,
	};

	enum
	{
		kWeight_Light =	0,
		kWeight_Heavy,
		kWeight_None,
	};

	struct Data
	{
		UInt32	parts;			// 00 - init'd to 0
		UInt8	unk04;			// 04 - init'd to 0
		UInt8	pad05[3];		// 05
		UInt32	weightClass;	// 08 - init'd to 2 (none)
	};

	Data	data;	// 04

	UInt32 GetSlotMask() const { return data.parts; }
	void SetSlotMask(UInt32 mask) { data.parts = mask; }
	UInt32 AddSlotToMask(UInt32 mask);
	UInt32 RemoveSlotFromMask(UInt32 mask);
};

// 0C
class BGSBlockBashData : public BaseFormComponent
{
public:
	UInt32	unk04;	// 04
	UInt32	unk08;	// 08
};

// 08
class BGSDestructibleObjectForm : public BaseFormComponent
{
public:
	// 14
	struct Data
	{
		UInt32	unk00;		// 00
		UInt8	unk04;		// 04
		UInt8	unk05;		// 05
		UInt8	pad06[2];	// 06
		UInt32	unk08;		// 08
		UInt32	unk0C;		// 0C
		UInt32	unk10;		// 10 - object
	};

	Data	* data;	// 04
};

// 08
class BGSEquipType : public BaseFormComponent
{
public:
	virtual	UInt32	GetEquipType(void);
	virtual void	SetEquipType(UInt32 type);

	UInt32	unk04;	// 04
};

// 10
class BGSIdleCollection : public BaseFormComponent
{
public:
	UInt8	unk04;		// 04
	UInt8	unk05;		// 05 - unk08 length
	UInt8	unk06[2];	// 06
	void	** unk08;	// 08 - copyable struct
	UInt32	unk0C;		// 0C
};

// 0C
class BGSKeywordForm : public BaseFormComponent
{
public:
	virtual bool	HasKeyword(UInt32 keyword);
	virtual UInt32	GetDefaultKeyword(void);

	BGSKeyword	** keywords;	// 04
	UInt32		numKeywords;	// 08
};

// 08
class BGSMenuDisplayObject : public BaseFormComponent
{
public:
	virtual UInt32	GetMenuDisplayObject(void);

	TESObjectSTAT	* worldStatic;	// 04 - copyable
};

// 0C
class BGSMessageIcon : public BaseFormComponent
{
public:
	TESIcon	icon;	// 04
};

// 14
class BGSOverridePackCollection : public BaseFormComponent
{
public:
	UInt32	unk04;	// 04
	UInt32	unk08;	// 08
	UInt32	unk0C;	// 0C
	UInt32	unk10;	// 10
};

// 0C
class BGSPerkRankArray : public BaseFormComponent
{
public:
	// 08
	struct Data
	{
		UInt32	unk00;		// 00
		UInt8	unk04;		// 04
		UInt8	pad05[3];	// 05
	};

	Data	* perkRanks;	// 04
	UInt32	numPerkRanks;	// 08
};

// 0C
class BGSPickupPutdownSounds : public BaseFormComponent
{
public:
	BGSSoundDescriptorForm*	pickUp;	// 04
	BGSSoundDescriptorForm*	putDown;	// 08
};

// 04
class BGSPreloadable : public BaseFormComponent
{
public:
	virtual void	Unk_04(void);	// pure virtual
};

// 08
class BGSSkinForm : public BaseFormComponent
{
public:
	UInt32	unk04;	// 04
};

// 18
class TESAIForm : public BaseFormComponent
{
public:
	// 8 - might be ExtraDataList
	struct Data
	{
		UInt32	unk0;	// 0
		Data	* next;	// 4
	};

	UInt32	flags;		// 04
	UInt16	unk08[3];	// 08
	UInt8	pad0D[2];	// 0D
	Data	unk10;		// 10
};

// 38
class TESActorBaseData : public BaseFormComponent
{
public:
	virtual void	Unk_04(UInt32 arg);
	virtual bool	GetFlag20000000(void);
	virtual bool	GetFlag80000000(void);
	virtual bool	Unk_07(void);
	virtual UInt32	GetUnk20(void);

	UInt32	flags;	// 04 - init'd to 0
	UInt16	unk08;	// 08 - init'd to 0
	UInt16	unk0A;	// 0A - init'd to 0
	UInt16	unk0C;	// 0C - init'd to 1
	UInt16	unk0E;	// 0E - init'd to 0
	UInt16	unk10;	// 10 - init'd to 0
	UInt16	unk12;	// 12 - init'd to 0x64 (100)
	UInt16	unk14;	// 14 - init'd to iBaseDisposition (35)
	UInt16	unk16;	// 16 - init'd to 0
	UInt16	unk18;	// 18 - init'd to 0
	UInt16	unk1A;	// 1A - init'd to 0
	UInt32	unk1C;	// 1C
	UInt32	unk20;	// 20
	UInt32	unk24;	// 24
	UInt32	unk28;	// 28 - init'd to 0
	UnkArray	unk2C;	// 2C
};

// 8
class TESAttackDamageForm : public BaseFormComponent
{
public:
	virtual UInt16	GetAttackDamage(void);

	UInt16	attackDamage;	// 04
	UInt8	pad06[2];		// 06
};

// 14
class TESModel : public BaseFormComponent
{
public:
	virtual const char *	GetModelName(void);
	virtual void			SetModelName(const char * name);
	virtual UInt32			Unk_06(void);

	StringCache::Ref	name;	// 04

	UInt32		unk08;	// 08
	UInt32		unk0C;	// 0C
	UInt16		unk10;	// 10
	UInt8		unk12;	// 12
	UInt8		unk13;	// 13
};

// 1C
class TESModelTextureSwap : public TESModel
{
public:
	UInt32	unk14;	// 14
	UInt32	unk18;	// 18
};

// 14
class TESModelRDT : public TESModel
{
public:
};

// 78
class TESBipedModelForm : public BaseFormComponent
{
public:
	TESModelTextureSwap	textureSwap[2];	// 04
	TESIcon				icon[2];		// 3C
	BGSMessageIcon		messageIcon[2];	// 4C
	TESModelRDT			modelRDT;		// 64
};

// 0C
class TESContainer : public BaseFormComponent
{
public:
	struct Entry
	{
		struct Data
		{
			UInt32	unk00;
			UInt32	unk04;
			float	unk08;
		};

		UInt32	count;
		TESForm* form;
		Data	* data; // extra data?
	};

	Entry	** entries;	// 04
	UInt32	numEntries;	// 08

	template <class Op>
	UInt32 CountIf(Op& op) const
	{
		UInt32 count = 0;
		for (UInt32 n = 0; n < numEntries; n++) {
			Entry* pEntry = entries[n];
			if (pEntry && op.Accept(pEntry))
				count++;
		}
		return count;
	}

	template <class Op>
	Entry* Find(Op& op) const
	{
		bool bFound = false;
		UInt32 n = 0;
		Entry* pEntry = NULL;
		for (UInt32 n = 0; n < numEntries && !bFound; n++) {
			pEntry = entries[n];
			if (pEntry) {
				bFound = op.Accept(pEntry);
			}
		}
		return (bFound && pEntry) ? pEntry : NULL;
	}


};

// 0C
class TESDescription : public BaseFormComponent
{
public:
	UInt32	unk04;	// 04
	UInt32	unk08;	// 08 - init'd to FFFFFFFF
};

// C
class TESEnchantableForm : public BaseFormComponent
{
public:
	virtual UInt32	Unk_04(void);	// return unk08

	EnchantmentItem*	enchantment;		// 04 - init'd to 0
	UInt32	unk08;		// 08 - init'd to 3
};

// 08
class TESFullName : public BaseFormComponent
{
public:
	virtual UInt32	Unk_04(void);
	virtual UInt32	Unk_05(void);

	StringCache::Ref	name;	// 04
};

// 08
class TESImageSpaceModifiableForm : public BaseFormComponent
{
public:
	UInt32	unk04;	// 04
};

// 14
class TESLeveledList : public BaseFormComponent
{
public:
	struct Entry
	{
		UInt32	unk0;	// 00 - init'd to 0
		UInt16	unk4;	// 04 - init'd to 1
		UInt16	unk6;	// 06 - init'd to 1
		UInt32	unk8;	// 08 - init'd to 0
	};

	enum
	{
		kFlag_0 =	1 << 0,
	};

	virtual bool	Unk_04(void);
	virtual bool	GetFlag02(void);
	virtual UInt32	Unk_06(void);
	virtual void	Unk_07(void);	// pure

	Entry	* entries;	// 04
	UInt8	unk08;		// 08
	UInt8	flags;		// 09
	UInt8	length;		// 0A
	UInt8	unk0B;		// 0B
	void	* unk0C;	// 0C
	UInt32	unk10;		// 10
};

// 14
class BGSBehaviorGraphModel : public TESModel
{
public:
};

// 14
class BGSTextureModel : public TESModel
{
public:
};

// 14
class TESModelPSA : public TESModel
{
public:
};

// 14
class TESModelTri : public TESModel
{
public:
};

// 10?
class TESProduceForm : public BaseFormComponent
{
public:
	BGSSoundDescriptorForm*	harvestSound;		// 04
	IngredientItem*	produce;		// 08
	UInt8	unk0C[4];	// 0C
};

// 08
class TESQualityForm : public BaseFormComponent
{
public:
	UInt32	unk04;		// 04
};

// 08
class TESRaceForm : public BaseFormComponent
{
public:
	TESRace	* race;	// 04
};

// 10
class TESReactionForm : public BaseFormComponent
{
public:
	// 0C
	struct Data
	{
		UInt32	unk0;	// 00
		UInt32	unk4;	// 04
		UInt32	unk8;	// 08
	};

	// 08
	struct Entry
	{
		Data	* data;	// 00
		Entry	* unk4;	// 04
	};

	Entry	entry;		// 04
	UInt8	unk0C;		// 0C
	UInt8	pad0D[3];	// 0D
};

// 08
class TESSpellList : public BaseFormComponent
{
public:
	UInt32 GetSpellCount() const;
	SpellItem* GetNthSpell(UInt32 n) const;

	UInt32 GetShoutCount() const;
	TESShout* GetNthShout(UInt32 n) const;

	struct Data
	{
		SpellItem**	spells;	// SpellItem**  (null terminated array of spells)
		void	* unk4;
		TESShout**	shouts;
		UInt32		numSpells;
		UInt32		numUnk4;
		UInt32		numShouts;
	};

	Data	* unk04;	// 04
};

// 08
class TESValueForm : public BaseFormComponent
{
public:
	UInt32	value;	// 04
};

// 08
class TESWeightForm : public BaseFormComponent
{
public:
	float	weight;	// 04
};

//// the following are not form components
//// they live here until there's a better header file

// 40
class BSMaterialObject
{
public:
	// 30
	struct Data
	{
		// 0C
		struct Data10
		{
			UInt32	unk0;	// 0 - init'd to 0
			UInt32	unk4;	// 4 - init'd to 0
			float	unk8;	// 8 - init'd to -1
		};

		float	unk00;	// 00 - init'd to 0.4
		float	unk04;	// 04 - init'd to 0.4
		float	unk08;	// 08 - init'd to 512
		float	unk0C;	// 0C - init'd to 512
		Data10	unk10;	// 10
		float	unk1C;	// 1C - init'd to 1
		UInt32	unk20;	// 20
		UInt32	unk24;	// 24
		UInt32	unk28;	// 28
		UInt32	unk2C;	// 2C
	};

	BSMaterialObject();
	virtual ~BSMaterialObject();

	virtual void	Unk_01(void);

//	void	** _vtbl;	// 00
	
	Data	unk04;		// 04
	UnkArray	unk34;	// 34
};

STATIC_ASSERT(sizeof(BSMaterialObject) == 0x40);

// 04
class TESChildCell
{
public:
	virtual ~TESChildCell();

	virtual	UInt32	Unk_01(void);

//	void	** _vtbl;
};

// 04
class ActorValueOwner
{
public:
	virtual ~ActorValueOwner();

	virtual float	Unk_01(UInt32 arg);
	virtual float	Unk_02(UInt32 arg);
	virtual float	Unk_03(UInt32 arg);
	virtual void	Unk_04(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_05(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_06(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void	Unk_07(UInt32 arg0, UInt32 arg1);
	virtual bool	Unk_08(void);

//	void	** _vtbl;	// 00
};

// 08
class BSIMusicTrack
{
public:
	BSIMusicTrack();
	virtual ~BSIMusicTrack();

	virtual void	Unk_01(void);
	virtual void	Unk_02(void);
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual bool	Unk_07(void);
	virtual UInt32	Unk_08(void);	// return unk04
	virtual void	Unk_09(UInt32 arg);
	virtual void	Unk_0A(void);

//	void	** _vtbl;	// 00
	UInt32	unk04;		// 04
};

// 30
class BSIMusicType
{
public:
	virtual void	Unk_00(void);
	virtual void	Unk_01(void);
	virtual void	Unk_02(void);
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual void	Unk_07(void);

//	void	** _vtbl;	// 00
	UInt32	unk04;		// 04
	UInt8	unk08;		// 08
	UInt8	unk09;		// 09
	UInt16	unk0A;		// 0A
	float	unk0C;		// 0C
	UInt32	unk10;		// 10
	UnkArray	unk14;	// 14
	UnkArray	unk20;	// 20
	UInt32	unk2C;		// 2C
};

// 04
class BSIReverbType
{
public:
	virtual void	Unk_00(void);
	virtual void	Unk_01(void);
	virtual void	Unk_02(void);
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual void	Unk_07(void);
	virtual void	Unk_08(void);
	virtual void	Unk_09(void);
	virtual void	Unk_0A(void);

//	void	** _vtbl;	// 00
};

// 04
class BSISoundCategory
{
public:
	virtual ~BSISoundCategory();

	virtual void	Unk_01(void);
	virtual void	Unk_02(void);
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual void	Unk_07(void);
	virtual void	Unk_08(void);
	virtual void	Unk_09(void);
	virtual void	Unk_0A(void);
	virtual void	Unk_0B(void);

//	void	** _vtbl;	// 00
};

// 04
class BSISoundDescriptor
{
public:
	virtual ~BSISoundDescriptor();

	virtual void	Unk_01(void);
	virtual void	Unk_02(void);

//	void	** _vtbl;	// 00
};

// 04
class BSISoundOutputModel
{
public:
	virtual ~BSISoundOutputModel();

	virtual void	Unk_01(void);
	virtual void	Unk_02(void);
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual void	Unk_07(void);
	virtual void	Unk_08(void);
	virtual void	Unk_09(void);

//	void	** _vtbl;	// 00
};

// 04
class BGSOpenCloseForm
{
public:
	virtual ~BGSOpenCloseForm();

//	void	** _vtbl;	// 00
};

// C
class MagicTarget
{
public:
	virtual ~MagicTarget();

//	void	** _vtbl;	// 00
	UInt32 unk04;		// 04
	UInt32 unk08;		// 08
};

STATIC_ASSERT(sizeof(MagicTarget) == 0xC);
