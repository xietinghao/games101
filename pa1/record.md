# Model Transformation and Perspective Transformation
Model, view and perspective transformation is before rasterization.
Model tansformation put camera and things in local coordinate system, so relative postion between camera and things is determined.<br>
View transformation put camera in origin, and rotate it's look-up direction and shoot direction to -z and y. And then the direction of postion can be determined by look-up direction cross product by shoot direction. In fact, any rotation in 3D space can be decompose into three rotations, one is around x, another is around y and the other is z. By placing camera in origin and point camera correctly, we can reduce workload of computation.<br>
To implement transformation above, we need to deduce the transformation matrix. But is difficult to dededuce matrix from \[ $\mathbf{p}$, $\mathbf{look-up}$, $\mathbf{shoot}$ \] to \[ $\mathbf{a}$, $\mathbf{b}$, $\mathbf{c}$ \]. A is\[1,0,0\]<sup>T</sup>, b is \[0,1,0\]<sup>T</sup> and c is \[0,0,1\]<sup>T</sup>.But it's easy to deduce from the later to the former. It's easy to get matrix:
$$
    \begin{matrix}
    \mathbf{l}\times\mathbf{s} & \mathbf{l} & \mathbf{s}
    \end{matrix}
$$
As rotation matrix is orthogonal matrix, we can get the inverse matrix by transposing matrix above. Then the matrix we get is view matrix before translation. The last step is add translation to the matrix, it can be implemented by homogeneous coordinate.
Perspective transformation squeeze quadrangular frustum to cuboid, then orthographic transformation can make it be canonical cuboid.<br>
Perspective matrix is build on assumptions below:<br>
    1)y of dot before squeezing and y' of dot after squeezing are propotional, the ratio between y and y' is equal to n/z.<br>
    2)Any dot of near plain remain same position after squeezing.<br>
    3)Mid dot of far plain remain same position after squeezing.<br>
So we can deduce that first and scond row is \[n,0,0,0\] from assumption 1), and the forth row is \[0,0,1,0\], if we want the ration be n/z.<br>
And we can get the third row like \[0,0,A,B\]. So M<sub>perp</sub> is<br>  
$$
    \begin{matrix}
    n & 0 & 0 & 0\\
    0 & n & 0 & 0\\
    0 & 0 & A & B\\
    0 & 0 & 1 & 0
    \end{matrix}
$$
Multipile the matrix with vector\[0,0,f,1\]<sup>T</sup> and \[0,0,n,1\]<sup>T</sup> resprents the postion of dot in assumption 2) and 3),we should get the equalation:
 $$
\begin{cases}
    Af+b=f^2<br>
    An+B=n^2
\end{cases}
 $$
Resolving the equation sets, A=n+f,and B=-nf. So we get the perspective matrix.
Squeezeing cuboid to canonical cuboid need scale and translation, it's easy to get M<sub>ortho</sub>:
$$\begin{matrix}
    \frac{2}{r-l} & 0 & 0 & -\frac{l+r}{2}\\
    0 & \frac{2}{t-b} & 0 & -\frac{b+t}{2}\\
    0 & 0 & \frac{2}{n-f} & -\frac{n+f}{2}\\
    0 & 0 & 0 & 1
\end{matrix} 
$$
At last, we multipy dot by M<sub>perp</sub> and then M<sub>ortho</sub>, that's mean squeezing cuboid to canonical cuboid.
