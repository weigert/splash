/*
SDL_gifwrap v0.9.0
by Jonathan Dearborn
Pronounce it however you like...  I dare you.
SDL_gifwrap is a wrapper for giflib that makes loading and saving multi-frame GIF files reasonable.
It does not attempt to duplicate animation features, but rather loads and stores all the information you need
in a convenient container.
SDL_gifwrap manipulates the following objects:
Image - GIF_Image is the container for all GIF data
Frame - GIF_Frame is the container for a single frame of GIF bitmap data
Surface - SDL_Surface is used to store GIF bitmap data when loading a GIF_Image
Palette - SDL_Palette is used to store GIF palette data
Indices - These represent a single frame of GIF bitmap data and each index into a palette to refer to an RGB color
// TODO: Support indices on load, surfaces on save.
// Maybe remove surface from struct and rely on functions to do the conversion to/from indices.

Limitations:
* Color resolution is not handled.  Instead, it is assumed that the image is stored as 8 bits per color channel (a color resolution value of binary 111).
In particular, this means that images with reduced color resolutions, say, optimized black & white images, may not be loaded correctly and may not be saved that way.
* Saving to and loading from arbitrary (non-file) data sources via SDL_rwops is not implemented yet.
== Typical loading usage ==
Load image data
    GIF_LoadImage()
Access gif->frames[i]->surface and gif->frames[i]->delay
Free image data
    GIF_FreeImage()
== Typical saving usage ==
Create data buffer
    GIF_CreateImage()
Set size of GIF screen
    GIF_SetCanvasSize()
Set global palette
    GIF_CreatePalette() if using an array of SDL_Color objects
    GIF_SetGlobalPalette()
Set animation loop count, GIF_LOOP_FOREVER is normal
    GIF_SetLooping()
Create and add frames
    Fill an array of indices or an SDL_Surface with bitmap data
    Create frame buffer
        GIF_CreateFrameIndexed() or GIF_CreateFrame()
    Set local palette if using per-frame palette
        GIF_SetLocalPalette()
    Set frame delay (in milliseconds, though GIF will truncate to hundredths)
        GIF_SetDelay()
    Set frame's transparent color
    GIF_SetTransparentIndex() or GIF_SetTransparentColor()
    Add frame to image container
        GIF_AddFrame()
Write GIF file to disk
    GIF_SaveImage()
Free image data
    GIF_FreeImage()
Descriptions of other useful functions can be found in this header file.
== EXAMPLE ==
GIF_Image* gif = GIF_LoadImage("my_animation.gif");
Uint16 i;
for(i = 0; i < gif->num_frames; ++i)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gif->frames[i]->surface);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);
    SDL_Delay(gif->frames[i]->delay);
}
// Make changes...
GIF_Frame* new_frame = GIF_CreateFrame(other_surface, SDL_TRUE);  // Ownership is transferred to gif
GIF_SetDelay(new_frame, 500);
GIF_AddFrame(image, new_frame);
// Then save.
GIF_Save(gif, "new_animation.gif");
GIF_FreeImage(gif);
== LICENSE ==
SDL_gifwrap is licensed under the terms of the MIT license:
Copyright (c) 2017 Jonathan Dearborn
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _SDL_GIFWRAP_H__
#define _SDL_GIFWRAP_H__


#ifdef __cplusplus
extern "C" {
#endif



#define GIF_LOOP_FOREVER 0


typedef struct _GIF_Frame GIF_Frame;


// GIF container
// Represents the data needed to render a full GIF file
typedef struct _GIF_Image
{
    Uint16 width, height;
    SDL_Palette* global_palette;

    SDL_bool use_background_color;  // SDL_FALSE is default and uses background_index
    Uint8 background_index;  // Defaults to 0
    SDL_Color background_color;

    Uint16 num_loops;  // 0 for loop animation forever

    Uint16 num_frames;
    Uint32 frame_storage_size;
    GIF_Frame** frames;

    Uint16 num_comments;
    char** comments;

} GIF_Image;


// Represents a single image (or animation frame)
typedef struct _GIF_Frame
{
    int left_offset, top_offset;  // Relative to containing image canvas
    Uint16 width, height;

    SDL_Palette* local_palette;

    SDL_bool use_transparent_color;
    SDL_Color transparent_color;

    SDL_bool use_transparent_index;
    Uint8 transparent_index;


    SDL_bool overlay_previous;  // GIF disposal method
    Uint32 delay;  // In milliseconds (though GIF stores hundredths, so it is truncated when saved).  Defaults to 100.


    // Loaded frames will have an SDL_Surface
    SDL_Surface* surface;
    SDL_bool owns_surface;

    // You can save frames by just palette indices
    Uint8* indices;

} GIF_Frame;



// Create and initialize a blank GIF image.
extern DECLSPEC GIF_Image* SDLCALL GIF_CreateImage(void);

// Load a GIF image from a file.
extern DECLSPEC GIF_Image* SDLCALL GIF_LoadImage(const char* filename);

// Load a GIF image from an arbitrary data source.
//extern DECLSPEC GIF_Image* SDLCALL GIF_LoadImage_RW(SDL_RWops* rwops, int freerwops);

// Save the given image to a file.  Returns SDL_FALSE on failure.
extern DECLSPEC SDL_bool SDLCALL GIF_SaveImage(GIF_Image* image, const char* filename);

// Save the given image to an arbitrary destination.  Returns SDL_FALSE on failure.
//extern DECLSPEC SDL_bool SDLCALL GIF_SaveImage_RW(GIF_Image* image, SDL_RWops* rwops, int freerwops);

// Free the memory allocated for the given image.
extern DECLSPEC void SDLCALL GIF_FreeImage(GIF_Image* image);


// Sets the canvas size.
extern DECLSPEC void SDLCALL GIF_SetCanvasSize(GIF_Image* image, Uint16 width, Uint16 height);

// Sets the background color.
extern DECLSPEC void SDLCALL GIF_SetBackgroundColor(GIF_Image* image, SDL_Color background_color);

// Sets the background color index.
extern DECLSPEC void SDLCALL GIF_SetBackgroundIndex(GIF_Image* image, Uint8 background_color_index);

// Sets the number of animation loops.  0 loops means to loop forever.
extern DECLSPEC void SDLCALL GIF_SetLooping(GIF_Image* image, Uint16 num_loops);




// Sets the global palette of the given image.  The given palette's ownership is transferred to the GIF image, so it will be freed by GIF_Free().
extern DECLSPEC void SDLCALL GIF_SetGlobalPalette(GIF_Image* image, SDL_Palette* palette);


// These palette functions are just helpful.  You can manage palette data yourself with SDL_AllocPalette(), SDL_SetPaletteColors(), and SDL_FreePalette().

// Returns a new palette with a range of colors.
extern DECLSPEC SDL_Palette* SDLCALL GIF_CreateBasicPalette(void);

// Creates a new palette from the given colors.  GIF can only use the first 256 colors.  The colors are copied into a new palette object.
extern DECLSPEC SDL_Palette* SDLCALL GIF_CreatePalette(int ncolors, SDL_Color* colors);

// Copies a palette.
extern DECLSPEC SDL_Palette* SDLCALL GIF_CopyPalette(SDL_Palette* palette);



// Adds the given frame to the end of the given image.  The given frame's ownership is transferred to the image, so it will be freed by GIF_FreeImage().
extern DECLSPEC void SDLCALL GIF_AddFrame(GIF_Image* image, GIF_Frame* frame);

// Inserts the given frame before the frame at a specified index of the given image.  The given frame's ownership is transferred to the image, so it will be freed by GIF_FreeImage().
extern DECLSPEC void SDLCALL GIF_InsertFrame(GIF_Image* image, GIF_Frame* frame, Uint16 index);

// Removes a frame from the given image and returns the frame.
extern DECLSPEC GIF_Frame* SDLCALL GIF_RemoveFrame(GIF_Image* image, Uint16 index);

// Removes a frame from the given image and frees the frame.
extern DECLSPEC void SDLCALL GIF_DeleteFrame(GIF_Image* image, Uint16 index);


// Creates a new frame and returns it for further modification.  If give_ownership is true, the given surface's ownership is transferred to the GIF frame, so it will be freed by GIF_FreeFrame() or GIF_FreeImage().
extern DECLSPEC GIF_Frame* SDLCALL GIF_CreateFrame(SDL_Surface* surface, SDL_bool give_ownership);

// Creates a new frame and returns it for further modification.  The given indices are copied.
extern DECLSPEC GIF_Frame* SDLCALL GIF_CreateFrameIndexed(Uint16 width, Uint16 height, Uint8* indices);

// Free the memory allocated for the given frame.
extern DECLSPEC void SDLCALL GIF_FreeFrame(GIF_Frame* frame);



// Sets the global palette of the given frame.  This will override the global palette for this frame.  The given palette's ownership is transferred to the GIF image, so it will be freed by GIF_Free().
extern DECLSPEC void SDLCALL GIF_SetLocalPalette(GIF_Frame* frame, SDL_Palette* palette);

// Sets the given frame's delay time to the given delay, in milliseconds.  GIF only stores delays in hundredths of a second, so the saved image will truncate the given delay.
extern DECLSPEC void SDLCALL GIF_SetDelay(GIF_Frame* frame, Uint32 delay);

// Enables transparency for the given frame and sets the transparent color.
extern DECLSPEC void SDLCALL GIF_SetTransparentColor(GIF_Frame* frame, SDL_Color transparent_color);

// Enables transparency for the given frame and sets the transparent color index.
extern DECLSPEC void SDLCALL GIF_SetTransparentIndex(GIF_Frame* frame, Uint8 color_index);

// Disables transparency for the given frame.
extern DECLSPEC void SDLCALL GIF_UnsetTransparency(GIF_Frame* frame);

// Sets the disposal method for the colors of the previous frame.  SDL_FALSE discards the previous frame colors.
extern DECLSPEC void SDLCALL GIF_SetDisposal(GIF_Frame* frame, SDL_bool overlay_previous);


#ifdef __cplusplus
}
#endif

#endif
