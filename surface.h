#ifndef SURFACE_H
#define SURFACE_H

#include <stdint.h>
#include "allmodex.h"

typedef union {
	struct {
		// Opaque is a=255. Not pre-multiplied. However, only the rasterizer
		// and BMP import uses the alpha channel, all other code ignores it.
		unsigned char b, g, r, a;
	};
	uint32_t col;
} RGBcolor;

enum SurfaceFormat
{
	SF_8bit = 0,
	SF_32bit = 1,
};

enum SurfaceUsage
{
	SU_Source = 0,       // Surfaces that can be drawn to render targets
	SU_RenderTarget = 1,
	SU_Staging = 2,      // Surfaces that don't get sent to GPU
};

typedef struct Surface
{
	void* handle;
	int refcount;
	uint32_t width;
	uint32_t height;
	enum SurfaceFormat format;
	enum SurfaceUsage usage;
	Frame *frame;       // If not NULL, is a view onto a Frame which owns the data
	union
	{
		void* pRawData;
		uint32_t* pColorData;
		uint8_t* pPaletteData;
	};

#ifdef __cplusplus
	uint8_t& pixel8(int x, int y) { return pPaletteData[width * y + x]; }
	RGBcolor& pixel32(int x, int y) { return ((RGBcolor*)pColorData)[width * y + x]; }
#endif
} Surface;

typedef struct
{
	// right and bottom are INCLUSIVE
	int32_t left, top, right, bottom;
} SurfaceRect;

typedef struct
{
	void* handle;
	RGBcolor col[256];
} RGBPalette;

//interfaces
#ifdef __cplusplus
extern "C"
{
#endif

	// Software implementation
	int gfx_surfaceCreate_SW( uint32_t width, uint32_t height, enum SurfaceFormat format, enum SurfaceUsage usage, Surface** ppSurfaceOut );
	int gfx_surfaceWithFrame_SW( Frame* pFrameIn, Surface** ppSurfaceOut );
	int gfx_surfaceDestroy_SW( Surface** ppSurfaceIn );
	Surface *gfx_surfaceReference_SW( Surface* pSurfaceIn );
	int gfx_surfaceUpdate_SW( Surface* pSurfaceIn );
	int gfx_surfaceGetData_SW( Surface* pSurfaceIn );
	int gfx_surfaceFill_SW( uint32_t fillColor, SurfaceRect* pRect, Surface* pSurfaceIn );
	int gfx_surfaceStretch_SW( SurfaceRect* pRectSrc, Surface* pSurfaceSrc, RGBPalette* pPalette, int bUseColorKey0, SurfaceRect* pRectDest, Surface* pSurfaceDest );
	Surface* gfx_surfaceShrink_SW( Surface *surf, int destWidth, int destHeight );
	int gfx_surfaceCopy_SW( SurfaceRect* pRectSrc, Surface* pSurfaceSrc, RGBPalette* pPalette, Palette16* pPal8, int bUseColorKey0, SurfaceRect* pRectDest, Surface* pSurfaceDest );

	int gfx_paletteCreate_SW( RGBPalette** ppPaletteOut );
	int gfx_paletteFromRGB_SW( RGBcolor* pColorsIn, RGBPalette** ppPaletteOut );
	int gfx_paletteDestroy_SW( RGBPalette** ppPaletteIn );
	int gfx_paletteUpdate_SW( RGBPalette* pPaletteIn );

	// Function pointers to the selected implementation
	extern int (*gfx_surfaceCreate)( uint32_t width, uint32_t height, enum SurfaceFormat format, enum SurfaceUsage usage, Surface** ppSurfaceOut );
	extern int (*gfx_surfaceWithFrame)( Frame* pFrameIn, Surface** ppSurfaceOut );
	extern int (*gfx_surfaceDestroy)( Surface** ppSurfaceIn );
	extern Surface* (*gfx_surfaceReference)( Surface* pSurfaceIn );
	extern int (*gfx_surfaceUpdate)( Surface* pSurfaceIn );
	extern int (*gfx_surfaceGetData)( Surface* pSurfaceIn );
	extern int (*gfx_surfaceFill)( uint32_t fillColor, SurfaceRect* pRect, Surface* pSurfaceIn );
	extern int (*gfx_surfaceStretch)( SurfaceRect* pRectSrc, Surface* pSurfaceSrc, RGBPalette* pPalette, int bUseColorKey0, SurfaceRect* pRectDest, Surface* pSurfaceDest );
	extern Surface* (*gfx_surfaceShrink)( Surface *surf, int destWidth, int destHeight );
	extern int (*gfx_surfaceCopy)( SurfaceRect* pRectSrc, Surface* pSurfaceSrc, RGBPalette* pPalette, Palette16* pPal8, int bUseColorKey0, SurfaceRect* pRectDest, Surface* pSurfaceDest );

	extern int (*gfx_paletteCreate)( RGBPalette** ppPaletteOut );
	extern int (*gfx_paletteFromRGB)( RGBcolor* pColorsIn, RGBPalette** ppPaletteOut );
	extern int (*gfx_paletteDestroy)( RGBPalette** ppPaletteIn );
	extern int (*gfx_paletteUpdate)( RGBPalette* pPaletteIn );

	// Convenience wrappers not specific to any Surface implementation
	void surface_assign( Surface** ptr_to_replace, Surface* new_value );
	Surface* surface_scale( Surface *surf, int destWidth, int destHeight );
	Surface* surface_duplicate( Surface* surf );

#ifdef __cplusplus
};
#endif

#endif