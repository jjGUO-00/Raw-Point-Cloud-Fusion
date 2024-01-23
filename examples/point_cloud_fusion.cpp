/*
    Description: Point Cloud Fusion using ikd-Tree
    Author: Jiajie Guo
*/

#include "ikd_Tree_impl.h"
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <algorithm>
#include "pcl/point_types.h"
#include "pcl/common/common.h"
#include "pcl/point_cloud.h"
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <string>

using namespace std;
using namespace ikdtreeNS;
using PointType = pcl::PointXYZI; // x,y,z,intensity
using PointVector = KD_TREE<PointType>::PointVector; // vector存储点云
template class KD_TREE<PointType>;

void colorize( const PointVector &pc, pcl::PointCloud<pcl::PointXYZRGB> &pc_colored, const std::vector<int> &color) {
    int N = pc.size();

    pc_colored.clear();
    pcl::PointXYZRGB pt_tmp;

    for (int i = 0; i < N; ++i) {
        const auto &pt = pc[i];
        pt_tmp.x = pt.x;
        pt_tmp.y = pt.y;
        pt_tmp.z = pt.z;
        pt_tmp.r = color[0];
        pt_tmp.g = color[1];
        pt_tmp.b = color[2];
        pc_colored.points.emplace_back(pt_tmp);
    }
}



string formatFloatValue(float val) {
    string str = to_string(val);
    int cnt = 2;
    bool valid = true;
    for(int i=2;i<str.size();i++){
        if(str[i]>'0'){
            valid = false;
        }else{
            if(!valid) break;
        }
        cnt++;
    }
    cout << cnt << endl;
    return str.substr(0, cnt);
}


int main(int argc, char **argv){
    /*** 1. Initialize k-d tree */
    float downsample_size = 0.01; // 降采样的box length设为0.01米
    KD_TREE<PointType>::Ptr kdtree_ptr(new KD_TREE<PointType>(0.3, 0.6, downsample_size)); 
    KD_TREE<PointType>      &ikd_Tree        = *kdtree_ptr;
 
    /*** 2. Load point cloud data */
    pcl::PointCloud<PointType>::Ptr src(new pcl::PointCloud<PointType>);
    string filename = "../pcd/cloud1.pcd";
    if (pcl::io::loadPCDFile<PointType>(filename, *src) == -1) //* load the file
    {
        PCL_ERROR ("Couldn't read file cloud1.pcd \n");
        return (-1);
    }
    printf("Original: %d points are loaded\n", static_cast<int>(src->points.size()));

    // 复制一份一样的点云用于融合
    // pcl::PointCloud<PointType>::Ptr src2(new pcl::PointCloud<PointType>);
    // pcl::copyPointCloud(*src, *src2);
    pcl::PointCloud<PointType>::Ptr src2(new pcl::PointCloud<PointType>);
    string filename2 = "../pcd/cloud2.pcd";
    if (pcl::io::loadPCDFile<PointType>(filename2, *src2) == -1) //* load the file
    {
        PCL_ERROR ("Couldn't read file cloud2.pcd \n");
        return (-1);
    }
    // printf("Original: %d points are loaded\n", static_cast<int>(src2->points.size()));

    /*** 3. Build ikd-Tree */
    auto start = chrono::high_resolution_clock::now();
    ikd_Tree.Build((*src).points);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
    printf("Building tree takes: %0.3f ms\n", float(duration) / 1e3);
    printf("# of valid points: %d \n", ikd_Tree.validnum());

    /*** 4. Point Cloud Fusion */
    start = chrono::high_resolution_clock::now();
    bool downsample_on = true;
    ikd_Tree.Add_Points((*src2).points, downsample_on);
    end  = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
    printf("Point Cloud Fusion takes: %0.3f ms\n", float(duration) / 1e3);

    /*** 5. Save Fusion Result */
    // ikd tree获取有效点云
    pcl::PointCloud<PointType>::Ptr Remaining_Points(new pcl::PointCloud<PointType>);
    ikd_Tree.flatten(ikd_Tree.Root_Node, ikd_Tree.PCL_Storage, NOT_RECORD);  // 获取点云
    Remaining_Points->points = ikd_Tree.PCL_Storage;
    printf("Fusion result: %d points\n", static_cast<int>(Remaining_Points->points.size()));

    // 保存点云pcd
    string save_path = "../pcd/fusion_result_down_" + formatFloatValue(downsample_size) + ".pcd";
    pcl::io::savePCDFileASCII(save_path,*src);  // 保存ASCII格式文件至PCD文件中
    // pcl::io::savePCDFileBinary("test_pcd.pcd",*cloud);  //保存Binary格式文件至PCD文件中 
    // pcl::io::savePCDFileBinaryCompressed("test_pcd.pcd",*cloud);  //保存一个经过压缩后的Binary格式文件 

    /*** 6. Fusion Result visualization */
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr src_colored(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr remaining_colored(new pcl::PointCloud<pcl::PointXYZRGB>);

    pcl::visualization::PointCloudColorHandlerGenericField<PointType> src_color(src, "x");
    pcl::visualization::PointCloudColorHandlerGenericField<PointType> remaining_color(Remaining_Points, "x");

    pcl::visualization::PCLVisualizer viewer0("Source Point Cloud");
    viewer0.addPointCloud<PointType>(src,src_color, "src");
    viewer0.setCameraPosition(-5, 30, 175,  0, 0, 0, 0.2, -1.0, 0.2);
    viewer0.setSize(1600, 900);

    pcl::visualization::PCLVisualizer viewer1("Fusion Result");
    viewer1.addPointCloud<PointType>(Remaining_Points,remaining_color, "remain");
    viewer1.setCameraPosition(-5, 30, 175,  0, 0, 0, 0.2, -1.0, 0.2);
    viewer1.setSize(1600, 900);

    while (!viewer0.wasStopped() && !viewer1.wasStopped()) {
        viewer0.spinOnce();
        viewer1.spinOnce();
    }
    // while (!viewer0.wasStopped()) {
    //     viewer0.spinOnce();
    // }
    
}