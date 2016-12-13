#include <fstream>	//std::ifstream
#include "FL/fl_ask.H"	// fl_alert(-)
#include "ptbl_funct.h"
#include "ptbl_returncode.h"
#include "pri.h"
#include "gts_master.h"
#include "gts_gui.h"

int gts_master::read_and_save_crnt_( void )
{
	int crnt_file_num = this->cl_file_number_list.get_crnt_file_num();
	int crnt_list_num = this->cl_file_number_list.get_crnt_list_num();

	/*------------------------------------------------*/

	/* リストのセンタースクロール */
	cl_gts_gui.selbro_fnum_list->middleline(crnt_list_num);

	/*------------------------------------------------*/

	/* 読み込み(番号に対する)ファイルパスを得る */

	/* open dir , head ,extによるパス */

	std::string fpath_open(
		this->cl_level.get_openfilepath(crnt_file_num)
	);
	if (fpath_open.empty()) {
		pri_funct_err_bttvr(
	"Error : this->cl_level.get_openfilepath(%d) returns nullptr",
		crnt_file_num
		);
		return NG;
	}

	/* 画像ファイルがないなら読み込みはしないでその番号キャンセル */
	if (!ptbl_dir_or_file_is_exist(
		const_cast<char *>(fpath_open.c_str())
	)) {
		pri_funct_msg_ttvr( "Not exist \"%s\"", fpath_open.c_str());
		return OK;
	}

	/* 読み込み元ファイルパス設定 */
	if (OK != this->cl_iip_read.cl_name.set_name(fpath_open.c_str())) {
		pri_funct_err_bttvr(
	 "Error : this->cl_iip_read.cl_name.set_name(%s) returns NG",
			fpath_open.c_str());
		return NG;
	}

	/* 画像読み込み */
	if (OK != this->cl_iip_read.file()) {
		pri_funct_err_bttvr(
	 "Error : this->cl_iip_read.file() returns NG" );
		return NG;
	}

	/* 読み込んだ画像がフルカラーでないとエラー */
	if (this->cl_iip_read.get_l_channels() < 3) {
		pri_funct_err_bttvr(
   "Error : this->cl_iip_read.get_l_channels() is less than 3" );
		return NG;
	}

	/*------------------------------------------------*/
	// Rot90 and Effects(color Trace and Erase color dot noise)

	if (this->rot_and_trace_and_enoise_(
		&(this->cl_iip_read)
		,0	/* 画像コンバート処理のみで、回転はしない */
	) != OK) {
		return NG;
	}

	/*------------------------------------------------*/

	/* Trace保存する(番号に対する)ファイルパスを得る */

	std::string fpath_save(
		this->cl_level.get_savefilepath(crnt_file_num)
	);
	if (fpath_save.empty()) {
		pri_funct_err_bttvr(
	"Error : this->cl_level.get_savefilepath(%d) returns empty"
			, crnt_file_num
		);
		return NG;
	}

	/* 保存する */
	if (OK != this->_iipg_save(
		&(cl_gts_master.cl_iip_trac)
		, const_cast<char *>(fpath_save.c_str())
// cl_gts_gui.valinp_area_reso->value()
		, this->cl_iip_read.get_d_tif_dpi_x()
		/* rot90実行後なので(デフォルト)0度とする */
		/* (デフォルト)なしとする、
			&(this->cl_iip_read)は参照しない */
	)) {
		pri_funct_err_bttvr(
	 "Error : this->_iipg_save(-) returns NG" );
		return NG;
	}

	/*------------------------------------------------*/

	/* リストにマーク付いていなければ付ける */
	this->cl_file_number_list.add_S( crnt_list_num );

	/* リストの選択解除 */
	this->cl_file_number_list.unselect(crnt_list_num);

	/*------------------------------------------------*/

	/* 表示 */
	if (this->redraw_image_(
		&(this->cl_iip_read)
		, false /* crop sw */
		, false /* force view scanimage sw */
	)) {
		return NG;
	}

	/* 保存するタイプで画像を表示する */
	if ( cl_gts_gui.chkbtn_filter_rgb_color_trace_sw->value() ) {
		/* TracenImage画像のみ表示 */
		cl_gts_master.cb_change_wview_sub();

		/* 画像表示状態をメニューに設定 */
		cl_gts_gui.menite_wview_sub->setonly();
	}
	else {
		/* ScanImage(メイン)画像のみ表示 */
		cl_gts_master.cb_change_wview_main();

		/* 画像表示状態をメニューに設定 */
		cl_gts_gui.menite_wview_main->setonly();
	}

	return OK;
}

/*--------------------------------------------------------*/

void gts_master::cb_read_and_save_start( void )
{
	/* 先頭を得る - End設定で選択したフレーム番号をたどっていく */
	this->cl_file_number_list.counter_start(
		cl_gts_master.cl_file_number_list.get_end_type_value()
	);

	/* 開くファイルのLevel名がない */
	{
	std::string name(cl_gts_gui.strinp_level_open_file_head->value());
	if ( name.empty() ) {
		fl_alert("Need Open Level Name!");
		return;
	}
	}

	/* 保存ファイルのLevel名がない */
	{
	std::string name(cl_gts_gui.strinp_level_save_file_head->value());
	if ( name.empty() ) {
		fl_alert("Need Save Level Name!");
		return;
	}
	}

	/* 最初に番号が選択がない */
	if (this->cl_file_number_list.get_crnt_file_num() < 1) {
		fl_alert("Select number!");
		return;
	}

	/* 実行確認 */
	const int crnt_file_num =
		this->cl_file_number_list.get_crnt_file_num();
	const bool tsw =
		cl_gts_gui.chkbtn_filter_rgb_color_trace_sw->value() != 0;
	const bool esw =
		cl_gts_gui.chkbtn_filter_rgb_erase_dot_noise_sw->value()!=0;
	if (fl_ask(
		"%s%s\n%s\n-->\n%s\n..."
		,tsw ?"Trace" :"Not trace"
		,esw ?" and Erase Dot Noise" :""
		,this->cl_level.get_openfilepath(crnt_file_num).c_str()
		,this->cl_level.get_savefilepath(crnt_file_num).c_str()
	) != 1) {
		return; // Cancel
	}

	while (1 <= this->cl_file_number_list.get_crnt_list_num()) {
		/* カレントの読み込みと処理と保存をして */
		if (OK != this->read_and_save_crnt_()) {
			pri_funct_err_bttvr(
		 "Error : this->read_and_save_crnt_() returns NG" );
			return;
		}
		/* 次を得る - End設定で選択したフレーム番号をたどっていく */
		this->cl_file_number_list.counter_next(
		 cl_gts_master.cl_file_number_list.get_end_type_value()
		);

		Fl::check();
		int ekey = Fl::event_key();

		/* FL_Escapeと'q'と't'は効かない */
		//if (FL_Escape == ekey) {
		//if ('q' == ekey) {
		//if ('t' == ekey) { /* Tで開始し、tで終る */

		if ('e' == ekey) {
			break;
		}
	}
}
