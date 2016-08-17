/* please do not change this file ,
 * auto create by program 
 * create time 2016-8-17 16:11:1 
 */

#ifndef _AUTO_MACRO_DEFINE_H_
#define _AUTO_MACRO_DEFINE_H_
namespace NetMessage 
{
#define 	 ROLE_NAME_LENGTH  		40 		//��ɫ���ֳ��� 

#define 	 EQUIP_SLOT_NUMBER  		10 		//װ�������� 

#define 	 ROLE_ATTRS_NUMBER  		200 		//��ɫ���Ը��� 

#define 	 ROLE_ATTRS_ON_CREATE  		5 		//��ɫ����ʱ���ͻ��˷��͸������������Ը��� 

#define 	 PACKAGE_CAPACITY  		500 		//�������� 

#define 	 EQUIP_PACK_CAPACITY  		100 		//������������ 

#define 	 SKILL_CAPACITY  		20 		//�������� 

#define 	 MAX_MAP_INSTANCE_NUMBER  		200 		//��ͼ�������� 

#define 	 CLIENT_SETTING_SIZE  		4096 		//�ͻ������û����С 

#define 	 RANK_CAPACITY  		100 		//���а����� 

#define 	 PVP_TIME_LIMITED  		180 		//PVPʱ������s 

#define 	 DROP_ATTR_CAPACITY  		8 		//�������Խ������� 

#define 	 DROP_ITEM_CAPACITY  		10 		//һ����������Ʒ���� 

#define 	 PVP_MATCH_ROLE_NUM  		3 		//һ��ƥ��3�� 

#define 	 PVP_RANK_PAGE_SIZE  		20 		//ÿһҳ�ĸ��� 

#define 	 DRAP_WEIGHT_CAPACIT  		1024 		//�������Ȩ�صĸ��� 

#define 	 EQUIP_ATTR_NUMBER  		6 		//װ���������Ը��� 

#define 	 WEAPON_NUMBER  		2 		//�������� 

#define 	 DRAGON_EQUIP_SLOT_NUM  		8 		//����װ�����ĸ��� 

#define 	 DRAGON_SKILL_SLOT_NUM  		5 		//���ı������ܲ�λ 

#define 	 DRAGON_PASSIVE_SKILL_TYPE  		4 		//��Ӱ������ 

#define 	 SKILLS_NUM_PER_DRAGON  		4 		//ÿ�����еļ�������-�������� 

#define 	 WEAPON_MAX_ITEM_ID  		1000 		//�������� 

#define 	 TABLET_NUMBER  		128 		//ʯ���ڵ���� 

#define 	 DRAGON_CAPACITY  		40 		//���������ӵ�е��������� 

#define 	 PVP_MAP_DRAGON_CAPACITY  		5 		//PVP��ͼ��ҿ��Բ��õ��������� 

#define 	 SHOP_ITEMS_CAPACITY  		20 		//�̵���һ��ˢ����Ʒ������ 

#define 	 SHOPS_NUMBER  		20 		//��Ϸ���̵���� 

#define 	 MONSTERS_NUMBER_PER_SCENE  		100 		//ÿ���������������� 

#define 	 MAIL_TITLE_LENGTH  		40 		//�ʼ����ⳤ�� 

#define 	 MAIL_BODY_LENGTH  		256 		//�ʼ����ݳ��� 

// ��ͨ�������� 
enum eCommonErrorType { 
	COMMON_ERROR_TYPE_SHORT_ITEM =1,//ȱ����Ʒ param ��Ʒid
	COMMON_ERROR_TYPE_ATTR_LOW,//����̫�� param ��������
	COMMON_ERROR_TYPE_ATTR_HIGHT,//����̫�� param ��������
	COMMON_ERROR_TYPE_IN_CD,//cd��
	COMMON_ERROR_TYPE_IN_TABLE,//���ش������ж���Ĵ��� param �����
	COMMON_ERROR_TYPE_NEED_PASS_MAP,//��Ҫ����ض�����
};


enum eEquipStatBitsType { 
	EQUIPT_STAT_IN_DRESS =1,//�Ƿ���
	EQUIPT_STAT_NO_PARSE =2,//��ֹ�ֽ�
	EQUIPT_STAT_NO_SAIL =4,//��ֹ����
	EQUIPT_STAT_IN_CALLING =8,//���ٻ���
	EQUIPT_STAT_TAKE_OUT =16,//������ս
};


// ��Ʒ���� 
enum eItemType { 
	EITEM_TYPE_UNKNOWN =0,//δ֪����
	EITEM_TYPE_EQUIP =1,//װ��
	EITEM_TYPE_WEAPON,//weapon
	EITEM_TYPE_DEFANCE,//��
	EITEM_TYPE_DRAGON,//dragon
	EITEM_TYPE_DNA,//dna
	EITEM_TYPE_CONSUME,//����Ʒ
	EITEM_TYPE_NUMBER,//number
};


// ��������������� 
enum eAccelerateCD { 
	EACCEL_WEAPON_TABLET =0,//ʯ��
	EACCEL_NUMBER,//������������
};


// ����������� 
enum eTaskCompletedType { 
	ETASK_COMPLTET_UNKNOW =0,//δ֪����
	ETASK_COMPLTET_TIME,//�涨ʱ��
	ETASK_COMPLTET_VIP,//vip�Ǽ�
	ETASK_COMPLTET_MAKE_WISH_DRAGON,//��Ը����
	ETASK_COMPLTET_MAP_TYPE,//�ض����͸���
	ETASK_COMPLTET_MAP_ID,//�ض�id����
	ETASK_COMPLTET_ARENA,//������
	ETASK_COMPLTET_FEED_DRAGON,//ι��
	ETASK_COMPLTET_GOOD_FINGER,//ʹ�õ���
	ETASK_COMPLTET_BUY_IN_STORE,//�̵깺��
	ETASK_COMPLTET_BUY_IN_MALL,//�̳�����
	ETASK_COMPLTET_NUMBER,//number
};


// �ɾ�������� 
enum eAchievementCompletedType { 
	EACHIEV_COMPLTET_UNKNOW =0,//δ֪����
	EACHIEV_COMPLTET_ATTR,//���Դ��
	EACHIEV_COMPLTET_ITEM_TYPE,//�ռ���Ʒ
	EACHIEV_COMPLTET_ITEM_ID,//�ռ�ָ��������Ʒ
	EACHIEV_COMPLTET_MAP_TYPE,//�ض����͸���
	EACHIEV_COMPLTET_MAP_ID,//�ض�id����
	EACHIEV_COMPLTET_DRAGON_UPGRADE,//������
	EACHIEV_COMPLTET_EQUIP_UPGRADE,//����װ��
	EACHIEV_COMPLTET_WEAPON_UPGRADE,//��������
	EACHIEV_COMPLTET_BUY_IN_STORE,//�̵깺��
	EACHIEV_COMPLTET_BUY_IN_MALL,//�̳�����
	EACHIEV_COMPLTET_NUMBER,//number
};


};
#endif
