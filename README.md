# Fourier Series for Triangulations of Star-Shaped Surfaces

This is the main project showcasing the formulas found on my universities final project. It handles the 
formulas for the case in $L^2(\mathbb{S}^2)$, in particular for the case of triangulated surfaces.

To do that it follows the math found in the paper, calculating the Fourier coefficients of figures stored 
in data files. Then computing the Fourier Series and displaying it in an interactive 3D view.

The next image showcases the basis for the $L^2(\mathbb{S}^2)$ functions, the Spherical Harmonics, as seen 
in the application. The areas shaded in blue correspond to the positive values of the functions, and the 
ones shaded in yellow to the negative ones. All values are made positive for better view.

![Colage](https://github.com/MiquelNasarre/FourierS2/assets/124403865/0ce9f90d-27cf-4c72-ad95-f2ed7e0d0436)

This image shows for values of $\ell$ up to $5$, the program uses values up to $\ell=28$, with $|m|\leq\ell$. 

With these capabilities it can render lots of triangulated surfaces, here in the next image you can see some 
examples for $\ell\leq 20$.

![collage2](https://github.com/MiquelNasarre/FourierS2/assets/124403865/eff4a0f8-7020-48ba-8694-64df05efa4ff)

The figures on the right are the original triangulated surfaces, while the ones on the left are their Fourier 
Series.

This kind of computations allows for a big reduction in size, since the Fourier coefficients weigh considerably 
less than their triangulations, while maintaining a very small error. And also having it stored like this 
allows for easier compatibility. For example, you can interpolate between surfaces just by interpolating coefficients, 
or you can train AI much easier with a list of a couple hundred coefficients than with a fully triangulated surface.

Therefore, this program is a showcase of the capabilities of the math seen in the paper, as well as a great interactive 
tool to better understand how the series work.

## Requirements

- [Visual Studio](https://visualstudio.com) (For the program to be able to compile all the different C++ files correctly)

All the rendering of the program, as we will see, uses the DirectX11 dependencies, which are part of the Windows OS, and 
only available for Windows 7 or newer. If you want to run the application, have the correct operating system, and are not 
interested in compiling the program yourself, please look at the 
[release](https://github.com/MiquelNasarre/FourierS2/releases/tag/v1.0), where you will be able to download the application.

## How to Use it

First, clone the repository into your computer and open the solution with Visual Studio. You should compile the projects as 
follows. First compile the library dependencies for the 3D renderer, which are the projects

- Dxerr
- Image
- ImGui
- Math

Then compile the WindowCreation project and finally the Fourier project. Set the Fourier as startup and the application 
should be up and running. Note that if you want to build the application to run independently from Visual Studio you will 
need to modify the preprocessor definitions for the resource folders it uses.

## Features & Interactions

Before going through all the widgets and how they work let's explore how to interact directly with the plots. On any loaded 
plot you can drag it around with the mouse, changing its orientation. You can use the mouse wheel to resize it, and if you use 
it while holding the mouse button the figure will rotate on its axis.

There are also some keyboard interactions, if you maintain pressed `R` or `T`, the plot will return to its original position. 
If you press `S` the plot will stop its current motion. The `M` key hides or shows the menu, and you can also use the program 
at full screen by pressing `F11` to enter and `esc` to exit.

Now let's go through its features. Upon running the program, you are presented with the Spherical Harmonics, which will show as 
the only Figure loaded so far. The menu is located on the top left corner. The following image shows the menu as seen upon opening

![menu0](https://github.com/MiquelNasarre/FourierS2/assets/124403865/d7037e10-5070-4c23-94f5-e7ea279c047e)

For this first plot you can use the sliders to change the $\ell$ and $m$ values, as said before the program uses $\ell\leq 28$ and 
$|m|\leq\ell$. Up at the top bar we have different menus: 

- The `Figure` menu displays the figures you have currently loaded in the program, once you load new files they will be displayed there.

- The `Advanced` menu has two different options, it is recommended to press the first option `Generate Dataset` upon starting the program.
  This will compute and store all the values for all the Spherical Harmonics in every point of our representation of the sphere. It will
  take around 400MB on the RAM memory, but it will greatly improve the performance of the program.

  The other option, `Compute Errors`, once you have some surfaces loaded it will compute their $L^2$-error, comparing them with the
  original triangulations.

- The other two menus `Lights` and `Textures` are just for aesthetics, as the names say allow you to change the lighting and textures of
  your surfaces. Unfortunately, you cannot do that with the triangulations.

Now let's load a figure into our program. If you click the load button a load menu will popup, up the top left you can choose between three 
different options, `Figure`, `Coefficients` and `A Priori`:

![load menus](https://github.com/MiquelNasarre/FourierS2/assets/124403865/3d6090d5-645f-4085-9830-de10339ad8f3)

- `Figure` loads files from the _figures_ folder, which contains the triangulations, you can specify how many $\ell$ values you want as well
  as how many subdivisions should be done for each triangle. If the triangulation is already very fine, a higher depth might take a long
  time to process, it is advised to try first for smaller depth values.

- `Coefficients` loads files from the _coefficients_ folders, which store the coefficients of previously calculated series.

- `A Priori` loads files from the _figures_ folder, first write the name of the file and load the triangulation. Then the error estimates
  formulas will be computed and you can select a certain error for your series. Using the formulas seen on the paper it will calculate enough
  coefficients to ensure that the error is below such threshold. You can also precompute some coefficients, which using the second theorem
  should give a more accurate estimate for the coefficients needed.

  As remarked in the paper, such estimates can be wrong due to the approximations done. Nevertheless, you can adjust the fineness of the
  triangulation to ensure better results.

Once a figure is loaded, if it comes from a triangulation it will display both of them, and some new options will be added to the menu, 
let's go through them:

![menu1](https://github.com/MiquelNasarre/FourierS2/assets/124403865/7b0de4bf-4107-4453-9db0-72f8e18b1395)

- `Create Interpolation`: This option will be available once you have at least two surfaces loaded, and it allows for the creation of
  interpolations between them. Once you click it the interpolation editor will pop up, and the main menu will change

  ![imenus](https://github.com/MiquelNasarre/FourierS2/assets/124403865/c8a9512f-d082-49ac-80e8-bbfc2b0e4256)

  The `Add Figure` selector allows you to introduce new surfaces to your interpolation, to delete them just click on them on the list. To
  save the interpolation, name it and click save. Then through the main menu you will be able to change the $t$ value that interpolates the 
  coefficients. Press `P` for the $t$ to change with time and adapt the speed with the arrow buttons.

- `Save Figure`: Will pop up a save menu where you can write the name of the file, then your figures coefficients will be saved on the
  _coefficients_ folder. This will work for any Fourier Series, including the coefficients for any $t$ value of an interpolation.

- `Single View / Double View`: If your surface is linked to a triangulation or vice versa both can be showed at the same time for
  comparison or individually.

- `Delete`: It deletes the current main view you have. It also erases it from memory. Keep in mind deleting some of them may erase features
  from those linked to it, mainly for error computations and interpolations.

- `Show Curves / Hide Curves`: Shows or hides the $\theta$ and $\varphi$ curves on the current surface for a fixed $(\varphi,\theta)$ point on
  $\mathbb{S}^2$. This feature can produce lag if it has to update the curves every frame, for example in a moving interpolation or if you
  are constantly changing the values of $\theta$ and $\varphi$, so discretion is advised.

- `Coefficients`: Is not yet implemented, it is meant to be a visually clear way of viewing the Fourier coefficients of any surface.

## How it Works

The application uses the [LearnDirectX](https://github.com/MiquelNasarre/LearnDirectX) repository as the core for all 
the 3D rendering. For a more in-depth explanation of how all the rendering works I suggest you take a look at it. 
This also means that it inherits the [ImGui](https://github.com/ocornut/imgui) dependencies for all its widgets.

Therefore the _Fourier_ project is built as an application of the previously mentioned repository. The main file just calls 
the creation of a _Fourier_ class, which creates the window and runs it, calling the main loop until the window is closed. 
Let's make a brief run through all the _Fourier_ project files.

- **Fourier.cpp / Fourier.h:** As mentioned this is the center class of our application. It is set to run at 60fps, and every 
  frame it processes all input events, and calls all the other functions needed to display accordingly. The _processEvents_ 
  function is the one that takes care of all the processing, while the _doFrame_ does all the rendering. The processing includes 
  all the Fourier computations as well as all the Keyboard and Mouse updates and rendering updates.

- **IG_Fourier.cpp / IG_Fourier.h:** Since I wanted to keep separated the ImGui dependencies from the rest of my code this class 
  is the one that takes care of all the widgets. It communicates with the _Fourier_ class only via the _IG_ struct.

- **Polihedron.cpp / Polihedron.h:** This is a _Drawable_ class which is used to store the triangulated surfaces, and draw and 
  interact with them. Upon creation it also does some heavy computations. It calculates all the triangle projected areas, and the 
  angles within them, the normal vector of each triangle and it also creates a neighborhood for each vertex. All of which are 
  stored and can be called for future calculations that involve the triangulation, as we will see later.

- **FourierSurface.cpp / FourierSurface.h:** This is a _Drawable_ class which is used to store the Fourier Series, and draw and
  interact with them. It also takes the main role with all the Fourier related computations. Upon creation it uses an existing
  _Polihedron_ to compute its Fourier coefficients. Its subclass _Functions_ takes care of all the math involved. This subclass
  also contains the functions used to compute all the norms used for the error estimations.

  Within the _FourierSurface_ class we can also find the _FileManager_ class, which as the name indicates is used for file handling,
  and a _curve_ class, to display the curves on top of the surfaces.

- **Interpolated.cpp / Interpolated.h:** This file contains two _Drawable_ classes used for creating the interpolations between
  different _FourierSurface_'s. the _Interpolated_ class contains a single interpolation between two surfaces and the
  _InterpolatedString_ class contains multiple instances of the previous one, to interpolate between any number of surfaces.

## Math Involved & Additional Information

In this section we intent to cover the most important pieces of math used specifically for this program. The ones related to the 
Fourier computations are already explained in the original paper. However here we will go through them again as well as introducing 
some additional information.

### Multithreading

This program is my first attempt at multithreading, this meaning that the program runs on different instances on your computer, 
allowing for better CPU management as well as an uninterrupted experience. 

I am proud to say that such implementation has greatly improved the experience of using the program and its performance. Let's give some 
examples where different threads are used.

Every time you load a triangulation it launches a thread that calculates all the coefficients and another one that waits for the first one 
to finish and then calculates the surface using those coefficients. This allows the user to keep interacting with the application while 
the computations are being done in the background. Once the coefficients and surface are fully computed it will display them to the user.

Every time it needs to do a demanding computation, like the ones seen in the previous paragraph or the clearest example might be generating 
all the dataset, it launches multiple threads in order to spread the work. Nowadays most CPU's have multiple logical units, so that allows 
for a great boost in performance and a more efficient CPU usage.

### Quaternion Rotations

Though not exclusive to this project, and already covered in the [LearnDirectX](https://github.com/MiquelNasarre/LearnDirectX) repository, 
I think it is of great mathematical interest to discuss the motion of the plots. If you have played with the program you might have noticed 
that the movement of the figures feels really natural and not rigid in any way.

That would not be possible if it was not for the quaternions. For a good understanding of how they work and relate to the rotations in a 
three dimensional space i recommend the 3blue1brown videos [Visualizing quaternions](https://www.youtube.com/watch?v=d4EgbgTm0Bg) and 
[Quaternions and 3d rotation](https://www.youtube.com/watch?v=zjMuIxRvygQ), and its associated website https://eater.net/quaternions.

For a brief explanation, if we want to rotate our figure around an axis represented by the unitary vector $v\in\mathbb{R}^3$ by a given 
angle $\theta$. We consider the quaternions 

$$
q = \cos\frac{\theta}{2} + v\cdot(i,j,k)\ \sin\frac{\theta}{2}
$$

$$
q^\prime = \cos\frac{\theta}{2} - v\cdot(i,j,k)\ \sin\frac{\theta}{2}
$$

where $\cdot$ is the dot product. Then we can rotate any point $p\in\mathbb{R}^3$ by again converting it into a quaternion $P=p\cdot(i,j,k)$ 
and performing the operation 

$$
P^r = q\ P\ q^\prime
$$

Every drawable on our program then stores its own quaternion $q_0$, and if you want to add any further rotations given by the quaternion $q$,
is as simple as multiplying both of them to obtain the updated rotation $q_0=q\ q_0$, and for every vertex it computes $P^r = q_0\ P\ q_0^\prime$ 
to know its new position in $\mathbb{R}^3$.

### Representation of the Sphere

Since we will be working with functions on the unit sphere it is important to have a good way to represent it with a finite amount of 
points. One might think of using the parametrization we already have, with $\varphi$ and $\theta$ to subdivide the sphere in squares 
given by subdivisions of the intervals $(0,2\pi)$ and $(0,\pi)$ respectively. Such way however, would result in a very unevenly spaced set 
of points, with singularities on the poles. Therefore, a better way of triangulating the sphere is needed.

Let's introduce then the icosphere. Starting with the platonic solid containing the maximum amount of triangle faces, the icosahedron, 
hence the name, we consider it the icosphere of depth zero. For every value we increase in depth, every triangle of the icosphere gets 
subdivided into four equal-sized triangles, by adding a middle triangle formed by the middle points of each segment. Once the depth is 
the one specified, all the vertexs are normalized, and we have an icosphere. An icosphere of depth $n$ is formed by $20\cdot4^n$ 
triangles, $30\cdot4^n$ aristas and $10\cdot4^n+2$ vertexs.

![progression](https://github.com/MiquelNasarre/FourierS2/assets/124403865/7463d048-9c24-4efd-934f-5a4ca17e3287)

This picture shows the progression from the icosahedron, or depth zero, to the icosphere of depth $4$. 

The program however represents the sphere as the icosphere of depth $6$, with $81920$ faces, $122880$ aristas and $40962$ vertexs. 
Once the first creation of a _FourierSurface_ is called on the application, it generates all the points and triangles that it will be using as 
$\mathbb{S}^2$ for the rest of its runtime. Also, those are the vertexs where the dataset of all the Spherical Harmonic values is generated.

### Computation of the Spherical Harmonics

Given the parametrization of the sphere used on the paper

$$
\Phi:(0,2\pi)\times(0,\pi)\rightarrow\mathbb{S}^2\subset\mathbb{R}^3
$$

$$
(\varphi,\theta) \rightarrow (\sin\theta\cos\varphi,\sin\theta\sin\varphi,\cos\theta)
$$

Let's remember our expression for the spherical harmonics

$$
Y_\ell^m(\varphi,\theta) = \begin{cases}
	\sqrt{\frac{(2\ell + 1)(\ell - m)!}{2\pi(\ell + m)!}} 
	P_\ell^m (\cos\theta) \cos(m\varphi) 
	& \text{si }\ m>0
	\\
	\\
	\sqrt{\frac{2\ell + 1}{4\pi}}P_\ell^0 (\cos\theta) 
	& \text{si }\ m=0
	\\
	\\
	\sqrt{\frac{(2\ell + 1)(\ell - |m|)!}{2\pi(\ell + |m|)!}} 
	P_\ell^{|m|} (\cos\theta) \sin(|m|\varphi)
	& \text{si }\ m<0
\end{cases}
$$

We will divide the expression as in the paper and write $Y_\ell^m(\varphi,\theta) = K_\ell^m P_\ell^m(\cos\theta) \Upsilon_m(\varphi)$.

The constants up front are calculated for all the $\ell$ and $m$ values upon starting the program. Here we can explore the limiting factor that 
does not allow us to go further than $\ell=28$. For storing the values of any decimal number along the program we use _float_'s, this occupy $4$ Bytes 
in memory, and $8$ bits of them are used for storing the power of two. The smallest power of two it can store is $-127$. If we evaluate for example 
$K_{29}^{29}$ we obtain

$$
K_{29}^{29} \approx 1.99871\cdot 10^{-39} \approx 1.36025\cdot2^{-129}
$$

Therefore, such small value cannot be stored in a _float_. This is not a complete limitation, since it would be as easy as using _double_'s which allow 
for bigger precision, nevertheless $\ell\leq28$ allows $841$ coefficients, which I consider good enough for the purposes of this program.

Since most of the time we will be evaluating the Spherical Harmonics on unitary vectors, it is convenient that we find expressions for the 
trigonometric functions of $\varphi$ and $\theta$ only from the vector. Therefore first we compute, given $v=(x,y,z)\in\mathbb{S}^2$

$$
\cos\theta = z
$$

$$
\sin\theta = \sqrt{1-z^2}
$$

$$
\cos\varphi = \frac{x}{\sin\theta}
$$

$$
\sin\varphi = \frac{y}{\sin\theta}
$$

To evaluate $\Upsilon_m(\varphi)$, as discussed in the paper, it uses the Chebyshev polynomials, with its identities

$$
T_n(\cos\theta) = \cos(n\theta)
$$

$$
U_{n-1}(\cos\theta) \sin\theta = \sin(n\theta) 
$$

and its recurrences

$$
T_0(x) = 1,\ \ \ T_1(x) = x
$$

$$
T_{n+1}(x) = 2xT_n(x) - T_{n-1}(x)
$$

$$
U_0(x) = 1,\ \ \ U_1(x) = 2x
$$

$$
U_{n+1}(x) = 2xU_n(x) - U_{n-1}(x)
$$

And to evaluate $P_\ell^m(\cos\theta)$, as discussed in the paper, it uses the following recurrences

$$
P_{\ell+1}^{\ell+1} (x) = -(2\ell - 1) \sqrt{1-x^2} P_\ell^\ell(x)
$$

$$
P_{\ell+1}^\ell (x) =  x(2\ell+1)P_\ell^\ell(x)
$$

$$
(\ell-m+1) P_{\ell+1}^m(x) = (2\ell+1)x P_\ell^m(x) - (\ell+m) P_{\ell-1}^m(x)
$$

Using the first one to get up to $P_m^m$ from $P_0^0 = 1$, then the second one to get $P_{m+1}^m$, 
and finally, the third one to get all the way to $P_\ell^m$.

### Computation of the Fourier Coefficients

The Fourier Series is given by the following expression

$$
r(\varphi,\theta) = \sum_{\ell\in L} \sum_{m=-\ell}^\ell r_\ell^m Y_\ell^m(\varphi,\theta)
$$

Where $r_\ell^m$ are the Fourier coefficients. This sum will be computed for every point of our sphere in order to 
represent the surfaces. Befora that though we have to calculate the coefficients using our triangulation. Given a 
triangulation $\{ T_i \}_{i=0}^N$, let's suppose we already have our subset of frequencies selected, then for each 
frequency given by $(\ell,m)$ we approximate

$$
r_\ell^m = \sum_{i=0}^{N} ||\overline{T}\_i ||\ Y_{\ell}^m\left(\overline{T}_i^p\right) A(T_i^p)
$$

where $\overline{T}_i$ denotes the barycenter of the triangle $T_i$, and $A(T_i^p)$ denotes the area of the spherical 
triangle  $T_i^p$, created by the projection of the vertexs of $T_i$.

We already have a way to compute the Spherical Harmonics, now let's calculate the other two summands. For each triangle 
$T$ we have a collection of 3 vertexs $v_0,v_1,v_2\in\mathbb{R}^3$. The barycenter is just the mean of the vertexs

$$
\overline{T} = \frac{v_0 + v_1 + v_2}{3}
$$

Then the first summand is just its absolute value and the argument of the Spherical Harmonic is this vector divided by its 
absolute value. 

Finally, we have to compute the area of the spherical triangles. As mentioned before, this area is calculated upon 
the creation of the triangulation and stored, because we will be using it lots of times for other calculations. To compute 
it, it uses the formula 

$$
A = \alpha_0 + \alpha_1 + \alpha_2 - \pi
$$

where $\alpha_0$, $\alpha_1$ and $\alpha_2$ are the interior angles of the spherical triangle. To calculate such angles, we use 
the following formulas

$$
\alpha_i = \arccos (u_{ij} \cdot u_{ik}) 
$$

$$
u_{ij} = \frac{(v_i \times v_j) \times v_i}{||(v_i \times v_j) \times v_i||}
$$

Now we have the way to compute the coefficients. But let's remember that we can also set a depth value to make the triangulation 
more fine. All that does is, before starting the calculations, it divides each triangle in $4$ equally sized triangles, the same 
way as the icosphere, for every value of depth we add. Therefore, every additional depth quadruples the amount of time it takes 
to compute the coefficients, that is why caution is advised.

### Lighting the Surfaces and Differentiations of the Spherical Harmonics

As you can see, in the applications all the surfaces are lit up, and you can even change the lights position and color. But how 
do you light a Surface? Let me take this as an opportunity to explain how the _shaders_ in this program work.

Every time you issue a draw call in the program, the set of all the vertexs of the figure you want to draw, and all the indexes 
that create the triangulation of your surface are loaded in your GPU. As well as the quaternion that represents the rotation of 
the figure, the center screen position (for the single/double view), the scale, and each light with its position, intensities and color. 
Each vertex contains the $\mathbb{R}^3$ point, the color of the vertex and a normal vector of the vertex. 

We will consider the two most important steps of the rendering, first comes the vertex shader, where the position on the screen of 
each vertex is calculated, given its $\mathbb{R}^3$ position, its rotation and the scale. Then for every pixel in every triangle given 
by three vertexs it performs the pixel shader, that will return the color value of the pixel. 

The inputs of the pixel shader are the interpolated $\mathbb{R}^3$ position between the vertexs, the interpolated normal vector 
between the vertexs and the interpolated color of the vertexs. It also has the lights information. Then to compute the final color 
it just sees how far away it is from each light source and to know how the light is hitting the surface it does the dot product 
between the normal vector and the vector that goes from the point to the light normalized. Then it gives the color of the pixel 
accordingly.

Then a question arises, how do we calculate the normal vector of a surface? Normally for a point on any given parametrized surface 
the renderer calculates four extra points really close to the one prior, by slightly modifying the two parameters, then it does the 
vector difference to get a basis for the tangent space and performs the vector product to finally obtain the normal vector.

However, given the fact that the surfaces we will be computing are always Fourier Series, and the amount of computation it takes to 
calculate each point, it is best to derivate the Spherical Harmonics mathematically and get the normal vector without having to 
calculate any extra points. The math is detailed in the paper, lets parametrize our surface by 
$\sigma:(0,2\pi)\times(0,\pi)\rightarrow\mathbb{S}^2\subset\mathbb{R}^3$ where

$$
\sigma(\varphi,\theta) = \Phi(\varphi,\theta)r(\varphi,\theta) = \Phi(\varphi,\theta) 
\sum_{\ell\in L} \sum_{m=-\ell}^\ell r_\ell^m Y_\ell^m(\varphi,\theta)
$$

Then the normal vector can by calculated by

$$
N = \frac{\partial_\theta\sigma \times \partial_\varphi\sigma}{||\partial_\theta\sigma \times \partial_\varphi\sigma||}
$$

calculated with the following expressions

$$
\partial_\theta\sigma \times \partial_\varphi\sigma = 
r \cdot \left(-\partial_\theta r\ s\theta\ c\theta\ c\varphi + \partial_\varphi r\ s\varphi + r\ s^2\theta\ c\varphi,\ 
-\partial_\theta r\ s\theta\ c\theta\ s\varphi - \partial_\varphi r\ c\varphi + r\ s^2\theta\ s\varphi,\ 
\partial_\theta r\ s^2\theta + r\ s\theta\ c\theta \right)
$$

$$
\partial_\varphi r = \sum_{m=-\ell}^\ell r_\ell^m\ \partial_\varphi Y_\ell^m(\varphi,\theta)
$$

$$
\partial_\theta r = \sum_{m=-\ell}^\ell r_\ell^m\ \partial_\theta Y_\ell^m(\varphi,\theta)
$$

$$
\partial_\varphi Y_\ell^m(\varphi,\theta) = -m K_\ell^m P_\ell^m(\cos\theta) \Upsilon_{-m}(\varphi)
$$

$$
\partial_\theta Y_\ell^m(\varphi,\theta) = -\sin\theta\ K_\ell^m (P_\ell^m)^\prime(\cos\theta) \Upsilon_m(\varphi)
$$

And to derivate the associated Legendre polinomials it uses the identities

$$
(1 - x^2) \frac{d}{dx}P_\ell^m(x) = (\ell + m)P_{\ell-1}^m(x) - \ell x P_\ell^m(x)
$$

$$
(1 - x^2) \frac{d}{dx}P_\ell^\ell(x) = - \ell x P_\ell^\ell(x)
$$

That way, when generating the dataset, at each point it also calculates and stores the values of the partial derivatives with respect 
to $\varphi$ and $\theta$, allowing for an easy calculation of the normal vector of any surface given by a Fourier Series. 

This way of computing the normal vector however, has a problem. Since our parametrization is not well defined on the poles, 
and we are using it to calculate the normal vector, our normal vector is not well defined on the poles of the surface. 
Therefore, in the program when you load some of the triangulations you will clearly see that the surfaces it creates are not 
correctly lit at their poles.

### Norms & $L^2$-Error Computations

For knowing how many coefficients to compute we will be using the formulas from the paper. For reference the formula for the upper 
and lower bound of the coefficients to compute is

$$
L_f^\pm(\epsilon) = \frac{||df||^2 \pm \epsilon^{-1}
\sqrt{||\Delta f||^2||f||^2 - ||df||^4}}{||f||^2}
$$

Where $L_f^\pm$ determines the upper and lower bounds for the coefficients so that $\ell(\ell + 1) \in \left\[L_f^- , L_f^+ \right]$.

In case you have set some pre-computed values of $\ell$ given by the subset of frequencies $I\subset\Lambda$ it uses the formula 
for the new set upper and lower bounds

$$
L_f(\epsilon,I) = I \cup L_{f^{\Lambda\setminus I}}
\left(\frac{\epsilon||f||}{||f^{\Lambda\setminus I}||}\right)
$$

To calculate all the norms, it applies the formulas from the paper to the triangulations

$$
||r|| \approx \sum_{i=0}^{N} ||\overline{T}_i||^2 A(T_i^p)
$$

$$
||dr|| \approx \sum_{i=0}^N ||\overline{T}_i||^2 \left[ \frac{||\overline{T}_i||^2}{(N\overline{T}_i)^2} - 1\right] A(T_i^p)
$$

$$
||\Delta r|| \approx \sum_{i=0}^V \left[\frac
{4\sum_{j\in N(i)} (\cot\alpha_{ij} + \cot\beta_{ij})(||v_j||-||v_i||)}
{\sum_{j\in N(i)} (\cot\alpha_{ij} + \cot\beta_{ij})||p_j - p_i||^2} 
\right]^2\frac{1}{3}\sum_{j\in N(i)} A(T_{ij}^p)
$$

As said before, upon creation of each _Polihedron_ it generates the neighborhood of the triangulation by calling the function 
_generateNeighbourhood_. What it does is it iterates through all the triangles, orientating them correctly by using 
their normal vector and carefully filling up an array of neighbors for each and every vertex, with its corresponding angles. 
This function is responsible for computing and storing the values of the area of each triangle, the area associated to each vertex, 
the neighbors of each vertex and how many they have, the angles of each neighbor of each vertex, and the normal vector of each 
triangle. It is an interesting piece of code. 

### Interpolations

There is not much to the inner works of the interpolations. What it really does is store two different values for every vertex 
of the sphere and given a $t$ value it interpolates between them at the vertex shader. It is done like this because this way is 
much more computationally efficient than interpolating the coefficients and recomputing the full surface every frame. In the case 
of the curves it does it the non-efficient way, and that is why it can produce lag sometimes.

## Issues & Future Implementations

The program is not still fully finished and there are some future plans for it, here I will discuss some of the most important ones.

- **More efficient Spherical Harmonics calculations:** There are some ways where the calculations can be improved, for example every
  associated Legendre polynomial is calculated twice, for the positive and negative value of $m$, which is clearly inefficient. A much
  bigger improvement though, would be calculating the Spherical Harmonics using polinomials, as described in the paper. That would mean
  completely changing the way they are currently being calculated, but it would really improve performance.

- **Coefficients view feature:** It is a difficult concept to conceive, an intuitive way to view the set of coefficients
  $L\subset \mathbb{Z}^2$, so for the moment this feature is a plan for the future.

- **Size efficient storage of the files:** Since one of the benefits I said of using such series is the reduction in size of the
  files it would be interesting to explore the subject and try to store the files as efficiently as possible, and see how far it can get.

- **Drag and drop files:** This and some other user friendly ways of comfortably using the application may be introduced in the future.
  For example, being able to drop multiple files at the same time and the program creating an interpolation between them could be interesting 
  and allow for real applications of the program. Nevetheless, as said at the beggining this program is just a showcase, so it is not
  intended and does not need to have such capabilities.

I am really proud of how the application is looking so far, it is really efficient with computation and storage and really user friendly. 
But future installments might come, since it has been a long journey getting to this result and I want people who use it to see such 
efforts and feel that it has been done by someone with enthusiasm for the project.

## Credits

In the development of this program I have looked for help in many different places but three of them have been essential along the 
development.

- First, I want to mention my professor David Marín who gave me the ideas and the mathematical tools for this project.

- I want to mention [ocornut](https://github.com/ocornut) the developer of [ImGui](https://github.com/ocornut/imgui), a repository
  essential for the project since it has provided with all the widgets seen in the application. I greatly recommend it for any
  graphics project that may need such tools.

- And finally [ChiliTomatoNoodle](https://www.youtube.com/@ChiliTomatoNoodle) and its YouTube series
  [C++ 3D DirectX Programming](https://www.youtube.com/playlist?list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD), that has been essential
  for my learning of 3D programming. It provides a really understandable and detailed introduction for anyone who wants to learn
  DirectX programming, and I greatly recommend it.


