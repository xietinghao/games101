# AABB包围盒<br>
AABB包围盒是一种在空间中求射线或直线与物体交点的方式，接下来先从二维的情况开始回顾。<br>
设EF为从点 $\mathbf{E}$ = [-1, 2]出发，方向向量 $\mathbf{e}$ = [2, 3]的一条射线，二维空间中的包围面为直线AB与CD的包围面ABDC。
射线EF = $\mathbf{E}$ + t * $\mathbf{e}$,与构成包围面ABDC的四条直线分别交于点E、F、G、H。
<br>
![BoundingBox](https://github.com/xietinghao/games101/blob/master/PA6/BoundingBox.png)
<br>
AABB包围盒的中心思想是，射线和每一对平行轴的交点对都会构成一个区间，取区间的交集，当交集中包含大于0的元素时，射线和包围盒相交。
如图，求解 $\mathbf{E}$<sub>x</sub> + t * $\mathbf{e}$<sub>x</sub> = -1 和 $\mathbf{E}$<sub>x</sub> + t * $\mathbf{e}$<sub>x</sub> = 5
分别得到t<sub>min_x</sub>=0，t<sub>max_x</sub>=1，则射线EF与y轴向的平行轴对构成的区间为[0, 1]。
类似的可解t<sub>min_y</sub>=-1，t<sub>max_y</sub>=1，射线EF与x轴向的平行轴对构成的区间为[-1, 1]。
<br>当t<0时，交点落在起点后方，舍去这部分值得到区间的并集为[0,1]，在起点的前方，得到射线EF与ABDC的交集为t落在区间[0, 1]时。
<br>当EF为三维空间的射箭时，求解维度增加一个z轴，得到z轴向的区间，取所有区间的交集，若交集存在且含大于0的元素则EF与包围盒有交点。
