#ifndef OPTIONS_PROFIT_LOSS_DEF_H
#define OPTIONS_PROFIT_LOSS_DEF_H

#include "common.h"


enum OptionsType{Options_LongCall, Options_ShortCall, Options_LongPut, Options_ShortPut, Options_BullCallSpread, Options_BullPutSpread, Options_BearCallSpread, Options_BearPutSpread, Options_LongStraddle, Options_ShortStraddle, Options_LongStrangle, Options_ShortStrangle, Options_LongButterfly, Options_ShortButterfly, Options_LongIronCondor, Options_ShortIronCondor, OptionsSize, Options_None};

///////////////////////////////////////////////////////////////////////////////////

struct OneStrikePriceCfg
{
	float strike_price;
	float premium;
};

typedef OneStrikePriceCfg LongCallStrikePriceCfg;
typedef OneStrikePriceCfg ShortCallStrikePriceCfg;
typedef OneStrikePriceCfg LongPutStrikePriceCfg;
typedef OneStrikePriceCfg ShortPutStrikePriceCfg;

struct TwoStrikePriceCfg
{
	float strike_price1;
	float premium1;
	float strike_price2;
	float premium2;
};

typedef TwoStrikePriceCfg BullCallSpreadStrikePriceCfg;
typedef TwoStrikePriceCfg BullPutSpreadStrikePriceCfg;
typedef TwoStrikePriceCfg BearCallSpreadStrikePriceCfg;
typedef TwoStrikePriceCfg BearPutSpreadStrikePriceCfg;
typedef TwoStrikePriceCfg LongStraddleStrikePriceCfg;
typedef TwoStrikePriceCfg ShortStraddleStrikePriceCfg;
typedef TwoStrikePriceCfg LongStrangleStrikePriceCfg;
typedef TwoStrikePriceCfg ShortStrangleStrikePriceCfg;

struct FourStrikePriceCfg
{
	float strike_price1;
	float premium1;
	float strike_price2;
	float premium2;
	float strike_price3;
	float premium3;
	float strike_price4;
	float premium4;
};

typedef FourStrikePriceCfg LongButterflyStrikePriceCfg;
typedef FourStrikePriceCfg ShortButterflyStrikePriceCfg;
typedef FourStrikePriceCfg LongIronCondorStrikePriceCfg;
typedef FourStrikePriceCfg ShortIronCondorStrikePriceCfg;

struct StikePriceParam
{
	OptionsType options_type;
	union
	{
		float strike_price[8];
		LongCallStrikePriceCfg long_call;
		ShortCallStrikePriceCfg short_call;
		LongPutStrikePriceCfg long_put;
		ShortPutStrikePriceCfg short_put;
		BullCallSpreadStrikePriceCfg bull_call_spread;
		BullPutSpreadStrikePriceCfg bull_put_spread;
		BearCallSpreadStrikePriceCfg bear_call_spread;
		BearPutSpreadStrikePriceCfg bear_put_spread;
		LongStraddleStrikePriceCfg long_straddle;
		ShortStraddleStrikePriceCfg short_straddle;
		LongStrangleStrikePriceCfg long_strangle;
		ShortStrangleStrikePriceCfg short_strangle;
		LongButterflyStrikePriceCfg long_butterfly;
		ShortButterflyStrikePriceCfg short_butterfly;
		LongIronCondorStrikePriceCfg long_iron_condor;
		ShortIronCondorStrikePriceCfg short_iron_condor;
	}param;
};
typedef StikePriceParam* PSTRIKE_PRICE_PARAM;

////////////////////////////////////////////////////////////////////////////////////

struct OneStrikePriceResCfg
{
	float net_premium;
	float break_event_point;
};

#endif

