#include "xl_c_hexdump.h"


void hexdumpf(const void *block, int size, int cc, char isofst,FILE* pf)
{
    typedef unsigned char *ucptr;
    const int adsl = sizeof(char*) * 2; //��ַ�ַ����ĳ���
    const int spl = (2 * cc) + (cc - 1); //���ݺ����ݼ�ָ����ĳ���
    size_t ls = 0;  //�г���
    size_t as = 0; //�ܳ���
    size_t iy, y;   //����
    ucptr mem;
    ucptr db, de; //�����ݷ�Χ
    ucptr icx;    //mem������
    ucptr mab;     //ascii������ʾ����ͷ
    ucptr mend;
    unsigned char uc;

    //��ַǰ׺
    ls += sizeof("0x") - 1;
    //��ַ�ַ�������
    ls += adsl;
    //2���ո���Ϊ�ָ���
    ls += sizeof("  ") - 1;
    //���ݺ����ݼ�ָ����ĳ���
    ls += spl;
    //2���ո���Ϊ�ָ���
    ls += sizeof("  ") - 1;
    //�ɼ���ascii�����Ŀ
    ls += cc;
    //���з�
    ls += 1;

    //������
    if (size % cc)
        y = size / cc + 1;
    else
        y = size / cc;

    //offset��
    if (isofst)
        as += ls;
    //����������
    as += y * ls;
    //NUL����
    as += 1;


    mem = (ucptr)malloc(as);

    /////////////////////����ƫ�Ʊ��//////////////////////////
    if (isofst)
    {
        icx = (ucptr)"offset";
        db = mem;
        de = db + 2 + adsl;
        uc = sizeof("offset") - 1;
        mab = mem + (2 + adsl - uc) / 2;
        mend = mab + uc - 1;

        //��ʾoffset�ַ���
        while (db != de)
        {
            if (db >= mab && db <= mend)
                *db++ = *icx++;
            else
                *db++ = ' ';
        }

        *db++ = ' ';
        *db++ = ' ';

        de = db + spl;
        //*db++ = '#';//debug
        uc = 0; //�����ʾ256��

        //��ʾ�к�
        for (; db < de; db += 3, ++uc)
        {
            if (uc < 0x10)
            {
                db[0] = ' ';
                db[1] = uc + "\x30\x37"[uc > 9];
            }
            else
            {
                unsigned tmp = uc >> 4;
                db[0] = tmp + "\x30\x37"[tmp > 9];
                tmp = uc & 0xf;
                db[1] = tmp + "\x30\x37"[tmp > 9];
            }
            db[2] = ' ';
        }

        mend = mem + ls - 1;
        //*de++ = '@';  //debug
        while (de != mend)
            *de++ = ' ';

        *mend++ = '\n';
        icx = mend;
    }
    else
    {
        icx = mem;
    }

    ///////////////////////////////////////////////////////////
    //n + 3
    //n + 2
    db = (ucptr)block;
    mend = (ucptr)block + size;
    for (iy = 0; iy < y; ++iy)
    {
        //��ַ
        //sprintf((char*)icx, fmt, db);

        *icx++ = '0';
        *icx++ = 'x';

        //Ϊ���ڲ�ͬϵͳ����ʾͳһ�ĵ�ַ��ʽ�������ֶ�����
        mab = (ucptr)&db + sizeof(db) - 1;
        for (; mab >= (ucptr)&db; --mab)
        {
            uc = *mab;
            uc >>= 4;
            *icx++ = uc + "\x30\x37"[uc > 9];
            uc = *mab & 0xf;
            *icx++ = uc + "\x30\x37"[uc > 9];
        }
        //�ָ��
        *icx++ = ' ';
        *icx++ = ' ';

        //������
        de = db + cc;
        //ָ��ascii������ʾ��
        mab = icx + spl + (sizeof("  ") - 1);
        for (; db != de; ++db)
        {
            uc = *db;
            if (db < mend)
            {
                //д���׼ascii��ɼ��ַ�
                if (uc <= 0x20)
                    *mab++ = '.';
                else
                    *mab++ = uc;

                //��*dbת��Ϊascii��ʮ������
                uc >>= 4;
                *icx++ = uc + "\x30\x57"[uc > 9];
                uc = *db & 0xf;
                *icx++ = uc + "\x30\x57"[uc > 9];

            }
            else
            {
                *mab++ = '.';
                *icx++ = '0';
                *icx++ = '0';
            }

            *icx++ = ' ';
        }

        //�ָ��
        *icx++ = ' ';
        //��β
        icx = mab;

        //���з�
        *icx++ = '\n';


    }
    //������
    *icx++ = '\0';


    fwrite(mem, as - 1, 1, pf);


    free(mem);

}
