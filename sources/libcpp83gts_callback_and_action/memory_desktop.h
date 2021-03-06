#ifndef memory_desktop_h
#define memory_desktop_h

#include <string>

class memory_desktop {
public:
	memory_desktop()
	:str_desktop_filename_( ".GTS-desktop" )
	,str_desktop_filename2_( "_gts-desktop.txt" )

	,str_show_(
	    "show" )
	,str_hide_(
	    "hide" )

	,str_config_dir_path_(
	    "config_dir_path" )
	,str_image_dir_path_(
	    "image_dir_path" )

	,str_window_main_(
	    "window_main" )
	,str_window_next_scan_(
	    "window_next_scan" )

	,str_window_area_and_rot90(
	    "window_area_and_rot90" )
	,str_window_pixel_type_and_bright_(
	    "window_pixel_type_and_bright" )

	,str_window_scan_and_save_(
	    "window_scan_and_save" )
	,str_window_trace_files_(
	    "window_trace_files" )

	,str_window_number_(
	    "window_number" )

	,str_window_trace_parameters_(
	    "window_trace_parameters" )
	,str_window_trace_batch_(
	    "window_trace_batch" )
	,str_window_trace_thickness_(
	    "window_trace_thickness" )
	,str_window_trace_input_color_(
	    "window_trace_input_color" )
	,str_window_trace_output_color_(
	    "window_trace_output_color" )
# ifndef _WIN32
	,str_desktop_dir_( ".GTS" )
	,str_sane_device_name_(
	    "sane_device_name" )
# endif
	{
	}

	int save( void );
	int load( void );

# ifndef _WIN32
	const char* get_install_and_scan_area_and_desktop_dir( void )
	{
		return this->str_desktop_dir_;
	}
# endif

private:
	const char* str_desktop_filename_;
	const char* str_desktop_filename2_;

	const char* str_show_;
	const char* str_hide_;

	const char* str_config_dir_path_;
	const char* str_image_dir_path_;

	const char* str_window_main_;
	const char* str_window_next_scan_;

	const char* str_window_area_and_rot90;
	const char* str_window_pixel_type_and_bright_;

	const char* str_window_scan_and_save_;
	const char* str_window_trace_files_;

	const char* str_window_number_;

	const char* str_window_trace_parameters_;
	const char* str_window_trace_batch_;
	const char* str_window_trace_thickness_;
	const char* str_window_trace_input_color_;
	const char* str_window_trace_output_color_;
# ifndef _WIN32
	const char* str_desktop_dir_;
	const char* str_sane_device_name_;
# endif

	int _load_by_fp( FILE *fp );
	int _load_by_fname( char *cp_fname );

	int _save_by_fp( FILE *fp );
	int _save_by_fname( const char *cp_fname );
	int _save_startup_path_by_fp( FILE *fp );

	int set_desktop_file_path_( void );
	std::string user_home_;
	std::string desktop_file_path_;
};

#endif /* !memory_desktop_h */
