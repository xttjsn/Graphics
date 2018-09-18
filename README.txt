**Design choices:

1. I use inheritance when developing ConstantBrush, LinearBrush and QuadraticBrush. Specifically, I made it that ConstantBrush is the parent of both LinearBrush and QuadraticBrush. This decision was made because the the only difference among those brushes is the mask. As a result, the only thing that I need to write in LinearBrush and QuadraticBrush is minumum - different formula for calculating the mask. The only disadvantage about this approach is probably that the constructor of ConstantBrush
will call makeMask, and thus LinearBrush's constructor will call ConstantBrush's makeMask before it, and the same goes for QuadraticBrush. This causes some useless overhead at the constructor, but I think the advantage of this approach overweighs the disadvantage.

2. The SmudgeBrush uses a quadratic dropoff function, so I made it a child of QuadraticBrush. However, its brushDown and brushDragged functions are quite different, so I override it.

3. For quadratic dropoff function, I used (1.f - i / r)^2, where i is the distance between the pixel and the center of the click, and r is the radius.

4. Since I am doing the extra credit, so I developed a special brush - SpecialBrush1. For the SpecialBrush1, I tried to mimic the spider web brush shown in the demo. Basically, we have to memorize where the mouse has visited. We store that information in a boolean matrix `m_visited` of the same size as the canvas. If pixel (x, y) has been visited before, `m_visited[x][y] = 1`, otherwise `m_visited[x][y] = 0`. When the mouse visit a pixel, say at (x, y), for every pixel in the cicle of radius r, and center (x, y), we check whether it's been visited before.
And if so, we draw a stright line between that pixel and the center, with some offset in both ends.  A small difference between my version and the demo's version though, is that my line drawing algorithm does not have anti-aliasing enabled, and as a result, the line is sometimes discrete and jaggy. This could be improved by anti-aliasing techniques.

5. Alpha blending. To enable alpha blending, I add some variables in the Brush class. The reason for this is that ideally, alpha blending should be a feature that all brushes can have (although in the demo only ConstantBrush, LinearBrush and QuadraticBrush has that feature). I also separate the paint function to 2 cases depending on whether it has `fixAlphaBlending` enabled or not - paintFixAlpha and paintNormal. When fixAlphaBlending is enabled, we will first copy the canvas to a local
cache, and later use that cache as the pixel source instead of the current canvas (which might have been changed). Then inside the paint function, we will accumulate the mask value until it's 1.0f, therefore we have to store the previous mask of every pixel in a cache of the same size as the canvas.

6. In BGRA operator+, I add the glm::min(result, 255) to prevent the value from overflowing. Additionally, to facilitate BGRA * float value, I add an operator* in BGRA.cpp which deals with that situation.

7. I did not use the lerp function.

8. I reset the brush everytime the setting changes.

**Known Bugs:

ConstantBrush - None
LinearBrush - None
QuadraticBrush - None
SmudgeBrush - None
SpecialBrush1 - Lines are sometimes jaggy.
