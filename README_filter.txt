README for filter assignment

This is the readme for filter assignment.

In this assignment I created 7 filters: EdgeDetection, Blur, Scale, Rotate, Smoothing FFT, Edge Detection FFT and Sharpening FFT.

I firstly created a base class "Filter", which has the public method "apply", which accepts "Canvas2D" as a parameter and filter it accordingly. The Canvas2D class has a single unique_ptr of Filter type, which gets initialized to its appropriate derived class whenever Canvas2D is initialized or the setting is changed.

--Edge Detection:
For edge detection, I used a separable sobel kernel as described in the handout, which proves to be very fast and effective. I encountered one caveat where I didn't use floating point to store the value from intermediate pass, and as a result the negative values are trunked to 0 and the result looks bad. After I adjusted it, it works fine. Marquee selection is supported.

--Blur:
I implemented 7 different blur filters for comparison purpose.
    apply_ng: This is the gaussian filter where no separable kernel or pre-computation is used. It's slow.
    apply_ngs: This is the gaussian filter where separable kernel is used. It's conceivable faster than the previous one.
    apply_ngp: This is the gaussian filter where I pre-compute the kernel weights and store them in a vector. It's noticeably faster.
    apply_ngsp: This is the gaussian filter where both separable kernel and pre-computation are used. It's the fastest gaussian filter.
    apply_bo: This is the naive box filter, where no separable kernel is used. The kernel weight are one because I divide the result by the sum of used weights at the end of the computation.
    apply_bs: This is the separable box filter. It's the fastest filter so far.
    apply_fb: This is THE fastest filter of all. It uses O(1) space and O(1) time for each pixel! I've written down the idea of it in filterblur.cpp, but the core idea is based on the fact that adjacent values only differ by the difference of the value of the left pixel and the right pixel. This is also the filter that get's called when you click filter!

Marquee selection is supported.

--Scale:
I implemented a separable scale filter by using the formula given in the lecture notes.
Marquee selection is supported.

--Rotate:
I implemented a rotation filter. First I compute the scaled width and height, and then used back-map to find the corresponding location of each new pixel in the un-rotated image. This location probably contains fractions. To deal with fractions, I used the bicubic interpolation technique given in lab05 via glm::mix.

Issues for Rotate:
There are some defects I noticed. Firstly the edge handling isn't perfect, as I simply ignores those locations beyond bound. Ideally one could interpolate the value between it and the nearest valid pixel value. Secondly when rotated multiple times, the image gradually gets blurry, which I assume is the result of interpolation.

--Fourier Transform:
I implemented both naive DFT and FFT using Cooley-Tukey's method. Currently their only function is to transform the image to frequency domain and then transform back to spatial domain. The DFT, as expected, runs incredibly slowly. But the FFT runs much faster.

To facilitate coding on three channels of complex numbers, I created a new class RGBComplex which holds those values. I also created functions like fft, ifft (inverse fft), fft2 (fft for 2d), and ifft2(inverse fft for 2d) for code readability and reuse.

Based on this, I created three more filters: smoothing, edge detection and sharpening.

For smoothing, I choose a somewhat arbitrary ratio and computed an upper bound such that all frequencies (u, v) with absolute value sqrt(u^2 + v^2) above that limit are truncated (set to 0). In code I didn't do sqrt but use the square for comparison, which saves some computation.

Similarly for edge detection, I choose a lower bound such that all frequencies (u, v) with absolute value lower than the limit are truncated.

For sharpening, one cannot simply truncate the low frequencies since it will only preserve the edges. Instead, I truncated an intermediate range of frequencies.

Issues for FFT:
I noticed that the filtered results aren't ideal (lose of image features). I suspect that this is due to the fact that I simply remove all amplitudes for frequencies not desired. Maybe it is a better idea to just multiply the amplitude with a small constant instead of 0. Additionally, it's still not super fast, which is due to the recursion used in my FFT. It's known that there are faster FFT methods, but I haven't had time to implement them.
