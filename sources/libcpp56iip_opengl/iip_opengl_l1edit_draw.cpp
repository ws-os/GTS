#include <math.h>
#include <iostream>
#include "pri.h"
#include "../libcpp69iip_scale_by_subpixel/iip_crop_and_downsample.h"
#include "iip_opengl_l1edit.h"

void iip_opengl_l1edit::init_opengl( void )
{
	static GLushort glusa_red[2] = {0,0xffff},
			glusa_gre[2] = {0,0xffff},
			glusa_blu[2] = {0,0xffff};

	/* 画像データがなくても以下の初期化だけはやっちまう */

	/* シェーディングをしない */
	glShadeModel( GL_FLAT );

	/* 背景(BG)(画面クリア)色 */
	glClearColor ( 0.5, 0.5, 0.5, 0.0 );

	/* B/W表示することを予定してあらかじめ設定しておく */
	glPixelMapusv( GL_PIXEL_MAP_I_TO_R, 2, glusa_red );
	glPixelMapusv( GL_PIXEL_MAP_I_TO_G, 2, glusa_gre );
	glPixelMapusv( GL_PIXEL_MAP_I_TO_B, 2, glusa_blu );
}

void iip_opengl_l1edit::init_viewport( long l_xs, long l_ys, long l_xp, long l_yp )
{
	this->_l_view_x_size = l_xs;
	this->_l_view_y_size = l_ys;
	this->_l_view_x_pos = l_xp;
	this->_l_view_y_pos = l_yp;
}

void iip_opengl_l1edit::clear_viewport( void )
{
	glViewport(
		this->_l_view_x_pos,
		this->_l_view_y_pos,
		this->_l_view_x_size,
		this->_l_view_y_size);

	glClear(GL_COLOR_BUFFER_BIT);
}

void iip_opengl_l1edit::reshape_opengl( long l_xp, long l_yp, long l_xs, long l_ys )
{
	/* 引数が全てゼロのときはOpenGLの再設定をする */
	if ((0 == l_xp) && (0 == l_yp)
	&&  (0 == l_xs) && (0 == l_ys)) {
		l_xp = this->_l_view_x_pos;
		l_yp = this->_l_view_y_pos;
		l_xs = this->_l_view_x_size;
		l_ys = this->_l_view_y_size;
	} else {
		/* 意味のある引数のときはエリアの設定しなおし */
		this->_l_view_x_pos = l_xp;
		this->_l_view_y_pos = l_yp;
		this->_l_view_x_size = l_xs;
		this->_l_view_y_size = l_ys;
	}

	/* 拡大ズーム時のスムーズスクロールのための
	余白(this->_l_margin_for_zoomup_smooth_scroll)をつける */
	const int margin = this->_l_margin_for_zoomup_smooth_scroll;
	glViewport(
		l_xp - margin,
		l_yp - margin,
		(GLsizei)l_xs + margin*2L,
		(GLsizei)l_ys + margin*2L );

	/* 透視投影変換行列の設定 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/*
	画像と、RectやLineの位置を合わせるため、
	Pixelの中心がPixelの位置となるよう0.5ずらす
	*/
	gluOrtho2D(
		- 0.5 - (double)margin,
		(GLdouble)(l_xs) - 0.5 + (double)margin,
		- 0.5 - (double)margin,
		(GLdouble)(l_ys) - 0.5 + (double)margin );

	/* モデルビュー変換行列の設定(画像、枠線表示のため) */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	/* 画像データがあるなら、表示パラメータの設定しなおし */
	if (NULL != this->get_vp_canvas()) {
		this->_set_position();
	}
}

void iip_opengl_l1edit::draw_opengl( void )
{
	/* 画像データがないと表示しない */
	if (NULL == this->get_vp_canvas()) { return; }

	/* ここでは画面クリア
		"glClear(GL_COLOR_BUFFER_BIT);"
	をしないが、必ず必要なので注意 */
	this->_draw_image();		/* 画像表示 */
	this->_draw_crop();		/* 画像枠表示 */

	/* ここではOpenGLのハキダシ
		"glFlush();"
	をしないが、必ず必要なので注意。
	Redhat9ではハキダシをしないとGL_LINE_LOOP表示しない */
}

/*--------------------------------------------------------*/

void iip_opengl_l1edit::_draw_image( void )
{
	/* 画像データがないときは(以下の)なにもしない */
	if (NULL == this->get_vp_canvas()) { return; }

	const int view_w = static_cast<int>(
		this->_glsi_width * this->_d_zoom);
	const int view_h = static_cast<int>(
		this->_glsi_height * this->_d_zoom);

#if 0
std::cout << __FILE__ << " " << __LINE__ << std::endl;
std::cout << "image  "
<< " w=" << this->get_l_width()
<< " h=" << this->get_l_height()
<< " ch=" << this->get_l_channels()
<< " by=" << this->cl_ch_info.get_l_bytes()
//<< " ref=" << this->_ucharp_rrggbbaa
/* this->_ucharp_rrggbbaaは
this->_i_disp_chがCH_RED,CH_GRE,CH_BLU,CH_ALPの時使っていたが、
もう使われていないのあとでrefactoringすべき2017-05-23 */
<< " xo=" << this->_gli_skip_pixels
<< " yo=" << this->_gli_skip_rows
<< " xs=" << this->_glsi_width
<< " ys=" << this->_glsi_height
<< std::endl;

std::cout << "pixel " 
//<< " ch=" << this->_i_disp_ch,
/* this->_i_disp_chは
CH_RED,CH_GRE,CH_BLU,CH_ALPの時使っていたが、
もう使われていないのあとでrefactoringすべき2017-05-23 */
<< " gle_format=" << this->_gle_format
<< "=?GL_RGB=" << GL_RGB
<< " gle_type=" << this->_gle_type
<< "=?GL_UNSIGNED_BYTE=" << GL_UNSIGNED_BYTE
<< std::endl;

std::cout << "view  " 
<< " xp=" << this->_gli_rasterpos_x
<< " yp=" << this->_gli_rasterpos_y
<< " zm=" << this->_d_zoom
<< " xs=" << view_w
<< " ys=" << view_h
<< std::endl;
#endif

	/* 画面から画像がはみ出た時も考慮した位置
	1 整数位置でずれる場合ある
		glRasterPos2i(	this->_gli_rasterpos_x,
				this->_gli_rasterpos_y );
	2 浮動小数点位置にして(Pixelの左下を表示開始位置とするため)-0.5する
		glRasterPos2d(	(double)(this->_gli_rasterpos_x) - 0.5,
				(double)(this->_gli_rasterpos_y) - 0.5 );
	3 下方位置（y値マイナス？）で画像消える現象。
	  特定のマシンのみで起こる。
	  原因不明。
	  0.5を0.49999にして直る。2016-10-4
	*/
	glRasterPos2d(	(double)(this->_gli_rasterpos_x) - 0.49999,
			(double)(this->_gli_rasterpos_y) - 0.49999 );

	/* 画像表示のための設定 */
	if (this->_d_zoom < 1.0) {
		/* 画像データのスキャンラインバイト数の倍数設定 */
		glPixelStorei( GL_UNPACK_ALIGNMENT , 1 );

		/* 画面から画像がはみ出た時のための画像幅の指定 */
		glPixelStorei( GL_UNPACK_ROW_LENGTH , view_w );

		/* 画面から画像がはみ出た時の表示開始の横位置 */
		glPixelStorei( GL_UNPACK_SKIP_PIXELS ,0 );

		/* 画面から画像がはみ出た時の表示開始の縦位置 */
		glPixelStorei( GL_UNPACK_SKIP_ROWS ,0 );
	}
	else {
		/* 画像データのスキャンラインバイト数の倍数設定 */
		glPixelStorei( GL_UNPACK_ALIGNMENT , 1 );

		/* 画面から画像がはみ出た時のための画像幅の指定 */
		glPixelStorei( GL_UNPACK_ROW_LENGTH , this->get_l_width() );

		/* 画面から画像がはみ出た時の表示開始の横位置 */
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,this->_gli_skip_pixels);

		/* 画面から画像がはみ出た時の表示開始の縦位置 */
		glPixelStorei( GL_UNPACK_SKIP_ROWS , this->_gli_skip_rows );
	}

	if (this->_d_zoom < 1.0) {
		iip_crop_and_downsample	iip_crop_and_down;
		if (iip_crop_and_down.reserve_max_memory(2560,1600)) {
			/* WQXGA 2560x1600 16:10 =ScreenMax想定 */
			pri_funct_msg_ttvr(
	  "Error in iip_crop_and_down.reserve_max_memory(2560,1600)" );
			return;
		}
		iip_crop_and_down.set_subpixel_max_div( 2 );
		if (int ret = iip_crop_and_down.set_mapping(
			 this->get_vp_canvas()
			,this->get_l_width()
			,this->get_l_height()
			,this->get_l_channels()
			,this->cl_ch_info.get_l_bytes()
			,this->_gli_skip_pixels
			,this->_gli_skip_rows
			,this->_glsi_width
			,this->_glsi_height
			,view_w
			,view_h
		)) {
			pri_funct_msg_ttvr(
		    "Error in iip_crop_and_down.set_mapping(-) returns "
		    		,ret);
			return;
		}
		iip_crop_and_down.exec();

		glPixelZoom((GLfloat)1.0 ,(GLfloat)1.0);
		glDrawPixels(
			 view_w
			,view_h
			,this->_gle_format
			,this->_gle_type
			,static_cast<const GLvoid *>(
				iip_crop_and_down.child_out.data()
			)
		);
	}
	else {
		glPixelZoom((GLfloat)this->_d_zoom ,(GLfloat)this->_d_zoom);
		glDrawPixels(
			this->_glsi_width
			, this->_glsi_height
			, this->_gle_format
			, this->_gle_type
			, static_cast<const GLvoid *>(this->get_vp_canvas())
		);
	}
}
namespace {
 void opengl_line_rectangle_(
	GLdouble x1 ,GLdouble y1 ,GLdouble x2 ,GLdouble y2
 ) {
	glBegin( GL_LINE_LOOP );
	glVertex2d( x1 ,y1 );
	glVertex2d( x1 ,y2 );
	glVertex2d( x2 ,y2 );
	glVertex2d( x2 ,y1 );
	glEnd();
 }
 void opengl_line_rect_by_center_( GLdouble xc ,GLdouble yc ,GLdouble ra ) {
	opengl_line_rectangle_( xc - ra ,yc - ra ,xc + ra ,yc + ra );
 }
 void opengl_line_horizontal_( GLdouble x1 ,GLdouble x2 ,GLdouble y ) {
	glBegin( GL_LINES );
	glVertex2d( x1 ,y );
	glVertex2d( x2 ,y );
	glEnd();
 }
 void opengl_line_vertical_( GLdouble x ,GLdouble y1 ,GLdouble y2 ) {
	glBegin( GL_LINES );
	glVertex2d( x ,y1 );
	glVertex2d( x ,y2 );
	glEnd();
 }
 class opengl_line_for_crop_ {
 public:
	opengl_line_for_crop_(
		GLdouble x1 ,GLdouble y1 ,GLdouble x2 ,GLdouble y2
		,GLdouble radius
		,E_SELECT_PART select_on
		,GLdouble ar ,GLdouble ag ,GLdouble ab
		,GLdouble mr ,GLdouble mg ,GLdouble mb
		,GLdouble sr ,GLdouble sg ,GLdouble sb
	)	:x1_(x1) ,y1_(y1) ,x2_(x2) ,y2_(y2)
		,xc_((x1+x2)/2.0) ,yc_((y1+y2)/2.0) ,radius_(radius)
		,select_on_(select_on)
		,ar_(ar),ag_(ag),ab_(ab)
		,mr_(mr),mg_(mg),mb_(mb)
		,sr_(sr),sg_(sg),sb_(sb)
	{
	}
	/* cropエリアと中心線は赤黒色 */
	void color_area( void ) { glColor3d(ar_,ag_,ab_);}
	void draw_area( void ) {
		opengl_line_rectangle_( x1_ ,y1_ ,x2_ ,y2_ );
		opengl_line_vertical_(	xc_ ,y1_ ,y2_ );
		opengl_line_horizontal_(x1_ ,x2_ ,yc_ );
	}

	/* 掴むための四角マークは選択時はオレンジ色、非選択時は赤灰色 */
	void color_mark( void ) { glColor3d(this->mr_,this->mg_,this->mb_);}
	void draw_mark( E_SELECT_PART select_crnt ) {
		GLdouble xx=0,yy=0;
		switch (select_crnt) {
		case E_SELECT_LEFTBOTTOM:	xx= x1_; yy= y1_; break;
		case E_SELECT_LEFT:		xx= x1_; yy= yc_; break;
		case E_SELECT_LEFTTOP:		xx= x1_; yy= y2_; break;
		case E_SELECT_TOP:		xx= xc_; yy= y2_; break;
		case E_SELECT_RIGHTTOP:		xx= x2_; yy= y2_; break;
		case E_SELECT_RIGHT:		xx= x2_; yy= yc_; break;
		case E_SELECT_RIGHTBOTTOM:	xx= x2_; yy= y1_; break;
		case E_SELECT_BOTTOM:		xx= xc_; yy= y1_; break;
		case E_SELECT_CENTER:		xx= xc_; yy= yc_; break;
		}
		if (select_crnt==this->select_on_) {
			glColor3d(sr_,sg_,sb_);
			opengl_line_rect_by_center_(xx,yy,this->radius_   );
			opengl_line_rect_by_center_(xx,yy,this->radius_-1.);
		}
		else {
			glColor3d(mr_,mg_,mb_);
			opengl_line_rect_by_center_( xx,yy,this->radius_ );
		}
	}
 private:
	//const GLdouble
	GLdouble
		x1_ ,y1_ ,x2_ ,y2_ ,xc_ ,yc_ ,radius_
		,ar_ ,ag_ ,ab_	/* cropエリア枠と中心十字線の色 */
		,mr_ ,mg_ ,mb_	/* (掴むための四角)マークの色 */
		,sr_ ,sg_ ,sb_	/* マークの選択時の色 */
		;
	//const E_SELECT_PART select_on_;
	E_SELECT_PART select_on_;
 };
}
void iip_opengl_l1edit::_draw_crop( void )
{
	GLdouble x1,y1, x2,y2;

	if (OFF == this->_i_crop_disp_sw) { return; }

	if (ON == this->get_i_mv_sw()) {
		pri_funct_msg_ttvr( "iip_opengl_l1edit::_draw_crop()" );
	}

	this->_get_crop_disp_area(
		(double)(this->_l_xp),
		(double)(this->_l_yp),
		(double)(this->_l_crop_xp),
		(double)(this->_l_crop_yp),
		(double)(this->_l_crop_width),
		(double)(this->_l_crop_height),
		&x1, &y1, &x2, &y2
	);

	if (ON == this->get_i_pv_sw()) {
		pri_funct_msg_vr( " GL_LINE_LOOP x1 %g y1 %g x2 %g y2 %g",
			x1, y1, x2, y2 );
	}

	/* 表示用object */
	opengl_line_for_crop_ o_l_c_(
		x1 ,y1 ,x2 ,y2
		,this->_l_crop_select_radius
		,this->_e_select_part
		,0.5  ,0.0 ,0.0 /* 赤黒色 : cropエリア枠と中心十字線の色 */
		,0.75 ,0.0 ,0.0 /* 赤灰色 : (掴むための四角)マークの色 */
		,1.0  ,0.5 ,0.0 /* オレンジ色 : マークの選択時の色 */
	);

	/* cropエリア枠と中心十字線を表示 */
	o_l_c_.color_area();
	o_l_c_.draw_area();

	/* (掴むための四角)マーク(選択してるものは選択色)を表示 */
	o_l_c_.color_mark();
	o_l_c_.draw_mark( E_SELECT_LEFTBOTTOM );
	o_l_c_.draw_mark( E_SELECT_LEFT );
	o_l_c_.draw_mark( E_SELECT_LEFTTOP );
	o_l_c_.draw_mark( E_SELECT_TOP );
	o_l_c_.draw_mark( E_SELECT_RIGHTTOP );
	o_l_c_.draw_mark( E_SELECT_RIGHT );
	o_l_c_.draw_mark( E_SELECT_RIGHTBOTTOM );
	o_l_c_.draw_mark( E_SELECT_BOTTOM );
	o_l_c_.draw_mark( E_SELECT_CENTER );
}

void iip_opengl_l1edit::_get_crop_disp_area( double d_image_xp, double d_image_yp, double d_crop_xp, double d_crop_yp, double d_crop_xs, double d_crop_ys, double *dp_x1, double *dp_y1, double *dp_x2, double *dp_y2 )
{
	*dp_x1 = d_image_xp + this->_d_zoom * d_crop_xp;
	*dp_y1 = d_image_yp + this->_d_zoom * d_crop_yp;

	if ((this->_l_margin_for_zoomup_smooth_scroll <= 0L)
	&&  (1.0 < this->_d_zoom)) {
		if (d_image_xp < 0.0) {
			*dp_x1 /= this->_d_zoom;
			*dp_x1 = ceil(*dp_x1);
			*dp_x1 *= this->_d_zoom;
		}

		if (d_image_yp < 0.0) {
			*dp_y1 /= this->_d_zoom;
			*dp_y1 = ceil(*dp_y1);
			*dp_y1 *= this->_d_zoom;
		}
	}

	*dp_x1 -= 1.0;
	*dp_y1 -= 1.0; 
	*dp_x2 = *dp_x1 + ceil( this->_d_zoom * d_crop_xs ) + 1.0; 
	*dp_y2 = *dp_y1 + ceil( this->_d_zoom * d_crop_ys ) + 1.0;
}
