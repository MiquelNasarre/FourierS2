# Fourier Series for Triangulizations of Star-Shaped Surfaces

This is the main project showcasing the formulas found on my universities final project. It handles the 
formulas for the case in $L^2(\mathbb{S}^2)$, in particular for the case of triangularized surfaces.

To do that it follows the math found in the paper, caculating the Fourier coefficients of figures stored 
in data files. Then computing the Fourier Series and displaying it in an interactive 3D view.

The next image showcases the basis for the $L^2(\mathbb{S}^2)$ functions, the Spherical Harmonics, as seen 
in the application. THe areas shaded in blue correspond to the positive values of the functions, and the 
ones shaded in yellow to the negative. All values are made positive for better view.

![Colage](https://github.com/MiquelNasarre/FourierS2/assets/124403865/09cb5a4e-1da1-42a6-b77b-6c1559bf8a05)

This image shows for values of $\ell$ up to $5$, the program uses values up to $\ell=28$, with $|m|\leq\ell$. 

With this capabilities it can render lots of triangularized surfaces, here in the next image you can see some 
examples for $\ell\leq 20$.

![collage2](https://github.com/MiquelNasarre/FourierS2/assets/124403865/c3f30c3a-8c91-41e3-8d45-50ff03338ed1)

The figures on the right are the original triangularized surfaces, while the ones on the left are their Fourier 
Series.

This kind of computations allows for a big reduction in size, since the Fourier coefficients weight considerably 
less than the triangularizations, while maintaining a very small error. And also having it stored like this 
allows for easier compatibility. For example you can interpolate between surfaces just by interpolating coefficients, 
or you can train AI much easier with a list of a couple hundred coefficients than with a fully triangulated surface.

Therefore this program is a showcase of the capabilities of the math seen in the paper, as well as a great interactive 
tool to better understand how the series work.

## Requirements

- [Visual Studio](https://visualstudio.com) (For the program to be able to compile all the diffent C++ files correctly)

All the rendering of the program, as we will see, uses the DirectX11 dependecies, which are part of the Windows OS, and 
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

Before going through all the widgets and how they work lets explore how to interact with directly with the plots. On any loaded 
plot you can drag it around with the mouse, changing its orientation. You can use the mouse wheel to resize it, and if you use 
it while holding the mouse button the figure will rotate on its axis.

There are also some keyboard interactions, if you maintain pressed `R` or `T`, the plot will return to its original position. 
If you press `S` the plot will stop its current motion. The `M` key hides or shows the menu, and you can also use the program 
at full screen by pressing `F11` to enter and `esc` to exit.

Now lets go through its features. Upon running the program you are presented with the Spherical Harmonics, which will shows as 
the only Figure loaded so far. The menu is located on the top left corner. The following image shows the menu as seen upon opening

![menu0](https://github.com/MiquelNasarre/FourierS2/assets/124403865/30da0d36-d4e5-4870-9eaf-7b98ab33de75)

For this first plot you can use the sliders to change the $\ell$ end $m$ values, as said before the program uses $\ell\leq 28$ and 
$|m|\leq\ell$. Up at the top bar we have different menus: 

- The `Figure` menu displays the figures you have currently loaded in the program, once you load new files they will be displayed there.

- The `Advanced` menu has two different options, it recommended to press the first option `Generate Dataset` upon starting the program.
  This will compute and store all the values for all the Spherical Harmonics in every point of our representation of the sphere. It will
  take around 400MB on the RAM memory, but it will clearly iprove the performance of the program.

## How it Works

The application uses the [LearnDirectX](https://github.com/MiquelNasarre/LearnDirectX) repository as the core for all 
the 3D rendering. For a more in depth explanation of how all the rendering works I suggest you take a look at it. 
This also means that it inherits the [ImGui](https://github.com/ocornut/imgui) dependencies for all its widgets.

Therefore the Fourier project is built as an application of the previously mentioned repository. The main file just calls 
the creation of a _Fourier_ class, which creates the window and runs it, calling the main loop until the window is closed. 
Lets make a brief run through all its files before getting into its features.

- **Fourier.cpp / Fourier.h:** As mentioned this is the center class of our application. It is set to run at 60fps, and every 
  frame it processes all input events, and calls all the other functions needed to dysplay accordingly. The _processEvents_ 
  function is the one that takes care of all the processing, while the _doFrame_ does all the rendering. The processing includes 
  all the Fourier computations as well as all the Keyboard and Mouse updates and rendering updates.

- **IG_Fourier.cpp / IG_Fourier.h:** Since I wanted to keep separated the ImGui dependecies from the rest of my code this class 
  is the one that takes care of all the widgets. It comunicates with the _Fourier_ class only via the _IG_ struct.

- **Polihedron.cpp / Polihedron.h:** This is a _Drawable_ class which is used to store the triangularized surfaces, and draw and 
  interact with them. Upon creation it also does some heavy computations. It calculates all the triangle projected areas, and the 
  angles within them, the normal vector of each triangle and it also creates a neighbourhood for each vertex. All of which are 
  stored and can be called for future calculations involving the triangularization, as we will see later.

- **FourierSurface.cpp / FourierSurface.h:** This is a _Drawable_ class which is used to store the Fourier Series, and draw and
  interact with them. It also takes the main role with all the Fourier related computations. Upon creation it uses an existing
  _Polihedron_ to compute its fourier coefficients, and its subclass _Functions_ takes care of all the math involved. This subclass
  also contains the functions used to compute all the norms used for the error estimations.

  Within the _FourierSurface_ class we can also find the _FileManager_ class, which as the name indicates is used for file handling,
  and a _curve_ class, to display the curves on top of the surfaces.

- **Interpolated.cpp / Interpolated.h:** This file constains two _Drawable_ classes used for creating the interpolations between
  different _FourierSurface_'s. the _Interpolated_ class contains a single interpolation between two surfaces and the
  _InterpolatedString_ class contains multiple instances of the previous one, to interpolate between any number of surfaces.
