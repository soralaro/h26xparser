#include "media/h264/h264-util.h"
#include "media/h264/h264-parser.h"
#include "media/h265/h265-sps.h"
#include <string>
#include "h26X_div.h"
typedef unsigned char uint8_t;  

int main(){
     H26X_DIV h26XDiv;
     unsigned int pb_frame_num_=0;
     unsigned int i_frame_num_=0;
     bool only_dec_iframe_=false;
     unsigned int sn=0;
     h26XDiv.init("vlc.h264");
    uint8_t *buffer = new uint8_t[CACH_LEN];
    uint8_t *iFrame_bufer = new uint8_t[CACH_LEN];
    int iFrame_len = 0;   
    int len = 0;
    int send_len = 0;

    while (1) {
        if (iFrame_len == 0) {
            if ((len = h26XDiv.getOneNal(buffer + send_len, CACH_LEN - send_len)) <= 0) {
                break;
            }
            int type;

            type = h26XDiv.checkNal(buffer + send_len, len);

            send_len += len;
            if (type == NALU_TYPE_SPS || type == NALU_TYPE_SEI || type == NALU_TYPE_PPS) {
                continue;
            }
            if (type == NALU_TYPE_SLICE) {
                pb_frame_num_++;
            } else if (type == NALU_TYPE_IDR) {
                i_frame_num_++;
            }
            if (only_dec_iframe_) {
                if (type != NALU_TYPE_IDR) {
                    send_len = 0;
                    continue;
                } else {
                    memcpy(iFrame_bufer, buffer, send_len);
                    iFrame_len = send_len;
                }
            }

        } else {
            memcpy(buffer, iFrame_bufer, iFrame_len);
            send_len = iFrame_len;
            i_frame_num_++;
            sn++;
        }
        h264_parser_t *handle = h264_parser_create();
        h264_parser_input(handle,buffer,send_len);

        //get stream info such as color range info
        if(handle->ctx._sps->vui_parameters_present_flag > 0){
            printf("handle->ctx._sps->vui.video_full_range_flag %d \n",handle->ctx._sps->vui.video_full_range_flag );
            if(handle->ctx._sps->vui.video_full_range_flag > 0){
                //color range full
            }else{
                //color range limited
            }
        }else{
            //can not parse vui param from h264 stream
        }
        h264_parser_destroy(handle);
    }
}