#include "h26xParserInterface.h"
#include "media/h264/h264-util.h"
#include "media/h264/h264-parser.h"
#include "media/h265/h265-sps.h"
#include <string.h>
class H26xParserT:public H26xParser{
    public:
    H26xParserT();
    virtual ~H26xParserT();
    virtual int getColorRang(const u_int8_t *frame,int len,H26x_Type type) override;
    virtual void init(H26x_Type type) override;
    private:
    h264_parser_t *handle_;
};
H26xParserT::H26xParserT(){
    handle_=nullptr;
}
H26xParserT::~H26xParserT(){
    if(!handle_){
        h264_parser_destroy(handle_);
    }
}
void H26xParserT::init(H26x_Type type){
    if(type==H26x_Type::H264){
        handle_ = h264_parser_create();
    }
}

int H26xParserT::getColorRang(const u_int8_t *frame,int len,H26x_Type type){
    if(type==H26x_Type::H265){
        const uint8_t *startCode = h264_startcode(frame,len);
        if(!startCode){
            //can not find h265 nalu
            return -1;
        }
        while(startCode){
            //find h265 sps startCode
            if((((*startCode) & 0x7E) >> 1) == 33){
                //parse sps
                struct h265_sps_t sps;
                memset(&sps,0x00,sizeof(h265_sps_t));
                int ret = h265_sps_parse((const void *)startCode,(uint32_t)len,&sps);
                if(ret < 0){
                    //impossible
                    return -1;
                }
                if(sps.vui_parameters_present_flag > 0){
                    //get color range
                    if(sps.vui.video_full_range_flag > 0){
                        //color range full
                        return 2;
                    }else{
                        //color range limited
                        return 1;
                    }
                }else{
                    return -1;
                    //can not get color range from h265 sps
                }
            }
            len = len - (startCode - frame);
            frame = startCode;
            startCode = h264_startcode(frame,len);
        }
        return -1;
    }else if(type==H26x_Type::H264){
        h264_parser_input(handle_,frame,len);
        if(handle_->ctx._sps->vui_parameters_present_flag > 0){
            if(handle_->ctx._sps->vui.video_full_range_flag > 0){
                return 2;
                //color range full
            }else{
                return 1;
                //color range limited
            }
        }else{
            return -1;
            //can not parse vui param from h264 stream
        }
    }
    return -1;
}

extern "C"{
    H26xParser *CreatH26xParser(){
        return new H26xParserT;
    }
    void DestroyH26xParser(H26xParser *h26xParser){
        H26xParserT *h26xParserT=dynamic_cast<H26xParserT *>(h26xParser);
        if(!h26xParserT)
            delete h26xParserT;
    }
}
