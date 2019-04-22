#include "xl_c_hexdump.h"


void hexdumpf(const void *block, int size, int cc, char isofst,FILE* pf)
{
    typedef unsigned char *ucptr;
    const int adsl = sizeof(char*) * 2; //地址字符串的长度
    const int spl = (2 * cc) + (cc - 1); //数据和数据间分隔符的长度
    size_t ls = 0;  //行长度
    size_t as = 0; //总长度
    size_t iy, y;   //行数
    ucptr mem;
    ucptr db, de; //块数据范围
    ucptr icx;    //mem迭代器
    ucptr mab;     //ascii编码显示区开头
    ucptr mend;
    unsigned char uc;

    //地址前缀
    ls += sizeof("0x") - 1;
    //地址字符串长度
    ls += adsl;
    //2个空格作为分隔符
    ls += sizeof("  ") - 1;
    //数据和数据间分隔符的长度
    ls += spl;
    //2个空格作为分隔符
    ls += sizeof("  ") - 1;
    //可见的ascii编的数目
    ls += cc;
    //换行符
    ls += 1;

    //计算行
    if (size % cc)
        y = size / cc + 1;
    else
        y = size / cc;

    //offset行
    if (isofst)
        as += ls;
    //其他数据行
    as += y * ls;
    //NUL长度
    as += 1;


    mem = (ucptr)malloc(as);

    /////////////////////处理偏移标尺//////////////////////////
    if (isofst)
    {
        icx = (ucptr)"offset";
        db = mem;
        de = db + 2 + adsl;
        uc = sizeof("offset") - 1;
        mab = mem + (2 + adsl - uc) / 2;
        mend = mab + uc - 1;

        //显示offset字符串
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
        uc = 0; //最多显示256列

        //显示列号
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
        //地址
        //sprintf((char*)icx, fmt, db);

        *icx++ = '0';
        *icx++ = 'x';

        //为了在不同系统上显示统一的地址格式，所以手动处理
        mab = (ucptr)&db + sizeof(db) - 1;
        for (; mab >= (ucptr)&db; --mab)
        {
            uc = *mab;
            uc >>= 4;
            *icx++ = uc + "\x30\x37"[uc > 9];
            uc = *mab & 0xf;
            *icx++ = uc + "\x30\x37"[uc > 9];
        }
        //分割符
        *icx++ = ' ';
        *icx++ = ' ';

        //块数据
        de = db + cc;
        //指向ascii编码显示区
        mab = icx + spl + (sizeof("  ") - 1);
        for (; db != de; ++db)
        {
            uc = *db;
            if (db < mend)
            {
                //写入标准ascii码可见字符
                if (uc <= 0x20)
                    *mab++ = '.';
                else
                    *mab++ = uc;

                //把*db转换为ascii的十六进制
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

        //分割符
        *icx++ = ' ';
        //行尾
        icx = mab;

        //换行符
        *icx++ = '\n';


    }
    //结束符
    *icx++ = '\0';


    fwrite(mem, as - 1, 1, pf);


    free(mem);

}
