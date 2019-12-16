package com.libjpeg

import android.graphics.Bitmap

/**
 *  Created by CHEN on 2019/12/16
 *  Email:1181785848@qq.com
 *  Introduce:  libjpeg压缩图片工具类
 **/
object LibjpegUtil {
    init {
        System.loadLibrary("compress")
    }

    /**
     * libjpeg压缩
     */
    external fun nativeCompressBitmap(
        bitmap: Bitmap, //原始bitmap
        quality: Int,   //压缩程度
        dstFile: String,    //目标路径
        optimize: Boolean   //是否启用哈夫曼算法
    );
}