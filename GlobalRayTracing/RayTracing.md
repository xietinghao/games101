# 全局光线追踪算法
基于Whitted-Style的Ray Generation虽然能渲染出物体的阴影，但是仅限于直接光照。多次光照对阴影的影响是Whitted-Style RayTracing无法渲染的。<br>
全局光线追踪算法则是一种能渲染多次光照的算法，它的假设如下：<br>
1）场景中打出n条光线，计算每条光线和场景物体的交点p。<br>
2）若物体为光源，则直接返回其发光色，递归终止。<br>
3）若物体为反射体，则从该交点p再次生成n条光线递归追踪，回到1）。<br>
在算法的实现过程中，存在如下问题: <br>
- 路径爆炸问题：每次递归生成n条光线，算法性能开销随着碰撞次数的增加指数级上升，无法容忍。
- 递归终止问题：可能存在一条光线，在各反光体间无限次反弹而不结束递归。<br>
对应的解决方案为:
路径爆炸问题，当n=1时由于1<sup>n</sup>永远是1，该问题得到解决，对于每一个像素，发出n条光线，每一条光线应用RT算法，即可得到像素颜色值。<br>
递归终止问题，采用俄罗斯轮盘赌的思想，每进入深层递归前，生成一个随机数RR，若RR<常数C，则进入下一层循环，否则终止递归，返回物体的发光值。<br>

## 辐射度量学与全局光线追踪
在上述算法流程中，返回的颜色值是用辐射度量学来计算的。对于一个反射点p，光线以 $\omega$<sub>i</sub> 打到点 p 并以 $\omega$<sub>o</sub> 反射出去，点 p 的法向量为 $\mathbf{N}$ ,有<br>
E(p, $\omega$<sub>o</sub>) = $\int$ L(p, $\omega$<sub>o</sub>) * f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) * ($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>) d $\omega$ 。<br>
上述公式是辐射度量学定义的反射方程，描述了辐射(Radiance)和辐照度(Irradiance)之间的转换关系，f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) 函数描述了入射方向的辐射和出射方向的辐照度之间的比例，($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>)描述了角度 $\omega$<sub>i</sub> 角度入射的光线在 p 点的能量损失。<br>
总结一下，上式的几个物理量 L(p, $\omega$<sub>o</sub>) 代表了入射方向的辐射，f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) 描述了入射和出射能量的转换关系， $\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub> 描述了能量损失。<br>

## 蒙特卡洛积分与全局光线追踪
蒙特卡洛积分是一种基于概率统计来计算定积分的方法，对以某种概率分布的变量x，其在积分域 [a, b] 上的定积分 $\int_a^b$ f(x) dx ,可以用 $\frac{1}{N}$ $\sum_1^n$ $\frac{f(xi)}{pdf(xi)}$ 来表示。<br>
蒙特卡洛积分的核心思想是用 f(xi)/pdf(i)来近似f(x)的值，以求解二维坐标系上x $\epsilon$ [a,b]的函数f(x) = k 与坐标轴的包围面积为例，其积分形式为 $\int_a^b$ f(x) dx = k $\int_a^b$ dx = k(b-a)。<br>
蒙特卡洛积分的近似模拟为 $\frac{1}{N}$ $\sum_1^n$ f(x<sub>i</sub>)/pdf(x<sub>i</sub>) = $\frac{1}{N}$ $\sum_1^n$ $\frac{k}{1/(b-a)}$ = k(b-a)。<br>
通过借助每一次模拟，当 N -> $\infty$ ，积分值无限接近/等于定积分值。<br>
回到RayTracing算法中， E(p, $\omega$<sub>o</sub>) = $\int$ L(p, $\omega$<sub>o</sub>) * f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) * ($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>) d $\omega$ 的蒙特卡洛积分形式为
$\frac{1}{N}$ $\sum_1^n$ L(p, $\omega$<sub>o</sub>) * f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) * ($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>) * 1/pdf($\omega$) , <br>
实际反射的物理过程，立体角 $\omega$ 位于法向量$\mathbf{N}$ 指向的上半球面，用均匀分布律描述 $\omega$, 则pdf( $\omega$ ) = $\frac{1}{2 * pi} 。 <br>
最后，蒙特卡洛积分形式的 反射方程为 $\frac{1}{N}$ $\sum_1^n$ L(p, $\omega$<sub>o</sub>) * f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) * ($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>) * $\frac{1}{2 * pi}$,

