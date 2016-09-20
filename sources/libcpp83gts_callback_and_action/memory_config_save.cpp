#include <FL/Fl.H>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ptbl_returncode.h"
#include "ptbl_funct.h"
#include "pri.h"
#include "memory_config.h"
#include "gts_gui.h"
#include "gts_master.h"

int memory_config::_save_comment_by_fp( FILE *fp )
{
	int i_ret;
	time_t tt;
	char *cp;

	tt = time(NULL);
	cp = asctime(localtime(&tt));

	/* cpは26文字とヌル文字の入った文字列へのポインター
		Example : "Fri Sep 13 00:00:00 1986\n\0"
		cpの示す文字列はstatic領域なのでfree()してはいけません
	*/

	i_ret = fprintf(fp, "# date and time : %s", cp );
	if (i_ret < 0) { return NG; }

	/*------------------------------------------------*/

	cp = ptbl_get_cp_username();
	if (NULL == cp) { cp = "unknown"; }
	i_ret = fprintf(fp, "# current user  : %s\n#\n", cp );
	if (i_ret < 0) { return NG; }

	return OK;
}

int memory_config::_save_config_by_fp( FILE *fp )
{
	int i_ret;

	/* config loadとsave saとsaveは同じdirを示す */
	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_config_dir_,
		cl_gts_gui.filinp_config_load_dir->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_config_load_file_,
		cl_gts_gui.strinp_config_load_file->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_config_save_as_file_,
		cl_gts_gui.strinp_config_save_as_file->value() );
	if (i_ret < 0) { return NG; }

	return OK;
}

int memory_config::_save_level_by_fp( FILE *fp )
{
	int i_ret;

	/* 01. Dirパス */
	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_level_dir_,
		cl_gts_gui.filinp_level_dir->value() );
	if (i_ret < 0) { return NG; }

	/* 02 file/levelのlist表示切替(Renumberボタンの表示OFF/ONも) */
	i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_level_list_form_
		, cl_gts_gui.choice_level_list_form->text()
	);
	if (i_ret < 0) { return NG; }

	/* 03 Level名 */
	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_level_file_,
		cl_gts_gui.strinp_level_file->value() );
	if (i_ret < 0) { return NG; }

	/* 04 Level開始番号 */
	i_ret = fprintf(fp, "%-24s %.16g\n"
			, this->str_level_num_start_
		, cl_gts_gui.valinp_level_start->value() );
	if (i_ret < 0) { return NG; }

	/* 05 Level終端番号 */
	i_ret = fprintf(fp, "%-24s %.16g\n"
			, this->str_level_num_end_
		, cl_gts_gui.valinp_level_end->value() );
	if (i_ret < 0) { return NG; }

	/* 06 Level終端タイプ */
	i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_level_num_continue_type_
		, cl_gts_gui.choice_level_continue_type->text()
	);
	if (i_ret < 0) { return NG; }

	/* 07 Level Endless時増減方向 */
	i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_level_num_endless_direction_
		, cl_gts_gui.choice_level_endless_direction->text()
	);
	if (i_ret < 0) { return NG; }

	/* 08 保存(開くときも)画像ファイル書式 */
	i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_level_image_file_format_
		, cl_gts_gui.choice_level_image_file_format->text()
	);
	if (i_ret < 0) { return NG; }

	/* 09 等倍画像表示スイッチ */
	/* 10 画像全体表示スイッチ */
	
	/* 11 _full dirパス */
	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_level_rgb_scan_dir_,
		cl_gts_gui.filinp_level_rgb_scan_dir->value() );
	if (i_ret < 0) { return NG; }

	/* 12 RGBスキャン時、トレスを実行し保存するスイッチ */
	i_ret = fprintf(fp, "%-24s %s\n",
			this->str_level_rgb_trace_save_sw_,
		cl_gts_gui.chkbtn_level_rgb_trace_save_sw->value()?
			this->str_on_:this->str_off_ );
	if (i_ret < 0) { return NG; }

	/* 13 RGBスキャン画像保存sw */
	i_ret = fprintf(fp, "%-24s %s\n",
			this->str_level_rgb_full_save_sw_,
		cl_gts_gui.chkbtn_level_rgb_full_save_sw->value()?
			this->str_on_:this->str_off_ );
	if (i_ret < 0) { return NG; }

	/* 14 RGBスキャン画像ファイル名に_full付けるsw */
	i_ret = fprintf(fp, "%-24s %s\n",
			this->str_level_rgb_with_full_sw_,
		cl_gts_gui.chkbtn_level_rgb_with_full_sw->value()?
			this->str_on_:this->str_off_ );
	if (i_ret < 0) { return NG; }

	/* 15 dirパス/_full dirパスのブラウジング切替えスイッチ */
	/* 16 _full等倍画像表示スイッチ */

	return OK;
}

int memory_config::_save_area_by_fp( FILE *fp )
{
	int i_ret;

	i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_area_select_
		, cl_gts_gui.choice_area_selecter->text()
	);
	if (i_ret < 0) { return NG; }

		i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_area_x_pos_,
			cl_gts_gui.valinp_area_x_pos->value() );
		if (i_ret < 0) { return NG; }

		i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_area_y_pos_,
			cl_gts_gui.valinp_area_y_pos->value() );
		if (i_ret < 0) { return NG; }

		i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_area_aspect_ratio_select_
		, cl_gts_gui.choice_area_aspect_ratio_selecter->text()
		);
		if (i_ret < 0) { return NG; }

		i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_area_x_size_,
			cl_gts_gui.valinp_area_x_size->value() );
		if (i_ret < 0) { return NG; }

		i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_area_y_size_,
			cl_gts_gui.valinp_area_y_size->value() );
		if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_area_x_pixel_,
		cl_gts_gui.valinp_area_x_pixel->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_area_y_pixel_,
		cl_gts_gui.valinp_area_y_pixel->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_area_resolution_dpi_,
		cl_gts_gui.valinp_area_reso->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_rotate_per_90_
		, cl_gts_gui.choice_rot90->text()
	);
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_scanner_type_,
		cl_gts_gui.txtout_scanner_type->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_scanner_x_max_,
		cl_gts_gui.valout_scanner_width_max->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_scanner_y_max_,
		cl_gts_gui.valout_scanner_height_max->value() );
	if (i_ret < 0) { return NG; }

	return OK;
}

int memory_config::_save_pixel_type_by_fp( FILE *fp )
{
	int i_ret;

	i_ret = fprintf(fp, "%-24s \"%s\"\n"
			, this->str_pixel_type_
		, cl_gts_gui.choice_pixel_type->text()
	);
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_bw_threshold_,
		cl_gts_gui.valinp_bw_threshold->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_grayscale_brightness_,
		cl_gts_gui.valinp_grays_brightness->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_grayscale_contrast_,
		cl_gts_gui.valinp_grays_contrast->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_grayscale_gamma_,
		cl_gts_gui.valinp_grays_gamma->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_rgb_brightness_,
		cl_gts_gui.valinp_rgb_brightness->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_rgb_contrast_,
		cl_gts_gui.valinp_rgb_contrast->value() );
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %.16g\n",
			this->str_rgb_gamma_,
		cl_gts_gui.valinp_rgb_gamma->value() );
	if (i_ret < 0) { return NG; }

	return OK;
}

int memory_config::_save_trace_by_fp( FILE *fp )
{
	int i_ret;

	/*- color trace window other ----*/

	i_ret = fprintf(fp, "%-24s %s\n",
			this->str_color_trace_erase_1dot_,
			cl_gts_gui.chkbtn_color_trace_erase_1dot->value()?
			this->str_on_:this->str_off_);
	if (i_ret < 0) { return NG; }

	i_ret = fprintf(fp, "%-24s %s\n",
			this->str_color_trace_real_time_,
			cl_gts_gui.menite_color_trace_real_time->value()?
			this->str_on_:this->str_off_);
	if (i_ret < 0) { return NG; }

	return OK;
}

int memory_config::_save_fnum_by_fp( FILE *fp )
{
	int i_ret;
	int ii;

	/* リストを全てサーチ */
	for (ii = 1; ii <= cl_gts_gui.selbro_fnum_list->size(); ++ii) {
		i_ret = fprintf(fp, "%s %s",
			this->str_file_number_frame_,
			cl_gts_gui.selbro_fnum_list->text(ii) );
		if (i_ret < 0) { return NG; }

		if ( cl_gts_gui.selbro_fnum_list->selected(ii) ) {
			i_ret = fprintf(fp, " %s\n",
				this->str_file_number_selected_ );
		} else {
			i_ret = fprintf(fp, "\n");
		}
		if (i_ret < 0) { return NG; }
	}

	return OK;
}

int memory_config::_save_trace_batch_by_fp( FILE *fp )
{
	int i_ret;
	int ii;

	i_ret = fprintf(fp, "%-24s \"%s\"\n",
			this->str_trace_batch_dir_,
		cl_gts_gui.filinp_trace_batch_dir->value() );
	if (i_ret < 0) { return NG; }

	/* リストを全てサーチ */
	for (ii = 1; ii <= cl_gts_gui.selbro_trace_batch_run_list->size();
	++ii) {
		i_ret = fprintf(fp, "%s \"%s\"\n",
			this->str_trace_batch_list_,
			cl_gts_gui.selbro_trace_batch_run_list->text(ii) );
		if (i_ret < 0) { return NG; }
	}

	return OK;
}

int memory_config::save( const char *cp_file_path )
{
	FILE *fp;
	char current_path[PTBL_PATH_MAX];

	/* ファイル名の一時記憶 */
	strncpy( current_path,cp_file_path,PTBL_PATH_MAX );
	current_path[PTBL_PATH_MAX-1] = '\0';

	/* ファイル開く */
	fp = fopen( current_path, "w" );
	if (NULL == fp) {
		pri_funct_err_bttvr(
		"fopen(%s,w) returns NULL.",current_path);
		return NG;
	} 
	/* ファイルに設定保存 */
	if (OK != this->_save_comment_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_comment_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_config_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_config_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_level_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_level_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_area_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_area_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_pixel_type_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_pixel_type_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_trace_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_trace_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_trace_src_hsv_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_trace_src_hsv_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_fnum_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_fnum_by_fp(-) returns NG.");
		return NG;
	}
	if (OK != this->_save_trace_batch_by_fp( fp )) {
		pri_funct_err_bttvr(
	"memory_config::_save_trace_batch_by_fp(-) returns NG.");
		return NG;
	}
	/* ファイル閉じる */
	if (fclose(fp)) {
		pri_funct_err_bttvr("fclose(-) returns not zero.");
		return NG;
	}

	/* 正常に設定したなら、ファイル名の記憶 */
	this->memory_of_path = current_path;

	/* ファイル名を表示する */
	cl_gts_master._print_window_headline();

	return OK;
}
