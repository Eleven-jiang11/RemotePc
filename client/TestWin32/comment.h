#pragma once


//cmd��������
#define PACKET_REQ_CMD  10001 //PACKET_REQ_CMD request cmd ��ʾcmd������������ �ͻ���-->�����
#define PACKET_RLY_CMD  10001 //PACKET_RLY_CMD reply cmd ��ʾcmd���ͻָ����� �����-->�ͻ���


//������������
#define PACKET_REQ_KEYBOARD  20001 //PACKET_REQ_KEYBOARD request KEYBOARD ��ʾKEYBOARD������������ �ͻ���-->�����
#define PACKET_RLY_KEYBOARD  20001 //PACKET_RLY_KEYBOARD reply KEYBOARD ��ʾKEYBOARD	���ͻָ����� �����-->�ͻ���
//����һЩ�����Ľṹ��
#pragma pack(push)
#pragma pack(1)
struct MyPacket
{
	unsigned int type; //��ʾ������
	unsigned int length; //��ʾ������
	char data[1];  //����һ��1�ֽڵ�����
};
#pragma pack(pop)