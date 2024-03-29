#include "compress.h"


typedef u_int8_t BYTE;

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr
              cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    LOGI("jpeg_message_table[%d]:%s",
         myerr->pub.msg_code, myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    longjmp(myerr
                    ->setjmp_buffer, 1);
}

int generateJPEG(BYTE *data, int w, int h, int quality, const char *name, boolean optimize) {
    int nComponent = 3;
    struct jpeg_compress_struct jcs;
    //自定义的error
    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;

    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    //为JPEG对象分配空间并初始化
    jpeg_create_compress(&jcs);
    //获取文件信息
    FILE *f = fopen(name, "wb");
    if (f == NULL) {
        return 0;
    }

    //指定压缩数据源
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;
    jcs.image_height = h;

    jcs.arith_code = false;
    jcs.input_components = nComponent;
    jcs.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;

    //为压缩设定参数，包括图像大小，颜色空间
    jpeg_set_quality(&jcs, quality, true);
    //开始压缩
    jpeg_start_compress(&jcs, true);
    JSAMPROW row_point[1];
    int row_stride;
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_point[0] = &data[jcs.next_scanline * row_stride];
        jpeg_write_scanlines(&jcs, row_point, 1);
    }

    if (optimize) {
        LOGI("使用了哈夫曼算法完成压缩");
    } else {
        LOGI("未使用哈夫曼算法");
    }
    //压缩完毕
    jpeg_finish_compress(&jcs);
    //释放资源
    jpeg_destroy_compress(&jcs);
    fclose(f);
    return 1;
}

char *jstringToString(JNIEnv *env, jstring jstr){
    char *ret;
    const char *tempStr = (*env)->GetStringUTFChars(env, jstr, NULL);
    jsize len = (*env)->GetStringUTFLength(env, jstr);
    if (len > 0) {
        ret = (char *) malloc(len + 1);
        memcpy(ret, tempStr, len);
        ret[len] = 0;
    }
    (*env)->ReleaseStringUTFChars(env, jstr, tempStr);
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_libjpeg_LibjpegUtil_nativeCompressBitmap(JNIEnv *env, jclass type,
                                                  jobject bitmap, jint quality,
                                                  jstring dstFile_,
                                                  jboolean optimize) {

    AndroidBitmapInfo androidBitmapInfo;
    BYTE *pixelsColor;
    int ret;
    BYTE *data;
    BYTE *tmpData;
    char *dstFileName = jstringToString(env, dstFile_);
    //解码Android Bitmap信息
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &androidBitmapInfo)) < 0) {
        LOGI("AndroidBitmap_getInfo() failed error=%d", ret);
        return ret;
    }
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, (void **) &pixelsColor)) < 0) {
        LOGI("AndroidBitmap_lockPixels() failed error=%d", ret);
        return ret;
    }

    LOGI("bitmap: width=%d,height=%d,size=%d , format=%d ",
         androidBitmapInfo.width, androidBitmapInfo.height,
         androidBitmapInfo.height * androidBitmapInfo.width,
         androidBitmapInfo.format);

    BYTE r, g, b;
    int color;

    int w, h, format;
    w = androidBitmapInfo.width;
    h = androidBitmapInfo.height;
    format = androidBitmapInfo.format;

    data = (BYTE *) malloc(w * h * 3);
    tmpData = data;
    // 将bitmap转换为rgb数据
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            //只处理 RGBA_8888
//            if (format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
                color = (*(int *) (pixelsColor));
                // 这里取到的颜色对应的 A B G R  各占8位
//                b = (color >> 16) & 0xFF;
//                g = (color >> 8) & 0xFF;
//                r = (color >> 0) & 0xFF;
//                *data = r;
//                *(data + 1) = g;
//                *(data + 2) = b;


                r = ((color & 0x00FF0000) >> 16);
                g = ((color & 0x0000FF00) >> 8);
                b = color & 0x000000FF;
                *data = b;
                *(data + 1) = g;
                *(data + 2) = r;

                data += 3;
                pixelsColor += 4;

//            } else {
//                return -2;
//            }
        }
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    //进行压缩
    ret = generateJPEG(tmpData, w, h, quality, dstFileName, optimize);
    free((void *) dstFileName);
    free((void *) tmpData);
    return ret;
}


/*typedef u_int8_t BYTE;
char *error;
struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr
              cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    LOGW("jpeg_message_table[%d]:%s",
         myerr->pub.msg_code, myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    longjmp(myerr
                    ->setjmp_buffer, 1);

}


int generateJPEG(BYTE* data, int w, int h, int quality,
                 const char* outfilename, jboolean optimize) {
    int nComponent = 3;

    struct jpeg_compress_struct jcs;

    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    jpeg_create_compress(&jcs);
    FILE* f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;
    jcs.image_height = h;
    if (optimize) {
        LOGI("optimize==ture");
    } else {
        LOGI("optimize==false");
    }
    jcs.arith_code = false;
    jcs.input_components = nComponent;
    if (nComponent == 1)
        jcs.in_color_space = JCS_GRAYSCALE;
    else
        jcs.in_color_space = JCS_RGB;

    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;
    jpeg_set_quality(&jcs, quality, true);

    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];

        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }
    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    fclose(f);

    return 1;
}

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb;

char* jstrinTostring(JNIEnv* env, jbyteArray barr) {
    char* rtn = NULL;
    jsize alen = (*env)->GetArrayLength(env, barr);
    jbyte* ba = (*env)->GetByteArrayElements(env, barr, 0);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
    return rtn;
}

jbyteArray stoJstring(JNIEnv* env, const char* pat,int len) {
    jbyteArray bytes = (*env)->NewByteArray(env, len);
    (*env)->SetByteArrayRegion(env, bytes, 0, len,  pat);
    jsize alen = (*env)->GetArrayLength(env, bytes);
    return bytes;
}
jstring Java_com_libjpeg_LibjpegUtil_nativeCompressBitmap(JNIEnv* env,
                                                                  jobject thiz, jobject bitmapcolor, int w, int h, int quality,
                                                                  jbyteArray fileNameStr, jboolean optimize) {

    AndroidBitmapInfo infocolor;
    BYTE* pixelscolor;
    int ret;
    BYTE * data;
    BYTE *tmpdata;
    char * fileName = jstrinTostring(env, fileNameStr);
    if ((ret = AndroidBitmap_getInfo(env, bitmapcolor, &infocolor)) < 0) {
        LOGD("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return (*env)->NewStringUTF(env, "0");;
    }
    if ((ret = AndroidBitmap_lockPixels(env, bitmapcolor, &pixelscolor)) < 0) {
        LOGD("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    BYTE r, g, b;
    data = NULL;
    data = malloc(w * h * 3);
    tmpdata = data;
    int j = 0, i = 0;
    int color;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            color = *((int *) pixelscolor);
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = color & 0x000000FF;
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data = data + 3;
            pixelscolor += 4;

        }

    }
    AndroidBitmap_unlockPixels(env, bitmapcolor);
    int resultCode= generateJPEG(tmpdata, w, h, quality, fileName, optimize);
    free(tmpdata);
    if(resultCode==0){
        jstring result=(*env)->NewStringUTF(env, error);
        error=NULL;
        return result;
    }
    return (*env)->NewStringUTF(env, "1"); //success
}


void jstringTostring(JNIEnv* env, jstring jstr, char * output, int * de_len) {
    *output = NULL;
    jclass clsstring = (*env)->FindClass(env, "java/lang/String");
    jstring strencode = (*env)->NewStringUTF(env, "utf-8");
    jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes",
                                        "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)(*env)->CallObjectMethod(env, jstr, mid,
                                                           strencode);
    jsize alen = (*env)->GetArrayLength(env, barr);
    *de_len = alen;
    jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
    if (alen > 0) {
        output = (char*) malloc(alen + 1);
        memcpy(output, ba, alen);
        output[alen] = 0;
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
}*/

