//
// Created by chenzhenxiong on 4/28/20.
//

#ifndef VEGACUDA_H26X_DIV_H
#define VEGACUDA_H26X_DIV_H
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif
class H26X_DIV {
        typedef unsigned char uint8_t;     //无符�?8位数
#define H264_TYPE 0        
#define H265_TYPE 1        
#define NALU_TYPE_SLICE 1
#define NALU_TYPE_DPA 2
#define NALU_TYPE_DPB 3
#define NALU_TYPE_DPC 4
#define NALU_TYPE_IDR 5
#define NALU_TYPE_SEI 6
#define NALU_TYPE_SPS 7
#define NALU_TYPE_PPS 8
#define NALU_TYPE_AUD 9
#define NALU_TYPE_EOSEQ 10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL 12
#define NALU_H265_VPS   0x4001
#define NALU_H265_SPS   0x4201
#define NALU_H265_PPS   0x4401
#define NALU_H265_SEI   0x4e01
#define NALU_H265_IDR   0x2601
#define NALU_H265_PB    0x0201

#define CACH_LEN (1024*1024*3)//缓冲区不能设置太小，如果出现比某一帧比缓冲区大，会被覆盖掉一部分

    public:
        enum nalu_type {SLICE,IDR,PB,SPS,PPS,AUD,VPS,SEI};
        H26X_DIV() {
            g_cach[0] = nullptr;
            g_cach[1] = nullptr;
            fp_inH264 = nullptr;
            icach = 0;
            ioffset = 0;
            bLoop = 0;
            I_count = 0;
            PB_count = 0;
            All_count = 0;
            SPS_count = 0;
            PPS_count = 0;
            AUD_count = 0;
            file_end = 0;
        };

        ~H26X_DIV() {};
    private:
        uint8_t *g_cach[2];
        FILE *fp_inH264;
        int icach = 0;
        int ioffset = 0;
        int bLoop = 0;
        int I_count = 0;
        int PB_count = 0;
        int All_count = 0;
        int SPS_count = 0;
        int PPS_count = 0;
        int AUD_count = 0;
        bool file_end = 0;
        int file_end_len = 0;
    private:
        inline int min(int a, int b) {
            return ((a) < (b) ? (a) : (b));
        }

        int checkFlag(uint8_t *buffer, int offset) {
            static uint8_t mMark[4] = {0x00, 0x00, 0x00, 0x01};
            return !memcmp(buffer + offset, mMark, 4);
        }

    public:
        nalu_type checkNal(uint8_t *nal_buf, int len,int h26x) {
            nalu_type nal_type = SLICE;
            if (h26x == H264_TYPE){
                uint8_t nalHeader = nal_buf[4];
                char type = (nalHeader & 0x1f);
                switch (type)
                {
                case NALU_TYPE_SPS:
                    SPS_count++;
                    nal_type =SPS;
                    break;
                case NALU_TYPE_PPS:
                    PPS_count++;
                    nal_type =PPS;
                    break;
                case NALU_TYPE_IDR:
                    I_count++;
                    nal_type =IDR;
                    break;
                case NALU_TYPE_SLICE:
                    PB_count++;
                    nal_type=PB;
                    break;
                case NALU_TYPE_AUD:
                    AUD_count++;
                    break;
                case NALU_TYPE_SEI:
                    nal_type=SEI;
                    break;
                default:
                    printf("nalu type %x\n", type);
                    break;
                }
                if (IsH264KeyFrame(nal_buf, len))
                {
                    nal_type = IDR;
                    I_count++;
                }
            }else if(h26x==H265_TYPE){
                uint16_t nalHeader = nal_buf[4];
                nalHeader = (nalHeader<<8)+nal_buf[5];
                switch (nalHeader){
                case NALU_H265_VPS:
                    nal_type=VPS;
                    break;
                case NALU_H265_IDR:
                    nal_type=IDR;
                    I_count++;
                    break;
                case NALU_H265_PPS:
                    nal_type=PPS;
                    PPS_count++;
                    break;
                case NALU_H265_SPS:
                    nal_type=SPS;
                    SPS_count++;
                    break;
                case NALU_H265_PB:
                    nal_type=PB;
                    PB_count++;
                    break;
                case NALU_H265_SEI:
                    nal_type=SEI;
                    PB_count++;
                    break;
                default:
                    break;
                }
            }
            return nal_type;
        }


        int init(std::string path) {
            if (g_cach[0] == nullptr) {
                g_cach[0] = (uint8_t *) malloc(CACH_LEN);
            }
            if (g_cach[1] == nullptr) {
                g_cach[1] = (uint8_t *) malloc(CACH_LEN);
            }

            if (fp_inH264 == nullptr) {
                fp_inH264 = fopen(path.c_str(), "r");
                if (fp_inH264 == nullptr) {
                    printf("fopen %s erro\n", path.c_str());
                    return -1;
                }
            }
            file_end = 0;
            if (fread(g_cach[icach], 1, CACH_LEN, fp_inH264) < CACH_LEN) {
                printf("intpufile too short . to decrease CACH_LEN \n");

                return -1;
            }
            return 0;
        }

        int deinit() {
            if (g_cach[0]) {
                free(g_cach[0]);
                g_cach[0] = nullptr;
            }
            if (g_cach[1]) {
                free(g_cach[1]);
                g_cach[1] = nullptr;
            }

            if (fp_inH264) {
                fclose(fp_inH264);
                fp_inH264 = nullptr;
            }
            return 0;

        }


//获取一个Nal�? buf, bufLen表示缓冲区最大可以容纳的数据
//返回实际的帧数据长度
        int getOneNal(uint8_t *buf, int bufLen) {
            int i = 0;
            int startpoint = ioffset;
            int endpoint = ioffset;
            for (i = ioffset + 4; i <= CACH_LEN - 4; i++) {
                if (checkFlag(g_cach[icach], i)) {
                    startpoint = ioffset;
                    endpoint = i;
                    break;
                }
            }
            if (file_end && i > CACH_LEN - 4) {
                endpoint = file_end_len;
                file_end = false;
            }
            if (endpoint - startpoint > 0) {
                int dataLen = endpoint - startpoint;
                if (bufLen < dataLen) {
                    printf("recive buffer too short , need %d bytes\n",  dataLen);
                }
                memcpy(buf, g_cach[icach] + startpoint, MIN(dataLen, bufLen));
                ioffset = endpoint;

                return MIN(dataLen, bufLen);
            } else {
                int oldLen = CACH_LEN - startpoint;
                memcpy(g_cach[(icach + 1) % 2], g_cach[icach] + startpoint, oldLen);

                int newLen = 0;
                newLen = fread(g_cach[(icach + 1) % 2] + oldLen, 1, CACH_LEN - (oldLen), fp_inH264);
                if (newLen < CACH_LEN - (oldLen)) {
                    if (bLoop) {
                        fseek(fp_inH264, 0, SEEK_SET);
                        ioffset = 0;
                        icach = 0;
                        auto fLen=fread(g_cach[icach], 1, CACH_LEN, fp_inH264);
                        if(fLen<CACH_LEN){
                           memset(g_cach[icach]+fLen,0,(CACH_LEN-fLen));
                        }
                        return getOneNal(buf, bufLen);
                    } else {
                        if (newLen <= 0) {
                            return -1;
                        }
                        memset(g_cach[(icach + 1) % 2] + oldLen + newLen, 0, CACH_LEN - (oldLen) - newLen);

                    }
                    file_end = true;
                    file_end_len = oldLen + newLen;

                }

                ioffset = 0;
                icach = (icach + 1) % 2;

                return getOneNal(buf, bufLen);
            }

        }

        bool IsH264KeyFrame(uint8_t *data, int32_t data_size) {
            bool bRet = false;
            uint32_t flag = 0xFFFFFFFF;
            bool bFoundStartCode = false;
            for (int i = 0; i < data_size; i++) {
                bFoundStartCode = false;
                flag = (flag << 8) + data[i];
                if (flag == 0x00000001) {
                    bFoundStartCode = true;
                } else if ((flag & 0xFFFFFF) == 0x000001) {
                    bFoundStartCode = true;
                }

                if (bFoundStartCode) {
                    uint8_t nalu = data[i + 1];
                    if ((nalu & 0x1F) == 0x05) {
                        bRet = true;
                        break;
                    }
                }
            }

            return bRet;
        }

        bool IsH265KeyFrame(uint8_t *data, int32_t data_size) {
            bool bRet = false;

            uint32_t flag = 0xFFFFFFFF;
            bool bFoundStartCode = false;
            for (int i = 0; i < data_size; i++) {
                bFoundStartCode = false;
                flag = (flag << 8) + data[i];
                if (flag == 0x00000001) {
                    bFoundStartCode = true;
                } else if ((flag & 0xFFFFFF) == 0x000001) {
                    bFoundStartCode = true;
                }

                if (bFoundStartCode) {
                    uint8_t nalu = data[i + 1];
                    uint8_t nalu_type = ((nalu & 0x7E) >> 1);
                    if (nalu_type >= 16 && nalu_type <= 23) {
                        bRet = true;
                        break;
                    }
                }
            }

            return bRet;
        }

        void div_info_print() {
            printf("All_count %d\n", All_count);
            printf("I_count %d\n", I_count);
            printf("PB_count %d\n", PB_count);
            printf("AUD_count %d\n", AUD_count);
            printf("SPS_count %d\n", SPS_count);
            printf("PPS_count %d\n", PPS_count);
        }
    };
#endif //VEGACUDA_H26X_DIV_H
