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


