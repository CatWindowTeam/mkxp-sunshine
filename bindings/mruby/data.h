#pragma once
#include <mruby.h>
enum CommonSymbol{
	CSfont = 0,
	CSviewport,
	CSbitmap,
	CScolor,
	CSout_color,
	CStone,
	CSrect,
	CSsrc_rect,
	CStilemap,
	CStileset,
	CSautotiles,
	CSmap_data,
	CSflash_data,
	CSpriorities,
	CSwindowskin,
	CScontents,
	CScursor_rect,
	CSpath,
	CSarray,
	CSdefault_color,
	CSdefault_out_color,
	CSchildren,
	CS_mkxp_dispose_alias,
	CommonSymbolsMax
};

struct MRBBindingData {
  struct RClass* shudownExc;
  mrb_sym symbols[CommonSymbolsMax];
};
