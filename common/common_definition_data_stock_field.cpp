#include "common_definition_base.h"
#include "common_definition_data_stock_field.h"


const int COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_INT, // 持股1-999人數
	FinanceField_LONG, // 持股1-999股數
	FinanceField_FLOAT, // 持股1-999佔集保庫存數比例
	FinanceField_INT, // 持股1,000-5,000人數
	FinanceField_LONG, // 持股1,000-5,000股數
	FinanceField_FLOAT, // 持股1,000-5,000佔集保庫存數比例
	FinanceField_INT, // 持股5,001-10,000人數
	FinanceField_LONG, // 持股5,001-10,000股數
	FinanceField_FLOAT, // 持股5,001-10,000佔集保庫存數比例
	FinanceField_INT, // 持股10,001-15,000人數
	FinanceField_LONG, // 持股10,001-15,000股數
	FinanceField_FLOAT, // 持股10,001-15,000佔集保庫存數比例
	FinanceField_INT, // 持股15,001-20,000人數
	FinanceField_LONG, // 持股15,001-20,000股數
	FinanceField_FLOAT, // 持股15,001-20,000佔集保庫存數比例
	FinanceField_INT, // 持股20,001-30,000人數
	FinanceField_LONG, // 持股20,001-30,000股數
	FinanceField_FLOAT, // 持股20,001-30,000佔集保庫存數比例
	FinanceField_INT, // 持股30,001-40,000人數
	FinanceField_LONG, // 持股30,001-40,000股數
	FinanceField_FLOAT, // 持股30,001-40,000佔集保庫存數比例
	FinanceField_INT, // 持股40,001-50,000人數
	FinanceField_LONG, // 持股40,001-50,000股數
	FinanceField_FLOAT, // 持股40,001-50,000佔集保庫存數比例
	FinanceField_INT, // 持股50,001-100,000人數
	FinanceField_LONG, // 持股50,001-100,000股數
	FinanceField_FLOAT, // 持股150,001-100,000佔集保庫存數比例
	FinanceField_INT, // 持股100,001-200,000人數
	FinanceField_LONG, // 持股100,001-200,000股數
	FinanceField_FLOAT, // 持股100,001-200,000佔集保庫存數比例
	FinanceField_INT, // 持股200,001-400,000人數
	FinanceField_LONG, // 持股200,001-400,000股數
	FinanceField_FLOAT, // 持股200,001-400,000佔集保庫存數比例
	FinanceField_INT, // 持股400,001-600,000人數
	FinanceField_LONG, // 持股400,001-600,000股數
	FinanceField_FLOAT, // 持股400,001-600,000佔集保庫存數比例
	FinanceField_INT, // 持股600,001-800,000人數
	FinanceField_LONG, // 持股600,001-800,000股數
	FinanceField_FLOAT, // 持股600,001-800,000佔集保庫存數比例
	FinanceField_INT, // 持股800,001-1,000,000人數
	FinanceField_LONG, // 持股800,001-1,000,000股數
	FinanceField_FLOAT, // 持股800,001-1,000,000佔集保庫存數比例
	FinanceField_INT, // 持股1,000,001以上人數
	FinanceField_LONG, // 持股1,000,001以上股數
	FinanceField_FLOAT, // 持股1,000,001以上佔集保庫存數比例
	FinanceField_INT, // 合計人數
	FinanceField_LONG, // 合計股數
	FinanceField_FLOAT, // 合計佔集保庫存數比例
};
const int BALANCE_SHEET_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_LONG, // 現金及約當現金
	FinanceField_LONG, // 透過損益按公允價值衡量之金融資產－流動
	FinanceField_LONG, // 備供出售金融資產－流動淨額
	FinanceField_LONG, // 無活絡市場之債務工具投資－流動淨額
	FinanceField_LONG, // 應收票據淨額
	FinanceField_LONG, // 應收帳款淨額
	FinanceField_LONG, // 應收帳款－關係人淨額
	FinanceField_LONG, // 其他應收款淨額
	FinanceField_LONG, // 其他應收款－關係人淨額
	FinanceField_LONG, // 存貨
	FinanceField_LONG, // 預付款項
	FinanceField_LONG, // 待出售非流動資產（淨額）
	FinanceField_LONG, // 其他流動資產
	FinanceField_LONG, // 流動資產合計
	FinanceField_LONG, // 備供出售金融資產－非流動淨額
	FinanceField_LONG, // 以成本衡量之金融資產－非流動淨額
	FinanceField_LONG, // 無活絡市場之債務工具投資－非流動淨額
	FinanceField_LONG, // 採用權益法之投資淨額
	FinanceField_LONG, // 不動產、廠房及設備
	FinanceField_LONG, // 無形資產
	FinanceField_LONG, // 遞延所得稅資產
	FinanceField_LONG, // 其他非流動資產
	FinanceField_LONG, // 非流動資產合計
	FinanceField_LONG, // 資產總計
	FinanceField_LONG, // 短期借款
	FinanceField_LONG, // 應付短期票券
	FinanceField_LONG, // 透過損益按公允價值衡量之金融負債－流動
	FinanceField_LONG, // 應付票據
	FinanceField_LONG, // 應付帳款
	FinanceField_LONG, // 應付帳款－關係人
	FinanceField_LONG, // 其他應付款
	FinanceField_LONG, // 本期所得稅負債
	FinanceField_LONG, // 負債準備－流動
	FinanceField_LONG, // 其他流動負債
	FinanceField_LONG, // 流動負債合計
	FinanceField_LONG, // 應付公司債
	FinanceField_LONG, // 長期借款
	FinanceField_LONG, // 遞延所得稅負債
	FinanceField_LONG, // 其他非流動負債
	FinanceField_LONG, // 非流動負債合計
	FinanceField_LONG, // 負債總計
	FinanceField_LONG, // 普通股股本
	FinanceField_LONG, // 預收股本
	FinanceField_LONG, // 待分配股票股利
	FinanceField_LONG, // 股本合計
	FinanceField_LONG, // 資本公積－發行溢價
	FinanceField_LONG, // 資本公積－庫藏股票交易
	FinanceField_LONG, // 資本公積－實際取得或處分子公司股權價格與帳面價值差額
	FinanceField_LONG, // 資本公積－採用權益法認列關聯企業及合資股權淨值之變動數
	FinanceField_LONG, // 資本公積－合併溢額
	FinanceField_LONG, // 資本公積－員工認股權
	FinanceField_LONG, // 資本公積－限制員工權利股票
	FinanceField_LONG, // 資本公積－其他
	FinanceField_LONG, // 資本公積－認股權
	FinanceField_LONG, // 資本公積合計
	FinanceField_LONG, // 法定盈餘公積
	FinanceField_LONG, // 特別盈餘公積
	FinanceField_LONG, // 未分配盈餘（或待彌補虧損）
	FinanceField_LONG, // 保留盈餘合計
	FinanceField_LONG, // 國外營運機構財務報表換算之兌換差額
	FinanceField_LONG, // 備供出售金融資產未實現損益
	FinanceField_LONG, // 其他權益－其他
	FinanceField_LONG, // 其他權益合計
	FinanceField_LONG, // 庫藏股票
	FinanceField_LONG, // 歸屬於母公司業主之權益合計
	FinanceField_LONG, // 非控制權益
	FinanceField_LONG, // 權益總計
	FinanceField_LONG, // 負債及權益總計
	FinanceField_LONG, // 母公司暨子公司所持有之母公司庫藏股股數（單位：股）
};
const int INCOME_STATEMENT_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_LONG, // 銷貨收入
	FinanceField_LONG, // 銷貨收入淨額
	FinanceField_LONG, // 營業收入合計
	FinanceField_LONG, // 銷貨成本
	FinanceField_LONG, // 營業成本合計
	FinanceField_LONG, // 營業毛利（毛損）
	FinanceField_LONG, // 未實現銷貨（損）益
	FinanceField_LONG, // 已實現銷貨（損）益
	FinanceField_LONG, // 營業毛利（毛損）淨額
	FinanceField_LONG, // 推銷費用
	FinanceField_LONG, // 管理費用
	FinanceField_LONG, // 研究發展費用
	FinanceField_LONG, // 營業費用合計
	FinanceField_LONG, // 其他收益及費損淨額
	FinanceField_LONG, // 營業利益（損失）
	FinanceField_LONG, // 其他收入
	FinanceField_LONG, // 其他利益及損失淨額
	FinanceField_LONG, // 財務成本淨額
	FinanceField_LONG, // 採用權益法認列之關聯企業及合資損益之份額淨額
	FinanceField_LONG, // 營業外收入及支出合計
	FinanceField_LONG, // 稅前淨利（淨損）
	FinanceField_LONG, // 所得稅費用（利益）合計
	FinanceField_LONG, // 繼續營業單位本期淨利（淨損）
	FinanceField_LONG, // 本期淨利（淨損）
	FinanceField_LONG, // 不重分類至損益之其他項目
	FinanceField_LONG, // 確定福利計畫之再衡量數
	FinanceField_LONG, // 採用權益法認列之關聯企業及合資之其他綜合損益之份額-不重分類至損益之項目
	FinanceField_LONG, // 與不重分類之項目相關之所得稅
	FinanceField_LONG, // 國外營運機構財務報表換算之兌換差額
	FinanceField_LONG, // 採用權益法認列關聯企業及合資之其他綜合損益之份額-可能重分類至損益之項目
	FinanceField_LONG, // 備供出售金融資產未實現評價損益
	FinanceField_LONG, // 採用權益法認列關聯企業及合資之其他綜合損益之份額-可能重分類至損益之項目
	FinanceField_LONG, // 與可能重分類之項目相關之所得稅
	FinanceField_LONG, // 其他綜合損益（淨額）
	FinanceField_LONG, // 本期綜合損益總額
	FinanceField_LONG, // 母公司業主（淨利／損）
	FinanceField_LONG, // 非控制權益（淨利／損）
	FinanceField_LONG, // 母公司業主（綜合損益）
	FinanceField_LONG, // 非控制權益（綜合損益）
	FinanceField_LONG, // 繼續營業單位淨利（淨損）
	FinanceField_LONG, // 基本每股盈餘
	FinanceField_LONG, // 繼續營業單位淨利（淨損）
	FinanceField_LONG, // 停業單位淨利（淨損）
	FinanceField_LONG, // 稀釋每股盈餘
};
const int CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION[] =
{
	FinanceField_DATE, // 日期
	FinanceField_LONG, // 繼續營業單位稅前淨利（淨損）
	FinanceField_LONG, // 本期稅前淨利（淨損）
	FinanceField_LONG, // 折舊費用
	FinanceField_LONG, // 攤銷費用
	FinanceField_LONG, // 呆帳費用提列（轉列收入）數
	FinanceField_LONG, // 透過損益按公允價值衡量金融資產及負債之淨損失（利益）
	FinanceField_LONG, // 利息費用
	FinanceField_LONG, // 利息收入
	FinanceField_LONG, // 股利收入
	FinanceField_LONG, // 採用權益法認列之關聯企業及合資損失（利益）之份額
	FinanceField_LONG, // 股份基礎給付酬勞成本
	FinanceField_LONG, // 應付公司債匯率影響數
	FinanceField_LONG, // 採用權益法認列之關聯企業及合資損失（利益）之份額
	FinanceField_LONG, // 處分及報廢不動產、廠房及設備損失（利益）
	FinanceField_LONG, // 不動產、廠房及設備轉列費用數
	FinanceField_LONG, // 已實現銷貨損失（利益）
	FinanceField_LONG, // 處分投資損失（利益）
	FinanceField_LONG, // 金融資產減損損失
	FinanceField_LONG, // 非金融資產減損損失
	FinanceField_LONG, // 非金融資產減損迴轉利益
	FinanceField_LONG, // 未實現銷貨利益（損失）
	FinanceField_LONG, // 已實現銷貨損失（利益）
	FinanceField_LONG, // 買回應付公司債損失（利益）
	FinanceField_LONG, // 未實現外幣兌換損失（利益）
	FinanceField_LONG, // 其他項目
	FinanceField_LONG, // 收益費損項目合計
	FinanceField_LONG, // 持有供交易之金融資產（增加）減少
	FinanceField_LONG, // 應收票據（增加）減少
	FinanceField_LONG, // 應收帳款（增加）減少
	FinanceField_LONG, // 應收帳款－關係人（增加）減少
	FinanceField_LONG, // 其他應收款（增加）減少
	FinanceField_LONG, // 其他應收款－關係人（增加）減少
	FinanceField_LONG, // 存貨（增加）減少
	FinanceField_LONG, // 預付款項（增加）減少
	FinanceField_LONG, // 其他流動資產（增加）減少
	FinanceField_LONG, // 其他營業資產（增加）減少
	FinanceField_LONG, // 其他金融資產（增加）減少
	FinanceField_LONG, // 與營業活動相關之資產之淨變動合計
	FinanceField_LONG, // 應付票據－關係人增加（減少）
	FinanceField_LONG, // 持有供交易之金融負債增加（減少）
	FinanceField_LONG, // 應付票據增加（減少）
	FinanceField_LONG, // 應付帳款增加（減少）
	FinanceField_LONG, // 應付帳款－關係人增加（減少）
	FinanceField_LONG, // 其他應付款增加（減少）
	FinanceField_LONG, // 負債準備增加（減少）
	FinanceField_LONG, // 預收款項增加（減少）
	FinanceField_LONG, // 其他流動負債增加（減少）
	FinanceField_LONG, // 淨確定福利負債增加(減少)
	FinanceField_LONG, // 其他營業負債增加（減少）
	FinanceField_LONG, // 與營業活動相關之負債之淨變動合計
	FinanceField_LONG, // 與營業活動相關之資產及負債之淨變動合計
	FinanceField_LONG, // 調整項目合計
	FinanceField_LONG, // 營運產生之現金流入（流出）
	FinanceField_LONG, // 收取之利息
	FinanceField_LONG, // 收取之股利
	FinanceField_LONG, // 支付之利息
	FinanceField_LONG, // 退還（支付）之所得稅
	FinanceField_LONG, // 營業活動之淨現金流入（流出）
	FinanceField_LONG, // 取得無活絡市場之債務工具投資
	FinanceField_LONG, // 採用權益法之被投資公司減資退回股款
	FinanceField_LONG, // 處分備供出售金融資產
	FinanceField_LONG, // 處分無活絡市場之債務工具投資
	FinanceField_LONG, // 取得採用權益法之投資
	FinanceField_LONG, // 取得原始認列時指定為透過損益按公允價值衡量之金融資產
	FinanceField_LONG, // 處分原始認列時指定為透過損益按公允價值衡量之金融資產
	FinanceField_LONG, // 取得備供出售金融資產
	FinanceField_LONG, // 處分備供出售金融資產
	FinanceField_LONG, // 取得採用權益法之投資
	FinanceField_LONG, // 對子公司之收購（扣除所取得之現金）
	FinanceField_LONG, // 處分子公司
	FinanceField_LONG, // 處分以成本衡量之金融資產
	FinanceField_LONG, // 取得不動產、廠房及設備
	FinanceField_LONG, // 處分不動產、廠房及設備
	FinanceField_LONG, // 存出保證金增加
	FinanceField_LONG, // 存出保證金減少
	FinanceField_LONG, // 取得無形資產
	FinanceField_LONG, // 其他金融資產減少
	FinanceField_LONG, // 其他投資活動
	FinanceField_LONG, // 其他金融資產增加
	FinanceField_LONG, // 預付設備款增加
	FinanceField_LONG, // 預付設備款減少
	FinanceField_LONG, // 收取之利息
	FinanceField_LONG, // 投資活動之淨現金流入（流出）
	FinanceField_LONG, // 短期借款增加
	FinanceField_LONG, // 應付短期票券增加
	FinanceField_LONG, // 短期借款減少
	FinanceField_LONG, // 應付短期票券減少
	FinanceField_LONG, // 舉借長期借款
	FinanceField_LONG, // 償還長期借款
	FinanceField_LONG, // 存入保證金增加
	FinanceField_LONG, // 存入保證金減少
	FinanceField_LONG, // 發行公司債
	FinanceField_LONG, // 償還公司債
	FinanceField_LONG, // 舉借長期借款
	FinanceField_LONG, // 償還長期借款
	FinanceField_LONG, // 存入保證金增加
	FinanceField_LONG, // 庫藏股票處分
	FinanceField_LONG, // 取得子公司股權
	FinanceField_LONG, // 支付之利息
	FinanceField_LONG, // 非控制權益變動
	FinanceField_LONG, // 其他非流動負債減少
	FinanceField_LONG, // 發放現金股利
	FinanceField_LONG, // 員工執行認股權
	FinanceField_LONG, // 庫藏股票買回成本
	FinanceField_LONG, // 員工購買庫藏股
	FinanceField_LONG, // 支付之利息
	FinanceField_LONG, // 籌資活動之淨現金流入（流出）
	FinanceField_LONG, // 匯率變動對現金及約當現金之影響
	FinanceField_LONG, // 本期現金及約當現金增加（減少）數
	FinanceField_LONG, // 期初現金及約當現金餘額
	FinanceField_LONG, // 期末現金及約當現金餘額
	FinanceField_LONG, // 資產負債表帳列之現金及約當現金
	FinanceField_LONG, // 其他符合國際會計準則第七號現金及約當現金定義之項目
};
const int STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION[] =
{

};
const int DAILY_STOCK_PRICE_AND_VOLUME_FIELD_TYPE_DEFINITION[] =
{

};
const int TOP3_LEGAL_PERSONS_STOCK_NET_BUY_AND_SELL_SUMMARY_FIELD_TYPE_DEFINITION[] =
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"持股1-999人數", // FinanceField_INT
	"持股1-999佔集保庫存數比例", // FinanceField_FLOAT
	"持股1,000-5,000人數", // FinanceField_INT
	"持股1,000-5,000股數", // FinanceField_LONG
	"持股1,000-5,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股5,001-10,000人數", // FinanceField_INT
	"持股5,001-10,000股數", // FinanceField_LONG
	"持股5,001-10,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股10,001-15,000人數", // FinanceField_INT
	"持股10,001-15,000股數", // FinanceField_LONG
	"持股10,001-15,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股15,001-20,000人數", // FinanceField_INT
	"持股15,001-20,000股數", // FinanceField_LONG
	"持股15,001-20,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股20,001-30,000人數", // FinanceField_INT
	"持股20,001-30,000股數, // FinanceField_LONG"
	"持股20,001-30,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股30,001-40,000人數", // FinanceField_INT
	"持股30,001-40,000股數", // FinanceField_LONG
	"持股30,001-40,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股40,001-50,000人數", // FinanceField_INT
	"持股40,001-50,000股數", // FinanceField_LONG
	"持股40,001-50,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股50,001-100,000人數", // FinanceField_INT
	"持股50,001-100,000股數", // FinanceField_LONG
	"持股150,001-100,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股100,001-200,000人數", // FinanceField_INT
	"持股100,001-200,000股數", // FinanceField_LONG
	"持股100,001-200,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股200,001-400,000人數", // FinanceField_INT
	"持股200,001-400,000股數", // FinanceField_LONG
	"持股200,001-400,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股400,001-600,000人數", // FinanceField_INT
	"持股400,001-600,000股數", // FinanceField_LONG
	"持股400,001-600,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股600,001-800,000人數", // FinanceField_INT
	"持股600,001-800,000股數", // FinanceField_LONG
	"持股600,001-800,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股800,001-1,000,000人數", // FinanceField_INT
	"持股800,001-1,000,000股數", // FinanceField_LONG
	"持股800,001-1,000,000佔集保庫存數比例", // FinanceField_FLOAT
	"持股1,000,001以上人數", // FinanceField_INT
	"持股1,000,001以上股數", // FinanceField_LONG
	"持股1,000,001以上佔集保庫存數比例", // FinanceField_FLOAT
	"合計人數", // FinanceField_INT
	"合計股數", // FinanceField_LONG
	"合計佔集保庫存數比例", // FinanceField_FLOAT
};
const char* BALANCE_SHEET_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"現金及約當現金", // FinanceField_LONG
	"透過損益按公允價值衡量之金融資產－流動", // FinanceField_LONG
	"備供出售金融資產－流動淨額", // FinanceField_LONG
	"無活絡市場之債務工具投資－流動淨額", // FinanceField_LONG
	"應收票據淨額", // FinanceField_LONG
	"應收帳款淨額", // FinanceField_LONG
	"應收帳款－關係人淨額", // FinanceField_LONG
	"其他應收款淨額", // FinanceField_LONG
	"其他應收款－關係人淨額", // FinanceField_LONG
	"存貨", // FinanceField_LONG
	"預付款項", // FinanceField_LONG
	"待出售非流動資產（淨額）", // FinanceField_LONG
	"其他流動資產", // FinanceField_LONG
	"流動資產合計", // FinanceField_LONG
	"備供出售金融資產－非流動淨額", // FinanceField_LONG
	"以成本衡量之金融資產－非流動淨額", // FinanceField_LONG
	"無活絡市場之債務工具投資－非流動淨額", // FinanceField_LONG
	"採用權益法之投資淨額", // FinanceField_LONG
	"不動產、廠房及設備", // FinanceField_LONG
	"無形資產", // FinanceField_LONG
	"遞延所得稅資產", // FinanceField_LONG
	"其他非流動資產", // FinanceField_LONG
	"非流動資產合計", // FinanceField_LONG
	"資產總計", // FinanceField_LONG
	"短期借款", // FinanceField_LONG
	"應付短期票券", // FinanceField_LONG
	"透過損益按公允價值衡量之金融負債－流動", // FinanceField_LONG
	"應付票據", // FinanceField_LONG
	"應付帳款", // FinanceField_LONG
	"應付帳款－關係人", // FinanceField_LONG
	"其他應付款", // FinanceField_LONG
	"本期所得稅負債", // FinanceField_LONG
	"負債準備－流動", // FinanceField_LONG
	"其他流動負債", // FinanceField_LONG
	"流動負債合計", // FinanceField_LONG
	"應付公司債", // FinanceField_LONG
	"長期借款", // FinanceField_LONG
	"遞延所得稅負債", // FinanceField_LONG
	"其他非流動負債", // FinanceField_LONG
	"非流動負債合計", // FinanceField_LONG
	"負債總計", // FinanceField_LONG
	"普通股股本", // FinanceField_LONG
	"預收股本", // FinanceField_LONG
	"待分配股票股利", // FinanceField_LONG
	"股本合計", // FinanceField_LONG
	"資本公積－發行溢價", // FinanceField_LONG
	"資本公積－庫藏股票交易", // FinanceField_LONG
	"資本公積－實際取得或處分子公司股權價格與帳面價值差額", // FinanceField_LONG
	"資本公積－採用權益法認列關聯企業及合資股權淨值之變動數", // FinanceField_LONG
	"資本公積－合併溢額", // FinanceField_LONG
	"資本公積－員工認股權", // FinanceField_LONG
	"資本公積－限制員工權利股票", // FinanceField_LONG
	"資本公積－其他", // FinanceField_LONG
	"資本公積－認股權", // FinanceField_LONG
	"資本公積合計", // FinanceField_LONG
	"法定盈餘公積", // FinanceField_LONG
	"特別盈餘公積", // FinanceField_LONG
	"未分配盈餘（或待彌補虧損）", // FinanceField_LONG
	"保留盈餘合計", // FinanceField_LONG
	"國外營運機構財務報表換算之兌換差額", // FinanceField_LONG
	"備供出售金融資產未實現損益", // FinanceField_LONG
	"其他權益－其他", // FinanceField_LONG
	"其他權益合計", // FinanceField_LONG
	"庫藏股票", // FinanceField_LONG
	"歸屬於母公司業主之權益合計", // FinanceField_LONG
	"非控制權益", // FinanceField_LONG
	"權益總計", // FinanceField_LONG
	"負債及權益總計", // FinanceField_LONG
	"母公司暨子公司所持有之母公司庫藏股股數（單位：股）", // FinanceField_LONG
};
const char* INCOME_STATEMENT_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"銷貨收入", // FinanceField_LONG
	"銷貨收入淨額", // FinanceField_LONG
	"營業收入合計", // FinanceField_LONG
	"銷貨成本", // FinanceField_LONG
	"營業成本合計", // FinanceField_LONG
	"營業毛利（毛損）", // FinanceField_LONG
	"未實現銷貨（損）益", // FinanceField_LONG
	"已實現銷貨（損）益", // FinanceField_LONG
	"營業毛利（毛損）淨額", // FinanceField_LONG
	"推銷費用", // FinanceField_LONG
	"管理費用", // FinanceField_LONG
	"研究發展費用", // FinanceField_LONG
	"營業費用合計", // FinanceField_LONG
	"其他收益及費損淨額", // FinanceField_LONG
	"營業利益（損失）", // FinanceField_LONG
	"其他收入", // FinanceField_LONG
	"其他利益及損失淨額", // FinanceField_LONG
	"財務成本淨額", // FinanceField_LONG
	"採用權益法認列之關聯企業及合資損益之份額淨額", // FinanceField_LONG
	"營業外收入及支出合計", // FinanceField_LONG
	"稅前淨利（淨損）", // FinanceField_LONG
	"所得稅費用（利益）合計", // FinanceField_LONG
	"繼續營業單位本期淨利（淨損）", // FinanceField_LONG
	"本期淨利（淨損）", // FinanceField_LONG
	"不重分類至損益之其他項目", // FinanceField_LONG
	"確定福利計畫之再衡量數", // FinanceField_LONG
	"採用權益法認列之關聯企業及合資之其他綜合損益之份額-不重分類至損益之項目", // FinanceField_LONG
	"與不重分類之項目相關之所得稅", // FinanceField_LONG
	"國外營運機構財務報表換算之兌換差額", // FinanceField_LONG
	"採用權益法認列關聯企業及合資之其他綜合損益之份額-可能重分類至損益之項目", // FinanceField_LONG
	"備供出售金融資產未實現評價損益", // FinanceField_LONG
	"採用權益法認列關聯企業及合資之其他綜合損益之份額-可能重分類至損益之項目", // FinanceField_LONG
	"與可能重分類之項目相關之所得稅", // FinanceField_LONG
	"其他綜合損益（淨額）", // FinanceField_LONG
	"本期綜合損益總額", // FinanceField_LONG
	"母公司業主（淨利／損）", // FinanceField_LONG
	"非控制權益（淨利／損）", // FinanceField_LONG
	"母公司業主（綜合損益）", // FinanceField_LONG
	"非控制權益（綜合損益）", // FinanceField_LONG
	"繼續營業單位淨利（淨損）", // FinanceField_LONG
	"基本每股盈餘", // FinanceField_LONG
	"繼續營業單位淨利（淨損）", // FinanceField_LONG
	"停業單位淨利（淨損）", // FinanceField_LONG
	"稀釋每股盈餘", // FinanceField_LONG
};
const char* CASH_FLOW_STATEMENT_FIELD_DESCRIPTION[] =
{
	"日期", // FinanceField_DATE
	"繼續營業單位稅前淨利（淨損）", // FinanceField_LONG
	"本期稅前淨利（淨損）", // FinanceField_LONG
	"折舊費用", // FinanceField_LONG
	"攤銷費用", // FinanceField_LONG
	"呆帳費用提列（轉列收入）數", // FinanceField_LONG
	"透過損益按公允價值衡量金融資產及負債之淨損失（利益）", // FinanceField_LONG
	"利息費用", // FinanceField_LONG
	"利息收入", // FinanceField_LONG
	"股利收入", // FinanceField_LONG
	"採用權益法認列之關聯企業及合資損失（利益）之份額", // FinanceField_LONG
	"股份基礎給付酬勞成本", // FinanceField_LONG
	"應付公司債匯率影響數", // FinanceField_LONG
	"採用權益法認列之關聯企業及合資損失（利益）之份額", // FinanceField_LONG
	"處分及報廢不動產、廠房及設備損失（利益）", // FinanceField_LONG
	"不動產、廠房及設備轉列費用數", // FinanceField_LONG
	"已實現銷貨損失（利益）", // FinanceField_LONG
	"處分投資損失（利益）", // FinanceField_LONG
	"金融資產減損損失", // FinanceField_LONG
	"非金融資產減損損失", // FinanceField_LONG
	"非金融資產減損迴轉利益", // FinanceField_LONG
	"未實現銷貨利益（損失）", // FinanceField_LONG
	"已實現銷貨損失（利益）", // FinanceField_LONG
	"買回應付公司債損失（利益）", // FinanceField_LONG
	"未實現外幣兌換損失（利益）", // FinanceField_LONG
	"其他項目", // FinanceField_LONG
	"收益費損項目合計", // FinanceField_LONG
	"持有供交易之金融資產（增加）減少", // FinanceField_LONG
	"應收票據（增加）減少", // FinanceField_LONG
	"應收帳款（增加）減少", // FinanceField_LONG
	"應收帳款－關係人（增加）減少", // FinanceField_LONG
	"其他應收款（增加）減少", // FinanceField_LONG
	"其他應收款－關係人（增加）減少", // FinanceField_LONG
	"存貨（增加）減少", // FinanceField_LONG
	"預付款項（增加）減少", // FinanceField_LONG
	"其他流動資產（增加）減少", // FinanceField_LONG
	"其他營業資產（增加）減少", // FinanceField_LONG
	"其他金融資產（增加）減少", // FinanceField_LONG
	"與營業活動相關之資產之淨變動合計", // FinanceField_LONG
	"應付票據－關係人增加（減少）", // FinanceField_LONG
	"持有供交易之金融負債增加（減少）", // FinanceField_LONG
	"應付票據增加（減少）", // FinanceField_LONG
	"應付帳款增加（減少）", // FinanceField_LONG
	"應付帳款－關係人增加（減少）", // FinanceField_LONG
	"其他應付款增加（減少）", // FinanceField_LONG
	"負債準備增加（減少）", // FinanceField_LONG
	"預收款項增加（減少）", // FinanceField_LONG
	"其他流動負債增加（減少）", // FinanceField_LONG
	"淨確定福利負債增加(減少)", // FinanceField_LONG
	"其他營業負債增加（減少）", // FinanceField_LONG
	"與營業活動相關之負債之淨變動合計", // FinanceField_LONG
	"與營業活動相關之資產及負債之淨變動合計", // FinanceField_LONG
	"調整項目合計", // FinanceField_LONG
	"營運產生之現金流入（流出）", // FinanceField_LONG
	"收取之利息", // FinanceField_LONG
	"收取之股利", // FinanceField_LONG
	"支付之利息", // FinanceField_LONG
	"退還（支付）之所得稅", // FinanceField_LONG
	"營業活動之淨現金流入（流出）", // FinanceField_LONG
	"取得無活絡市場之債務工具投資", // FinanceField_LONG
	"採用權益法之被投資公司減資退回股款", // FinanceField_LONG
	"處分備供出售金融資產", // FinanceField_LONG
	"處分無活絡市場之債務工具投資", // FinanceField_LONG
	"取得採用權益法之投資", // FinanceField_LONG
	"取得原始認列時指定為透過損益按公允價值衡量之金融資產", // FinanceField_LONG
	"處分原始認列時指定為透過損益按公允價值衡量之金融資產", // FinanceField_LONG
	"取得備供出售金融資產", // FinanceField_LONG
	"處分備供出售金融資產", // FinanceField_LONG
	"取得採用權益法之投資", // FinanceField_LONG
	"對子公司之收購（扣除所取得之現金）", // FinanceField_LONG
	"處分子公司", // FinanceField_LONG
	"處分以成本衡量之金融資產", // FinanceField_LONG
	"取得不動產、廠房及設備", // FinanceField_LONG
	"處分不動產、廠房及設備", // FinanceField_LONG
	"存出保證金增加", // FinanceField_LONG
	"存出保證金減少", // FinanceField_LONG
	"取得無形資產", // FinanceField_LONG
	"其他金融資產減少", // FinanceField_LONG
	"其他投資活動", // FinanceField_LONG
	"其他金融資產增加", // FinanceField_LONG
	"預付設備款增加", // FinanceField_LONG
	"預付設備款減少", // FinanceField_LONG
	"收取之利息", // FinanceField_LONG
	"投資活動之淨現金流入（流出）", // FinanceField_LONG
	"短期借款增加", // FinanceField_LONG
	"應付短期票券增加", // FinanceField_LONG
	"短期借款減少", // FinanceField_LONG
	"應付短期票券減少", // FinanceField_LONG
	"舉借長期借款", // FinanceField_LONG
	"償還長期借款", // FinanceField_LONG
	"存入保證金增加", // FinanceField_LONG
	"存入保證金減少", // FinanceField_LONG
	"發行公司債", // FinanceField_LONG
	"償還公司債", // FinanceField_LONG
	"舉借長期借款", // FinanceField_LONG
	"償還長期借款", // FinanceField_LONG
	"存入保證金增加", // FinanceField_LONG
	"庫藏股票處分", // FinanceField_LONG
	"取得子公司股權", // FinanceField_LONG
	"支付之利息", // FinanceField_LONG
	"非控制權益變動", // FinanceField_LONG
	"其他非流動負債減少", // FinanceField_LONG
	"發放現金股利", // FinanceField_LONG
	"員工執行認股權", // FinanceField_LONG
	"庫藏股票買回成本", // FinanceField_LONG
	"員工購買庫藏股", // FinanceField_LONG
	"支付之利息", // FinanceField_LONG
	"籌資活動之淨現金流入（流出）", // FinanceField_LONG
	"匯率變動對現金及約當現金之影響", // FinanceField_LONG
	"本期現金及約當現金增加（減少）數", // FinanceField_LONG
	"期初現金及約當現金餘額", // FinanceField_LONG
	"期末現金及約當現金餘額", // FinanceField_LONG
	"資產負債表帳列之現金及約當現金", // FinanceField_LONG
	"其他符合國際會計準則第七號現金及約當現金定義之項目", // FinanceField_LONG
};
const char* STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_DESCRIPTION[] =
{

};
const char* DAILY_STOCK_PRICE_AND_VOLUME_FIELD_DESCRIPTION[] =
{

};
const char* TOP3_LEGAL_PERSONS_STOCK_NET_BUY_AND_SELL_SUMMARY_FIELD_DESCRIPTION[] =
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION_SIZE = sizeof(COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION) / sizeof(COMPANY_DEPOSITORY_SHAREHOLDER_DISTRIBUTION_FIELD_TYPE_DEFINITION[0]);
const int BALANCE_SHEET_FIELD_TYPE_DEFINITION_SIZE = sizeof(BALANCE_SHEET_FIELD_TYPE_DEFINITION) / sizeof(BALANCE_SHEET_FIELD_TYPE_DEFINITION[0]);
const int INCOME_STATEMENT_FIELD_TYPE_DEFINITION_SIZE = sizeof(INCOME_STATEMENT_FIELD_TYPE_DEFINITION) / sizeof(INCOME_STATEMENT_FIELD_TYPE_DEFINITION[0]);
const int CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION_SIZE = sizeof(CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION) / sizeof(CASH_FLOW_STATEMENT_FIELD_TYPE_DEFINITION[0]);
const int STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION_SIZE = sizeof(STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION) / sizeof(STATEMENT_OF_CHANGES_IN_EQUITY_FIELD_TYPE_DEFINITION[0]);
const int DAILY_STOCK_PRICE_AND_VOLUME_FIELD_TYPE_DEFINITION_SIZE = sizeof(DAILY_STOCK_PRICE_AND_VOLUME_FIELD_TYPE_DEFINITION) / sizeof(DAILY_STOCK_PRICE_AND_VOLUME_FIELD_TYPE_DEFINITION[0]);
const int TOP3_LEGAL_PERSONS_STOCK_NET_BUY_AND_SELL_SUMMARY_FIELD_TYPE_DEFINITION_SIZE = sizeof(TOP3_LEGAL_PERSONS_STOCK_NET_BUY_AND_SELL_SUMMARY_FIELD_TYPE_DEFINITION) / sizeof(TOP3_LEGAL_PERSONS_STOCK_NET_BUY_AND_SELL_SUMMARY_FIELD_TYPE_DEFINITION[0]);
