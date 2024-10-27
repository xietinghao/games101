# Moller-Trumbore
Moller-Trumbore是一种求射线与空间平面相交点的算法，其推理如下。<br>
空间射线 $\mathbf{o}$ + t * $\mathbf{D}$ 与空间平面相交于点 $\mathbf{p}$，该点用平面上三角形ABC的重心坐标表示为:
<br> $\mathbf{p}$ = (1-b<sub>1</sub>-b<sub>2</sub>) * $\mathbf{P}$<sub>0</sub> + b<sub>1</sub> * $\mathbf{P}$<sub>1</sub> + b<sub>2</sub> * $\mathbf{P}$<sub>2</sub>。<br>
求交点问题转变为解线性方程组问题：
<br> ( $\mathbf{o}$ - $\mathbf{p}$<sub>0</sub> ) = -t * $\mathbf{D}$ + b<sub>1</sub> * ( $\mathbf{p}$<sub>1</sub> - $\mathbf{p}$<sub>0</sub>) + b<sub>2</sub> * ($\mathbf{p}$<sub>2</sub> - $\mathbf{p}$<sub>0</sub>) <br>
令 
<br> $\mathbf{o}$ - $\mathbf{p}$<sub>0</sub> = $\mathbf{S}$, 
<br> ($\mathbf{p}$<sub>1</sub> - $\mathbf{p}$<sub>0</sub>) = $\mathbf{E}$<sub>1</sub>,
<br> ($\mathbf{p}$<sub>2</sub> - $\mathbf{p}$<sub>0</sub>) = $\mathbf{E}$<sub>2</sub>,
<br>得:  $\mathbf{S}$ = -t * $\mathbf{D}$ + b<sub>1</sub> * $\mathbf{E}$<sub>1</sub> + b<sub>2</sub> * $\mathbf{E}$<sub>2</sub>).
<br>即线性方程组 $\mathbf{S}$ = [-t, b<sub>1</sub>, b<sub>2</sub>] * [ $\mathbf{D}$, $\mathbf{E}$<sub>1</sub>, $\mathbf{E}$<sub>2</sub> ]<sup>T</sup>.
<br>求解[-t, b1, b2]，由克拉默法则得:
<br> -t = $\frac{$\mathbf{det}$ [ $\mathbf{D}$, $\mathbf{E}$<sub>1</sub>, $\mathbf{E}$<sub>2</sub> ]}{$\mathbf{det}$ [ $\mathbf{S}$, $\mathbf{E}$<sub>1</sub>, $\mathbf{E}$<sub>2</sub> ]}$
<br> -t = $\frac{$\mathbf{E}$<sub>2</sub> $\cdot$ ($\mathbf{D}$ $\times$ $\mathbf{E}$<sub>1</sub>)}{$\mathbf{E}$<sub>2</sub> $\cdot$ ($\mathbf{S}$ $\times$ $\mathbf{E}$<sub>1</sub>)}$,
<br> 设 $\mathbf{D}$ $\times$ $\mathbf{E}$<sub>2</sub> = $\mathbf{S}$<sub>1</sub>,
<br> $\mathbf{S}$ $\times$ $\mathbf{E1}$ = $\mathbf{S}$<sub>2</sub>,
<br> 得  -t = $\frac{$\mathbf{E}$<sub>2</sub> $\cdot$ $\mathbf{S}$<sub>1</sub>}{$\mathbf{E}$<sub>1</sub> $\cdot$ $\mathbf{S}$<sub>1</sub>}$
