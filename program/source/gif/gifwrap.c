/*
    Note: This source code was taken from
    grimfang4 on github!

    ...because I was too lazy to write it myself.
*/

#include "gifwrap.h"
#include "gif_lib.h"
#include <string.h>
#include <stdlib.h>

#define MIN(x,y) ((x) < (y)? (x) : (y))

// Create and initialize a blank GIF image.
DECLSPEC GIF_Image* SDLCALL GIF_CreateImage(void)
{
    GIF_Image* result = (GIF_Image*)SDL_malloc(sizeof(GIF_Image));
    if(result == NULL)
        return NULL;

    memset(result, 0, sizeof(GIF_Image));

    result->background_color.r = result->background_color.g = result->background_color.b = 0;
    result->background_color.a = 255;

    return result;
}

static SDL_Surface* gif_create_surface32(Uint32 width, Uint32 height)
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    #else
        return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    #endif
}


static Uint32 get_pixel(SDL_Surface *Surface, int x, int y)
{
    Uint8* bits;
    Uint32 bpp;

    if(x < 0 || x >= Surface->w)
        return 0;  // Best I could do for errors

    bpp = Surface->format->BytesPerPixel;
    bits = ((Uint8*)Surface->pixels) + y*Surface->pitch + x*bpp;

    switch (bpp)
    {
        case 1:
            return *((Uint8*)Surface->pixels + y * Surface->pitch + x);
            break;
        case 2:
            return *((Uint16*)Surface->pixels + y * Surface->pitch/2 + x);
            break;
        case 3:
            {
                // Endian-correct, but slower
                Uint8 r, g, b;
                r = *((bits)+Surface->format->Rshift/8);
                g = *((bits)+Surface->format->Gshift/8);
                b = *((bits)+Surface->format->Bshift/8);
                return SDL_MapRGB(Surface->format, r, g, b);
            }
            break;
        case 4:
            return *((Uint32*)Surface->pixels + y * Surface->pitch/4 + x);
            break;
    }

    return 0;  // FIXME: Handle errors better
}


static void set_pixel(SDL_Surface* surface, int x, int y, Uint32 color)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8* bits = ((Uint8 *)surface->pixels) + y*surface->pitch + x*bpp;

    /* Set the pixel */
    switch(bpp)
    {
        case 1:
            *((Uint8 *)(bits)) = (Uint8)color;
            break;
        case 2:
            *((Uint16 *)(bits)) = (Uint16)color;
            break;
        case 3: { /* Format/endian independent */
            Uint8 r,g,b;
            r = (color >> surface->format->Rshift) & 0xFF;
            g = (color >> surface->format->Gshift) & 0xFF;
            b = (color >> surface->format->Bshift) & 0xFF;
            *((bits)+surface->format->Rshift/8) = r;
            *((bits)+surface->format->Gshift/8) = g;
            *((bits)+surface->format->Bshift/8) = b;
            }
            break;
        case 4:
            *((Uint32 *)(bits)) = (Uint32)color;
            break;
    }
}

// Load a GIF image from a file.
DECLSPEC GIF_Image* SDLCALL GIF_LoadImage(const char* filename)
{
    int error;
    GifFileType* gif = DGifOpenFileName(filename, &error);
    if(gif == NULL)
        return NULL;

    if(DGifSlurp(gif) == GIF_ERROR)
    {
        DGifCloseFile(gif, &error);
        return NULL;
    }

    GIF_Image* result = (GIF_Image*)SDL_malloc(sizeof(GIF_Image));
    memset(result, 0, sizeof(GIF_Image));
    result->width = gif->SWidth;
    result->height = gif->SHeight;

    // Unused?
    //gif->SColorResolution  // Number of bits per color channel, roughly.
                             // Binary 111 (7) -> 8 bits per channel
                             // Binary 001 (1) -> 2 bits per channel
    //gif->AspectByte
    //gif->SBackGroundColor

    SDL_Palette* global_palette = NULL;
    int i;
    int j;

    if(gif->SColorMap != NULL)
    {
        SDL_Color* global_colors = (SDL_Color*)SDL_malloc(sizeof(SDL_Color)*gif->SColorMap->ColorCount);
        for(i = 0; i < gif->SColorMap->ColorCount; ++i)
        {
            global_colors[i].r = gif->SColorMap->Colors[i].Red;
            global_colors[i].g = gif->SColorMap->Colors[i].Green;
            global_colors[i].b = gif->SColorMap->Colors[i].Blue;
            global_colors[i].a = 255;
        }

        global_palette = GIF_CreatePalette(gif->SColorMap->ColorCount, global_colors);
    }

    for(j = 0; j < gif->ExtensionBlockCount; ++j)
    {
        if(gif->ExtensionBlocks[j].Function == APPLICATION_EXT_FUNC_CODE)
        {
            if(gif->ExtensionBlocks[j].ByteCount >= 14)
            {
                char name[12];
                memcpy(name, gif->ExtensionBlocks[j].Bytes, 11);
                if(strcmp(name, "NETSCAPE2.0") == 0)
                {
                    result->num_loops = gif->ExtensionBlocks[j].Bytes[12] + gif->ExtensionBlocks[j].Bytes[13]*256;
                }
            }
        }
    }

    result->num_frames = gif->ImageCount;
    result->frames = (GIF_Frame**)SDL_malloc(sizeof(GIF_Frame*)*result->num_frames);
    memset(result->frames, 0, sizeof(GIF_Frame*)*result->num_frames);

    for(i = 0; i < gif->ImageCount; ++i)
    {
        SavedImage* img = &gif->SavedImages[i];

        GIF_Frame* frame = (GIF_Frame*)SDL_malloc(sizeof(GIF_Frame));
        memset(frame, 0, sizeof(GIF_Frame));

        result->frames[i] = frame;

        // Load basic attributes
        frame->width = img->ImageDesc.Width;
        frame->height = img->ImageDesc.Height;
        frame->left_offset = img->ImageDesc.Left;
        frame->top_offset = img->ImageDesc.Top;


        // Load palette
        if(img->ImageDesc.ColorMap != NULL)
        {
            SDL_Color* local_colors = (SDL_Color*)SDL_malloc(sizeof(SDL_Color)*img->ImageDesc.ColorMap->ColorCount);
            for(j = 0; j < img->ImageDesc.ColorMap->ColorCount; ++j)
            {
                local_colors[j].r = img->ImageDesc.ColorMap->Colors[j].Red;
                local_colors[j].g = img->ImageDesc.ColorMap->Colors[j].Green;
                local_colors[j].b = img->ImageDesc.ColorMap->Colors[j].Blue;
                local_colors[j].a = 255;
            }

            frame->local_palette = GIF_CreatePalette(img->ImageDesc.ColorMap->ColorCount, local_colors);
        }

        SDL_Palette* pal = global_palette;
        if(frame->local_palette != NULL)
            pal = frame->local_palette;

        // Look for graphics extension to get delay and transparency
        for(j = 0; j < img->ExtensionBlockCount; ++j)
        {
            if(img->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE)
            {
                Uint8 block[4];
                memcpy(block, img->ExtensionBlocks[j].Bytes, 4);

                // Check for transparency
                if(block[0] & 0x01)
                    frame->use_transparent_index = SDL_TRUE;
                frame->transparent_index = block[3];

                // Disposal mode
                frame->overlay_previous = (SDL_bool)((block[0] & 0x08) != 0x08);

                // Reconstruct delay
                frame->delay = 10*(block[1] + block[2]*256);

                // Get transparent color
                if(pal != NULL && frame->transparent_index < pal->ncolors)
                    frame->transparent_color = pal->colors[frame->transparent_index];
            }
        }

        int count = img->ImageDesc.Width * img->ImageDesc.Height;
        frame->surface = gif_create_surface32(frame->width, frame->height);
        frame->owns_surface = SDL_TRUE;
        if(pal != NULL)
        {
            for(j = 0; j < count; ++j)
            {
                SDL_Color c = pal->colors[img->RasterBits[j]];
                if(frame->transparent_index == img->RasterBits[j])
                    c.a = 0;

                set_pixel(frame->surface, j%frame->width, j/frame->width, SDL_MapRGBA(frame->surface->format, c.r, c.g, c.b, c.a));
            }
        }
    }

    DGifCloseFile(gif, &error);

    return result;
}

// Load a GIF image from an arbitrary data source.
/*DECLSPEC GIF_Image* SDLCALL GIF_LoadImage_RW(SDL_RWops* rwops, int freerwops)
{
    // TODO: Implement this.
    return NULL;
}*/


static ColorMapObject* gif_copy_palette(GifFileType* gif, SDL_Palette* palette)
{
    if(palette == NULL || palette->ncolors <= 0)
        return NULL;

    int num_colors = MIN(palette->ncolors, 256);
    GifColorType* colors = (GifColorType*)SDL_malloc(sizeof(GifColorType)*num_colors);
    SDL_Color* source_colors = palette->colors;
    int i;
    for(i = 0; i < num_colors; ++i)
    {
        colors[i].Red = source_colors[i].r;
        colors[i].Green = source_colors[i].g;
        colors[i].Blue = source_colors[i].b;
    }

    ColorMapObject* result = GifMakeMapObject(num_colors, colors);
    SDL_free(colors);

    return result;
}

static Uint8 gif_find_matching_index(SDL_Palette* palette, SDL_Color background_color)
{
    if(palette == NULL)
        return 0;

    int num_colors = MIN(palette->ncolors, 256);

    int i;
    for(i = 0; i < num_colors; ++i)
    {
        if(background_color.r == palette->colors[i].r
            && background_color.g == palette->colors[i].g
            && background_color.b == palette->colors[i].b)
            return i;
    }

    return 0;
}


static SDL_bool gif_add_loop_extension(GifFileType* gif, int num_loops)
{
    if(EGifPutExtensionLeader(gif, APPLICATION_EXT_FUNC_CODE) == GIF_ERROR)
        return SDL_FALSE;

    if(EGifPutExtensionBlock(gif, 11, "NETSCAPE2.0") == GIF_ERROR)
        return SDL_FALSE;

    Uint8 block[3] = {1, num_loops % 256, num_loops / 256};  // Loops bytes are low, high right?
    if(EGifPutExtensionBlock(gif, 3, block) == GIF_ERROR)
        return SDL_FALSE;

    return (SDL_bool)(EGifPutExtensionTrailer(gif) != GIF_ERROR);
}


static int gif_get_closest_palette_match(Uint8 r, Uint8 g, Uint8 b, SDL_Color* palette, int num_colors)
{
    int distance = 1000;
    int result = 0;

    int index = 0;
    int i;
    for(i = 0; i < num_colors; ++i)
    {
        int dist = abs((int)(palette[i].r) - r) + abs((int)(palette[i].g) - g) + abs((int)(palette[i].b) - b);
        if(dist < distance)
        {
            result = index;
            distance = dist;
        }
        ++index;
    }

    return result;
}

static void gif_copy_surface_to_indices(GIF_Frame* frame, SDL_Palette* global_palette)
{
    SDL_Surface* surface = frame->surface;
    if(surface == NULL)
        return;

    SDL_Palette* pal = (global_palette != NULL? global_palette : frame->local_palette);
    if(pal == NULL)
        return;

    int n = 0;
    Uint8 r,g,b;

    int i,j;
    for(i = 0; i < surface->w; ++i)
    {
        for(j = 0; j < surface->h; ++j)
        {
            Uint32 pixel = get_pixel(surface, i, j);
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            frame->indices[n] = gif_get_closest_palette_match(r, g, b, pal->colors, MIN(pal->ncolors, 256));
            ++n;
        }
    }
}


// Save the given image to a file.  Returns SDL_FALSE on failure.
DECLSPEC SDL_bool SDLCALL GIF_SaveImage(GIF_Image* image, const char* filename)
{
    if(image == NULL || image->num_frames == 0)
        return SDL_FALSE;

    int error;

    // Create the output object
    GifFileType* gif = EGifOpenFileName(filename, SDL_FALSE, &error);
    if(!gif)
        return SDL_FALSE;


    // Set up screen description
    ColorMapObject* palette = gif_copy_palette(gif, image->global_palette);

    if(image->global_palette != NULL && image->use_background_color)
        image->background_index = gif_find_matching_index(image->global_palette, image->background_color);

    if(EGifPutScreenDesc(gif, image->width, image->height, 8, image->background_index, palette) == GIF_ERROR)
        return SDL_FALSE;


    if(!gif_add_loop_extension(gif, image->num_loops))
        return SDL_FALSE;

    int i;
    for(i = 0; i < image->num_frames; ++i)
    {
        GIF_Frame* frame = image->frames[i];

        if(image->num_frames > 1 || frame->use_transparent_color || frame->use_transparent_index)
        {
            if(frame->use_transparent_color)
                frame->transparent_index = gif_find_matching_index((frame->local_palette == NULL? image->global_palette : frame->local_palette), frame->transparent_color);

            Uint8 block[4] = {0, (frame->delay/10)%256, (frame->delay/10)/256, frame->transparent_index};

            if(frame->use_transparent_color || frame->use_transparent_index)
                block[0] |= 0x01;
            if(!frame->overlay_previous)
                block[0] |= 0x08;

            if(EGifPutExtension(gif, GRAPHICS_EXT_FUNC_CODE, 4, block) == GIF_ERROR)
                return SDL_FALSE;
        }


        if(frame->surface != NULL)
        {
            frame->width = frame->surface->w;
            frame->height = frame->surface->h;
        }
        ColorMapObject* palette = gif_copy_palette(gif, frame->local_palette);
        if(EGifPutImageDesc(gif, frame->left_offset, frame->top_offset, frame->width, frame->height, SDL_FALSE, palette) == GIF_ERROR)
        {
            //printf("EGifPutImageDesc error: %d\n", gif->Error);
            return SDL_FALSE;
        }

        if(frame->indices == NULL || frame->surface != NULL)
        {
            SDL_free(frame->indices);
            frame->indices = (Uint8*)SDL_malloc(sizeof(Uint8)*(frame->width*frame->height));

            // Copy surface data if it's there
            gif_copy_surface_to_indices(frame, image->global_palette);
        }

        // Write lines of pixel indices
        int y;
        int j = 0;
        for(y = 0; y < frame->height; ++y)
        {
            if(EGifPutLine(gif, &(frame->indices[j]), frame->width) == GIF_ERROR)
                return SDL_FALSE;
            j += frame->width;
        }
    }

    if(EGifCloseFile(gif, &error) == GIF_ERROR)
        return SDL_FALSE;

    return SDL_TRUE;
}

// Save the given image to an arbitrary destination.  Returns SDL_FALSE on failure.
/*DECLSPEC SDL_bool SDLCALL GIF_SaveImage_RW(GIF_Image* image, SDL_RWops* rwops, int freerwops)
{
    return SDL_FALSE;
}*/

// Free the memory allocated for the given image.
DECLSPEC void SDLCALL GIF_FreeImage(GIF_Image* image)
{
    if(image == NULL)
        return;

    SDL_FreePalette(image->global_palette);
    image->global_palette = NULL;


    Uint16 i;
    for(i = 0; i < image->num_frames; ++i)
        GIF_FreeFrame(image->frames[i]);

    SDL_free(image->frames);
    image->frames = NULL;


    for(i = 0; i < image->num_comments; ++i)
        SDL_free(image->comments[i]);

    SDL_free(image->comments);
    image->comments = NULL;


    SDL_free(image);
}


// Sets the canvas size.
DECLSPEC void SDLCALL GIF_SetCanvasSize(GIF_Image* image, Uint16 width, Uint16 height)
{
    if(image == NULL)
        return;

    image->width = width;
    image->height = height;
}

// Sets the background color.
DECLSPEC void SDLCALL GIF_SetBackgroundColor(GIF_Image* image, SDL_Color background_color)
{
    if(image == NULL)
        return;

    image->use_background_color = SDL_TRUE;
    image->background_color = background_color;
}

// Sets the background color index.
DECLSPEC void SDLCALL GIF_SetBackgroundIndex(GIF_Image* image, Uint8 background_color_index)
{
    if(image == NULL)
        return;

    image->use_background_color = SDL_FALSE;
    image->background_index = background_color_index;
}

// Sets the number of animation loops.  0 loops means to loop forever.
DECLSPEC void SDLCALL GIF_SetLooping(GIF_Image* image, Uint16 num_loops)
{
    if(image == NULL)
        return;

    image->num_loops = num_loops;
}




// Sets the global palette of the given image.  The given palette's ownership is transferred to the GIF image, so it will be freed by GIF_Free().
DECLSPEC void SDLCALL GIF_SetGlobalPalette(GIF_Image* image, SDL_Palette* palette)
{
    if(image == NULL)
        return;

    SDL_FreePalette(image->global_palette);
    image->global_palette = palette;
}



// Returns a new palette with a range of colors.
DECLSPEC SDL_Palette* SDLCALL GIF_CreateBasicPalette(void)
{
    SDL_Palette* result = SDL_AllocPalette(256);
    int i = 0;
    for(i = 0; i < 256; ++i)
    {
        SDL_Color c = {i, i, i, 255};
        result->colors[i] = c;
    }

    return result;
}

// Creates a new palette from the given colors.  GIF can only use the first 256 colors.  The colors are copied into a new palette object.
DECLSPEC SDL_Palette* SDLCALL GIF_CreatePalette(int ncolors, SDL_Color* colors)
{
    SDL_Palette* result = SDL_AllocPalette(ncolors);

    int i = 0;
    for(i = 0; i < ncolors; ++i)
    {
        result->colors[i] = colors[i];
    }

    return result;
}


// Copies a palette.
DECLSPEC SDL_Palette* SDLCALL GIF_CopyPalette(SDL_Palette* palette)
{
    SDL_Palette* result;
    if(palette == NULL)
        return NULL;

    result = SDL_AllocPalette(palette->ncolors);
    SDL_SetPaletteColors(result, palette->colors, 0, palette->ncolors);

    return result;
}

static void gif_grow_image_storage(GIF_Image* image)
{
    GIF_Frame** old_frames;
    if(image == NULL)
        return;

    old_frames = image->frames;
    image->frame_storage_size += 10;
    image->frames = (GIF_Frame**)SDL_malloc(sizeof(GIF_Frame*)*image->frame_storage_size);
    if(old_frames == NULL)
        memset(image->frames, 0, sizeof(GIF_Frame*)*image->frame_storage_size);
    else
    {
        memcpy(image->frames, old_frames, sizeof(GIF_Frame*)*image->num_frames);
        SDL_free(old_frames);
    }
}

// Adds the given frame to the end of the given image.  The given frame's ownership is transferred to the image, so it will be freed by GIF_FreeImage().
DECLSPEC void SDLCALL GIF_AddFrame(GIF_Image* image, GIF_Frame* frame)
{
    if(image == NULL || frame == NULL)
        return;

    // Do we need to grow the storage?
    if(image->frame_storage_size < image->num_frames + 1)
        gif_grow_image_storage(image);

    image->frames[image->num_frames] = frame;
    image->num_frames += 1;
}

// Inserts the given frame before the frame at a specified index of the given image.  The given frame's ownership is transferred to the image, so it will be freed by GIF_FreeImage().
DECLSPEC void SDLCALL GIF_InsertFrame(GIF_Image* image, GIF_Frame* frame, Uint16 index)
{
    if(image == NULL || frame == NULL || index >= image->num_frames)
        return;

    // Do we need to grow the storage?
    if(image->frame_storage_size < image->num_frames + 1)
        gif_grow_image_storage(image);

    // Move the rest of the frames up
    memmove(&(image->frames[index+1]), &(image->frames[index]), (image->num_frames - index)*sizeof(GIF_Frame*));
    image->frames[image->num_frames] = frame;
    image->num_frames += 1;
}

// Removes a frame from the given image and returns the frame.
DECLSPEC GIF_Frame* SDLCALL GIF_RemoveFrame(GIF_Image* image, Uint16 index)
{
    GIF_Frame* result = NULL;
    if(image == NULL || image->num_frames <= index)
        return result;

    result = image->frames[index];

    // Shift down the array
    image->num_frames--;
    memmove(&(image->frames[index]), &(image->frames[index+1]), (image->num_frames - index)*sizeof(GIF_Frame*));

    return result;
}

// Removes a frame from the given image and frees the frame.
DECLSPEC void SDLCALL GIF_DeleteFrame(GIF_Image* image, Uint16 index)
{
    if(image == NULL || image->num_frames <= index)
        return;

    GIF_FreeFrame(image->frames[index]);

    // Shift down the array
    image->num_frames--;
    memmove(&(image->frames[index]), &(image->frames[index+1]), (image->num_frames - index)*sizeof(GIF_Frame*));
}


// Creates a new frame and returns it for further modification.  If give_ownership is true, the given surface's ownership is transferred to the GIF frame, so it will be freed by GIF_FreeFrame() or GIF_FreeImage().
DECLSPEC GIF_Frame* SDLCALL GIF_CreateFrame(SDL_Surface* surface, SDL_bool give_ownership)
{
    GIF_Frame* result = (GIF_Frame*)SDL_malloc(sizeof(GIF_Frame));
    if(result == NULL)
        return NULL;

    memset(result, 0, sizeof(GIF_Frame));

    result->delay = 100;

    result->surface = surface;
    result->owns_surface = give_ownership;
    if(surface != NULL)
    {
        result->width = surface->w;
        result->height = surface->h;
    }

    return result;
}

// Creates a new frame and returns it for further modification.  The given indices are copied.
DECLSPEC GIF_Frame* SDLCALL GIF_CreateFrameIndexed(Uint16 width, Uint16 height, Uint8* indices)
{
    GIF_Frame* result = (GIF_Frame*)SDL_malloc(sizeof(GIF_Frame));
    if(result == NULL)
        return NULL;

    memset(result, 0, sizeof(GIF_Frame));

    result->delay = 100;

    int num_indices = width*height;

    if(num_indices > 0 && indices != NULL)
    {
        result->indices = (Uint8*)SDL_malloc(sizeof(Uint8)*num_indices);

        memcpy(result->indices, indices, num_indices);
    }

    result->width = width;
    result->height = height;

    return result;
}

// Free the memory allocated for the given frame.
DECLSPEC void SDLCALL GIF_FreeFrame(GIF_Frame* frame)
{
    if(frame == NULL)
        return;

    SDL_FreePalette(frame->local_palette);
    frame->local_palette = NULL;

    if(frame->owns_surface)
        SDL_FreeSurface(frame->surface);
    frame->surface = NULL;

    SDL_free(frame->indices);
    frame->indices = NULL;

    SDL_free(frame);
}


// Sets the global palette of the given frame.  This will override the global palette for this frame.  The given palette's ownership is transferred to the GIF image, so it will be freed by GIF_Free().
DECLSPEC void SDLCALL GIF_SetLocalPalette(GIF_Frame* frame, SDL_Palette* palette)
{
    if(frame == NULL)
        return;

    SDL_FreePalette(frame->local_palette);
    frame->local_palette = palette;
}

// Sets the given frame's delay time to the given delay, in milliseconds.  GIF only stores delays in hundredths of a second, so the saved image will truncate the given delay.
DECLSPEC void SDLCALL GIF_SetDelay(GIF_Frame* frame, Uint32 delay)
{
    if(frame == NULL)
        return;

    frame->delay = delay;
}

// Enables transparency for the given frame and sets the transparent color.
DECLSPEC void SDLCALL GIF_SetTransparentColor(GIF_Frame* frame, SDL_Color transparent_color)
{
    if(frame == NULL)
        return;

    frame->use_transparent_color = SDL_TRUE;
    frame->use_transparent_index = SDL_FALSE;
    frame->transparent_color = transparent_color;
}

// Enables transparency for the given frame and sets the transparent color index.
DECLSPEC void SDLCALL GIF_SetTransparentIndex(GIF_Frame* frame, Uint8 color_index)
{
    if(frame == NULL)
        return;

    frame->use_transparent_color = SDL_FALSE;
    frame->use_transparent_index = SDL_TRUE;
    frame->transparent_index = color_index;
}

// Disables transparency for the given frame.
DECLSPEC void SDLCALL GIF_UnsetTransparency(GIF_Frame* frame)
{
    if(frame == NULL)
        return;

    frame->use_transparent_color = SDL_FALSE;
    frame->use_transparent_index = SDL_FALSE;
}

// Sets the disposal method for the colors of the previous frame.  SDL_FALSE discards the previous frame colors.
DECLSPEC void SDLCALL GIF_SetDisposal(GIF_Frame* frame, SDL_bool overlay_previous)
{
    if(frame == NULL)
        return;

    frame->overlay_previous = overlay_previous;
}
