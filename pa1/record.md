# Model Transformation and Perspective Transformation
Model, view and perspective transformation is before rasterization. 
Model tansformation put camera and things in local coordinate system, so relative postion between camera and things is determined.
View transformation put camera in origin, and rotate it's look-up direction and shoot direction to -z and y. And then the direction of postion can be determined by look-up direction cross product by shoot direction. In fact, any rotation in 3D space can be decompose into three rotations, one is around x, another is around y and the other is z. By placing camera in origin and point camera correctly, we can reduce workload of computation.
To implement transformation above, we need to deduce the transformation matrix. But is difficult to dededuce matrix from \[ \vec{p}, \mathbf{look-up}, \mathbf{shoot} \] to \[ \mathbf{a}, \mathbf{b}, \mathbf{c}\]. A is\[1,0,0\]^T, b is \[0,1,0\]^T and c is \[0,0,1\]^T.But it's easy to deduce from the later to the former. It's easy to get matrix:
$$
\begin{matrix}
    \mathbf{l}x\mathbf{s} & \mathbf{l} \mathbf{s}
\end{matrix}
$$
As rotation matrix is orthogonal matrix, we can get the inverse matrix by transposing matrix above.Then the matrix we get is view matrix before translation.The last step is add translation to the matrix, it can be implemented by homogeneous coordinate.
Perspective transformation squeeze quadrangular frustum to cuboid, then orthographic transformation can make it be canonical cuboid.
Perspective matrix is build on assumptions below:
    1)y of dot before squeezing and y' of dot after squeezing are propotional, the ratio between y and y' is equal to n/z.
    2)Any dot of near plain remain same position after squeezing.
    3)The mid dot of far plain remain same position after squeezing.
So we can deduce that first and scond row is \[n,0,0,0\] from assumption 1), and the forth row is \[0,0,1,0\], if we want the ration be n/z.
And we can get the third row like \[0,0,A,B\]. So M_perp is 
$$
    \begin{matrix}
    n & 0 & 0 & 0
    0 & n & n & 0
    0 & 0 & A & B
    0 & 0 & 1 & 0
    \end{matrix} \tag{1}
$$
Multipile the matrix with column vector\[0,0,f,1\] and \[0,0,n,1\] resprents the postion of dot in assumption 2) and 3),we should get the equalation:
$$
\begin{cases}
    \Af+b=f^2
    \An+B=n^2
\end{cases}
$$
Resolving the equation sets, A=n+f,and B=-nf. So we get the perspective matrix.
Squeezeing cuboid to canonical cuboid need scale and translation, it's easy to get M_ortho:
$$
\begin{matrix}
    \frac{2}{r-l} & 0 & 0 & -\frac{l+r}{2}
    0 & \frac{2}{t-b} & 0 & -\frac{b+t}{2}
    0 & 0 & \frac{2}{n-f} & -\frac{n+f}{2}
    0 & 0 & 0 & 1
\end{matrix} \tag{3}
$$
At last, we multipy dot by M_perp and then M_ortho, that's mean squeezing cuboid to canonical cuboid.
