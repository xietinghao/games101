# Whitted Style Ray Tracing
Whitted Style光线追踪的假设:<br>
1)光线打到物体时，若为光源则直接返回，若为反光物体则采用其Phong Shading的结果作为直接颜色。<br>
2)Whitted Style不考虑多次反射的间接光照，相比全局光照在细节上有缺失。<br>
3)使用BVH结构加速光线追踪，当光线和包围盒不相交时，直接结束追踪。若光线和包围盒相交，则递归得和子包围盒求交点。<>
完成上述部分的关键函数后，得到Whitted Style光线追踪的兔子渲染结果如下。<br>
![rabbit](https://github.com/xietinghao/games101/blob/master/PA6/Rabbit.png)
<br>(二进制文件为Rabbit.ppm)
