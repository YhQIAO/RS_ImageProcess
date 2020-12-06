# 遥感数字图像处理
主要依赖库：
- OpenCV 4以上
- Eigen 3

Problem X-y表示第X章的y小题

## 第五章
半经验C值实现地形校正

## 第六章 几何校正
使用SIFT特征点提取与匹配

二次多项式模型

重采样（最邻近插值，二次线性，双三次卷积等）


## 第七章 图像去噪
### 空间域
均值滤波/中值滤波

边缘保持的K邻近均值。中值滤波

### 频率域
傅里叶变换到频率空间

高斯低通滤波去高频降噪

去除周期性噪声

## 第八章 
### 图像融合
- PCA图像融合
- Brovey图像融合
- HSI图像融合
### 直方图操作
- 直方图均衡化
- 直方图匹配
