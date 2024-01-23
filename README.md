# 点云融合
## 任务描述

使用测绘车沿不同轨迹采集到了原始点云，不同轨迹点云的坐标系已配准到同一坐标系，只需要进行融合。

## 项目功能

实现了两个点云的融合（参考`examples/point_cloud_fusion.cpp`），利用ikd-tree自带的downsample实现（根据预设的分辨率将空间分割成一个个voxel，保留每个voxel中离中心点最新的点）。示例使用了NuScenes数据集中的点云数据，通过`pcd/bin_2_pcd.ipynb`将`.bin`文件转化为`cloud1.pcd`文件。为了检验融合效果，对同一个点云文件的每个点加入随机噪声，得到`cloud2.pcd`。

## 编译

```bash
# 拷贝项目文件夹
cd ikd-Tree/build
cmake ..
make -j 9
```

## 调参

- 选择不同的点云
- 选择不同的downsample_size

```C++
float downsample_size = 0.01; // 降采样的box length设为0.01米
KD_TREE<PointType>::Ptr kdtree_ptr(new KD_TREE<PointType>(0.3, 0.6, downsample_size)); 
```



## 实验结果

- 融合时间统计：

```
Building tree takes: 8.752 ms
Point Cloud Fusion takes: 42.932 ms
number of points: 34720 + 34720
```

- 融合结果：

```
随机数范围：(-0.005,0.005)   降采样分辨率：0.004 融合结果：34720 60821
随机数范围：(-0.005,0.005)   降采样分辨率：0.005 融合结果：34720 58341
随机数范围：(-0.005,0.005)   降采样分辨率：0.01 融合结果：34720 47294
随机数范围：(-0.005,0.005)   降采样分辨率：0.015 融合结果：34720 41178

随机数范围：(-0.001,0.001)   降采样分辨率：0.004 融合结果：34720 41411
随机数范围：(-0.001,0.001)   降采样分辨率：0.01 融合结果：34720 34120
```



## 日志

1.12 综述

基于学习的融合方法主要是在特征级（feature level），即通过Transformer，CNN等提取特征，用于3D目标检测。

原始点云的融合可以采用普通的逻辑上的融合。例如：根据距离近判定为重叠点，直接去掉或者两个点取加权平均。问题转变为：**如何存储点云能实现快速的搜索近邻点。**

可参考的方法有：ikd-tree（Fastlio2），iVox（Faster-lio）。

ikd-tree提供了点云降采样的方法，可以直接参考。iVox不能提供Strict k-NN，因此暂定采用ikd-tree。

iVox的优点是效率更高，可以并行处理（？）。因此，若ikd-tree方法不可用时，可尝试使用iVox。

1.13 

数据集调研：暂时使用nuScenes数据集

项目思路：先用python将两个点云转换成pcd格式，用pcl库读取。还需要查看ikd tree的点云接口类型。这里不考虑nuscenes不同点云之间的坐标系转换问题，后面跑通代码可以通过token获取点云到全局坐标系的变换，再进行转换

1.17 

只使用一个点云即可，尝试将两个相同的点云融合在一起，看看融合后是不是一样的。可以给其中一个点云的x加0.001m的随机数

[copyPointCloud](https://blog.csdn.net/qq_43407445/article/details/94395358)

1.21

使用了`header_only_ikdtree`[分支](https://github.com/KennyWGH/ikd-Tree-detailed/tree/header_only_ikdtree)的两个头文件，ikd-tree类的定义全都包含在这两个头文件中，不再需要 `ikd_Tree.cpp`。但在`ikd_Tree.cpp`对部分函数的功能进行了注释。未来应该把注释迁移到在两个头文件中，删除`ikd_Tree.cpp`。
