#pragma once

#define		kMaxPaletteEntries		238

#include "CPlotResource.h"
// Palette names
enum {
	kPaletteGreyscale = IDR_CT_BWLINEAR,
		kPaletteRainbow = IDR_CT_RAINBOW,
		kPaletteRedTemperature = IDR_CT_REDTEMPERATURE,
		kPalette16Level = IDR_CT_16LEVEL,
		kPaletteBeach = IDR_CT_BEACH,
		kPaletteBluePastelRed = IDR_CT_BLUEPASTELRED,
		kPaletteBlueRed = IDR_CT_BLUERED,
		kPaletteBlueWaves = IDR_CT_BLUEWAVES,
		kPaletteEOSA = IDR_CT_EOSA,
		kPaletteEOSB = IDR_CT_EOSB,
		kPaletteGreenPink = IDR_CT_GREENPINK,
		kPaletteGreenRedBlueWhite = IDR_CT_GREENREDBLUEWHITE,
		kPaletteHardCandy = IDR_CT_HARDCANDY,
		kPaletteHaze = IDR_CT_HAZE,
		kPaletteHSV1 = IDR_CT_HSV1,
		kPaletteHSV2 = IDR_CT_HSV2,
		kPaletteMacStyle = IDR_CT_MACSTYLE,
		kPaletteNature = IDR_CT_NATURE,
		kPaletteOcean = IDR_CT_OCEAN,
		kPalettePastels = IDR_CT_PASTELS,
		kPalettePeppermint = IDR_CT_PEPPERMINT,
		kPalettePlasma = IDR_CT_PLASMA,
		kPalettePrism = IDR_CT_PRISM,
		kPalettePurpleRedStripes = IDR_CT_PURPLEREDSTRIPES,
		kPaletteRainbow18 = IDR_CT_RAINBOW18,
		kPaletteRainbowBlack = IDR_CT_RAINBOWBLACK,
		kPaletteRainbowWhite = IDR_CT_RAINBOWWHITE,
		kPaletteRedPurple = IDR_CT_REDPURPLE,
		kPaletteSTDGammaII = IDR_CT_STDGAMMAII,
		kPaletteSteps = IDR_CT_STEPS,
		kPaletteSternSpecial = IDR_CT_STERNSPECIAL,
		kPaletteVolcano = IDR_CT_VOLCANO,
		kPaletteWaves = IDR_CT_WAVES
};

class CImagePalette : public CObject
{
public:
	DECLARE_SERIAL( CImagePalette )
	
	CImagePalette();
	~CImagePalette();
	virtual void	Serialize(CArchive& ar);

	void	SetPalette( int whichPalette );

	int		GetCurPaletteNumber( void ) { return curPaletteNum; }
	BITMAPINFO *GetBitmapInfo( void ) { return m_pBitmapInfo; }
	CPalette	*GetPalette( void ) { return &palette; }
	int		GetNumEntries( void );

	// We'll keep our variables private for now
private:
	CPalette		palette;
	int				curPaletteNum;
	BITMAPINFO			*m_pBitmapInfo;

	BOOL	LoadPalette( int which );
	BOOL	LoadPalette( LPTSTR string );
	void	SetupBitmapInfo( PALETTEENTRY *entries, int numEntries );
};