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
上述公式是辐射度量学定义的反射方程，描述了辐射(Radiance)和辐照度(Irradiance)之间的转换关系，f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) 函数描述了入射方向的辐射和出射方向的辐照度之间的比例，($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>)描述了角度 $\omega$<sub>i</sub> 入射的光线在 p 点的能量损失。<br>
总结一下，上式的几个物理量 L(p, $\omega$<sub>o</sub>) 代表了入射方向的辐射，f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) 描述了入射和出射能量的转换关系， $\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub> 描述了能量损失。<br>

## 蒙特卡洛积分与全局光线追踪
蒙特卡洛积分是一种基于概率统计来计算定积分的方法，对以某种概率分布的变量x，其在积分域 [a, b] 上的定积分 $\int_a^b$ f(x) dx ,可以用<br> 
$\frac{1}{N}$ $\sum_1^n$ $\frac{f(x_i)}{pdf(x_i)}$ 来表示。<br>
蒙特卡洛积分的核心思想是用 $\frac{f(x_i)}{pdf(x_i)}$ 来近似f(x)的值，以求解二维坐标系上x $\epsilon$ [a,b]的函数f(x) = k 与坐标轴的包围面积为例，<br>
其积分形式为 $\int_a^b$ f(x) dx = k $\int_a^b$ dx = k(b-a)。<br>
蒙特卡洛积分的近似模拟为 $\frac{1}{N}$ $\sum_1^n$ $\frac{f(x_i)}{pdf(x_i)}$ = $\frac{1}{N}$ $\sum_1^n$ $\frac{k}{1/(b-a)}$ = k(b-a)。<br>
通过借助每一次模拟，当 N -> $\infty$ ，积分值无限接近/等于定积分值。<br>
回到RayTracing算法中， E(p, $\omega$<sub>o</sub>) = $\int$ L(p, $\omega$<sub>o</sub>) * f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) * ($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>) d $\omega$ ,<br>
它的蒙特卡洛积分形式为 $\frac{1}{N}$ $\sum_1^n$ L(p, $\omega$<sub>o</sub>) * f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) * ($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>) * $${1 \over pdf(\omega)}$$ , <br>
实际反射的物理过程，立体角 $\omega$ 位于法向量 $\mathbf{N}$ 指向的上半球面，用均匀分布律描述 $\omega$, 则pdf( $\omega$ ) = $\frac{1}{2 * pi}$ 。 <br>
最后，蒙特卡洛积分形式的反射方程为 $\frac{1}{N}$ $\sum_1^n$ L(p, $\omega$<sub>o</sub>) * f<sub>brdf</sub>(p, $\omega$<sub>i</sub>, , $\omega$<sub>o</sub>) * ($\mathbf{N}$ $\cdot$ $\omega$<sub>o</sub>) * (2 * pi) 。<br>
现在使用蒙特卡洛积分求解光线追踪算法，当每条光线命中反光物体时，返回上述的反射方程的积分解。

## 全局光线追踪算法的优化
使用光线追踪算法时，当场景中的非光源数量远大于光源数量(这是一种很常见的情况)，每一个物体发出的光线只有很少的一部分能够在多重递归中最终抵达光源，剩下没有抵达光源的光线对结果不产生任何影响。<br>
显然，这一部分的光线追踪计算是无效的。如果可以避免这部分的无效计算，光线追踪算法的性能将会有很大提升。<br>
考虑这种情况，假设光线 r 不再是从物体出发，而是从光源出发，并能得到 r 与物体的交点 p , 如果交点 p 位于目标物体上，说明目标物体能够被照亮。<br>
则进一步从 p 点打出新光线 q， 并递归的计算 q 和下一个物体的交点，最终也能得到同样的渲染结果。<br>
因为光线是从光源出发的，所以所有的光线追踪计算一定是有效的，因为如果这些光线能够打到目标物体上，那么该结果一定是非零值(经过光源)，所以就避免了无效的计算。<br>
应用上述的蒙特卡洛积分形式的值，可以得到光源出发的 r 在点 p 的反射值。与物体出发时的情况不同，因为光线从光源射出，所以还需要计算光源在入射方向上的面积投影(该算法智能在面光源下成立),<br>
面积投影的值为 $${S_{light} * {(x_{light} - x_{material})^2 \over \omega \cdot \mathbf{N_light} }   = S_{material}}$$ , 即：<br>
$${S_{light} \over S_{material} }$$ = $${ \omega \cdot \mathbf{N_{light}} \over (x_{light} - x_{material})^2} $$ ,<br>
代入蒙特卡洛积分得到： $${ \frac{1}{N} \sum_1^n L(p, \omega_{light}) * f_{brdf}(p, \omega_{light}, \omega_o) * (\mathbf{N} \cdot \omega_o) * {\omega_{light} \cdot \mathbf{N_{light}} \over (x_{light} - x_{material})^2} / pdf(light)}$$  。

## 伪代码
综上，每一个渲染点的颜色由两部分组成，一部分是光源的直接光照，另一部分是从其他物体反射回来的间接光照，伪代码如下。
```
Shading(ray):
  p=Intersect(ray); //场景求交
  if(p is lightSource)return p.lightColor;

  Inersection x; float pdf_light;
  pdfSample(x, pdf_light);
  material=Intersect(rayPX) //rayPX is ray from P to X
  vec3 light_dir, light_indir;

  //MonteCarlo(lightSource) = x.lightColor * f(brdf) * dot(w_light, N_x) * dot(w_light, N_p) / length(x,p) / pdf_light; 
  if(material is lightSource || material is closer or little more remote than x)light_dir=MonteCarlo(lightSource); 
  float rr;

  //MonteCarlo(otherObject) = Shading(ray(p, w_i)) * f(brdf) * dot(w_i, N_p) / pdf , pdf = 2*pi
  if(rr<RR)light_indir=MonteCarlo(otherObject)/RR; 

  return light_dir + light_indir;
  
```
最终得到全局光线追踪的渲染结果，渲染的材质模型是Cornell Box。
<br>![Cornell Box](https://github.com/xietinghao/games101/blob/master/GlobalRayTracing/CornellBox.png)<br>

## 多线程的优化思路
线程是进程的子概念，一个进程可以创建属于其的多个子线程，进程享有硬件资源的分配，线程享有cpu的使用权，对计算密集型的任务，为了充分利用cpu的核心数量，可以创建多个子线程分布在不同核心上进行并行计算。 <br>
c++的多线程实现可以用thread库操作，一个多线程的c++实例代码如下:
```
#include <thread>
#include <vector>
#include <iostream>
void printHello(const string &str){
  std::cout<<"hello"<<str<<std::endl;
}
void main(){
  const int threadNum=3;
  std::vector<std::thread> threadVec(threadNum);
  for(int i=0;i<threadNum;i++)threaVec.push_back(std::thread(printHello, std::cref(std::to_string(i))));
  for(int i=0;i<threadNum;i++)threadVec[i].join();
}
```
c++的thread对象在传递函数参数时，一律传递右值，即不能传递左值或左值引用给调用函数，当函数参数必须通过引用或者常量引用传递时，需要使用std::ref(refName)或者std::cref(constRefName)包装参数，实现参数传递。<br>
光线追踪的渲染过程中对每一个像素点采用递归的光线追踪算法，并且不同像素点之间的计算是并行进行的，不存在互斥或者同步问题，是一个适合用多线程优化的场景问题，因为该框架是运行在cpu上的，无法利用gpu的并行计算，那么c++的thread库就有了用武之地。<br>
多线程的伪代码：
```
  create threadVec;     //vector<thread> threadVec;
  count num of pixel;   //pixelNum = screenWidth * screenHeight;

  divide tasks to each thread;
  /*
  pixelBegin=0, pixelEnd=N;
  for(int i=0;i<threadNum;i++){
    threadVec.push_back(threadFunc, pixelBegin, pixelEnd ...);
    pixelBegin+=N;
    pixelEnd= min(maxPixelNums, pixelBegin+N);
  }
  */

  updateScreen;
```
多线程的优化代码为Renderer_MultiThread.cpp，结果一致。
