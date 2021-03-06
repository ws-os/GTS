#ifndef ids_path_extensions_h
#define ids_path_extensions_h

#include <vector>
#include <string>

namespace ids {
namespace path {

/* 拡張子管理 */
class extensions {
public:
	extensions();

	/* 名前と拡張子をセット */
	void set_filter(const std::string& name ,const std::string& dotex);

	/* セットした数 */
	const int size(void) { return this->dotex_.size(); }

	/* 位置から拡張子を得る */
	const std::string str_from_num( const int num );

	/* 拡張子から位置を得る */
	const int num_from_str( const std::string& ext );

	/* fltk Nativeウインドウへ渡すための拡張子フィルタ文字列 */
	const std::string get_native_filters( void );

	/* fltk Fl_Choiceに渡すための拡張子フィルタ文字列 */
	const std::string get_fltk_filter( const int num );
private:
	std::vector< std::string >	 names_
					,dotex_;
};

} // path
} // ids

#endif /* !ids_path_extensions_h */
